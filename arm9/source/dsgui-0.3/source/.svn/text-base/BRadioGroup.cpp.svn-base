// BRadioGroup.cpp (this is -*- C++ -*-)
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
#include "BRadioGroup.h"

BRadioGroup::BRadioGroup(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _deleg = NULL;
}

void
BRadioGroup::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

void
BRadioGroup::selectButton(BButton* button)
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      BButton* b;
      try {
	b = dynamic_cast<BButton*>(*iter);
	if(!b) continue;
      } catch(...) {
	continue;
      }

      if(b == button)
	{
	  b->setValue(true);
	  b->setAcceptsInput(false);
	}
      else
	{
	  b->setValue(false);
	  b->setAcceptsInput(true);
	}
    }  
}

void
BRadioGroup::selectButtonWithTag(int tag)
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      BButton* b;
      try {
	b = dynamic_cast<BButton*>(*iter);
	if(!b) continue;
      } catch(...) {
	continue;
      }

      if(b->tag() == tag)
	{
	  b->setValue(true);
	  b->setAcceptsInput(false);
	}
      else
	{
	  b->setValue(false);
	  b->setAcceptsInput(true);
	}
    }  
}

BButton*
BRadioGroup::selectedButton()
{
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      BButton* b;
      try {
	b = dynamic_cast<BButton*>(*iter);
	if(!b) continue;
      } catch(...) {
	continue;
      }

      if(b->value() == true) return b;
    }
  return NULL;
}

void
BRadioGroup::registerChildren()
{
  int numButtons = 0;
  
  for(std::vector<BWidget*>::iterator iter = _children.begin();
      iter != _children.end(); iter++)
    {
      BButton* b;
      try {
	b = dynamic_cast<BButton*>(*iter);
	if(!b) continue;
      } catch(...) {
	continue;
      }

      b->setType(BButton::BT_TOGGLE);
      
      numButtons++;
      if(numButtons == 1)
	b->setValue(true);
      else
	b->setValue(false);

      b->setDelegate(this);
    }      
}

void BRadioGroup::onButtonClick(BButton* button)
{
  selectButton(button);
  if(_deleg) _deleg->onSelectButton(button);
}

#if BRadioGroup_test
int main(int argc, char **argv)
{
}
#endif /* BRadioGroup_test */
