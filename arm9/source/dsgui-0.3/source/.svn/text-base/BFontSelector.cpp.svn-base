// BFontSelector.cpp (this is -*- C++ -*-)
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
#include "BFontSelector.h"
#include "BFontManager.h"
#include "BStringUtils.h"

static char* testtext = "The quick brown fox jumped over the lazy dog. 12345 67890 äöüÄÖÜß !\"§$% &/()= \\|{} #+*_- .,;:@";

BFontSelector::BFontSelector(BScreen* screen):
  BDialog(screen, BSize(180, 150))
{
  _namesBox = new BListbox(this, BRect(2, 2, 134, 57));
  _namesBox->setTextAlignment(BWidget::ALIGN_LEFT);
  _namesBox->setReliefType(BWidget::RELIEF_SIMPLE);
  _namesBox->setDelegate(this);
  
  BScroller *s = new BScroller(this, BRect(136, 2, 10, 57));
  _namesBox->setVScroller(s);

  BFontManager *fm = BFontManager::get();
  const std::vector<std::string>& names = fm->fontNames();
  for(unsigned int i=0; i<names.size(); i++)
    _namesBox->addEntry(names[i]);

  _sizesBox = new BListbox(this, BRect(148, 2, 20, 57));
  _sizesBox->setTextAlignment(BWidget::ALIGN_LEFT);
  _sizesBox->setReliefType(BWidget::RELIEF_SIMPLE);
  _sizesBox->setDelegate(this);
  
  s = new BScroller(this, BRect(168, 2, 10, 57));
  _sizesBox->setVScroller(s);

  _boldButton = new BButton(this, BRect(2, 59, 87, 15));
  _boldButton->setText("Bold");
  _boldButton->setType(BButton::BT_TOGGLE);
  _boldButton->setDelegate(this);
  
  _italicButton = new BButton(this, BRect(91, 59, 87, 15));
  _italicButton->setText("Italic");
  _italicButton->setType(BButton::BT_TOGGLE);
  _italicButton->setDelegate(this);
  
  _preview = new BText(this, BRect(2, 76, 166, 50));
  //  _preview->setTextAlignment(BWidget::ALIGN_LEFT);
  _preview->setReliefType(BWidget::RELIEF_SIMPLE);
  _preview->setText(testtext);
  _preview->setHPadding(2);

  s = new BScroller(this, BRect(168, 76, 10, 50));
  _preview->setVScroller(s);

  _okButton = new BButton(this, BRect(128, 128, 50, 20));
  _okButton->setDelegate(this);
  _okButton->setText("OK");

  _cancelButton = new BButton(this, BRect(76, 128, 50, 20));
  _cancelButton->setDelegate(this);
  _cancelButton->setText("Cancel");

  setFont(_preview->font());
}

void
BFontSelector::setFont(BFont* font)
{
  _namesBox->selectEntry(font->fontHeader().name);
  updateSizesBox();
  _sizesBox->selectEntryWithTag(font->fontHeader().height);
  updateVariants();
  if(font->fontHeader().fontflags & BFont::FLAG_BOLD)
    _boldButton->setValue(true);
  else
    _boldButton->setValue(false);
  if(font->fontHeader().fontflags & BFont::FLAG_OBLIQUE)
    _italicButton->setValue(true);
  else
    _italicButton->setValue(false);
    
  updatePreview();
}

void
BFontSelector::onEntryClick(BListbox* box, BListbox::Entry& e)
{
  if(box == _namesBox)
    {
      updateSizesBox();
      updateVariants();
      updatePreview();
    }
  else if(box == _sizesBox)
    {
      updateVariants();
      updatePreview();
    }
}

void
BFontSelector::onButtonClick(BButton* button)
{
  if(button == _okButton)
    {
      _cancelled = false;
      _font = _preview->font();
      stop();
    }
  else if(button == _cancelButton)
    {
      _cancelled = true;
      stop();
    }
  else if(button == _boldButton)
    {
      std::string name = _namesBox->selectedEntry().title;
      int size = _sizesBox->selectedEntry().tag;

      BFontManager *fm = BFontManager::get();
      if(!fm->hasFont(name, size, true, true))
	_italicButton->setValue(false);
      updatePreview();
    }
  else if(button == _italicButton)
    {
      std::string name = _namesBox->selectedEntry().title;
      int size = _sizesBox->selectedEntry().tag;

      BFontManager *fm = BFontManager::get();
      if(!fm->hasFont(name, size, true, true))
	_italicButton->setValue(false);
      updatePreview();
    }
}

void
BFontSelector::updateSizesBox()
{
  _sizesBox->clear();
  BFontManager *fm = BFontManager::get();
  std::vector<int> sizes = fm->sizesForFont(_namesBox->selectedEntry().title);
  for(unsigned int i=0; i<sizes.size(); i++)
    _sizesBox->addEntry(format("%d", sizes[i]),
			RGB15(0, 0, 0)|BIT(15),
			sizes[i]);
  _sizesBox->selectEntry(_preview->font()->fontHeader().height);
}

void
BFontSelector::updateVariants()
{
  std::string name = _namesBox->selectedEntry().title;
  int size = _sizesBox->selectedEntry().tag;

  BFontManager *fm = BFontManager::get();
  if(fm->hasFont(name, size, true, false) ||
     fm->hasFont(name, size, true, true))
    {
      _boldButton->setState(BWidget::STATE_ENABLED);

      if(_preview->font()->fontHeader().fontflags & BFont::FLAG_BOLD)
	_boldButton->setValue(true);
      else
	_boldButton->setValue(false);
    }
  else
    {
      _boldButton->setValue(false);
      _boldButton->setState(BWidget::STATE_DISABLED);
    }

  if(fm->hasFont(name, size, false, true) ||
     fm->hasFont(name, size, true, true))
    {
      _italicButton->setState(BWidget::STATE_ENABLED);

      if(_preview->font()->fontHeader().fontflags & BFont::FLAG_OBLIQUE)
	_italicButton->setValue(true);
      else
	_italicButton->setValue(false);
    }
  else
    {
      _italicButton->setValue(false);
      _italicButton->setState(BWidget::STATE_DISABLED);
    }
}

void
BFontSelector::updatePreview()
{
  std::string name = _namesBox->selectedEntry().title;
  int size = _sizesBox->selectedEntry().tag;
  bool bold = _boldButton->value();
  bool italic = _italicButton->value();

  BFont *font = BFontManager::get()->font(name, size, bold, italic);
  if(font)
    {
      _preview->setFont(font);
      _preview->setText(testtext);
    }
  
  else _preview->setText("Error");
}

void
BFontSelector::setRotation(Rotation rot)
{
  BWidget::setRotation(rot);
  switch(rot)
    {
    case ROT_0:
    case ROT_180:
      _frame.sz = BSize(180, 150);
      break;
    case ROT_90:
    case ROT_270:
      _frame.sz = BSize(150, 180);
      break;
    }
  center();
}

#if BFontSelector_test
int main(int argc, char **argv)
{
}
#endif /* BFontSelector_test */
