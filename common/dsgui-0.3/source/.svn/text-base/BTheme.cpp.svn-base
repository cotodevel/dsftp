// BTheme.cpp (this is -*- C++ -*-)
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
#include <stdlib.h>

/* my includes */
#include "BTheme.h"
#include "BFont.h"
#include "BFontManager.h"

inline uint16 coladd(uint16 col1, uint16 col2)
{
  uint8 red = (col1 & 0x7c00) >> 10;
  uint8 green = (col1 & 0x3e0) >> 5;
  uint8 blue = col1 & 0x1f;
  red += (col2 & 0x7c00) >> 10;
  green += (col2 & 0x3e0) >> 5;
  blue += col2 & 0x1f;
  if(red > 31) red = 31;
  if(green > 31) green = 31;
  if(blue > 31) blue = 31;
  return RGB15(red, green, blue) | BIT(15);
}

inline uint16 coldel(uint16 col1, uint16 col2)
{
  int8 red = (col1 & 0x7c00) >> 10;
  int8 green = (col1 & 0x3e0) >> 5;
  int8 blue = col1 & 0x1f;
  red -= (col2 & 0x7c00) >> 10;
  green -= (col2 & 0x3e0) >> 5;
  blue -= col2 & 0x1f;
  if(red < 0) red = 0;
  if(green < 0) green = 0;
  if(blue < 0) blue = 0;
  return RGB15(red, green, blue) | BIT(15);
}

BTheme*
BTheme::currentTheme()
{
  if(!current) current = new BSimpleTheme;
  return current;
}

BTheme* BTheme::current = NULL;

BSimpleTheme::BSimpleTheme()
{
#if 0
  BMemFile fontfile1((u32)Bitstream_Vera_Sans_Roman_10_bin,
		    Bitstream_Vera_Sans_Roman_10_bin_size);
  _font = new BFont(fontfile1);

  BMemFile fontfile2((u32)Bitstream_Vera_Sans_Roman_8_bin,
		    Bitstream_Vera_Sans_Roman_8_bin_size);
  _smallFont = new BFont(fontfile2);

  BMemFile fontfile3((u32)Bitstream_Vera_Sans_Roman_12_bin,
		    Bitstream_Vera_Sans_Roman_12_bin_size);
  _bigFont = new BFont(fontfile3);

  BFontManager::get()->addFont(_font);
  BFontManager::get()->addFont(_smallFont);
  BFontManager::get()->addFont(_bigFont);
#endif
}

BSimpleTheme::~BSimpleTheme()
{
#if 0
  delete _font;
  delete _smallFont;
  delete _bigFont;
#endif
}

BRect
BSimpleTheme::contentRectForWidget(const BWidget& widget)
{
  unsigned int hpad = widget.hPadding();
  unsigned int vpad = widget.vPadding();

  BRect rect = widget.transformDown(widget.frame(), false);
  rect.pt = BPoint(0, 0);

  rect = rect.insetRect(hpad, hpad, vpad, vpad);
  if(widget.reliefType() == BWidget::RELIEF_NONE)
    return rect;
  return rect.insetRect(1, 1, 1, 1);
}

void
BSimpleTheme::drawWidgetBackground(BImage& img, BWidget& widget)
{
  BRect frame = widget.frame();
  if(widget.parent())
    frame = widget.parent()->transformUp(frame);

  uint16* buf = (uint16*)img.bytes();
  int w = img.width(), h = img.height();

  char bgalpha = widget.backgroundAlpha();
  uint16 dest = widget.backgroundColor();

  if(widget.state() == BWidget::STATE_DISABLED)
    dest = coldel(dest, RGB15(5, 5, 5));
  else if(widget.state() == BWidget::STATE_SELECTED)
    dest = coldel(dest, RGB15(5, 5, 0));


  for(int r=frame.pt.y; r<(int)(frame.pt.y+frame.sz.height); r++)
    for(int c=frame.pt.x; c<(int)(frame.pt.x+frame.sz.width); c++)
      {
	if(r < 0 || c < 0 || r >= h || c >= w) continue;

	uint16& src = buf[r*w+c];

	if(bgalpha < 31)
	  src = alphablend(src, dest, bgalpha);
	else
	  src = dest;
      }
}

void
BSimpleTheme::drawWidgetFrame(BImage& img, BWidget& widget)
{
  if(widget.reliefType() == BWidget::RELIEF_NONE) return;
  
  BRect frame = widget.frame();
  if(widget.parent())
    frame = widget.parent()->transformUp(frame);
  
  uint16 col1 = RGB15(0, 0, 0) | 1<<15;
  uint16 col2 = RGB15(0, 0, 0) | 1<<15;
  switch(widget.reliefType())
    {
    case BWidget::RELIEF_NONE:
      break;
      
    case BWidget::RELIEF_SIMPLE:
      frame.outline(img, widget.textColor());
      return;
      break;
    case BWidget::RELIEF_RAISED:
      col1 = RGB15(31, 31, 31) | 1<<15;
      col2 = RGB15(0, 0, 0) | 1<<15;
      break;
    case BWidget::RELIEF_SUNKEN:
      col1 = RGB15(0, 0, 0) | 1<<15;
      col2 = RGB15(31, 31, 31) | 1<<15;
      break;
    }

  BPoint p1 = frame.pt;
  BPoint p2(frame.pt.x + frame.sz.width-1, frame.pt.y);
  BPoint p3(frame.pt.x + frame.sz.width-1, frame.pt.y + frame.sz.height-1);
  BPoint p4(frame.pt.x, frame.pt.y + frame.sz.height-1);

  switch(widget.accumulatedRotation())
    {
    case BWidget::ROT_0:
      img.drawLine(p1, p2, col1);
      img.drawLine(p2, p3, col2);
      img.drawLine(p3, p4, col2);
      img.drawLine(p4, p1, col1);
      break;
    case BWidget::ROT_90:
      img.drawLine(p1, p2, col1);
      img.drawLine(p2, p3, col1);
      img.drawLine(p3, p4, col2);
      img.drawLine(p4, p1, col2);
      break;
    case BWidget::ROT_180:
      img.drawLine(p1, p2, col2);
      img.drawLine(p2, p3, col1);
      img.drawLine(p3, p4, col1);
      img.drawLine(p4, p1, col2);
      break;
    case BWidget::ROT_270:
      img.drawLine(p1, p2, col2);
      img.drawLine(p2, p3, col2);
      img.drawLine(p3, p4, col1);
      img.drawLine(p4, p1, col1);
      break;
    }
}

void BSimpleTheme::drawButton(BImage& img, BWidget& w)
{
  uint16 highlight, shadow1, shadow2;
  uint16 background;

  BRect frame = w.frame();
  if(w.parent())
    frame = w.parent()->transformUp(frame);

  if(w.state() == BWidget::STATE_DISABLED)
    {
      highlight = RGB15(15, 15, 15) | BIT(15);
      shadow1 = RGB15(15, 15, 15) | BIT(15);
      shadow2 = RGB15(15, 15, 15) | BIT(15);
    }
  else
    {
      highlight = RGB15(31, 31, 31) | BIT(15);
      shadow1 = RGB15(7, 7, 7) | BIT(15);
      shadow2 = RGB15(0, 0, 0) | BIT(15);
    }

  if(w.state() == BWidget::STATE_SELECTED)
    background = RGB15(31, 31, 31) | BIT(15);
  else
    background = RGB15(15, 15, 15) | BIT(15);

  frame.fill(img, background);
  frame = w.transformDown(w.frame(), false);

  if(w.state() == BWidget::STATE_ACTIVE)
    {
      BPoint tl1(0, 0), tr1(frame.sz.width-1, 0),
	tl2(1, 1), tr2(frame.sz.width-2, 1),
	bl1(0, frame.sz.height-1), br1(frame.sz.width-1, frame.sz.height-1),
	bl2(1, frame.sz.height-2),
	bl3(1, frame.sz.height-1), tr3(frame.sz.width-1, 1);
      tl1 = w.transformUp(tl1); 
      tr1 = w.transformUp(tr1); 
      bl1 = w.transformUp(bl1); 
      br1 = w.transformUp(br1); 
      tl2 = w.transformUp(tl2); 
      tr2 = w.transformUp(tr2); 
      bl2 = w.transformUp(bl2);
      tr3 = w.transformUp(tr3); 
      bl3 = w.transformUp(bl3); 

      BLine(tl1, tr1).draw(img, shadow2);
      BLine(bl1, tl1).draw(img, shadow2);
      BLine(tl2, tr2).draw(img, shadow1);
      BLine(bl2, tl2).draw(img, shadow1);
      BLine(bl3, br1).draw(img, highlight);
      BLine(br1, tr3).draw(img, highlight);
    }
  else
    {
      BPoint tl1(0, 0), tr1(frame.sz.width-1, 0),
	tr2(frame.sz.width-2, 1),
	bl1(0, frame.sz.height-1), br1(frame.sz.width-1, frame.sz.height-1),
	bl2(1, frame.sz.height-2), br2(frame.sz.width-2, frame.sz.height-2);
      tl1 = w.transformUp(tl1); 
      tr1 = w.transformUp(tr1); 
      bl1 = w.transformUp(bl1); 
      br1 = w.transformUp(br1); 
      tr2 = w.transformUp(tr2); 
      bl2 = w.transformUp(bl2); 
      br2 = w.transformUp(br2); 

      BLine(tl1, tr1).draw(img, highlight);
      BLine(bl1, tl1).draw(img, highlight);
      BLine(bl1, br1).draw(img, shadow2);
      BLine(br1, tr1).draw(img, shadow2);
      BLine(bl2, br2).draw(img, shadow1);
      BLine(br2, tr2).draw(img, shadow1);
    }
}

void
BSimpleTheme::drawSelectionRect(BImage& img, const BRect& rect,
				const BWidget& w)
{
  if(w.state() == BWidget::STATE_SELECTED)
    rect.fill(img, RGB15(5, 5, 15) | BIT(15));
  else
    rect.fill(img, RGB15(7, 7, 7) | BIT(15));
}

void BSimpleTheme::drawScroller(BImage& img, BScroller& w)
{
  BRect frame = w.frame();
  if(w.parent())
    frame = w.parent()->transformUp(frame);

  // draw scroller background
  frame.fill(img, RGB15(10, 10, 10) | BIT(15));
  frame.outline(img, RGB15(0, 0, 0) | BIT(15));

  frame = w.transformDown(w.frame(), false);

  float thumbheight = 1;
  float thumbpos = 0;
  if(w.maximum() != w.minimum())
    {
      thumbheight = ((float)w.amountRepresentedByThumb() /
		     (float)(w.maximum() - w.minimum()));
      thumbpos = ((float)w.position() /
		  (float)(w.maximum() - w.minimum()));
    }
      
  int tp=0, th=0; BRect thumb;

  switch(w.orientation())
    {
    case BScroller::ORIENT_VERTICAL:
      tp = (int)((frame.sz.height - 12)*thumbpos);
      th = (int)((frame.sz.height - 12)*thumbheight);
      if(tp+th > frame.sz.height)
	th = frame.sz.height-tp-1;
  
      thumb = BRect(0, tp+1, frame.sz.width-1, th);
      break;

    case BScroller::ORIENT_HORIZONTAL:
      tp = (int)((frame.sz.width - 12)*thumbpos);
      th = (int)((frame.sz.width - 12)*thumbheight);
      if(tp+th > frame.sz.width)
	th = frame.sz.width-tp-1;
  
      thumb = BRect(tp+1, 0, th-1, frame.sz.height);
      break;
    }
  
  thumb = thumb.insetRect(1, 1, 1, 1);
  w.transformUp(thumb).fill(img, RGB15(15, 15, 15) | BIT(15));

  BPoint tl1, tr1, bl1, br1, tr2, bl2, br2;
  switch(w.orientation())
    {
    case BScroller::ORIENT_VERTICAL:
      tl1 = w.transformUp(BPoint(1, tp)); 
      tr1 = w.transformUp(BPoint(frame.sz.width-1, tp)); 
      bl1 = w.transformUp(BPoint(1, tp+th)); 
      br1 = w.transformUp(BPoint(frame.sz.width-1, tp+th)); 
      tr2 = w.transformUp(BPoint(frame.sz.width-2, tp+1)); 
      bl2 = w.transformUp(BPoint(2, tp+th-1));
      br2 = w.transformUp(BPoint(frame.sz.width-2, tp+th-1));
      break;

    case BScroller::ORIENT_HORIZONTAL:
      tl1 = w.transformUp(BPoint(tp, 1)); 
      tr1 = w.transformUp(BPoint(tp+th, 1)); 
      bl1 = w.transformUp(BPoint(tp, frame.sz.height-1));
      br1 = w.transformUp(BPoint(tp+th, frame.sz.height-1));
      tr2 = w.transformUp(BPoint(tp+th-1, 2)); 
      bl2 = w.transformUp(BPoint(tp+1, frame.sz.height-2));
      br2 = w.transformUp(BPoint(tp+th-1, frame.sz.height-2));
      break;
    }

  uint16 highlight = RGB15(31, 31, 31)|BIT(15);
  uint16 shadow1 = RGB15(7, 7, 7)|BIT(15);
  uint16 shadow2 = RGB15(0, 0, 0)|BIT(15);
  BLine(tl1, tr1).draw(img, highlight);
  BLine(bl1, tl1).draw(img, highlight);
  BLine(bl1, br1).draw(img, shadow2);
  BLine(br1, tr1).draw(img, shadow2);
  BLine(bl2, br2).draw(img, shadow1);
  BLine(br2, tr2).draw(img, shadow1);
}

int
BSimpleTheme::scrollerHitTest(BScroller& w, const BEvent& event)
{
  BRect frame = w.transformDown(w.frame(), false);
  frame.pt = BPoint(0, 0);

  BPoint evtpt = w.transformDown(BPoint(event.touch.x, event.touch.y));
  
  if(!frame.containsPoint(evtpt)) return -2;

  float thumbheight = ((float)w.amountRepresentedByThumb() /
		       (float)(w.maximum() - w.minimum()));
  float thumbpos = ((float)w.position() /
		    (float)(w.maximum() - w.minimum() -
			    w.amountRepresentedByThumb()));

  int tp, th;
  
  switch(w.orientation())
    {
    case BScroller::ORIENT_VERTICAL:
      tp = (int)((frame.sz.height - 12)*thumbpos);
      th = (int)((frame.sz.height - 12)*thumbheight);
      
      if(evtpt.y < tp) return -1;
      else if(evtpt.y > tp+th) return 1;
      else return 0;
      break;
      
    case BScroller::ORIENT_HORIZONTAL:
      tp = (int)((frame.sz.width - 12)*thumbpos);
      th = (int)((frame.sz.width - 12)*thumbheight);
      
      if(evtpt.x < tp) return -1;
      else if(evtpt.x > tp+th) return 1;
      else return 0;
      break;
    }

  return -3;
}

