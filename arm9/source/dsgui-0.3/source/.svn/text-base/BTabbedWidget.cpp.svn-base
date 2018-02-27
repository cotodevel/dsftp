// BTabbedWidget.cpp (this is -*- C++ -*-)
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
#include "BTabbedWidget.h"

BTabbedWidget::BTabbedWidget(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _rgrp = new BRadioGroup(this, BRect(0, 0, frame.sz.width, 15));
  _rgrp->setBackgroundColor(RGB15(10, 0, 0)|BIT(15));
  _rgrp->setDelegate(this);
  _deleg = NULL;
}

BWidget*
BTabbedWidget::addTabbedPane(const std::string& title)
{
  TabAndPane tp;

  int n = _rgrp->children().size();
  int x, w;
  x = 0;
  w = _frame.sz.width / (n+1);

  for(int i=0; i<n; i++)
    {
      _rgrp->children()[i]->setFrame(BRect(x, 0, w, 15));
      x+=w;
    }
	
  tp.tab = new BButton(_rgrp, BRect(x, 0, w, 15));
  tp.tab->setText(title);
  _rgrp->registerChildren();

  tp.pane = new BWidget(this, BRect(0, 15,
				    _frame.sz.width,
				    _frame.sz.height-15));
  tp.pane->setReliefType(BWidget::RELIEF_SIMPLE);
  if(n != 0)
    tp.pane->hide();

  _tabs.push_back(tp);

  return tp.pane;
}

void BTabbedWidget::onSelectButton(BButton* button)
{
  for(unsigned int i=0; i<_tabs.size(); i++)
    {
      if(button == _tabs[i].tab)
	{
	  _tabs[i].pane->show();
	  if(_deleg)
	    _deleg->onSelectTabbedPane(this, button->text(), _tabs[i].pane);
	}
      else
	_tabs[i].pane->hide();
    }
}

void BTabbedWidget::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

#if BTabbedWidget_test
int main(int argc, char **argv)
{
}
#endif /* BTabbedWidget_test */
