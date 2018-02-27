// BLogWidget.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

#define _GLIBCXX_USE_CXX11_ABI 0

/* system includes */
#include <nds/jtypes.h>
#include <BFont.h>
#include <BTheme.h>

/* my includes */
#include "BLogWidget.h"

BLogWidget::BLogWidget(BWidget* parent, const BRect& frame):
  BScrollable(parent, frame)
{
  maxlines = 100;
  firstline = 0;
  autoscroll = true;
  numlines = 0;
}

void
BLogWidget::draw(BImage& img)
{
  if(!_needsRedraw) return;
  BWidget::draw(img);
  if(!_font) return;

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  unsigned int fh = _font->height();

  switch(textDirection())
    {
    case BWidget::DIR_LEFTTORIGHT:
    case BWidget::DIR_RIGHTTOLEFT:
      numlines = clip.sz.height/fh;
      break;
    case BWidget::DIR_TOPTOBOTTOM:
    case BWidget::DIR_BOTTOMTOTOP:
      numlines = clip.sz.width/fh;
      break;
    }
  updateScroller(_vScroller);

  clip = clip.insetRect(2, 2, 0, 0);

  BPoint pt = clip.pt;
  clip = _frame;
  
  for(unsigned int i=firstline;
      i<lines.size() && i < firstline + numlines;
      i++)
    {
      uint16 color = RGB15(0, 0, 0);
      switch(lines[i].level)
	{
	case LOG_FATAL:
	case LOG_CRIT:
	case LOG_ERROR:
	  color = RGB15(20, 0, 0);
	  break;
	case LOG_WARN:
	  color = RGB15(15, 15, 0);
	  break;
	case LOG_INFO:
	  color = RGB15(0, 0, 0);
	  break;
	case LOG_BLAH:
	  color = RGB15(8, 8, 8);
	  break;
	}
      
      _font->drawString(img, transformUp(pt),
			lines[i].text, color, clip,
			textDirectionRelativeToScreen());
      pt.y += fh;
    }
}

void
BLogWidget::log(LogLevel level, const std::string& text)
{
  LogEntry entry = {level, text};
  lines.push_back(entry);
  while(lines.size() > maxlines) lines.pop_front();

  if(autoscroll)
    {
      if(lines.size() <= numlines)
	firstline = 0;
      else
	firstline = lines.size() - numlines;
    }

  updateScroller(_vScroller);
}

void
BLogWidget::setMaxLines(unsigned int maxlines)
{
  if(numlines > maxlines)
    this->maxlines = numlines;
  else
    this->maxlines = maxlines;
}

void
BLogWidget::setFirstLine(unsigned int line)
{
  if(line > lines.size() - numlines ||
     lines.size() <= numlines) return;
  firstline = line;

  updateScroller(_vScroller);
}

bool
BLogWidget::atEnd()
{
  if(lines.size() < numlines ||
     firstline == lines.size() - numlines)
    return true;
  return false;
}

void
BLogWidget::setAutoscroll(bool autoscroll)
{
  this->autoscroll = autoscroll;
}

void
BLogWidget::updateScroller(BScroller *scroller)
{
  if(_vScroller && scroller == _vScroller)
    {
      _vScroller->setStepIncrement(1);
      _vScroller->setPageIncrement((_frame.sz.height - 1) / _font->height());
      _vScroller->setPosition(firstline);
      _vScroller->setMinimum(0);
      _vScroller->setMaximum(lines.size());
      _vScroller->setAmountRepresentedByThumb(_frame.sz.height / _font->height());
    }
}

void
BLogWidget::scrollToPosition(BScroller *scroller,
			     unsigned int position)
{
  if(scroller != _vScroller ||
     position >= lines.size())
    return;
  firstline = position;
  updateScroller(scroller);
  setNeedsRedraw(true);
}
