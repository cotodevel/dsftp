// BException.cpp (this is -*- C++ -*-)
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

#include <nds.h>

/* system includes */
#include <stdarg.h>
#include <errno.h>
#include <strstream>

//iprintf
#include <nds/ndstypes.h>
#include <nds/memory.h>

#include <nds/arm9/video.h>
#include <nds/arm9/console.h>
#include <nds/arm9/exceptions.h>
#include <nds/arm9/background.h>


/* my includes */
#include "BException.h"

static bool handlerinstalled = false;

void uncaught_exception_handler(void)
{
  try {
    throw;
  } catch(BException& err) {
    //iprintf("%s\n", err.description().c_str());
  } catch(std::exception& err) {
    //iprintf("%s\n", err.what());
  } catch(...) {
    //iprintf("Unhandled exception\n");
  }
  // abort();
}
static void install_handler(void)
{
  if(!handlerinstalled)
    {
      std::set_terminate(uncaught_exception_handler);
      handlerinstalled = true;
    }
}

BException::BException(const std::string& file, int line,
		       const std::string& text)
{
  install_handler();

  this->file = file;
  this->line = line;
  this->text = text;
}

std::string BException::description(bool msgonly)
{
  char descr[255];
  memset(descr, 0, 255);
  if(msgonly){
    //snprintf(descr, 254, "%s", text.c_str());
  }
  else{
    //snprintf(descr, 254, "%s[%d]: %s",file.c_str(), line, text.c_str());
  }
  return std::string(descr);
}

BSystemException::BSystemException(const std::string& file, int line,
				   const std::string& text)
{
  install_handler();

  this->file = file;
  this->line = line;

  char outer[255];
  memset(outer, 0, 255);
  //snprintf(outer, 254, "%s (errno %d)", text.c_str(), errno);

  this->text = outer;
}

#if BException_test
int main(int argc, char **argv)
{
}
#endif /* BException_test */
