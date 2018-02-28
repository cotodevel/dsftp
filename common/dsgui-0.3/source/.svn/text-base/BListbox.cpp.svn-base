// BListbox.cpp (this is -*- C++ -*-)
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
#include "BListbox.h"
#include "BTheme.h"
#include "BMessagebox.h"
#include "BStringUtils.h"

BListbox::BListbox(BWidget* parent, const BRect &frame):
  BScrollable(parent, frame)
{
  _firstIndex = 0;
  _selectedIndex = 0;
  _spacing = 0;
  _deleg = NULL;
  _drawsSelection = true;
}

void
BListbox::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

void
BListbox::addEntry(const Entry &entry)
{
  _entries.push_back(entry);
  setNeedsRedraw(true);
}

void
BListbox::clear()
{
  _entries.clear();
  _firstIndex = 0;
  _selectedIndex = 0;
  setNeedsRedraw(true);
}

void
BListbox::setEntrySpacing(int spacing)
{
  _spacing = spacing;
  setNeedsRedraw(true);
}

int
BListbox::entrySpacing()
{
  return _spacing;
}

void
BListbox::draw(BImage &img)
{
  if(!isShown() || !_needsRedraw)
    return;
  
  BWidget::draw(img);

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  
  BPoint p1(clip.pt.x, clip.pt.y);

  for(unsigned int i=_firstIndex;
      i < _entries.size() && p1.y-_spacing+_font->height() < clip.sz.height;
      i++)
    {
      BPoint p2(p1.x+clip.sz.width-1, p1.y+_font->height()-1);
      BRect r(p1, p2);

      if(i == _selectedIndex && _drawsSelection)
	BTheme::currentTheme()->drawSelectionRect(img, transformUp(r), *this);

      u16 color = _entries[i].color;
      if(color == 0)
	color = _textcolor;

      img.drawText(_font, _entries[i].title, -1, r, *this,
		   color);
      
      p1.y += _font->height() + _spacing;
    }
}

void
BListbox::updateScroller(BScroller *scroller)
{
  if(_hScroller && scroller == _hScroller)
    {
      _hScroller->setStepIncrement(1);
      _hScroller->setPageIncrement(1);
      _hScroller->setPosition(0);
      _hScroller->setMinimum(0);
      _hScroller->setMaximum(0);
      _hScroller->setAmountRepresentedByThumb(0);
    }

  if(_vScroller && scroller == _vScroller)
    {
      _vScroller->setStepIncrement(_font->height());
      _vScroller->setPageIncrement(_frame.sz.height - _font->height());
      _vScroller->setPosition(_firstIndex*_font->height());
      _vScroller->setMinimum(0);
      _vScroller->setMaximum(_font->height()*_entries.size());
      _vScroller->setAmountRepresentedByThumb(_frame.sz.height);
    }
}

void
BListbox::scrollToPosition(BScroller *scroller,
			   unsigned int position)
{
  if(scroller != _vScroller) return;
  
  int first = position / _font->height();
  if(first < 0)
    first = 0;
  else if(first >= (int)(_entries.size()))
    first = _entries.size()-1;
  _firstIndex = first;

  updateScroller(scroller);

  setNeedsRedraw(true);
}

void
BListbox::handleEvent(const BEvent& event)
{
  if(_state == STATE_DISABLED || !isShown()) return;

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  
  switch(event.type)
    {
    case EVT_TOUCHDOWN:
      {
	BPoint point = transformDown(BPoint(event.touch.x, event.touch.y));
	if(!clip.containsPoint(point)) return;
	
	int index = (point.y-clip.pt.y)/(_font->height() + _spacing);

	index += _firstIndex;
	if(index >= 0 && index < (int)_entries.size())
	  {
	    if((int)_selectedIndex == index) // event.timestamp close to last
	      {
		if(_deleg)
		  _deleg->onEntryDoubleClick(this, _entries[index]);
	      }
	    else
	      {
		_selectedIndex = index;
		if(_deleg)
		  _deleg->onEntryClick(this, _entries[index]);
		setNeedsRedraw(true);
	      }
	  }
      }
      
      break;
    default:
      break;
    }
  
  updateScroller(_vScroller);
}

const BListbox::Entry&
BListbox::selectedEntry()
{
  return _entries[_selectedIndex];
}

void
BListbox::selectEntry(const std::string& title)
{
  for(unsigned int i=0; i<_entries.size(); i++)
    {
      if(_entries[i].title == title)
	{
	  selectEntry(i);
	  break;
	}
    }
}

void
BListbox::selectEntry(int index)
{
  if(index < 0)
    index = 0;
  else if(index >= (int)_entries.size())
    index = _entries.size()-1;

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  
  _selectedIndex = index;
  _firstIndex = 0;
  while(_selectedIndex - _firstIndex >
	(int)(clip.sz.height / _font->height()))
    _firstIndex += clip.sz.height / _font->height();
  updateScroller(_vScroller);
  setNeedsRedraw(true);
}

void
BListbox::selectEntryWithTag(int tag)
{
  for(unsigned int i=0; i<_entries.size(); i++)
    {
      if(_entries[i].tag == tag)
	{
	  selectEntry(i);
	  break;
	}
    }
}

#if BListbox_test
int main(int argc, char **argv)
{
}
#endif /* BListbox_test */
