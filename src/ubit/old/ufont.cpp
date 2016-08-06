/************************************************************************
 *
 *  ufont.cpp: Font Attribute
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/ucall.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ufontImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uconf.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

static const float MEDIUM_FONT_SIZE = 12;


/* NOTES
 * - la specif 'normal' deconne sous certains Linux
 * - italique est tantot specifie par 'o' ou par 'i'
 * - fixed n'a pas forcement d'italique (ou alors d'une taille delirante)
static const FontSize FONT_SIZES[] = {
  {2,"2"}, 
  {6,"6"},        // XX_SMALL
  {7,"7"}, 
  {8,"8"},        // X_SMALL
  {9,"9"}, 
  {10,"10"},      // SMALL
  {11,"11"}, 
  {12,"12"},      // MEDIUM
  {14,"14"},      // LARGE
  //{15,"15"}, 
  {16,"16"},      // X_LARGE
  //{17,"17"}, 
  {18,"18"},
  //{19,"19"}, 
  {20,"20"},      // XX_LARGE
  // {22,"22"},   // word - degueu sur le Mac
  {24,"24"},      // word, X
  //{26,"26"},    // word - degueu sur le Mac
  //{28,"28"},    // word - degueu sur le Mac
  //{30,"30"},
  //{32,"32"},
  //{33,"33"},
  {34,"34"},      // X
  //{36,"36"},    // word - degueu sur le Mac
  {48,"48"},      // word
  {72,"72"}       // word
};
*/
/* ==================================================== ===== ======= */
//NB: UFontFamily created in uconf.cpp to avoid init dependency problems 

UFont UFont::plain(null, -BOLD|-ITALIC, 0, UCONST); // NOT bold nor Italic
//UFont& UFont::normal = UFont::plain;

UFont UFont::bold(null,      BOLD,      0, UCONST);
UFont UFont::italic(null,    ITALIC,    0, UCONST);
UFont UFont::fill(null,      FILL,      0, UCONST);
UFont UFont::underline(null, UNDERLINE, 0, UCONST);
UFont UFont::overline(null,  OVERLINE,  0, UCONST);
UFont UFont::strikethrough(null, STRIKETHROUGH, 0, UCONST);

UFont UFont::xx_small(null,  0, 6,  UCONST);
UFont UFont::x_small(null,   0, 8,  UCONST);
UFont UFont::small(null,     0, 10, UCONST);
UFont UFont::medium(null,    0, 12, UCONST);
UFont UFont::large(null,     0, 14, UCONST);
UFont UFont::x_large(null,   0, 16, UCONST);
UFont UFont::xx_large(null,  0, 20, UCONST);

UFont UFont::sans_serif(&UFontFamily::sans_serif, 0,0, UCONST);
UFont UFont::serif(&UFontFamily::serif, 0,0, UCONST);
UFont UFont::monospace(&UFontFamily::monospace, 0,0, UCONST);
//UFont UFont::fixed(&UFontFamily::fixed, 0,0, UCONST);

/* ==================================================== ===== ======= */
 
int UFontFamily::family_count = 0;

UFontFamily::UFontFamily(const char* _name, UConst) {     // constr PRIVE
  // defs = defaults.defs; PAS pour ce constr PPRIVE
  ffindex = -1;
  ready = false;
  name = _name;
}

UFontFamily::UFontFamily(const UStr& _name) {   // constr PUBLIC
  defs = defaults.defs;  // recopier les defaults !
  ffindex = -1;
  ready = false;
  name = _name;
}

UFontFamily& UFontFamily::operator=(const UFontFamily& f2) {
  defs = f2.defs;
  fsizes.clear();
  ready = false;
  return *this;
}

void UFontFamily::setFamily(const char* _families) {
  defs.families = _families;
  ready = false;
}

void UFontFamily::setStyles(const char* _normal_style, const char* _italic_style) {
  if (_normal_style) defs.normal_style = _normal_style;
  if (_italic_style) defs.italic_style = _italic_style;
  ready = false;
}
  
void UFontFamily::setWeights(const char* _normal_weight, const char* _bold_weight) {
  if (_normal_weight) defs.normal_weight = _normal_weight;
  if (_bold_weight) defs.bold_weight = _bold_weight;
  ready = false;
}

void UFontFamily::setCompression(const char* arg) {
  defs.compression = arg;
  ready = false;
}

void UFontFamily::setEncoding(const char* arg) {
  defs.encoding = arg;
  ready = false;
}

void UFontFamily::setPreferredSizes(const char* arg) {
  defs.preferred_sizes = arg;
  fsizes.clear();
  ready = false;
}

void UFontFamily::setFreeType(const char* regular_files, 
                              const char* bold_files,
                              const char* italic_files, 
                              const char* bold_italic_files) {
  defs.fty_regular = regular_files;
  defs.fty_bold = bold_files;
  defs.fty_italic = italic_files;
  defs.fty_bold_italic = bold_italic_files;
  ready = false;
}

void UFontFamily::initSizes() const {
  if (fsizes.size() > 0) return;
  const char* p1 = defs.preferred_sizes.c_str();
  char *p2 = 0;
  while (true) {
    long l = strtol(p1, &p2, 0);
    if (l == 0 || p2 == p1) break;
    fsizes.push_back(int(l));
    if (*p2 == ',') p2++;
    p1 = p2;
  }
}

/* ==================================================== ===== ======= */
//ATT: faut retourner la plus proche QUI EST PLUS PETITE !
// (si on retourne une taille plus grande le texte va etre clippe
//  ou ca va entrainer des passages a la ligne

int UFontFamily::sizeToIndex(float size) const {
  if (size <= 0) return 0;
  if (fsizes.size() == 0) initSizes();

  // NB: CECI IMPLIQUE UNE LIMITE MAX DE TAILLE DES FONTS !
  if (size >= fsizes[fsizes.size()-1]) 
    return fsizes.size()-1;
  
  // NB: pas tres efficace!
  int smaller = 0;
  for (unsigned int n = 0; n < fsizes.size(); ++n) {
    if (fsizes[n] > size) break;
    smaller = n;
  }
  
  return smaller;
}

/* ==================================================== ===== ======= */

//private constr
UFont::UFont(const UFontFamily* ff, int _styles, float pixsize, UConst m) : UAttr(m) {
  family = ff;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

UFont::UFont(const UFontFamily& ff, int _styles, float pixsize) {
  family = &ff;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

UFont::UFont(const UFont& f, int _styles, float pixsize) {
  family = f.family;
  if (pixsize <= 0) fsize = 0; else fsize = pixsize;
  if (_styles > 0) {on_styles = _styles; off_styles = 0;}
  else {on_styles = 0; off_styles = -_styles;}
}

UFont::UFont(const UFont& f) {
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles= f.off_styles;
}

UFont::UFont() {
  UFont& f = *UAppli::conf.default_font;
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles = f.off_styles;
}

/* ==================================================== ======== ======= */

void UFont::set(const UFont& f) {
  if (checkConst()) return;
  if (equals(f)) return;  
  family = f.family;
  fsize = f.fsize;
  on_styles = f.on_styles;
  off_styles = f.off_styles;
  changed(true);
}

void UFont::merge(const UFont& f) {
  if (checkConst()) return;
  
  if (f.family) family = f.family;
  if (f.fsize > 0) fsize = f.fsize;   // 0 means don't impose size

  on_styles  = on_styles  | f.on_styles;  // combines styles
  off_styles = off_styles | f.off_styles;
  changed(true);
}

bool UFont::equals(const UFont& f) const {
  if (this == &f) return true;
  else return (family == f.family && fsize == f.fsize
               && on_styles == f.on_styles && off_styles == f.off_styles);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
UFont& UFont::setFamily(const UFont &f) {
  if (checkConst()) return *this;
  family = f.family;
  changed(true);
  return *this;
}
 UFont& UFont::setPixelSize(const UFont &f) {
 if (checkConst()) return *this;
 size = f.size;
 changed(true);
 return *this;
 }
 UFont& UFont::setStyles(const UFont &f) {
 if (checkConst()) return *this;
 on_styles  = f.on_styles;
 off_styles = f.off_styles;
 changed(true);
 return *this;
 }
*/
  
UFont& UFont::setFamily(const UFontFamily& ff) {
  if (checkConst()) return *this;
  family = &ff;
  changed(true);
  return *this;
}

UFont& UFont::setFamily(const UStr& font_families) {
  if (checkConst()) return *this;
  //family = ...;
  changed(true);
  return *this;
}
/*
 void UStyleProps::setFontFamily(UFont& f, const UStr& _val) {
 UStr v;
 int pos = _val.find(',');
 if (pos > 0) v = _val.substring(0, pos);
 else v = _val;
 v.trim();
 
 if (isEq(v,"sans-serif") || isEq(v,"helvetica") || isEq(v,"arial"))
 f.setFamily(UFont::sans_serif.getFamily());
 else if (isEq(v,"serif") || isEq(v,"times"))
 f.setFamily(UFont::serif.getFamily());
 else if (isEq(v,"monospace") || isEq(v,"courier"))
 f.setFamily(UFont::monospace.getFamily());
 //else if (isEq(v,"fixed"))
 //  f.setFamily(UFont::fixed);
 }
 
 */

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UFont& UFont::setSize(const UStr& size) {
  if (checkConst()) return *this;
  float val = 0;
  const char* unit = null;
  if (!size.scanValue(val, unit)) return *this;   // no value specified

  if (!unit || !*unit || UCstr::equals(unit,"px",true))
    return setPixelSize(val); 
  else if (UCstr::equals(unit,"pt",true))
    return setPointSize(val); 
  else if (UCstr::equals(unit,"%",true))
    return setPointSize(MEDIUM_FONT_SIZE * val / 100.);
  else {
    error("UFont::setSize","wrong font unit");
    return *this;
  }
}

UFont& UFont::setPixelSize(float s) {
  if (checkConst()) return *this;
  if (s <= 0) fsize = 0;    // 0 means don't impose a specific size
  else fsize = s;
  changed(true);
  return *this;
}

UFont& UFont::setPointSize(float s) {
  if (checkConst()) return *this;
  if (s <= 0) fsize = 0;
  else fsize = s * UAppli::getDisp()->PT_TO_PX;  // !!! devrait etre adapté au UDisp
  //cerr << "fsize " << fsize << " " << UAppli::getDisp()->PT_TO_PX<<endl;
  // Point: A unit of measure used by printers, equal to 1/72 inch.
  // EX: static const double POINT_TO_PIXEL = 1.33;  // conventionnel, a la louche
  changed(true);
  return *this;
}

float UFont::getPointSize() const {
  return fsize / UAppli::getDisp()->PT_TO_PX;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UFont& UFont::setStyles(int s) {
  if (checkConst()) return *this;
  if (s > 0) {on_styles = s; off_styles = 0;}
  else {on_styles = 0; off_styles = -s;}
  changed(true);
  return *this;
}

UFont& UFont::changeStyles(int s, bool add) {
  if (checkConst()) return *this;
  if (s > 0) {
    off_styles = off_styles & ~s;
    if (add) on_styles = on_styles | s;
    else on_styles = on_styles & ~s;
  }
  else {
    s = -s;
    on_styles = on_styles & ~s;
    if (add) off_styles = off_styles | s;
    else off_styles = off_styles & ~s;
  }
  changed(true);
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UFont::isPlain()  const {return on_styles == 0;}
bool UFont::isBold()   const {return (on_styles & BOLD) != 0;}
bool UFont::isItalic() const {return (on_styles & ITALIC) != 0;}
bool UFont::isFilled() const {return (on_styles & FILL) != 0;}
bool UFont::isUnderlined() const {return (on_styles & UNDERLINE) != 0;}
bool UFont::isOverlined()  const {return (on_styles & OVERLINE) != 0;}
bool UFont::isStrikethrough() const {return (on_styles & STRIKETHROUGH) != 0;}

UFont& UFont::setBold(bool state) {
  return changeStyles(UFont::BOLD, state);
}

UFont& UFont::setItalic(bool state) {
  return changeStyles(UFont::ITALIC, state);
}

UFont& UFont::setFilled(bool state) {
  return changeStyles(UFont::FILL, state);
}

UFont& UFont::setUnderlined(bool state) {
  return changeStyles(UFont::UNDERLINE, state);
}

UFont& UFont::setOverlined(bool state) {
  return changeStyles(UFont::OVERLINE, state);
}

UFont& UFont::setStrikethrough(bool state) {
  return changeStyles(UFont::STRIKETHROUGH, state);
}

/* ==================================================== ===== ======= */

void UFont::update() {
  updateAutoParents(UUpdate::LAYOUT_PAINT);  // size changed in both directions
}

// This method initializes a UFont for a given UDisplay (or UAppli)
// It returns true if the font could be found and false otherwise
// (a default font will be used in this case)
// NOTE: it is not necessary to call this function explicitely since
// version 99.10 (it will automatically be called the first time 
// this UFont is used for drawing a string on the screen)
//
void UFont::realize(UDisp* d) {
  if (!d) d = UAppli::getDisp();
  d->realizeFont(*this);
}

/* ==================================================== ======== ======= */

void UFont::putProp(UUpdateContext* props, UElem&) {
  UFontDesc& fd = props->fontdesc;
  if (family) fd.family = family;
  
  // ajouter les 'on_styles' et enlever les 'off_styles'
  fd.styles = (fd.styles | on_styles) & ~off_styles;

  //NB: size = 0 means "any size" and thus, does not change df.size 
  if (fsize > 0) fd.def_size = fsize;
  fd.setScale(props->xyscale);
}

/* ==================================================== ======== ======= */

UFontDesc::UFontDesc(const UFont& f) {
  set(f);
}

void UFontDesc::set(const UFont& f) {
  family = f.family;
  styles = f.on_styles;
  def_size = f.fsize; 
  scaled_size = def_size;
  findex = -1; 
  actual_size = 0;  // NOT INIT!
}

void UFontDesc::merge(const UFont& f) {
  if (f.family) family = f.family;
  // NB: size = 0 means "any size" and thus, does not change f.size 
  if (f.fsize > 0) {def_size = f.fsize; scaled_size = def_size;}  
  styles = styles | f.on_styles;   // combine styles
  findex = -1; 
  actual_size = 0;  // NOT INIT!
}

void UFontDesc::setScale(float xyscale) {
  if (def_size == 0) def_size = UFont::medium.getPixelSize();
  scaled_size = def_size * xyscale;     // scaled pixel size
  if (!family) {
    UAppli::internalError("UFontDesc::setScale","null font family; UFontDef: %p", this);
    family = UAppli::conf.default_font->family;
  }
  // find font index from scaled pixel size
  findex = family->sizeToIndex(scaled_size);
  actual_size = family->fsizes[findex];

  // calculate font index and actual rescaled point size
  findex = (findex << 2) + (styles & UFont::BOLD) + (styles & UFont::ITALIC);
}

}

