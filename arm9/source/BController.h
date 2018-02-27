// BController.h (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

#ifndef BCONTROLLER_H
#define BCONTROLLER_H

#define _GLIBCXX_USE_CXX11_ABI 0

/* system includes */
#include <stdio.h>
#include <stdarg.h>
#include <BImage.h>
#include <BText.h>
#include <BLabel.h>
#include <BFont.h>
#include <BScroller.h>

/* my includes */
#include "BFTPServer.h"
#include "BLogWidget.h"

#ifdef __cplusplus

class BController:
  public BFTPServer::Delegate,
  public BFTPControlConn::Delegate {
public:
  static BController *get();

  bool wifiConnect();
  void run();

  void setStatus(const std::string& status);

  void setScreenSaverTimeout(int seconds);
  void setScreenSaver(bool offon);
  void resetScreenSaver();
  static void resetScreenSaver_s(void* arg);

  void powerOff();

  /*!
    \name Delegate methods
    \{
  */
  bool controlConnectionWillOpen(BFTPServer *server,
				 long address);
  void controlConnectionDidOpen(BFTPServer *server,
				BFTPControlConn *conn);
  void controlConnectionDidClose(BFTPControlConn *conn);
  bool handleUnknownCommand(BFTPControlConn *cconn,
			    std::string& cmd, std::string& arg);
  /*!
    \}
  */
    
private:
  BController();

  void initScreen();
  void initGUI();
  void initWifi();
  void initFilesystem();
  void initFTP();
  void screenSaverTimer(bool reset = false);

  void updateStatus();
  void updateGUI();

  void doScreenshot();
  
  static BController* sharedController;

  int ssaverTimeout, ssaverLastTime;
  bool ssaverOn;

  BImage *dsftpImg, *backbufImg;
  
  BLogWidget* logDisplay;
  BLabel* versionLabel;
  BLabel* statusLabel;
  BScroller* vScroller;
  BFont* font;

  BFTPServer* server;

  int sshotnum;
  int fsUsageCount;
};
#endif

#endif /* BCONTROLLER_H */
