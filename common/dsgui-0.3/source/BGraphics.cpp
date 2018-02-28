// BGraphics.cpp (this is -*- C++ -*-)
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
#include "BGraphics.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif

void BLine::draw(BImage& img, uint16 color) const
{
  int x1 = p0.x; int y1 = p0.y; int x2 = p1.x; int y2 = p1.y;

  int dx, dy, dx2, dy2, ix, iy, err, i;

  uint16* bytes = (uint16*)img.bytes();
  int w = img.width();

  // identify the first pixel
  uint16 *ptr_vid = bytes + x1 + (y1 * w);

  // difference between starting and ending points
  dx = x2 - x1;
  dy = y2 - y1;

  // calculate direction of the vector and store in ix and iy
  if (dx >= 0)
    ix = 1;
  else
    {
      ix = -1;
      dx = abs(dx);
    }

  if (dy >= 0)
    iy = w;
  else
    {
      iy = -w;
      dy = abs(dy);
    }

  // scale deltas and store in dx2 and dy2
  dx2 = dx * 2;
  dy2 = dy * 2;

  if (dx > dy)	// dx is the major axis
    {
      // initialize the error term
      err = dy2 - dx;
      
      for (i = 0; i <= dx; i++)
	{
	  *ptr_vid = color;
	  if (err >= 0)
	    {
	      err -= dx2;
	      ptr_vid += iy;
	    }
	  err += dy2;
	  ptr_vid += ix;
	}
    }
  else 		// dy is the major axis
    {
      // initialize the error term
      err = dx2 - dy;
      
      for (i = 0; i <= dy; i++)
	{
	  *ptr_vid = color;
	  if (err >= 0)
	    {
	      err -= dy2;
	      ptr_vid += ix;
	    }
	  err += dx2;
	  ptr_vid += iy;
	}
    }
}

BRect::BRect(int x, int y, unsigned int width, unsigned int height)
{
  this->pt.x = x; this->pt.y = y;
  this->sz.width = width; this->sz.height = height;
}

BRect::BRect(const BPoint& point, const BSize& size)
{
  pt = point;
  sz = size;
}

BRect::BRect(const BPoint& p1, const BPoint& p2)
{
  pt.x = MIN(p1.x, p2.x);
  pt.y = MIN(p1.y, p2.y);
  sz.width = abs(p1.x-p2.x) + 1;
  sz.height = abs(p1.y-p2.y) + 1;
}

BRect
BRect::insetRect(int left, int right, int top, int bottom) const
{
  return BRect(pt.x + left, pt.y + top,
	       sz.width - left - right, sz.height - top - bottom);
}

void
BRect::outline(BImage& img, uint16 color) const
{
  BPoint p2(pt.x+sz.width, pt.y);
  BPoint p3(pt.x+sz.width, pt.y+sz.height);
  BPoint p4(pt.x, pt.y+sz.height);
  img.drawLine(pt, p2, color);
  img.drawLine(p2, p3, color);
  img.drawLine(p3, p4, color);
  img.drawLine(p4, pt, color);
}

void
BRect::fill(BImage& img, uint16 color) const
{
  uint16* bytes = (uint16*)img.bytes();
  int w = img.width();
  for(int y = pt.y; y < pt.y + sz.height; y++)
    for(int x = pt.x; x < pt.x + sz.width; x++)
      bytes[y*w + x] = color;
}

bool
BRect::containsPoint(const BPoint& p) const
{
  return (p.x >= pt.x && p.x < pt.x + sz.width &&
	  p.y >= pt.y && p.y < pt.y + sz.height);
}
