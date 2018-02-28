// BFontManager.cpp (this is -*- C++ -*-)
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
#include <string>
#include <algorithm>


/* my includes */
#include "BFontManager.h"

BFontManager* BFontManager::_fm = NULL;

static bool nameSorter(const std::string& s1, const std::string& s2)
{
  return s1 < s2;
}

static bool sizeSorter(int i1, int i2)
{
  return i1 < i2;
}

BFontManager*
BFontManager::get()
{
  if(!_fm) _fm = new BFontManager;
  return _fm;
}

void
BFontManager::addFont(BFont* font)
{
  const BFont::FontHeader& hdr = font->fontHeader();
  FontDescription descr = { hdr.name,
			    (hdr.fontflags & BFont::FLAG_BOLD),
			    (hdr.fontflags & BFont::FLAG_OBLIQUE),
			    hdr.height,
			    font,
			    "" };

  if(!fontAlreadyIn(descr))
    {
      _fonts.push_back(descr);
      addUniqueFontName(descr.name);
    }
}

void
BFontManager::scanDirectory(const std::string& dir)
{
  BFileManager* fm = BFileManager::get();

  std::vector<BFileManager::FileAndType> contents = fm->directoryContents(dir);

  for(unsigned int i=0; i<contents.size(); i++)
    {
      std::string filename = dir + "/" + contents[i].filename;
      BFATFile *file;
      try {
	file = new BFATFile(filename.c_str());
      } catch(...) {
	continue;
      }
      
      BFont::FontHeader hdr;

      if(!BFont::readFontHeader(*file, hdr)) continue;
      
      FontDescription descr = { hdr.name,
				(hdr.fontflags & BFont::FLAG_BOLD),
				(hdr.fontflags & BFont::FLAG_OBLIQUE),
				hdr.height,
				NULL,
				filename };

      if(!fontAlreadyIn(descr))
	{
	  _fonts.push_back(descr);
	  addUniqueFontName(descr.name);
	}
      delete file;
    }
}

std::vector<int>
BFontManager::sizesForFont(const std::string& name)
{
  std::vector<int> retval;
  for(unsigned int i=0; i<_fonts.size(); i++)
    {
      if(name != _fonts[i].name) continue;

      bool found = false;
      
      for(unsigned int j=0; j<retval.size(); j++)
	{
	  if(retval[j] == _fonts[i].height)
	    {
	      found = true;
	      break;
	    }
	}
      if(found) continue;
      
      retval.push_back(_fonts[i].height);
    }

  std::sort(retval.begin(), retval.end(), sizeSorter);
  return retval;
}

bool
BFontManager::hasFont(const std::string& name, int size, bool bold, bool oblique)
{
  for(unsigned int i=0; i<_fonts.size(); i++)
    {
      if(name == _fonts[i].name &&
	 size == _fonts[i].height &&
	 bold == _fonts[i].bold &&
	 oblique == _fonts[i].oblique)
	return true;
    }
  return false;
}

BFont*
BFontManager::font(const std::string& name, int size, bool bold, bool oblique)
{
  for(unsigned int i=0; i<_fonts.size(); i++)
    {
      if(name == _fonts[i].name &&
	 size == _fonts[i].height &&
	 bold == _fonts[i].bold &&
	 oblique == _fonts[i].oblique)
	{
	  if(_fonts[i].font) return _fonts[i].font;

	  BFATFile *file;
	  try {
	    file = new BFATFile(_fonts[i].filename.c_str());
	  } catch(...) {
	    return NULL;
	  }

	  try {
	    _fonts[i].font = new BFont(*file);
	  } catch(...) {
	    delete file;
	    return NULL;
	  }

	  delete file;
	  
	  return _fonts[i].font;
	}
    }
  return false;
}

void
BFontManager::fontIsDeleted(BFont* font)
{
  for(unsigned int i=0; i<_fonts.size(); i++)
    {
      if(_fonts[i].font == font)
	_fonts[i].font = NULL;
    }
}

bool
BFontManager::fontAlreadyIn(const FontDescription& d1)
{
  for(unsigned int i=0; i<_fonts.size(); i++)
    {
      const FontDescription& d2 = _fonts[i];

      if(d1.name == d2.name &&
	 d1.bold == d2.bold &&
	 d1.oblique == d2.oblique &&
	 d1.height == d2.height)
	return true;
    }

  return false;
}

void
BFontManager::addUniqueFontName(const std::string& name)
{
  for(unsigned int i=0; i<_names.size(); i++)
    {
      if(_names[i] == name) return;
    }
  _names.push_back(name);

  std::sort(_names.begin(), _names.end(), nameSorter);
}

#if BFontManager_test
int main(int argc, char **argv)
{
}
#endif /* BFontManager_test */
