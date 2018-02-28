// BImage.cpp (this is -*- C++ -*-)
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
#include <string.h>
#include <stdlib.h>
#include <nds/arm9/video.h>

/* my includes */
#include "BImage.h"
#include "BFont.h"
#include "BWidget.h"

BImage::BImage(char** xpmData)
{
  long ncolors, chars_per_pixel;
  char *fmtString = xpmData[0];
  char *tmp = fmtString;
  int len = strlen(fmtString);

  _width = (int)strtol(tmp, &tmp, 10);
  tmp++; // skip space
  if(tmp - fmtString > len)
    return;

  _height = (int)strtol(tmp, &tmp, 10);
  tmp++; // skip space
  if(tmp - fmtString > len)
    return;

  ncolors = strtol(tmp, &tmp, 10);
  tmp++; // skip space
  if(tmp - fmtString > len)
    return;

  chars_per_pixel = strtol(tmp, &tmp, 10);
  if(tmp - fmtString != len)
    return;

  uint16* colortable = new uint16[ncolors];
  char **colornames = new char *[ncolors];
  for(int i=0; i<ncolors; i++)
    {
      char* colorline = xpmData[i+1];
      colornames[i] = colorline;
      char* color = colorline + chars_per_pixel + 3;
      if(color - colorline > (int)strlen(colorline))
	return;

      if(strcmp(color, "None") == 0)
	colortable[i] = 1<<15; 
      else if(color[0] == '#')
	{
	  long color24 = strtol(color+1, NULL, 16);
	  char r = (color24 & 0x00ff0000) >> 16;
	  char g = (color24 & 0x0000ff00) >> 8;
	  char b = (color24 & 0x000000ff);
	  r >>= 3;
	  g >>= 3;
	  b >>= 3;
	  colortable[i] = RGB15(r, g, b);
	}
      else
	return;
    }

  _image = new uint8[_width*_height*2];
  _format = FMT_RGB15;
  _freeImage = true;
  
  for(int row=0; row<_height; row++)
    {
      char* line = xpmData[1+ncolors+row];
      for(int col=0; col<_width; col++)
	{
	  char* colorname = &(line[col*chars_per_pixel]);
	  bool colorfound = false;
	  for(int i=0; i<ncolors; i++)
	    {
	      if(strncmp(colorname, colornames[i], chars_per_pixel) == 0)
		{
		  _image[row*_width+col] = colortable[i];
		  colorfound = true;
		  break;
		}
	    }

	  if(!colorfound)
	    return;
	}
    }

  delete colortable;
  delete colornames;
}

BImage::BImage(int width, int height,
	       ImageFormat format)
{
  _width = width;
  _height = height;

  if(format == FMT_RGB15)
    {
      _image = new uint8[width*height*2];
      memset(_image, 0xff, width*height*2);
    }
  else
    {
      _image = new uint8[width*height];
      memset(_image, 0xff, width*height);
    }

  _freeImage = true;
  _format = format;
}
  
BImage::BImage(int width, int height,
	       uint8* image, ImageFormat format, bool copy)
{
  if(copy)
    {
      if(format == FMT_RGB15)
	{
	  _image = new uint8[width*height*2];
	  memcpy(_image, image, width*height*2);
	}
      else
	{
	  _image = new uint8[width*height];
	  memcpy(_image, image, width*height);
	}
	
      _freeImage = true;
    }
  else
    {
      _image = image;
      _freeImage = false;
    }
  _format = format;
  _width = width;
  _height = height;
}

BImage::BImage(const BImage& img)
{
  _width = img._width;
  _height = img._height;
  _format = img._format;
  if(_format == FMT_RGB15)
    {
      _image = new uint8[_width*_height*2];
      memcpy(_image, img._image, _width*_height*2);
    }
  else
    {
      _image = new uint8[_width*_height];
      memcpy(_image, img._image, _width*_height);
    }
  _freeImage = true;
}

BImage::~BImage()
{
  if(_freeImage)
    delete _image;
}

void
BImage::operator=(const BImage& img)
{
  if(_freeImage) delete _image;
  
  _width = img._width;
  _height = img._height;
  _format = img._format;
  
  if(_format == FMT_RGB15)
    {
      _image = new uint8[_width*_height*2];
      memcpy(_image, img._image, _width*_height*2);
    }
  else
    {
      _image = new uint8[_width*_height];
      memcpy(_image, img._image, _width*_height);
    }
  _freeImage = true;
}

void BImage::blit(const BPoint& pt, uint16* screen)
{
  int _x = pt.x;
  int _y = pt.y;
  
  if(_x > SCREEN_WIDTH || _x + _width < 0 ||
     _y > SCREEN_HEIGHT || _y + _height < 0 ||
     _format != FMT_RGB15)
    // nothing to draw.
    return;

  int x = (_x < 0 ? 0 : _x);
  int y = (_y < 0 ? 0 : _y);
  int xoffs = 0, yoffs = 0;
  int xlen = _width, ylen = _height;
  if(_x<0)
    {
      xoffs = -_x;
      xlen -= xoffs;
    }
  if(_y<0)
    {
      yoffs = -_y;
      ylen -= yoffs;
    }
  if(_x + xlen > SCREEN_WIDTH)
    xlen = SCREEN_WIDTH - _x;
  if(_y + ylen > SCREEN_HEIGHT)
    ylen = SCREEN_HEIGHT - _y;

  //  uint16 *img = &(_image[_width]);
  uint16 *img = (uint16*)_image;
  for(int row = 0; row<_height; row++)
    {
      memcpy(&(screen[(y+row)*SCREEN_WIDTH+x]),
	     &(img[(row+yoffs)*_width + xoffs]),
	     2*xlen);
    }
}

void BImage::drawRect(const BRect& rect, uint16 color)
{
  BPoint pt1, pt2, pt3, pt4;
  pt1 = rect.pt;
  pt2 = rect.pt; pt2.x += rect.sz.width;
  pt3 = rect.pt; pt3.x += rect.sz.width; pt3.y += rect.sz.height;
  pt4 = rect.pt; pt4.y += rect.sz.height;
  drawLine(pt1, pt2, color);
  drawLine(pt2, pt3, color);
  drawLine(pt3, pt4, color);
  drawLine(pt4, pt1, color);
}

BPoint BImage::drawText(BFont* font,
			const std::string& text,
			int cursorPos,
			const BRect& clip,
			const BWidget& widget,
			uint16 color)
{
  int fh = font->height();
  int w = font->widthOfString(text);
  
  BPoint origin, cursor1, cursor2;
  int cursorOffs = 0;
  if(cursorPos >= 0 && cursorPos <= (int)text.size())
    {
      if(cursorPos == (int)text.size()) cursorOffs = w;
      else cursorOffs = font->widthOfString(text.substr(0, cursorPos));
    }
  
  switch(widget.textDirection())
    {
    case BWidget::DIR_LEFTTORIGHT:
      switch(widget.textAlignment())
	{
	case BWidget::ALIGN_LEFT:
	  origin = BPoint(clip.pt.x,
			  clip.pt.y + ((int)clip.sz.height-fh)/2);
	  break;
	case BWidget::ALIGN_CENTER:
	  origin = BPoint(clip.pt.x + ((int)clip.sz.width-w)/2,
			  clip.pt.y + ((int)clip.sz.height-fh)/2);
	  break;
	case BWidget::ALIGN_RIGHT:
	  origin = BPoint(clip.pt.x + clip.sz.width-w,
			  clip.pt.y + ((int)clip.sz.height-fh)/2);
	  break;
	}
      cursor1 = BPoint(origin.x + cursorOffs, origin.y);
      cursor2 = BPoint(cursor1.x, cursor1.y + fh);
      break;
      
    case BWidget::DIR_TOPTOBOTTOM:
      switch(widget.textAlignment())
	{
	case BWidget::ALIGN_LEFT:
	  origin = BPoint(clip.pt.x + (clip.sz.width+fh)/2,
			  clip.pt.y);
	  break;
	case BWidget::ALIGN_CENTER:
	  origin = BPoint(clip.pt.x + (clip.sz.width+fh)/2,
			  clip.pt.y + ((int)clip.sz.height-w)/2);
	  break;
	case BWidget::ALIGN_RIGHT:
	  origin = BPoint(clip.pt.x + (clip.sz.width+fh)/2,
			  clip.pt.y + (int)clip.sz.height-w);
	  break;
	}
      cursor1 = BPoint(origin.x, origin.y + cursorOffs);
      cursor2 = BPoint(cursor1.x - fh, cursor1.y);
      break;
      
    case BWidget::DIR_RIGHTTOLEFT:
      switch(widget.textAlignment())
	{
	case BWidget::ALIGN_LEFT:
	  origin = BPoint(clip.pt.x + clip.sz.width,
			  clip.pt.y + (clip.sz.height+fh)/2);
	  break;
	case BWidget::ALIGN_CENTER:
	  origin = BPoint(clip.pt.x + (clip.sz.width+w)/2,
			  clip.pt.y + (clip.sz.height+fh)/2);
	  break;
	case BWidget::ALIGN_RIGHT:
	  origin = BPoint(clip.pt.x + w,
			  clip.pt.y + (clip.sz.height+fh)/2);
	  break;
	}
      cursor1 = BPoint(origin.x - cursorOffs, origin.y);
      cursor2 = BPoint(cursor1.x, cursor1.y - fh);
      break;
      
    case BWidget::DIR_BOTTOMTOTOP:
      switch(widget.textAlignment())
	{
	case BWidget::ALIGN_LEFT:
	  origin = BPoint(clip.pt.x + ((int)clip.sz.width-fh)/2,
			  clip.pt.y + clip.sz.height);
	  break;
	case BWidget::ALIGN_CENTER:
	  origin = BPoint(clip.pt.x + ((int)clip.sz.width-fh)/2,
			  clip.pt.y + (clip.sz.height+w)/2);
	  break;
	case BWidget::ALIGN_RIGHT:
	  origin = BPoint(clip.pt.x + ((int)clip.sz.width-fh)/2,
			  clip.pt.y + w);
	  break;
	}
      cursor1 = BPoint(origin.x, origin.y - cursorOffs);
      cursor2 = BPoint(cursor1.x + fh, cursor1.y);
      break;
    }

  origin = widget.transformUp(origin);
  BRect clip2 = widget.transformUp(clip);

  if(widget.drawsDebugStuff())
    drawPixel(origin.x, origin.y, RGB15(31, 0, 0)|BIT(15));
      
  origin = font->drawString(*this, origin, text, color, clip2,
			    widget.textDirectionRelativeToScreen());

  if(cursorPos >= 0 && cursorPos <= (int)text.size())
    {
      drawLine(widget.transformUp(cursor1), widget.transformUp(cursor2), color);
    }
  return widget.transformDown(origin);
}


void BImage::drawLine(const BPoint& pt1, const BPoint& pt2,
		      uint16 color)
{
  int dx, dy, dx2, dy2, ix, iy, err, i;
  int x1 = pt1.x;
  int y1 = pt1.y;
  int x2 = pt2.x;
  int y2 = pt2.y;

  uint16 *img = (uint16*)_image;
  
  // identify the first pixel
  uint16 *ptr_vid = img + y1*_width + x1;
  
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
    iy = _width;
  else
    {
      iy = -_width;
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

void
BImage::drawPixel(const BPoint& pt, uint16 color)
{
  ((uint16*)_image)[pt.y*_width+pt.x] = color;
}

void
BImage::drawPixel(int x, int y, uint16 color)
{
  ((uint16*)_image)[y*_width+x] = color;
}

void
BImage::fill(uint16 color)
{
  if(_format != FMT_RGB15) return;
  for(int i=0; i<_width*_height; i++)
    ((uint16*)_image)[i] = color;
}
