// BStringUtils.cpp (this is -*- C++ -*-)
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
#include <nds.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* my includes */
#include "BStringUtils.h"

std::string strip(const std::string& str, bool asciionly)
{
  std::string retval = str;
  if(!retval.size()) return "";
  
  if(retval == " ") return "";

  char first = retval[0];
  while(first == ' ' || first == '\t' ||
	first == '\n' || first == '\r' ||
	(asciionly && !isascii(first)))
    {
      if(retval.size() <= 1) return "";
      retval = retval.substr(1, retval.size()-1);
      first = retval[0];
    }
  int last = retval[retval.size()-1];
  while(last == ' ' || last == '\t' ||
	last == '\n' || last == '\r' ||
	(asciionly && !isascii(first)))
    {
      if(retval.size() == 1) return "";
      retval = retval.substr(0, retval.size()-1);
      last = retval[retval.size()-1];
    }
  return retval;
}

std::deque<std::string> split(const std::string& str,
			       const std::string& separator) 
{
  int begin, end;
  std::deque<std::string> retval;
  std::string current = str;

  if(!current.length()) return retval;
  
  begin = 0; end = 0;

  while(end != -1)
    {
      begin = current.find(separator);
      if(begin == 0)
	{
	  while(begin == 0)
	    {
	      current = current.substr(separator.length());
	      begin = current.find(separator);
	      if(begin != 0 && begin != -1)
		{
		  begin = 0;
		  break;
		}
	    }
	}
      else
	begin = 0;
      
      end = current.find(separator);
      if(end == -1)
	{
	  if(current.length())
	    retval.insert(retval.end(), current);
	  break;
	}

      if(begin == end)
	break;
      
      retval.insert(retval.end(), current.substr(begin, end-begin));
      current = current.substr(end-begin);
    }

  return retval;
}

std::string toUpper(const std::string& str)
{
  std::string retval = str;
  for(unsigned int i=0; i<retval.size(); i++)
    retval[i] = toupper(retval[i]);
  return retval;
}

std::string toLower(const std::string& str)
{
  std::string retval = str;
  for(unsigned int i=0; i<retval.size(); i++)
    retval[i] = tolower(retval[i]);
  return retval;
}

std::string format(const std::string& fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  char buf[255];
  memset(buf, 0, 255);
  vsnprintf(buf, 254, fmt.c_str(), ap);
  va_end(ap);
  return buf;
}

bool startsWith(const std::string& str, const std::string& suffix)
{
  if(str.size() < suffix.size()) return false;
  return str.substr(0, suffix.size()) == suffix;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
  if(str.size() < suffix.size()) return false;
  return str.substr(str.size()-suffix.size(), suffix.size()) == suffix;
}
