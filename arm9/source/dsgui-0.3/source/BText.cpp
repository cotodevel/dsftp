// BText.cpp (this is -*- C++ -*-)
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
#include "BText.h"
#include "BTheme.h"
#include "BFont.h"
#include "BStringUtils.h"
#include "BMessagebox.h"

static inline char next(const std::string& str,
			int& strpos,
			int& skipped,
			bool stripSingleNewlines)
{
  if(!stripSingleNewlines || strpos < 1) return str[strpos++];

  char oldchar = str[strpos-1];
  if(oldchar == '\r') oldchar = str[strpos-2];
  char newchar = str[strpos++];
  if(newchar == '\r')
    {
      skipped++;
      newchar = str[strpos++];
    }
  
  if(oldchar != '\n' &&          // no newline before,
     newchar == '\n' &&          // newline now and
     strpos <= (int)(str.size())-2)     // not at the end of the file
    {
      char nextchar = str[strpos];
      if(nextchar == '\r')
	nextchar = str[strpos+1];

      // more than a single newline or next line starts with space?
      if(nextchar == '\n' || nextchar == ' ') 
	{
	  return '\n';           // return that newline
	}
      else if(oldchar != ' ' && nextchar != ' ')
	{
	  // insert a space between the words @ line end & start
	  return ' ';
	}
      else
	{
	  // skip the single newline
	  strpos++;
	  skipped++;
	  return nextchar;
	}
    }
  else return newchar;
}

BText::StringLine* newline(const std::string& l,
			   BFont* font,
			   int index, int skipped)
{
  BText::StringLine* line = new BText::StringLine;
  line->color = RGB15(31, 0, 0)|BIT(15);
  line->line = l;
  line->index = index; line->length = l.size() + skipped;
  
  line->spacingBefore =
    line->leftIndent =
    line->rightIndent = 0;
  line->height = font->height();
  line->align = BWidget::ALIGN_LEFT;

  return line;
}

BText::BText(BWidget* parent, const BRect& frame):
  BScrollable(parent, frame)
{
  _firstline = 0;
  _slave = NULL;
  _full = false;
  _firstlinecolor = _textcolor;
  _firstlinecolorset = false;
  _stripSingleNL = false;
}

BText::~BText()
{
  clear();
}

void
BText::setText(const std::string& text)
{
  _text = text;
  clear();
  rewrap(0, _text.size());
  setNeedsRedraw(true);
}

int
BText::indexOfLastCharacterOnScreen()
{
  if(_lines.size() == 0) return 0;
  
  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  int indexOfLastLine = _firstline + (clip.sz.height / _font->height()) - 1;
  if(indexOfLastLine >= (int)_lines.size())
    indexOfLastLine = _lines.size()-1;

  int index = _lines[indexOfLastLine]->index,
    length = _lines[indexOfLastLine]->length;
  
  return index+length;
}

void
BText::setStripSingleNewlines(bool stripSingleNL)
{
  if(stripSingleNL == _stripSingleNL) return;
  _stripSingleNL = stripSingleNL;
  clear();
  rewrap(0, _text.size());
}

void
BText::setFont(BFont* font)
{
  BWidget::setFont(font);
  clear();
  rewrap(0, _text.size());
}

void
BText::draw(BImage& img)
{
  BWidget::draw(img);
  if(!_lines.size()) return;

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  int height = clip.sz.height;
  BPoint pt = clip.pt;

  clip = transformUp(clip);

#if 0
  _font->drawString(img, clip.pt, format("%d lines", _lines.size()),
		    RGB15(0, 0, 0)|BIT(15), clip);
  y += _font->height();
#endif

  for(std::vector<Line*>::iterator iter = _lines.begin() + _firstline;
      iter != _lines.end(); iter++)
    {
      if(_firstlinecolorset && (iter == _lines.begin() + _firstline))
	(*iter)->draw(this, img, clip, pt, _firstlinecolor);
      else
	(*iter)->draw(this, img, clip, pt, _textcolor);
      if(pt.y + _font->height() > height)
	break;
    }
}

void
BText::clear()
{
  for(unsigned int i=0; i<_lines.size(); i++)
    delete _lines[i];
  _lines.clear();
  _firstline = 0;
  setNeedsRedraw(true);
  updateScroller(_vScroller);
}

void
BText::setSlaveText(BText* slave)
{
  _slave = slave;

  if(_full && _slave)
    {
      int i = indexOfLastCharacterOnScreen() + 1;
      if(i < (int)_text.size())
	 _slave->setText(_text.substr(i, _text.size()-i));
    }
}

void
BText::updateScroller(BScroller *scroller)
{
  if(_vScroller && scroller == _vScroller)
    {
      _vScroller->setStepIncrement(1);
      _vScroller->setPageIncrement((_frame.sz.height - 1)/_font->height());
      _vScroller->setPosition(_firstline);
      _vScroller->setMinimum(0);
      _vScroller->setMaximum(_lines.size());
      _vScroller->setAmountRepresentedByThumb(_frame.sz.height/_font->height());
    }
}

void
BText::scrollToPosition(BScroller *scroller,
			unsigned int position)
{
  if(scroller != _vScroller) return;
  if(position >= _lines.size()) return;
  _firstline = position;
  updateScroller(scroller);
  setNeedsRedraw(true);

  if(_full && _slave)
    {
      int i = indexOfLastCharacterOnScreen() + 1;
      if(i < (int)_text.size())
	_slave->setText(_text.substr(i, _text.size()-i));
    }
} 

void
BText::rewrap(unsigned int index, unsigned int length)
{
  setNeedsRedraw(true);
  _full = false;

  // find line containing index
  std::vector<Line*>::iterator lpos = _lines.begin();
  int y = 0;

  if(_lines.size())
    {
      while(lpos != _lines.end())
	{
	  if((*lpos)->index <= index &&
	     (*lpos)->index + length > index)
	    break;
	  y += (*lpos)->height;
	  lpos++;
	}

      std::vector<Line*>::iterator lend = lpos;
      while(lend != _lines.end())
	{
	  if((*lend)->index <= index + length &&
	     (*lend)->index + length > index + length)
	    break;
	  lend++;
	}

      for(std::vector<Line*>::iterator l = lpos;
	  l != lend+1 && l != _lines.end(); l++)
	{
	  delete *l;
	}

      lpos = _lines.erase(lpos, lend);
    }

  std::string wraptext = _text.substr(index, length);

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);

  int maxlinelen = clip.sz.width;

  std::string curline, curword;
  int curlinelen = 0;

  int strpos = 0, skipped = 0, strposold = strpos;
  bool done = false;
  
  while(strpos < (int)length && !done)
    {
      strposold = strpos;
      skipped = 0;
      
      // read single word
      while(strpos < (int)length && !done)
	{
	  char c = next(wraptext, strpos, skipped, _stripSingleNL);
	  
	  if(c == ' ')
	    {
	      // finish word
	      if(curword != "")
		{
		  int w1, w2;
		  w1 = w2 = _font->widthOfString(curword);
		  
		  if(curline != "") w2 += _font->widthOfGlyph(' ');
		  
		  // finish line
		  if(curlinelen + w2 > maxlinelen)
		    {
		      StringLine* line = newline(curline, _font,
						 strposold, skipped);
		      _lines.push_back(line);
		      
		      strposold = strpos - curword.size() - 1;
		      curline = curword;
		      skipped = 0;
		      curlinelen = w1;
		      
		      curword = "";
		    }
		  else
		    {
		      if(curline != "")
			curline += " ";
		      curline += curword;
		      curword = "";
		      curlinelen += w2;
		    }
		  continue;
		}
	      
	      // finish line
	      else if(curlinelen + _font->widthOfGlyph(c) > maxlinelen)
		{
		  StringLine* line = newline(curline, _font,
					     strposold, skipped);
		  _lines.push_back(line);

		  strposold = strpos - 1;
		  skipped = 0;
		  curline = "";
		  curlinelen = 0;
		  curword = "";
		  break;
		}
	      
	      // append space
	      else 
		{
		  curline += " ";
		  curlinelen += _font->widthOfGlyph(c);
		  curword = "";
		  continue;
		}
	    }
	  
	  else if(c == '\n' || c == '')
	    {
	      // finish word
	      if(curword != "")
		{
		  int w1, w2;
		  w1 = w2 = _font->widthOfString(curword);
		  
		  if(curline != "") w2 += _font->widthOfGlyph(' ');
		  
		  // finish line
		  if(curlinelen + w2 > maxlinelen)
		    {
		      // new line from curline
		      StringLine* line = newline(curline, _font,
						 strposold, skipped);
		      _lines.push_back(line);
		      
		      // new line from curword
		      strposold += curline.size()+1;
		      line = newline(curword, _font,
				     strposold, 0);
		      _lines.push_back(line);
		      skipped = 0;
		    }
		  else
		    {
		      curline += " ";
		      curline += curword;
		      
		      // new line from curline
		      StringLine* line = newline(curline, _font,
						 strposold, skipped);
		      _lines.push_back(line);
		      skipped = 0;
		    }

		  if(c == '')
		    {
		      _full = true;
		      done = true;
		      break;
		    }
		  
		  curline = "";
		  curword = "";
		  curlinelen = 0;
		  break;
		}
	      
	      // finish line
	      else
		{
		  // don't put empty lines at the start of the page
		  if(curline != "" || _lines.size())
		    {
		      // new line from curline
		      StringLine* line = newline(curline, _font,
						 strposold, skipped);
		      _lines.push_back(line);
		      skipped = true;
		    }
		  
		  if(c == '')
		    {
		      _full = true;
		      done = true;
		      break;
		    }
		  
		  curline = "";
		  curword = "";
		  curlinelen = 0;
		  break;
		}
	    }
	  else if(c != '\r') // no space and no "\n"
	    curword += c;
	}
    }

  if(curword != "" && !done)
    {
      if(curline == "") curline = curword;
      else curline = curline + " " + curword;
    }

  if(curline != "" && !done)
    {
      // new line from curline
      StringLine* line = newline(curline, _font,
				 strposold, skipped);
      _lines.push_back(line);
    }

  if((_lines.size()*_font->height()) >= (unsigned int)clip.sz.height)
    _full = true;

  if(_full && _slave)
    {
      int i = indexOfLastCharacterOnScreen() + 1;
      if(i < (int)_text.size())
	_slave->setText(_text.substr(i, _text.size()-i));
    }

  updateScroller(_vScroller);
}

void
BText::StringLine::draw(BText* text, BImage& img, const BRect& clip, BPoint& pt,
			uint16 color)
{
  int allowedWidth = clip.sz.width - leftIndent - rightIndent;
  //  std::string str = text->_text.substr(index, length);
  std::string& str = line;

  BPoint pt1 = pt;
  
  switch(align)
    {
    case BWidget::ALIGN_LEFT:
      pt1.x += leftIndent;
      break;
    case BWidget::ALIGN_CENTER:
      pt1.x += leftIndent + (allowedWidth - text->_font->widthOfString(str))/2;
      break;
    case BWidget::ALIGN_RIGHT:
      pt1.x += leftIndent + allowedWidth - text->_font->widthOfString(str);
      break;
    }

  pt1 = text->transformUp(pt1);
  text->_font->drawString(img, pt1, str, color, clip,
			  text->textDirectionRelativeToScreen());
  //  text->_font->drawString(img, pt1, str, color, clip,
  //  			  text->textDirectionRelativeToScreen());
  pt.y += height;
}

#if BText_test
int main(int argc, char **argv)
{
}
#endif /* BText_test */
