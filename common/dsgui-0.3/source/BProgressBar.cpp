// BProgressBar.cpp (this is -*- C++ -*-)
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
#include "BProgressBar.h"

BProgressBar::BProgressBar(BWidget* parent, const BRect& frame):
  BWidget(parent, frame)
{
  _total = 100;
  _progress = 0;
}

void
BProgressBar::setTotal(int total)
{
  _total = total;
  setNeedsRedraw(true);
}

void
BProgressBar::setProgress(int progress)
{
  _progress = progress;
  setNeedsRedraw(true);
}

void
BProgressBar::draw(BImage& img)
{
  if(!_needsRedraw) return;
  BWidget::draw(img);
  BRect clip = transformDown(_frame, false);
  
  int width = (clip.sz.width*_progress)/_total;
  if(width > clip.sz.width) width = clip.sz.width;
  else if(width < 0) width = 0;
  
  u32 col1 = alphablend(_bgcolor, _textcolor, 20);
  u32 col2 = alphablend(_bgcolor, _textcolor, 10);
  
  transformUp(clip).fill(img, col1);
  
  clip.sz.width = width;
  
  transformUp(clip).fill(img, col2);
}

#if BProgressBar_test
int main(int argc, char **argv)
{
}
#endif /* BProgressBar_test */
