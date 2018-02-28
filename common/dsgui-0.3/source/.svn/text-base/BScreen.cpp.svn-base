// BScreen.cpp (this is -*- C++ -*-)
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
#include "BScreen.h"

BScreen::BScreen(BImage* image)
{
  _img = image;
  _freeImg = false;
  _touch = false;
  _dialog = NULL;
  _bg = RGB15(31, 31, 31)|BIT(15);
}

BScreen::BScreen(uint16* address, BSize size)
{
  _img = new BImage(size.width, size.height, (uint8*)address,
		    BImage::FMT_RGB15, false);
  _freeImg = true;
  _touch = false;
  _dialog = NULL;
  _bg = RGB15(31, 31, 31)|BIT(15);
}

BScreen::~BScreen()
{
  if(_freeImg) delete _img;
}

void
BScreen::setBackgroundColor(uint16 color)
{
  _bg = color | BIT(15);
}

void
BScreen::setReactsToTouch(bool reactsToTouch)
{
  _touch = reactsToTouch;
}

void
BScreen::addWidget(BWidget* widget)
{
  _widgets.push_back(widget);
  widget->setScreen(this);
}

void
BScreen::removeWidget(BWidget* widget)
{
  for(std::vector<BWidget*>::iterator iter = _widgets.begin();
      iter != _widgets.end(); iter++)
    {
      if(*iter == widget)
	{
	  _widgets.erase(iter);
	  widget->setScreen(NULL);
	  return;
	}
    }
}

void
BScreen::setNeedsRedrawOnAllWidgetsCoveredBy(const BRect& rect, bool needs)
{
  for(std::vector<BWidget*>::iterator iter = _widgets.begin();
      iter != _widgets.end(); iter++)
    {
      if((*iter)->frame().intersects(rect))
	(*iter)->setNeedsRedraw(needs);
    }
}


void
BScreen::clear()
{
  _img->fill(_bg);
}

void
BScreen::repaint(bool force)
{
  bool redrawDlg = false;
  for(std::vector<BWidget*>::iterator iter = _widgets.begin();
      iter != _widgets.end(); iter++)
    {
      if((*iter)->needsRedraw() || force || _forceNextRedraw)
	{
	  (*iter)->draw(*_img);
	  redrawDlg = true;
	}
    }

  if(_dialog && (redrawDlg || force || _forceNextRedraw || _dialog->needsRedraw()))
    _dialog->draw(*_img);


  _forceNextRedraw = false;
}

void
BScreen::handleEvent(const BEvent& event)
{
  if(event.type & EVT_TOUCHMASK && !_touch) return;
  if(_dialog && _dialog->acceptsInput())
    _dialog->handleEvent(event);
  else
    {
      for(std::vector<BWidget*>::iterator iter = _widgets.begin();
	  iter != _widgets.end(); iter++)
	{
	  if((*iter)->acceptsInput())
	    (*iter)->handleEvent(event);
	}
    }
}

void
BScreen::setDialog(BWidget* dialog)
{
  _dialog = dialog;
  if(_dialog != NULL)
    _dialog->setScreen(this);
  else
    _forceNextRedraw = true;
}

#if BScreen_test
int main(int argc, char **argv)
{
}
#endif /* BScreen_test */
