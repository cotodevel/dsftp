// BMessagebox.cpp (this is -*- C++ -*-)
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
#include "BMessagebox.h"
#include "BTheme.h"
#include "BText.h"
#include "BButton.h"

BMessagebox::BMessagebox(BScreen* screen, const BSize& size,
			 const std::string& str,
			 BMessageboxType type):
  BDialog(screen, size)
{
  BRect cr = BTheme::currentTheme()->contentRectForWidget(*this);
  cr = cr.insetRect(5, 5, 5, 5);

  BText* text =
    new BText(this, BRect(cr.pt.x, cr.pt.y, cr.sz.width, cr.sz.height-20));
  text->setText(str);

  BButton* b;
  switch(type)
    {
    case TYPE_NONE:
      break;
      
    case TYPE_OK:
      b = new BButton(this, BRect(cr.pt.x+cr.sz.width-50, cr.pt.y+cr.sz.height-20,
				  50, 20));
      b->setDelegate(this);
      b->setText("OK");
      b->setTag(RESULT_OK);
      break;

    case TYPE_OK_CANCEL:
      b = new BButton(this, BRect(cr.pt.x+cr.sz.width-50, cr.pt.y+cr.sz.height-20,
				  50, 20));
      b->setDelegate(this);
      b->setText("OK");
      b->setTag(RESULT_OK);

      b = new BButton(this, BRect(cr.pt.x+cr.sz.width-105, cr.pt.y+cr.sz.height-20,
				  50, 20));
      b->setDelegate(this);
      b->setText("Cancel");
      b->setTag(RESULT_CANCEL);
      break;

    default:
      break;
    }

  _type = type;
}

BMessagebox::~BMessagebox()
{
}

void
BMessagebox::run()
{
  if(_type == TYPE_NONE) return;
  BDialog::run();
}

void
BMessagebox::onButtonClick(BButton* button)
{
  _result = button->tag();
  stop();
}
  
void BMessagebox::runMessage(BScreen* screen,
			     const std::string& txt,
			     BWidget::Rotation rot)
{
  BMessagebox* msg =
    new BMessagebox(screen, BSize(100, 100), txt,
		    BMessagebox::TYPE_OK);
  msg->setRotation(rot);
  msg->run();
  delete msg;
}


#if BMessagebox_test
int main(int argc, char **argv)
{
}
#endif /* BMessagebox_test */
