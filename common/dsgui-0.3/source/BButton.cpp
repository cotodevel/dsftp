// BButton.cpp (this is -*- C++ -*-)
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
#include "BButton.h"
#include "BTheme.h"

BButton::BButton(BWidget* parent, const BRect& frame,
		 ButtonType type):
  BLabel(parent, frame)
{
  _deleg = NULL;
  _touchdown = false;
  _value = false;
  _type = type;
  setVPadding(3);
  setHPadding(3);
}

void BButton::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

void BButton::setType(ButtonType type)
{
  _type = type;
  setNeedsRedraw(true);
}

void BButton::setValue(bool value)
{
  _value = value;
  if(_type == BT_TOGGLE)
    {
      if(value)
	setState(STATE_ACTIVE);
      else
	setState(STATE_ENABLED);
    }
  setNeedsRedraw(true);
}

void BButton::draw(BImage& img)
{
  if(!isShown())
    return;
  
  BTheme::currentTheme()->drawButton(img, *this);
  drawText(img);
  setNeedsRedraw(false);
}

void
BButton::handleEvent(const BEvent& event)
{
  if(_state == STATE_DISABLED || !isShown()) return;

  BRect rect = _frame;
  if(_parent) rect = _parent->transformUp(rect);
  
  switch(event.type)
    {
    case EVT_TOUCHDOWN:
      if(rect.containsPoint(BPoint(event.touch.x, event.touch.y)))
	{
	  _touchdown = true;
	  setState(STATE_ACTIVE);
	}
      break;

    case EVT_TOUCHDRAG:
      if(_touchdown)
	{
	  if(!rect.containsPoint(BPoint(event.touch.x, event.touch.y)))
	    setState(STATE_ENABLED);
	  else
	    setState(STATE_ACTIVE);
	}
      break;

    case EVT_TOUCHUP:
      if(_touchdown &&
	 rect.containsPoint(BPoint(event.touch.x, event.touch.y)))
	{
	  setState(STATE_ACTIVE);
	  if(_type == BT_TOGGLE)
	    {
	      if(_value == false)
		{
		  setValue(true);
		  if(_deleg) _deleg->onButtonClick(this);
		  setState(STATE_ACTIVE);
		}
	      else
		{
		  setValue(false);
		  if(_deleg) _deleg->onButtonClick(this);
		  setState(STATE_ENABLED);
		}
	    }
	  else
	    {
	      if(_deleg) _deleg->onButtonClick(this);
	      setValue(true);
	      setState(STATE_ENABLED);
	    }
	}

      _touchdown = false;
      break;

    default:
      // do nothing
      break;
    }
}

#if BButton_test
int main(int argc, char **argv)
{
}
#endif /* BButton_test */
