// BWidget.cpp (this is -*- C++ -*-)
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
#include "BWidget.h"
#include "BTheme.h"
#include "BFont.h"

BWidget::BWidget(BWidget* parent, const BRect& frame):
  _frame(frame)
{
  _screen = NULL;
  
  _parent = parent;
  if(_parent != NULL)
    {
      _parent->addChild(this);
      _screen = _parent->screen();
    }
  
  _relief = RELIEF_NONE;
  _bgcolor = RGB15(31, 31, 31) | BIT(15);
  _bgalpha = 31;
  _font = BTheme::currentTheme()->defaultFont();
  _textcolor = BTheme::currentTheme()->textColor();
  _selectedtextcolor = BTheme::currentTheme()->selectedTextColor();
  _widgetalpha = 31;
  _needsRedraw = true;
  _shown = true;
  _vpadding = 0;
  _hpadding = 0;
  _state = STATE_ENABLED;
  _tdir = DIR_LEFTTORIGHT;
  _rot = ROT_0;
  _align = ALIGN_CENTER;
  _debug = false;
  _tag = 0;

  _deleteChildren = true;
}

BWidget::~BWidget()
{
  if(_parent) _parent->removeChild(this);

  if(_deleteChildren)
    {
      deleteChildren();
    }
}

void
BWidget::setFrame(const BRect& frame)
{
  _frame = frame;
  setNeedsRedraw(true);
}

BWidget::TextDirection
BWidget::textDirectionRelativeToScreen() const
{
  return (TextDirection)((accumulatedRotation() + _tdir) % 4);
}

void
BWidget::setRotation(Rotation rot)
{
  _rot = (Rotation)(rot % 4);
  _needsRedraw = true;
}

BWidget::Rotation
BWidget::accumulatedRotation() const
{
  if(_parent)
    return (Rotation)((_parent->accumulatedRotation() + _rot) % 4);
  return _rot;
}

void
BWidget::setTextAlignment(TextAlignment align)
{
  _align = align;
  setNeedsRedraw(true);
}

void
BWidget::setReliefType(ReliefType relief)
{
  _relief = relief;
  setNeedsRedraw(true);
}

void
BWidget::setVPadding(unsigned int padding)
{
  _vpadding = padding;
  setNeedsRedraw(true);
}

void
BWidget::setHPadding(unsigned int padding)
{
  _hpadding = padding;
  setNeedsRedraw(true);
}

void BWidget::setDrawsDebugStuff(bool debug)
{
  _debug = debug;
  setNeedsRedraw(true);
}

void BWidget::show() 
{
  _shown = true;
  setNeedsRedraw(true);
}

void
BWidget::setBackgroundColor(uint16 color, bool recursive)
{
  _bgcolor = color;
  if(recursive)
    {
      for(unsigned i=0; i<_children.size(); i++)
	_children[i]->setBackgroundColor(color);
    }
  setNeedsRedraw(true);
}

void
BWidget::setBackgroundAlpha(uint8 alpha)
{
  _bgalpha = alpha;
  setNeedsRedraw(true);
}

void
BWidget::setWidgetAlpha(uint8 alpha)
{
  _widgetalpha = alpha;
  setNeedsRedraw(true);
}

void
BWidget::setFont(BFont* font)
{
  _font = font;
  setNeedsRedraw(true);
}

void
BWidget::setTextColor(uint16 color, bool recursive)
{
  _textcolor = color;
  if(recursive)
    {
      for(unsigned i=0; i<_children.size(); i++)
	_children[i]->setTextColor(color, true);
    }
  setNeedsRedraw(true);
}


void
BWidget::setSelectedTextColor(uint16 color)
{
  _selectedtextcolor = color;
  setNeedsRedraw(true);
}

void
BWidget::draw(BImage& img)
{
  if(!isShown()) return;

  if(_needsRedraw)
    {
      BTheme::currentTheme()->drawWidgetBackground(img, *this);
      BTheme::currentTheme()->drawWidgetFrame(img, *this);
      
      if(_debug)
	{
	  BRect rect = BTheme::currentTheme()->contentRectForWidget(*this);
	  for(int x=0; x<rect.sz.width; x+=5)
	    for(int y=0; y<rect.sz.height; y+=5)
	      {
		BPoint pt = transformUp(BPoint(x, y));
		if(x%10 == 0 && y%10 == 0)
		  img.drawPixel(pt, RGB15(15, 15, 15)|BIT(15));
		else
		  img.drawPixel(pt, RGB15(25, 25, 25)|BIT(15));
	      }
      
	  img.drawLine(transformUp(BPoint(0, 0)),
		       transformUp(BPoint(10, 0)),
		       RGB15(31, 0, 0)|BIT(15));
	  img.drawLine(transformUp(BPoint(0, 0)),
		       transformUp(BPoint(0, 10)),
		       RGB15(0, 31, 0)|BIT(15));
      
	}
    }

  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    if((*iter)->needsRedraw())
      (*iter)->draw(img);

  setNeedsRedraw(false);
}

bool
BWidget::needsRedraw()
{
  if(!_shown) return false;
  if(_needsRedraw) return true;
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    if((*iter)->needsRedraw()) return true;
  return false;
}

void
BWidget::setNeedsRedraw(bool needs)
{
  _needsRedraw = needs;
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    (*iter)->setNeedsRedraw(needs);
}

void
BWidget::addChild(BWidget* w)
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    if(*iter == w) return;
  _children.push_back(w);
}

void
BWidget::removeChild(BWidget* w)
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      if(*iter == w)
	{
	  _children.erase(iter);
	  return;
	}
    }
}

void
BWidget::handleEvent(const BEvent& event)
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      if((*iter)->acceptsInput())
	(*iter)->handleEvent(event);
    }
}

void
BWidget::deleteChildren()
{
  for(unsigned i=0; i<_children.size(); i++)
    delete _children[i];
  _children.clear();
}

BScreen*
BWidget::screen()
{
  if(_screen) return _screen;
  if(_parent) return _parent->screen();
  return NULL;
}

void
BWidget::setScreen(BScreen* screen)
{
  _screen = screen;
}
