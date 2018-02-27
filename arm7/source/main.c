/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.2 2005/09/07 20:06:06 wntrmute Exp $

	Simple ARM7 stub (sends RTC, TSC, and X/Y data to the ARM 9)

	$Log: template.c,v $
	Revision 1.2  2005/09/07 20:06:06  wntrmute
	updated for latest libnds changes
	
	Revision 1.8  2005/08/03 05:13:16  wntrmute
	corrected sound code


---------------------------------------------------------------------------------*/
#include <nds.h>

#include <nds/bios.h>
//#include <nds/reload.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/clock.h>

#include <dswifi7.h>

//#include <BIPCCodes.h>
#include "pm.h"
#include "touch.h"
#include "ipc_libnds_extended.h"

void VblankHandler(void)
{
  updateMyIPC();
  Wifi_Update(); // update wireless in vblank
}

// callback to allow wifi library to notify arm9
void arm7_synctoarm9()
{
  // send fifo message
  REG_IPC_FIFO_TX = IPC_WIFI_SYNC;
}

// interrupt handler to allow incoming notifications from arm9
void arm7_fifo()
{
  // check incoming fifo messages
  u32 msg = REG_IPC_FIFO_RX;
  bool top, bottom;
  switch(msg)
    {
    case IPC_WIFI_SYNC:
      Wifi_Sync();
      REG_IPC_FIFO_TX = IPC_OK;
      break;

    case IPC_BACKLIGHT:
      if(msg != IPC_GET)
	pmSwitchBacklight(msg & IPC_BACKLIGHT_TOP,
			  msg & IPC_BACKLIGHT_BOTTOM);
      pmGetBacklight(&top, &bottom);
      REG_IPC_FIFO_TX = (IPC_OK |
			 (top ? IPC_BACKLIGHT_TOP : 0) |
			 (bottom ? IPC_BACKLIGHT_BOTTOM : 0));
      break;
      
    case IPC_BRIGHTNESS:
      if(msg != IPC_GET)
	pmSetBacklightBrightness(msg);
      REG_IPC_FIFO_TX = IPC_OK | pmGetBacklightBrightness();
      break;
      
    case IPC_POWEROFF:
      pmPowerOff();
      REG_IPC_FIFO_TX = IPC_OK;
    }
#if 0
  // check incoming fifo messages
  u32 msg = REG_IPC_FIFO_RX;
  if(msg == IPC_WIFI_SYNC)
    Wifi_Sync();
  else if((msg & 0xfffffff0) == IPC_BACKLIGHT)
    pmSwitchBacklight(msg & IPC_BACKLIGHT_TOP,
		      msg & IPC_BACKLIGHT_BOTTOM);
  else if(msg == IPC_POWEROFF)
    pmPowerOff();
#endif
}

int main(int argc, char ** argv)
{
  pmSwitchBacklight(true, false);

  // enable & prepare fifo asap
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; 
  // Reset the clock if needed
  rtcReset();
  
  irqInit();
  irqSet(IRQ_VBLANK, VblankHandler);
  irqEnable(IRQ_VBLANK);
  
  irqSet(IRQ_WIFI, Wifi_Interrupt); // set up wifi interrupt
  irqEnable(IRQ_WIFI);
  
  { // sync with arm9 and init wifi

    u32 fifo_temp;   
    
    while(1) { // wait for magic number
      while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
      fifo_temp=REG_IPC_FIFO_RX;
      if(fifo_temp==IPC_WIFI_INIT) break;
    }
    while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
    fifo_temp=REG_IPC_FIFO_RX; // give next value to wifi_init
    Wifi_Init(fifo_temp);
    
    irqSet(IRQ_FIFO_NOT_EMPTY,arm7_fifo); // set up fifo irq
    irqEnable(IRQ_FIFO_NOT_EMPTY);
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
    
    Wifi_SetSyncHandler(arm7_synctoarm9); // allow wifi lib to notify arm9
  } // arm7 wifi init complete

  // initialize reboot address
  *(vu32*)0x27ffffc = 0;
  
  while(1)
    {
      swiWaitForVBlank();

      // reboot?
      if((*(vu32*)0x27ffffc) != 0)
	{
	  REG_IE = 0;
	  REG_IME = 0;
	  REG_IF = 0xffff;
	  
	  typedef void (*eloop_type)(void);
	  eloop_type eloop = *(eloop_type*)0x27ffffc;
	  *(vu32*)0x27ffffc = 0;
	  *(vu32*)0x27ffff8 = 0;
	  
	  eloop();
	}
    }
}


