// BKeyboard.cpp (this is -*- C++ -*-)
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
#include "BKeyboard.h"
#include "BTheme.h"
#include "BStringUtils.h"

BKeyboard::BKeyboard(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _deleg = NULL;
  _curLayoutnum = 0;
  setLayouts(defaultLayouts());
}

void
BKeyboard::setLayout(const std::string& layout)
{
  std::vector<std::string> layouts;
  layouts.push_back(layout);
  setLayouts(layouts);
}

void
BKeyboard::setLayouts(const std::vector<std::string>& layouts)
{
  // clear
  deleteChildren();
  _buttons.clear();
  _layouts = layouts;

  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);

  for(unsigned int i=0; i<layouts.size(); i++)
    {
      std::string curLayout = layouts[i];
      std::vector<BButton*> curButtons;

      BSize cellSize = sizeOfLayoutCell(curLayout);
      if(cellSize.width == 0 || cellSize.height == 0) continue;

      std::deque<std::string> lines = split(curLayout, "\n");
      if(lines.size() < 2) continue;

      BPoint pt = clip.pt;
      
      // skip name
      for(unsigned int j=1; j<lines.size(); j++)
	{
	  pt.x = clip.pt.x;
	  std::string& curLine = lines[j];
	  // iterate through chars
	  for(unsigned int k=0; k<curLine.size(); k++)
	    {
	      char c = curLine[k];
	      BSize size = cellSize;

	      // same char above or to the left? Skip
	      if(k>0 && curLine[k-1] == c)
		continue;
	      if(j>1 && lines[j-1].size() > k && lines[j-1][k] == c)
		{
		  pt.x += cellSize.width+1;
		  continue;
		}

	      // same char below or to the right? Increase size
	      for(unsigned int k1=k+1; k1<curLine.size(); k1++)
		{
		  if(curLine[k1] == c)
		    size.width += cellSize.width+1;
		  else break;
		}
	      for(unsigned int j1=j+1; j1<lines.size(); j1++)
		{
		  if(lines[j1].size() > k &&
		     lines[j1][k] == c)
		    size.height += cellSize.height+1;
		  else break;
		}

	      BButton* btn = new BButton(this, BRect(pt, size));
	      if(_buttons.size() != 0)
		btn->hide();
	      btn->setDelegate(this);

	      if(isupper(c))
		{
		  btn->setTag(c);
		  setupSpecialButton(btn);
		}
	      else
		{
		  btn->setText(format("%c", c));
		  btn->setTag(0);
		}

	      curButtons.push_back(btn);

	      pt.x += size.width+1;
	    }
	  pt.y += cellSize.height+1;
	}

      _buttons.push_back(curButtons);
    }
}

void
BKeyboard::chooseLayoutNum(unsigned int num)
{
  if(num == _curLayoutnum || num >= _buttons.size())
    return;

  // hide old layout
  for(unsigned int i=0; i<_buttons.size(); i++)
    for(unsigned int j=0; j<_buttons[i].size(); j++)
      _buttons[i][j]->hide();
  
  // show new layout
  std::vector<BButton*>& btns = _buttons[num];
  for(unsigned int i=0; i<btns.size(); i++)
    btns[i]->show();

  setNeedsRedraw(true);

  _curLayoutnum = num;
}

void
BKeyboard::chooseNextLayout()
{
  unsigned int num = _curLayoutnum + 1;
  if(num >= _layouts.size()) num = 0;
  chooseLayoutNum(num);
}

void
BKeyboard::onButtonClick(BButton* button)
{
  if(button->tag() == 0)
    {
      if(_deleg)
	_deleg->onKeyboardPressGlyph(this, button->text());
    }
  else
    {
      switch(button->tag())
	{
	case KEY_SHIFT:
	case KEY_CAPSLOCK:
	  if(button->value() == true)
	    uppercaseVisibleButtons();
	  else
	    lowercaseVisibleButtons();
	  break;

	case KEY_NEXTLAYOUT:
	  chooseNextLayout();
	  break;

	case KEY_CHOOSELAYOUT_0:
	  chooseLayoutNum(0);
	  break;

	case KEY_CHOOSELAYOUT_1:
	  chooseLayoutNum(1);
	  break;

	case KEY_CHOOSELAYOUT_2:
	  chooseLayoutNum(2);
	  break;

	case KEY_CHOOSELAYOUT_3:
	  chooseLayoutNum(3);
	  break;
	}

      if(_deleg)
	_deleg->onKeyboardPressSpecial(this, (SpecialKey)button->tag());
    }
}

void
BKeyboard::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

std::deque<std::string>
BKeyboard::loadLayouts(BVirtualFile* file)
{
  std::string contents = file->read();
  std::deque<std::string> layouts = split(contents, "\n\n");
  for(std::deque<std::string>::iterator iter = layouts.begin();
      iter != layouts.end(); iter++)
    {
      if(split(*iter, "\n").size() < 2)
	{
	  iter = layouts.erase(iter);
	  if(iter == layouts.end()) break;
	}
    }
  return layouts;
}

BSize
BKeyboard::sizeOfLayoutCell(const std::string& layout)
{
  BSize size;
  BRect clip = BTheme::currentTheme()->contentRectForWidget(*this);
  
  std::deque<std::string> lines = split(layout, "\n");
  if(lines.size() < 2)
    size.height = 0;
  else
    size.height = (clip.sz.height / (lines.size()-1))-1;

  int maxnum = 0;
  // first line is tag
  for(unsigned int i=1; i<lines.size(); i++)
    maxnum = lines[i].size();
  if(maxnum == 0)
    size.width = 0;
  else
    size.width = (clip.sz.width / maxnum)-1;

  return size;
}

void
BKeyboard::setupSpecialButton(BButton* button)
{
  BRect frame = button->frame();
  int dim;
  if(frame.sz.width > frame.sz.height)
    {
      dim = frame.sz.width;
      button->setTextDirection(BWidget::DIR_LEFTTORIGHT);
    }
  else
    {
      dim = frame.sz.height;
      button->setTextDirection(BWidget::DIR_BOTTOMTOTOP);
    }

  char *equivalents[] = { "Shift", "Shft", "^", NULL,
			 "Caps Lock", "Lock", "L", NULL,
			 "Backspace", "Bksp", "<-", NULL,
			 "Delete", "Del", "X", NULL,
			 "Return", "Ret", "R", NULL,
			 "???", NULL };
  char **equiv = NULL;
  unsigned int title = 0;

  switch(button->tag())
    {
    case KEY_SHIFT:
      equiv = equivalents;
      button->setType(BButton::BT_TOGGLE);
      break;

    case KEY_CAPSLOCK:
      equiv = &(equivalents[4]);
      button->setType(BButton::BT_TOGGLE);
      break;

    case KEY_BACKSPACE:
      equiv = &(equivalents[8]);
      break;

    case KEY_DEL:
      equiv = &(equivalents[12]);
      break;
      
    case KEY_RETURN:
      equiv = &(equivalents[16]);
      break;

    case KEY_NEXTLAYOUT:
      title = _curLayoutnum + 1;
      if(title > _buttons.size()) title = 0;
      break;

    case KEY_CHOOSELAYOUT_0:
      title = 0;
      break;

    case KEY_CHOOSELAYOUT_1:
      title = 1;
      break;

    case KEY_CHOOSELAYOUT_2:
      title = 2;
      break;

    case KEY_CHOOSELAYOUT_3:
      title = 3;
      break;

    default:
      equiv = &(equivalents[20]);
      break;

    }

  if(equiv)
    {
      do {
	char *cur = *equiv;
	if(!cur)
	  {
	    button->setText("XXX");
	    button->setState(BWidget::STATE_DISABLED);
	    break;
	  }
	
	equiv++;
	if(button->font()->widthOfString(cur) < dim || !(*equiv))
	  {
	    button->setText(cur);
	    break; // done
	  }
      } while(*equiv);
    }
  else
    {
      if(title >= _layouts.size())
	{
	  button->setText("???");
	  button->setState(BWidget::STATE_DISABLED);
	}
      else
	button->setText(split(_layouts[title], "\n")[0]);
    }
}

void
BKeyboard::uppercaseVisibleButtons()
{
  std::vector<BButton*>& btns = _buttons[_curLayoutnum];
  for(unsigned int i=0; i<btns.size(); i++)
    {
      if(btns[i]->tag() == 0)
	btns[i]->setText(toUpper(btns[i]->text()));
    }
}

void
BKeyboard::lowercaseVisibleButtons()
{
  std::vector<BButton*>& btns = _buttons[_curLayoutnum];
  for(unsigned int i=0; i<btns.size(); i++)
    {
      if(btns[i]->tag() == 0)
	btns[i]->setText(toLower(btns[i]->text()));
    }
}

std::string BKeyboard::qwertzLayout = "abc\nqwertzuiopB\nasdfghjklRB\nyxcvbnm,.RC\nSS     KKRC";
std::string BKeyboard::numLayout = "123\n112233#A\n445566*A\n778899RC\n,.00KKRC";
std::string BKeyboard::specialLayout = "Sym\n!\"'$%&/()=B\n@+#*,.-;:RB\n_<>[]{}\\?RA\n‰ˆ¸ƒ÷‹ﬂKKRA";

std::vector<std::string>
BKeyboard::defaultLayouts()
{
  std::vector<std::string> retval;
  retval.push_back(qwertzLayout);
  retval.push_back(numLayout);
  retval.push_back(specialLayout);
  return retval;
}

#if BKeyboard_test
int main(int argc, char **argv)
{
}
#endif /* BKeyboard_test */
