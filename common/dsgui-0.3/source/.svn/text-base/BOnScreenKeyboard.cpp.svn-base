// BOnScreenKeyboard.cpp (this is -*- C++ -*-)
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
#include "BOnScreenKeyboard.h"

#define WIDTH 17
#define HEIGHT 17

BOnScreenKeyboard::BOnScreenKeyboard(BScreen* screen):
  BDialog(screen, BSize(11*WIDTH+2, 6*HEIGHT+1))
{
  _cancelled = false;
  
  BButton* b;

  _label = new BLabel(this, BRect(2, 2, 10*WIDTH-5, 2*HEIGHT-5));
  _label->setReliefType(BWidget::RELIEF_SIMPLE);
  _label->setTextAlignment(BWidget::ALIGN_LEFT);
  _label->setDrawsCursor(true);
  _label->setVPadding(2);
  _label->setHPadding(2);

  b = new BButton(this, BRect(10*WIDTH, (HEIGHT-2)/2, WIDTH, HEIGHT));
  b->setText("X");
  b->setDelegate(this);

  _keyboard = new BKeyboard(this, BRect(1, 2*HEIGHT, 11*WIDTH, 4*HEIGHT));
  _keyboard->setDelegate(this);
}

BOnScreenKeyboard::~BOnScreenKeyboard()
{
}

void
BOnScreenKeyboard::onKeyboardPressGlyph(BKeyboard* keyboard,
					const std::string& glyph)
{
  _label->setText(_label->text() + glyph);
}

void
BOnScreenKeyboard::onKeyboardPressSpecial(BKeyboard* keyboard,
					  BKeyboard::SpecialKey special)
{
  switch(special)
    {
    case BKeyboard::KEY_RETURN:
      _cancelled = false;
      stop();
      break;

    case BKeyboard::KEY_BACKSPACE:
      _label->setText(_label->text().substr(0, _label->text().size()-1));
      break;

    default:
      break;
    }
}

void
BOnScreenKeyboard::onButtonClick(BButton* button)
{
  _cancelled = true;
  stop();
}

void
BOnScreenKeyboard::setRotation(Rotation rot)
{
  BWidget::setRotation(rot);
  switch(rot)
    {
    case ROT_0:
    case ROT_180:
      _frame.sz = BSize(11*WIDTH+2, 6*HEIGHT+1); 
      break;
    case ROT_90:
    case ROT_270:
      _frame.sz = BSize(6*HEIGHT+1, 11*WIDTH+2);
      break;
    }
  center();
}

#if BOnScreenKeyboard_test
int main(int argc, char **argv)
{
}
#endif /* BOnScreenKeyboard_test */
