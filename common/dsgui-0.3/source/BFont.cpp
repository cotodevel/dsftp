// BFont.cpp (this is -*- C++ -*-)
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
#include <nds.h>
#include <BException.h>

/* my includes */
#include "BFont.h"
#include "BFontManager.h"
#include "BGraphics.h"

bool
BFont::readFontHeader(BVirtualFile& f, FontHeader& header)
{
  char magic[4];
  memset(magic, 0, 4);
  if(f.read(magic, 4) < 4) return false;
  if(strncmp(magic, "BFNT", 4) != 0) return false;

  uint8 version = read8(f);
  if(version != 1)
    return false;
  
  header.formatflags = read8(f);
  header.encodingflags = read8(f);

  header.name = readstring(f);
  header.fullname = readstring(f);

  header.height = read8(f);

  header.fontflags = read8(f);
  
  return true;
}

bool
BFont::readFontBody(BVirtualFile& f, const FontHeader& header)
{
  int numranges = read8(f);
  int numchars = 0;
  
  for(int i=0; i<numranges; i++)
    {
      CharacterRange range;
      range.start = read32(f);
      range.end = read32(f);
      numchars += (range.end - range.start) + 1;
      ranges.push_back(range);
    }

  for(int i=0; i<numchars; i++)
    {
      GlyphDescription descr;
      descr.start = read16(f);
      descr.width = read8(f);
      indices.push_back(descr);
    }

  uint16 bmpw = read16(f);
  uint8 bmph = read8(f);

  long pos = f.tell();
  f.seek(0, BVirtualFile::WHENCE_END);
  long bufsize = f.tell() - pos;
  if(bufsize != bmpw*bmph)
    BTHROW(BSystemException, "Invalid bmp size (%dx%d != %d)",
	   bmpw, bmph, bufsize);

  f.seek(pos, BVirtualFile::WHENCE_START);
  uint8 *bmp = new uint8[bufsize];
  f.read(bmp, bufsize);

  img = new BImage(bmpw, bmph, bmp, BImage::FMT_GREY5, false);
  return true; 
}

BFont::BFont(const BImage& img, const GlyphDescription indices[],
	     int firstchar, int lastchar)
{
  this->img = new BImage(img);
  for(int i=0; i<(lastchar-firstchar); i++)
    this->indices.push_back(indices[i]);
  CharacterRange range = {firstchar, lastchar};
  ranges.push_back(range);
}

BFont::BFont(BVirtualFile& f)
{
  if(!readFontHeader(f, header))
    BTHROW(BSystemException, "Couldn't read font header");
  if(!readFontBody(f, header))
    BTHROW(BSystemException, "Couldn't read font body");
}

BFont::~BFont()
{
  BFontManager::get()->fontIsDeleted(this);
  delete img;
}

int
BFont::widthOfGlyph(uint32 glyph)
{
  if(glyph == ' ') return widthOfGlyph('f');
  else
    {
      int index = indexOfGlyph(glyph);
      if(index == -1) return widthOfGlyph('?');
      return indices[index].width;
    }
}

int
BFont::widthOfString(const std::string& str)
{
  int width = 0;
  for(unsigned int i=0; i<str.size(); i++)
    {
      int w = widthOfGlyph(str[i]);
      if(w > -1) width += w;
    }

  return width;
}

BPoint
BFont::drawGlyph(BImage& img, const BPoint& point, uint32 glyph, uint16 color,
		 const BRect& clip, BWidget::TextDirection dir)
{
  BPoint pt = point;
  
  if(glyph == ' ')
    {
      switch(dir)
	{
	case BWidget::DIR_LEFTTORIGHT:
	  pt.x += widthOfGlyph('f');
	  break;
	case BWidget::DIR_RIGHTTOLEFT:
	  pt.x -= widthOfGlyph('f');
	  break;
	case BWidget::DIR_TOPTOBOTTOM:
	  pt.y += widthOfGlyph('f');
	  break;
	case BWidget::DIR_BOTTOMTOTOP:
	  pt.y -= widthOfGlyph('f');
	  break;
	}
      return pt;
    }

  int x = point.x, y = point.y;

  int index = indexOfGlyph(glyph);
  if(index == -1) return point;

  int xstart = indices[index].start;
  int xwidth = indices[index].width;
  int xend = xstart + xwidth;

  int ystart = 0, yend = this->img->height();

  const uint8* srcBytes = this->img->bytes();
  unsigned int srcw = this->img->width();
  uint16* tgtBytes = (uint16*)img.bytes();
  unsigned int tgtw = img.width(), tgth = img.height();

  for(int r=ystart; r<yend; r++)
    for(int c=xstart; c<xend; c++)
      {
	int tgtx=0, tgty=0;

	switch(dir)
	  {
	  case BWidget::DIR_LEFTTORIGHT:
	    tgtx = x + (c - xstart);
	    tgty = y + (r - ystart);
	    break;
	  case BWidget::DIR_TOPTOBOTTOM:
	    tgtx = x - (r - ystart);
	    tgty = y + (c - xstart);
	    break;
	  case BWidget::DIR_RIGHTTOLEFT:
	    tgtx = x - (c - xstart);
	    tgty = y - (r - ystart);
	    break;
	  case BWidget::DIR_BOTTOMTOTOP:
	    tgtx = x + (r - ystart);
	    tgty = y - (c - xstart);
	    break;
	  }

	if(tgtx < 0 || tgtx < clip.pt.x ||
	   tgtx >= (int)tgtw || tgtx >= (int)(clip.pt.x + clip.sz.width) ||
	   tgty < 0 || tgty < clip.pt.y ||
	   tgty >= (int)tgth || tgty >= (int)(clip.pt.y + clip.sz.height))
	  continue;

	uint8 srcbyte = srcBytes[r*srcw+c];
	if(srcbyte >= 0x1f) // maxval
	  continue;

	uint16& tgtbyte = tgtBytes[tgty*tgtw+tgtx];
	tgtbyte = alphablend(color, tgtbyte, 0x1f-srcbyte);
      }


  switch(dir)
    {
    case BWidget::DIR_LEFTTORIGHT:
      pt.x += xwidth;
      break;
    case BWidget::DIR_TOPTOBOTTOM:
      pt.y += xwidth;
      break;
    case BWidget::DIR_RIGHTTOLEFT:
      pt.x -= xwidth;
      break;
    case BWidget::DIR_BOTTOMTOTOP:
      pt.y -= xwidth;
    }

  return pt;
}

BPoint
BFont::drawString(BImage& img, const BPoint& point, const std::string& str,
		  uint16 color, const BRect& clip, BWidget::TextDirection dir)
{
  BPoint pt = point;
  for(unsigned int i=0; i<str.size(); i++)
    pt = drawGlyph(img, pt, str[i], color, clip, dir);
  return pt;
}

BFont*
BFont::otherSizeFont(int delta)
{
  BFont* font = BFontManager::get()->font(header.name, header.height+delta,
					  header.fontflags & FLAG_BOLD,
					  header.fontflags & FLAG_OBLIQUE);
  if(font) return font;
  return this;
}

