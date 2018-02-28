// BMenu.cpp (this is -*- C++ -*-)
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
#include "BMenu.h"
#include "BStringUtils.h"
#include "BMessagebox.h"

BMenu::BMenu(BScreen* screen, const BSize& size):
  BDialog(screen, size)
{
  _lb = new BListbox(this, BRect(0, 0, size.width, size.height));
  _lb->setReliefType(BWidget::RELIEF_SIMPLE);
  _lb->setDelegate(this);
  _lb->setDrawsSelection(false);
  _lb->setVPadding(5);
  _cancelled = false;
}

BMenu::~BMenu()
{
}

void BMenu::addEntry(const std::string &title, u16 color, int tag)
{
  _lb->addEntry(title, color, tag);
}

const BListbox::Entry& BMenu::selectedEntry()
{
  return _lb->selectedEntry();
}
  
void BMenu::onEntryDoubleClick(BListbox *box, BListbox::Entry &entry)
{
  onEntryClick(box, entry);
}

void BMenu::onEntryClick(BListbox *box, BListbox::Entry &entry)
{
  _cancelled = false;
  stop();
}

void
BMenu::setRotation(Rotation rot)
{
  BWidget::setRotation(rot);
  switch(rot)
    {
    case ROT_0:
    case ROT_180:
      _frame.sz = BSize(_lb->frame().sz.width,
			_lb->frame().sz.height);
      break;
    case ROT_90:
    case ROT_270:
      _frame.sz = BSize(_lb->frame().sz.height,
			_lb->frame().sz.width);
      break;
    }
  center();
}

void BMenu::handleEvent(const BEvent& event)
{
  if(event.type == EVT_TOUCHDOWN)
    {
      if(!_frame.containsPoint(BPoint(event.touch.x, event.touch.y)))
	{
	  _cancelled = true;
	  stop();
	  return;
	}
    }
  BWidget::handleEvent(event);
}

void BMenu::run()
{
  _cancelled = false;
  _lb->selectEntry(0);
  
  BDialog::run();
}

#if BMenu_test
int main(int argc, char **argv)
{
}
#endif /* BMenu_test */
