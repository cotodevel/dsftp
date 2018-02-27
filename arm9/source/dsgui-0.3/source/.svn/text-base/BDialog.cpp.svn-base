// BDialog.cpp (this is -*- C++ -*-)
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
/* (none) */

/* my includes */
#include "BDialog.h"
#include "BGUI.h"

BDialog::BDialog(BScreen* screen, const BSize& size):
  BWidget(NULL, BRect(0, 0, size.width, size.height))
{
  setReliefType(BWidget::RELIEF_SIMPLE);
  
  _screen = screen;
  _loop = new BRunLoop;

  center();
}

BDialog::~BDialog()
{
  delete _loop;
}

void
BDialog::center()
{
  int w = _screen->image()->width();
  int h = _screen->image()->height();

  _frame.pt.x = (w-_frame.sz.width)/2;
  _frame.pt.y = (h-_frame.sz.height)/2;
}

void
BDialog::run()
{
  show();
  _screen->setDialog(this);

  _loop->run();

  _screen->setDialog(NULL);
}

void
BDialog::stop()
{
  _loop->stop();
  _screen->setNeedsRedrawOnAllWidgetsCoveredBy(_frame, true);
  hide();
}

#if BDialog_test
int main(int argc, char **argv)
{
}
#endif /* BDialog_test */
