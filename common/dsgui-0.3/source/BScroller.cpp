// BScroller.cpp (this is -*- C++ -*-)
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
#include "BScrollable.h"
#include "BScroller.h"
#include "BFont.h"
#include "BTheme.h"

BScroller::BScroller(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _orient = ORIENT_VERTICAL;
  lastpos = -2;
  _stepIncr = 1;
  _pageIncr = 10;
  _position = 0;
  _minimum = 0;
  _maximum = 100;
  _amountRepr = 10;
}

void BScroller::setOrientation(Orientation orient)
{
  _orient = orient;
  setNeedsRedraw(true);
}

void BScroller::setStepIncrement(unsigned int stepIncr)
{
  _stepIncr = stepIncr;
}

void BScroller::setPageIncrement(unsigned int pageIncr)
{
  _pageIncr = pageIncr;
}

void BScroller::setPosition(unsigned int position)
{
  _position = position;
  setNeedsRedraw(true);
}

void BScroller::setMinimum(unsigned int minimum)
{
  _minimum = minimum;
  setNeedsRedraw(true);
}

void BScroller::setMaximum(unsigned int maximum)
{
  _maximum = maximum;
  setNeedsRedraw(true);
}

void BScroller::setAmountRepresentedByThumb(unsigned int amountRepr)
{
  _amountRepr = amountRepr;
  setNeedsRedraw(true);
}

void BScroller::draw(BImage& img)
{
  if(!_needsRedraw || !isShown()) return;
  
  BWidget::draw(img);

  BTheme::currentTheme()->drawScroller(img, *this);

  if(_debug)
    {
      char debugstr[255];
      BRect frame = transformDown(_frame);
      sprintf(debugstr, "%d (%d,%d) (%d,%d,%d,%d)",
	      lastpos, lastpoint.x, lastpoint.y, frame.pt.x, frame.pt.y,
	      frame.sz.width, frame.sz.height);
      BFont* font = BTheme::currentTheme()->defaultFont();
      font->drawString(img, transformUp(BPoint(0, 0)), debugstr,
		       RGB15(0, 0, 0)|BIT(15), BRect(0, 0, 256, 192));
    }
}

void BScroller::handleEvent(const BEvent& event)
{
  if(!isShown()) return;
  
  BRect frame = _frame;
  if(_parent) frame = _parent->transformUp(frame);

  lastpoint = transformDown(BPoint(event.touch.x, event.touch.y));

  if(!frame.containsPoint(BPoint(event.touch.x, event.touch.y))) return;

  if(event.type == EVT_TOUCHDOWN)
    {
      lastpos = BTheme::currentTheme()->scrollerHitTest(*this, event);
      switch(lastpos)
	{
	case -1:
	  _position -= _pageIncr;
	  if(_position < _minimum) _position = _minimum;
	  if(_scrollable) _scrollable->scrollToPosition(this, _position);
	  break;
	case 1:
	  _position += _pageIncr;
	  if(_position > _maximum) _position = _maximum;
	  if(_scrollable) _scrollable->scrollToPosition(this, _position);
	  break;
	default:
	  return;
	}
    }

  setNeedsRedraw(true);
}

void
BScroller::setScrollable(BScrollable *scrollable)
{
  _scrollable = scrollable;
}

#if BScroller_test
int main(int argc, char **argv)
{
}
#endif /* BScroller_test */
