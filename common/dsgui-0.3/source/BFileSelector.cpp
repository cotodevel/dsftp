// BFileSelector.cpp (this is -*- C++ -*-)
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
#include <algorithm>

/* my includes */
#include "BFileSelector.h"
#include "BStringUtils.h"
#include "BTheme.h"

static bool SortFiles(const BFileManager::FileAndType& ft1,
		      const BFileManager::FileAndType& ft2)
{
  if(ft1.filetype == BFileManager::TYPE_DIRECTORY &&
     ft2.filetype != BFileManager::TYPE_DIRECTORY)
    return true;
  if(ft1.filetype != BFileManager::TYPE_DIRECTORY &&
     ft2.filetype == BFileManager::TYPE_DIRECTORY)
    return false;

  return ft1.filename < ft1.filename;
}

BFileSelector::BFileSelector(BScreen* screen,
			     const std::string& startingDirectory):
  BDialog(screen, BSize(150, 150))
{
  BRect cr = BTheme::currentTheme()->contentRectForWidget(*this);
  cr = cr.insetRect(5, 5, 5, 5);

  _toplabel = new BLabel(this, BRect(cr.pt.x, cr.pt.y, cr.sz.width, 15));
  _toplabel->setText("Open");
  _toplabel->setTextAlignment(BWidget::ALIGN_LEFT);

  _dirhier = new BButton(this, BRect(cr.pt.x, cr.pt.y+17,
				     cr.sz.width, 17));
  _dirhier->setReliefType(BWidget::RELIEF_SIMPLE);
  _dirhier->setText(startingDirectory);
  _dirhier->setDelegate(this);
  
  _box = new BListbox(this, BRect(cr.pt.x, cr.pt.y+34,
				  cr.sz.width-12, cr.sz.height-55));
  _box->setReliefType(BWidget::RELIEF_SIMPLE);
  _box->setTextAlignment(BWidget::ALIGN_LEFT);
  _box->setDelegate(this);

  _scroller = new BScroller(this, BRect(cr.pt.x+cr.sz.width-12, cr.pt.y+34,
					12, cr.sz.height-55));
  _box->setVScroller(_scroller);
  
  _ok = new BButton(this, BRect(cr.pt.x+cr.sz.width-50, cr.pt.y+cr.sz.height-20,
				50, 20));
  _ok->setDelegate(this);
  _ok->setText("OK");

  _cancel = new BButton(this, BRect(cr.pt.x+cr.sz.width-105, cr.pt.y+cr.sz.height-20,
				    50, 20));
  _cancel->setDelegate(this);
  _cancel->setText("Cancel");

  if(startingDirectory != "")
    BFileManager::get()->changeDirectory(startingDirectory);
  
  _cancelled = false;
}


BFileSelector::~BFileSelector()
{
}

std::string
BFileSelector::selectedFilename()
{
  return BFileManager::get()->normalizePath(BFileManager::get()->currentDirectory() + "/" + _selected);
}

void
BFileSelector::onButtonClick(BButton* button)
{
  if(button == _cancel)
    {
      _cancelled = true;
      stop();
    }
  else if(button == _ok)
    {
      _cancelled = false;
      _selected = _box->selectedEntry().title;
      stop();
    }
  else if(button == _dirhier)
    {
      BFileManager* fm =  BFileManager::get();
      fm->changeDirectory(fm->currentDirectory() + "/..");
      fillBox();
    }
}

void
BFileSelector::onEntryClick(BListbox* box, BListbox::Entry& entry)
{
  BFileManager* fm = BFileManager::get();
  std::string ext = fm->filenameExtension(entry.title);
  if(_filetypes.size())
    {
      for(unsigned int i=0; i<_filetypes.size(); i++)
	{
	  if(_filetypes[i] == ext)
	    {
	      _ok->setState(BWidget::STATE_ENABLED);
	      return;
	    }
	}
      _ok->setState(BWidget::STATE_DISABLED);
    }
}

void
BFileSelector::onEntryDoubleClick(BListbox* box, BListbox::Entry& entry)
{
  _selected = entry.title;

  BFileManager* fm = BFileManager::get();
  if(fm->typeOfFile(entry.title) == BFileManager::TYPE_DIRECTORY)
    {
      fm->changeDirectory(fm->currentDirectory() + "/" + entry.title);
      fillBox();
      return;
    }
  else
    {
      if(_filetypes.size())
	{
	  std::string ext = fm->filenameExtension(entry.title);
	  for(unsigned int i=0; i<_filetypes.size(); i++)
	    {
	      if(_filetypes[i] == ext)
		{
		  stop();
		  return;
		}
	    }
	}
      else
	stop();
    }
}

void
BFileSelector::addFileType(const std::string& filetype)
{
  _filetypes.push_back(toLower(filetype));
}

void
BFileSelector::run()
{
  fillBox();
  BDialog::run();
}

void
BFileSelector::fillBox()
{
  BFileManager* fm = BFileManager::get();
  std::vector<BFileManager::FileAndType> contents = fm->directoryContents();
  std::sort(contents.begin(), contents.end(), SortFiles);

  if(fm->currentDirectory() == "/")
    _dirhier->setText("/");
  else
    _dirhier->setText(fm->lastPathComponent(fm->currentDirectory()));
  
  _box->clear();
  for(unsigned int i=0; i<contents.size(); i++)
    {
      if(contents[i].filename[0] == '.' ||
	 contents[i].filename[0] == '_')
	continue;

      if(contents[i].filetype == BFileManager::TYPE_DIRECTORY)
	_box->addEntry(contents[i].filename, RGB15(0, 0, 15)|BIT(15));
      else
	{
	  std::string ext = fm->filenameExtension(contents[i].filename);
	  uint16 color = RGB15(0, 0, 0)|BIT(15);
	  if(_filetypes.size())
	    {
	      color = RGB15(15, 15, 15)|BIT(15);
	      for(unsigned int i=0; i<_filetypes.size(); i++)
		{
		  if(_filetypes[i] == ext)
		    {
		      color = RGB15(0, 0, 0)|BIT(15);
		      break;
		    }
		}
	    }
	  _box->addEntry(contents[i].filename, color);
	}
    }
}

#if BFileSelector_test
int main(int argc, char **argv)
{
}
#endif /* BFileSelector_test */
