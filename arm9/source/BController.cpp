// BController.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

//coto: prevent C++11 linking. Must be called before every c++ system header
#define _GLIBCXX_USE_CXX11_ABI 0


/* system includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <netinet/in.h>
#include <nds/registers_alt.h>

#include <nds.h>
#include <nds/arm9/console.h>
#include <dswifi9.h>
//#include <zlib.h>

#include <BGUI.h>
#include <BFont.h>
#include <BFontManager.h>
#include <BStringUtils.h>
#include <BIPCCodes.h>

/* my includes */
#include "ipc_libnds_extended.h"

#include "BController.h"

#include "BFTPConfigurator.h"
#include "BLogger.h"

#include "Exec.h"

#include "version.h"
#include "Bitstream_Vera_Serif_Roman_10_bin.h"

#define EXTERNAL_DATA(name) \
  extern const uint8 name[]; \
  extern const uint8 name##_end[]; \
  extern const uint32 name##_size
EXTERNAL_DATA(img_dsftp_bin);

#define WIFI_UPDATE_FREQ 50

#define SSAVER_TIMEOUT 5

//#define LOG_PRINTF

static std::string makeRandomPassword()
{
  char* passchars = "123456789"
    "abcdefghijkmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNPQRSTUVWXYZ";

  unsigned int seed = (-(~REG_VCOUNT)-1 | MyIPC->aux)*(ipc_get_secrtc());
  srand(seed);

  std::string str;
  for(int i=0; i<8; i++)
    {
      int index =
	(int)(((float)rand() / (float)RAND_MAX)*(float)(strlen(passchars)-1));
      str += passchars[index];
    }

  return str;
}


// notification function to send fifo message to arm7
static void myWifiSyncHandler()
{ // send fifo message
   REG_IPC_FIFO_TX=IPC_WIFI_SYNC;
}

// interrupt handler to receive fifo messages from arm7
static void myFifoIrq()
{ // check incoming fifo messages
   u32 value = REG_IPC_FIFO_RX;
   if(value == IPC_WIFI_SYNC) Wifi_Sync();
}

// wifi timer function
static void myWifiTimer(void)
{
  Wifi_Timer(WIFI_UPDATE_FREQ);
}

BController* BController::get()
{
  if(!sharedController)
    sharedController = new BController;
  return sharedController;
}

bool BController::wifiConnect()
{

  // simple WFC connect:
  int i;
  Wifi_AutoConnect(); // request connect
  while(1)
    {
      while(REG_VCOUNT>192); // wait for vblank
      while(REG_VCOUNT<192);

      i = Wifi_AssocStatus(); // check status

      char* animstr = "-\\|/";
      static int anim = 0;
      
      char indicator = animstr[anim];
      anim++; if(anim>3) anim = 0;

      switch(i)
	{
	case ASSOCSTATUS_SEARCHING:
	  setStatus(format("%c Searching for Access Point %c",
			   indicator, indicator));
	  break;
	  
	case ASSOCSTATUS_AUTHENTICATING:
	  setStatus(format("%c Authenticating %c",
			   indicator, indicator));
	  break;
	  
	case ASSOCSTATUS_ASSOCIATING:
	  setStatus(format("%c Associating %c",
			   indicator, indicator));
	  break;
	  
	case ASSOCSTATUS_ACQUIRINGDHCP:
	  setStatus(format("%c Acquiring DHCP %c",
			   indicator, indicator));
	  break;
	  
	case ASSOCSTATUS_ASSOCIATED:
	  setStatus("Connected");
	  return true;
	  
	case ASSOCSTATUS_CANNOTCONNECT:
	  setStatus("Could not connect");
	  Wifi_DisconnectAP();
	  return false;
	  
	default:
	  break;
	}
    }
  return false;
  
}

BController::BController()
{
  sharedController = this;
  ssaverTimeout = 60;
  ssaverLastTime = 0;
  ssaverOn = false;

  server = NULL;

  sshotnum = 0;

  powerOn(POWER_ALL);

  initScreen();
  initGUI();
  setScreenSaver(false);

  setStatus("Init Wifi.");
  initWifi();

  setStatus("Init file system.");
  if(BFileManager::get() == NULL)
    {
      setStatus("Could not init file system. Please restart.");
      while(1);
    }

  if(wifiConnect() != true)
    {
      setStatus("Could not connect. Please restart.");
      while(1);
    }
  
  initFTP();

  fsUsageCount = 0;
}

void BController::initScreen()
{
  /* main screen */
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  vramSetBankC(VRAM_C_SUB_BG_0x06200000);
  SUB_BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(0);
  SUB_BG3_XDX = 1 << 8;
  SUB_BG3_XDY = 0;
  SUB_BG3_YDX = 0;
  SUB_BG3_YDY = 1 << 8;
  SUB_BG3_CY = 0;
  SUB_BG3_CX = 0;

#if defined(LOG_PRINTF)
  videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
  vramSetBankA(VRAM_A_MAIN_BG);
  BG0_CR = BG_MAP_BASE(31);
  BG_PALETTE[255] = RGB15(31, 31, 31);
  consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);
#endif

  lcdSwap();
}

void
BController::initGUI()
{
  dsftpImg = new BImage(256, 192, (uint8*)img_dsftp_bin,
			BImage::FMT_RGB15, false);
  backbufImg = new BImage(*dsftpImg);

  BMemFile fontfile((u32)Bitstream_Vera_Serif_Roman_10_bin,
		    Bitstream_Vera_Serif_Roman_10_bin_size);
  font = new BFont(fontfile);
  BFontManager::get()->addFont(font);
  BFontManager::get()->scanDirectory("/data/fonts");

  logDisplay = new BLogWidget(NULL, BRect(10, 32, 230, 142));
  logDisplay->setFont(font);
  logDisplay->setBackgroundAlpha(20);
  logDisplay->setReliefType(BWidget::RELIEF_SUNKEN);
  logDisplay->setTextDirection(BWidget::DIR_LEFTTORIGHT);

  vScroller = new BScroller(NULL, BRect(240, 32, 6, 142));
  vScroller->setOrientation(BScroller::ORIENT_VERTICAL);
  logDisplay->setVScroller(vScroller);

  std::string versionStr = std::string("V") + VERSION;
  versionLabel = new BLabel(NULL, BRect(65, 26-font->height(),
				  font->widthOfString(versionStr),
				  font->height()));
  versionLabel->setFont(font);
  versionLabel->setBackgroundAlpha(30);
  versionLabel->setReliefType(BWidget::RELIEF_NONE);
  versionLabel->setText(std::string("V") + VERSION);

  statusLabel = new BLabel(NULL, BRect(0, 174, 256, 18));
  statusLabel->setFont(font);
  statusLabel->setBackgroundAlpha(30);
  statusLabel->setReliefType(BWidget::RELIEF_NONE);
  statusLabel->setText("Initializing");

  BLoggerManager::get()->addLogger(logDisplay);

  BLoggerManager::get()->setLogLevel(LOG_BLAH);

  updateGUI();
}

void BController::initWifi()
{

  // send fifo message to initialize the arm7 wifi
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

  // acquire magic pass number and transmit it to arm7
  u32 Wifi_pass = Wifi_Init(WIFIINIT_OPTION_USELED);
  REG_IPC_FIFO_TX = IPC_WIFI_INIT;
  REG_IPC_FIFO_TX = Wifi_pass;

  TIMER_CR(3) = 0; // disable timer 3 (fow now)
    
  irqInit(); 
  irqSet(IRQ_TIMER3, myWifiTimer); // setup timer IRQ
  irqEnable(IRQ_TIMER3);
  
  irqSet(IRQ_FIFO_NOT_EMPTY, myFifoIrq); // setup fifo IRQ
  irqEnable(IRQ_FIFO_NOT_EMPTY);
  
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ; // enable FIFO IRQ
    
  // tell wifi lib to use our handler to notify arm7
  Wifi_SetSyncHandler(myWifiSyncHandler); 
    
  // set timer3
  TIMER_DATA(3) = TIMER_FREQ_256(WIFI_UPDATE_FREQ);
  TIMER_CR(3) = TIMER_ENABLE | TIMER_IRQ_REQ | TIMER_DIV_256;
    
  // wait for arm7 to be initted successfully
  while(Wifi_CheckInit()==0)
  { 
    while(REG_VCOUNT>192); // wait for vblank
    while(REG_VCOUNT<192);
  }
    
  irqSet(IRQ_IPC_SYNC, Wifi_Update);
  irqEnable(IRQ_IPC_SYNC);
  REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE; 	

}

void
BController::initFTP()
{
  server = new BFTPServer;
  server->setDelegate(this);

  BLoggerManager::get()->setScreenSaverResetFunction(resetScreenSaver_s, this);

  BFTPConfigurator configurator(server);
  if(!configurator.configureFromFile("/data/settings/ftp.conf") ||
     server->getNumberOfUsers() == 0)
    {
      std::string pass = makeRandomPassword();
      LOG(LOG_WARN, "Adding user \"default\", pass \"%s\"", pass.c_str());
      server->addUser("default", pass, "/", "/", true, true);
    }

#if defined(LOG_PRINTF)
  BLoggerManager::get()->setLogLevel(LOG_BLAH);
#endif

  ssaverTimeout = server->screenSaverTimeout();
}

void
BController::setStatus(const std::string& status)
{
  statusLabel->setText(status);
  updateGUI();
}

void
BController::updateStatus()
{
  std::string str = "";
  
  char buf[255];
  memset(buf, 0, 255);
  
  u32 ip = Wifi_GetIP();
  sprintf(buf, "IP %d.%d.%d.%d",
	  (int)(ip & 0x000000ff),
	  (int)((ip & 0x0000ff00) >> 8),
	  (int)((ip & 0x00ff0000) >> 16),
	  (int)((ip & 0xff000000) >> 24));

  str += buf;

  if(server && server->numActiveConnections())
    {
      sprintf(buf, " - %d connections",
	      server->numActiveConnections());
      str += buf;
    }

  setStatus(str);
}


void
BController::updateGUI()
{
  *backbufImg = *dsftpImg;
  logDisplay->setNeedsRedraw(true);
  logDisplay->draw(*backbufImg);
  vScroller->setNeedsRedraw(true);
  vScroller->draw(*backbufImg);
  versionLabel->setNeedsRedraw(true);
  versionLabel->draw(*backbufImg);
  statusLabel->setNeedsRedraw(true);
  statusLabel->draw(*backbufImg);
  backbufImg->blit(BPoint(0, 0), (uint16*)BG_BMP_RAM_SUB(0));
}

void
BController::doScreenshot()
{
  char buf[255];
  memset(buf, 0, 255);
  sprintf(buf, "/sshot%d.ppm", sshotnum++);

  std::string filename = buf;

  LOG(LOG_INFO, "Storing %s", filename.c_str());
  updateGUI();

  BFATFile* file = new BFATFile(buf, "w");
  if(!file) return;

  memset(buf, 0, 255);
  sprintf(buf, "P6\n256 192\n31\n");
  file->write(buf, strlen(buf));

  uint16 *fbuf;
  uint8 *nbuf = new uint8[256*192*3];

  fbuf = (uint16*)BG_BMP_RAM_SUB(0);
  for(unsigned int i=0; i<256*192; i++)
    {
      nbuf[i*3] = (fbuf[i] & 0x1f);
      nbuf[i*3+1] = (fbuf[i] & (0x1f << 5)) >> 5;
      nbuf[i*3+2] = (fbuf[i] & (0x1f << 10)) >> 10;
    }
  file->write(nbuf, 256*192*3);

  delete file;
  LOG(LOG_INFO, "Screenshot %s stored.", filename.c_str());
  updateGUI();
}

void
BController::screenSaverTimer(bool reset)
{
  if(ssaverTimeout == -1)
    return;

  int seconds = ((ipc_get_hourrtc() * 60 + ipc_get_minrtc()) * 60 +
		 ipc_get_secrtc());

  if(ssaverLastTime == 0)
    {
      ssaverLastTime = seconds;
      return;
    }
  
  if(reset)
    {
      setScreenSaver(false);
      updateGUI();
    }
  else if(ssaverOn == false &&
	  seconds > (ssaverLastTime + ssaverTimeout))
    setScreenSaver(true);
}

void
BController::setScreenSaverTimeout(int seconds)
{
  ssaverTimeout = seconds;
}

void
BController::resetScreenSaver()
{
  screenSaverTimer(true);
}

void
BController::resetScreenSaver_s(void* arg)
{
  ((BController*)arg)->resetScreenSaver();
}

void
BController::setScreenSaver(bool offon)
{
  if(offon)
    {
      BGUI::get()->switchBacklight(false, false);
    }
  else
    {
#if defined(LOG_PRINTF)
      BGUI::get()->switchBacklight(true, true);
#else
      BGUI::get()->switchBacklight(true, false);
#endif
      ssaverLastTime = MyIPC->aux / 60;
    }
  ssaverOn = offon;
}

void
BController::powerOff()
{
  delete BLoggerManager::get(); // to make sure all logfiles are closed
  delete BFileManager::get();

  BGUI::get()->powerOff();
  //  REG_IPC_FIFO_TX = IPC_POWEROFF;
}

void BController::run()
{
  LOG(LOG_INFO, "Server running on port %u.", server->portNumber());
  if(ssaverTimeout == -1)
    LOG(LOG_INFO, "Screensaver: OFF");
  else
    {
      LOG(LOG_INFO, "Screensaver: %d seconds", ssaverTimeout);
      LOG(LOG_INFO, "Wake on Log: %s",
	      BLoggerManager::get()->wakesOnLog() ? "ON" : "OFF");
    }
  LOG(LOG_INFO, "Press A+B+X+Y for safe poweroff.");

  irqEnable(IRQ_VBLANK);
  scanKeys();
  uint32 buttons = keysHeld();

  while(true)
    {
#if defined(LOG_PRINTF)
      static int watchdogCounter = 0;
      watchdogCounter++;
      if(watchdogCounter >= 100)
	{
	  LOG(LOG_INFO, "Watchdog!");
	  watchdogCounter = 0;
	}
#endif
      
      if(!ssaverOn) updateStatus();
      
      scanKeys();
      uint32 tmpbuttons = keysHeld();
      if(tmpbuttons & KEY_LID)
	{
	  setScreenSaver(true);
	  buttons = tmpbuttons;
	}
      else if(tmpbuttons) // if(tmpbuttons != buttons)
	{
	  screenSaverTimer(true); // reset
	  buttons = tmpbuttons;

	  if(tmpbuttons & KEY_UP)
	    {
	      if(logDisplay->firstLine() > 0)
		{
		  logDisplay->setFirstLine(logDisplay->firstLine() - 1);
		  logDisplay->setAutoscroll(false);
		}
	    }
	  else if(tmpbuttons & KEY_DOWN)
	    {
	      logDisplay->setFirstLine(logDisplay->firstLine() + 1);
	      if(logDisplay->atEnd())
		logDisplay->setAutoscroll(true);
	    }
	  else
	    if(tmpbuttons & KEY_L &&
		  tmpbuttons & KEY_R &&
		  tmpbuttons & KEY_A)
	    doScreenshot();
	  else if(tmpbuttons & KEY_X &&
		  tmpbuttons & KEY_Y &&
		  tmpbuttons & KEY_A &&
		  tmpbuttons & KEY_B)
	    powerOff();
	}
      else
	screenSaverTimer();

      server->handle();
      swiWaitForVBlank();
    }
}
    
bool
BController::controlConnectionWillOpen(BFTPServer *server,
				       long address)
{
  return true;
}

void
BController::controlConnectionDidOpen(BFTPServer *server,
				      BFTPControlConn *conn)
{
  conn->setDelegate(this);
}

void
BController::controlConnectionDidClose(BFTPControlConn *conn)
{
  fsUsageCount--;
  if(fsUsageCount <= 0)
    {
      //LOG(LOG_INFO, "Freeing fs");
      //FAT_FreeFiles();
      fsUsageCount = 0;
    }
}

bool
BController::handleUnknownCommand(BFTPControlConn *cconn,
				  std::string& cmd, std::string& arg)
{
  if(cmd == "BOOT")
    {
      if(!cconn->currentUser().bootPermission)
	cconn->sendReply(530, "No boot permission.");
      else
	{
	  std::string filename = cconn->makeAbsoluteFilename(arg);
	  cconn->sendReply(250, "Booting %s.", filename.c_str());
	  cconn->close();
	  
	  REG_IPC_FIFO_TX = (IPC_BACKLIGHT |
			     IPC_BACKLIGHT_TOP |
			     IPC_BACKLIGHT_BOTTOM);
	  swiWaitForVBlank();
	  exec(filename.c_str());
	}
      
      return true; // never get here
    }
  
  else if(cmd == "POWR")
    {
      if(!cconn->currentUser().bootPermission)
	cconn->sendReply(530, "No permission to power off.");
      else
	{
	  cconn->sendReply(250, "Powering off.");
	  cconn->close();
	  swiWaitForVBlank();
	  powerOff();
	}

      return true; // never get here
    }
#if 0
  else if(cmd == "UNZP")
    {
      std::string filename = cconn->makeAbsoluteFilename(arg);

      if (filename.length() <= 3)
	{
	  cconn->sendReply(530, "The argument to UNZP must be <FILENAME>.GZ.");
	  return false;
	}
     
      std::string outputFilename(filename.begin(),
				 filename.begin() + filename.length() - 3);
      std::string extension(filename.begin() + filename.length() - 3,
			    filename.end());
      if(extension != ".GZ" && extension != ".gz")
	{
	  cconn->sendReply(530, "The argument to UNZP must be <FILENAME>.GZ.");
	  return true;
	}

      errno = 0;
      gzFile inputFile = gzopen(filename.c_str(), "r");
      if (!inputFile)
	{
	  cconn->sendReply(530, "Unable to gzopen \"%s\"", filename.c_str());
	  return true;
	}

      FAT_FILE* outputFile = FAT_fopen(outputFilename.c_str(), "w");
      
      char buffer[2048];
      int err;
      
      do
	{
	  err = gzread(inputFile, buffer, sizeof(buffer));
	  if (err == -1)
	    {
	      cconn->sendReply(530, "Unpacking failed: %s", gzerror(inputFile, NULL));
	      gzclose(inputFile);
	      FAT_fclose(outputFile);
	      return true;
	    }
	  else if (err == 0)
	    break; // done
	  else
	    FAT_fwrite(buffer, 1, err, outputFile);
  
	} while(true);
      
      FAT_fclose(outputFile);   
      
      gzclose(inputFile);
      cconn->sendReply(226, "Unpacked file stored as \"%s\".",
		       outputFilename.c_str());
      return true;
    }
#endif
  else
    return false;
}

BController* BController::sharedController = NULL;


