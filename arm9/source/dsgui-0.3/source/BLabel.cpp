// BLabel.cpp (this is -*- C++ -*-)
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
#include "BLabel.h"
#include "BOnScreenKeyboard.h"
#include "BTheme.h"

BLabel::BLabel(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _drawsCursor = false;
}

void
BLabel::draw(BImage& img)
{
  if(!isShown())
    return;
  
  BWidget::draw(img);
  drawText(img);
}

void
BLabel::drawText(BImage& img)
{
  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  
  if(_drawsCursor)
    img.drawText(_font, _text, _text.size(), clip, *this, _textcolor);
  else
    img.drawText(_font, _text, -1, clip, *this, _textcolor);
}

void
BLabel::setText(const std::string& text)
{
  _text = text;
  setNeedsRedraw(true);
}

void
BLabel::setEditable(bool editable)
{
  _editable = editable;
}

void
BLabel::handleEvent(const BEvent& event)
{
  if(!_editable || !isShown() || !screen()) return;

  BRect rect = _frame;
  if(_parent) rect = _parent->transformUp(rect);
  
  if(event.type == EVT_TOUCHDOWN)
    {
      if(rect.containsPoint(BPoint(event.touch.x, event.touch.y)))
	{
	  BOnScreenKeyboard *k = new BOnScreenKeyboard(screen());
	  k->setRotation(rotation());
	  k->setText(text());

	  k->run();

	  if(!k->cancelled())
	    setText(k->text());

	  delete k;
	}
    }
}
