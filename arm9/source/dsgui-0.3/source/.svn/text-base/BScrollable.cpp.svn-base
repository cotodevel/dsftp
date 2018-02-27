// BScrollable.cpp (this is -*- C++ -*-)
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
#include <unistd.h>

/* my includes */
#include "BScrollable.h"
#include "BScroller.h"

BScrollable::BScrollable(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _hScroller = _vScroller = NULL;
}

void
BScrollable::setHScroller(BScroller *scroller)
{
  _hScroller = scroller;
  if(scroller)
    {
      scroller->setScrollable(this);
      updateScroller(_hScroller);
    }
}

void
BScrollable::setVScroller(BScroller *scroller)
{
  _vScroller = scroller;
  if(scroller)
    {
      scroller->setScrollable(this);
      updateScroller(_vScroller);
    }
}

#if BScrollable_test
int main(int argc, char **argv)
{
}
#endif /* BScrollable_test */
