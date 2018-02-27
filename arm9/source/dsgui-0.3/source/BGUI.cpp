// BGUI.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

/* system includes */
#include <nds.h>
#include "ipc_libnds_extended.h"

/* my includes */
#include "BGUI.h"
#include "BIPCCodes.h"

bool readFifo(u32* val) {
  if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
    {
      *val = REG_IPC_FIFO_RX;
      return true;
    }
  return false;
}

BGUI* BGUI::get()
{
  if(!_gui)
    _gui = new BGUI();
  return _gui;
}

void
BGUI::addScreen(BScreen* screen)
{
  _screens.push_back(screen);
}

void
BGUI::run()
{
  _runloop->run();
}

void
BGUI::repaint(bool force)
{
  if(_redrawsDisabled && !force) return;
  
  for(std::vector<BScreen*>::iterator iter = _screens.begin();
      iter != _screens.end(); iter++)
    (*iter)->repaint(force);
}

void
BGUI::disableRedraws()
{
  _redrawsDisabled = true;
}

void
BGUI::enableRedraws()
{
  _redrawsDisabled = false;
}

void
BGUI::selectWidget(BWidget* widget)
{
  if(_selectedWidget &&
     _selectedWidget->state() == BWidget::STATE_SELECTED)
    _selectedWidget->setState(BWidget::STATE_ENABLED);
  _selectedWidget = widget;
  if(widget->state() != BWidget::STATE_DISABLED)
    widget->setState(BWidget::STATE_SELECTED);
}

bool
BGUI::backlight(bool& top, bool& bottom)
{
  u32 bl = sendFIFO(IPC_BACKLIGHT|IPC_GET);
  top = bl & IPC_BACKLIGHT_TOP;
  bottom = bl & IPC_BACKLIGHT_BOTTOM;
  if(bl & IPC_OK) return true;
  return false;
}

bool
BGUI::switchBacklight(bool top, bool bottom)
{
  u32 bl = sendFIFO((IPC_BACKLIGHT |
		     (top ? IPC_BACKLIGHT_TOP : 0) |
		     (bottom ? IPC_BACKLIGHT_BOTTOM : 0)));
  if(bl & IPC_OK) return true;
  return false;
}

bool
BGUI::backlightBrightness(u8& brightness)
{
  u32 bl = sendFIFO(IPC_BRIGHTNESS|IPC_GET);
  brightness = IPC_ARG(bl);
  if(bl & IPC_OK) return true;
  return false;
}

bool
BGUI::setBacklightBrightness(u8 brightness)
{
  if(brightness > 3) brightness = 3;
  u32 bl = sendFIFO(IPC_BRIGHTNESS|brightness);
  if(bl & IPC_OK) return true;
  return false;
}

bool
BGUI::powerOff()
{
  u32 bl = sendFIFO(IPC_POWEROFF);
  if(bl & IPC_OK) return true;
  return false;
}

u32
BGUI::sendFIFO(u32 command)
{
  REG_IPC_FIFO_TX = command;
  int nNum = 0;
  while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) && nNum++ < 500);
  return REG_IPC_FIFO_RX;
}

BGUI* BGUI::_gui = NULL;

BGUI::BGUI()
{
  _selectedWidget = false;
  _redrawsDisabled = false;

  irqInit();

  irqEnable(IRQ_VBLANK);
  memset((void*)MyIPC, 0, sizeof(MyIPC));
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;

  _runloop = new BRunLoop();
  BRunLoop::_current = _runloop;
}

#if BGUI_test
int main(int argc, char **argv)
{
}
#endif /* BGUI_test */
