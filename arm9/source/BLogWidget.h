// BLogWidget.h (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

#ifndef BLOGWIDGET_H
#define BLOGWIDGET_H

#define _GLIBCXX_USE_CXX11_ABI 0

/* system includes */
#include <BLogger.h>
#include <BFont.h>
#include <BWidget.h>
#include <BScrollable.h>

#ifdef __cplusplus

/* my includes */
/* (none) */

class BLogWidget: public BScrollable, public BLogger {
public:
  BLogWidget(BWidget* parent, const BRect& frame);

  virtual void draw(BImage& img);
  virtual void log(LogLevel level, const std::string& text);

  unsigned int maxLines() { return maxlines; }
  void setMaxLines(unsigned int maxlines);

  unsigned int firstLine() { return firstline; }
  void setFirstLine(unsigned int line);

  bool atEnd();

  bool doesAutoscroll() { return autoscroll; }
  void setAutoscroll(bool autoscroll);

  virtual void updateScroller(BScroller *scroller);
  virtual void scrollToPosition(BScroller *scroller,
				unsigned int position);

private:
  typedef struct {
    LogLevel level;
    std::string text;
  } LogEntry;
  
  std::deque<LogEntry> lines;
  unsigned int numlines, maxlines, firstline;
  bool autoscroll;
};

#endif

#endif /* BLOGWIDGET_H */
