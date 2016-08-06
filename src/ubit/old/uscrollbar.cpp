/************************************************************************
 *
 *  uscrollbar.cpp
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
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/uscrollbar.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/upix.hpp>
#include <ubit/usymbol.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


// USliderbutton for scrollbars.
struct USliderbuttonStyle : public UStyle {
  USliderbuttonStyle();
  virtual const UStyle& getStyle(UUpdateContext*) const;
private:
  UStyle vstyle;
};

const UStyle& USliderbuttonStyle::getStyle(UUpdateContext* ctx) const {
  bool is_horiz = true;
  UBox* parent_box = null;
  if (ctx && (parent_box = ctx->getBoxParent())) {
    is_horiz = !parent_box->isVertical();
  }
  if (is_horiz) return *this; else return vstyle;
}

USliderbuttonStyle::USliderbuttonStyle() {
  orient = UOrient::HORIZONTAL;
  halign = UHalign::LEFT;
  valign = UValign::CENTER; //TOP;
  hspacing = 0;
  vspacing = 0;  
  //local.size.set(36,UPX, 15,UPX);
  local.size.width  = 36|UPX;
  local.size.height = 15|UPX;
  local.padding.set(0,0);
  {
    UBackground* b = new UBackground(UPix::hslider);
    b->setTiling(false); // NOT tiled;
    local.background = b;
  }

  vstyle.orient = UOrient::VERTICAL;
  vstyle.halign = UHalign::CENTER; //LEFT;
  vstyle.valign = UValign::TOP;
  vstyle.hspacing = 0;
  vstyle.vspacing = 0;
  //vstyle.local.size.set(15,UPX, 36,UPX);
  vstyle.local.size.width  = 15|UPX;
  vstyle.local.size.height = 36|UPX;
  vstyle.local.padding.set(0,0);
  {
    UBackground* b = new UBackground(UPix::vslider);
    b->setTiling(false); // NOT tiled;
    vstyle.local.background = b;
  }
}

class USliderbutton: public UButton {
public:
  UCLASS(USliderbutton)
  USliderbutton(const UArgs& a = UArgs::none) : UButton(a) {}
  static UStyle* createStyle() {return new USliderbuttonStyle();}
};

/* ==================================================== [Elc] ======= */
// UScrollbutton for scrollbars.

struct UScrollbuttonStyle : public UStyle {
  UScrollbuttonStyle();
  //virtual const UStyle& getStyle(UUpdateContext*) const;
};

/*
const UStyle& UScrollbuttonStyle::getStyle(UUpdateContext* ctx) const {
  bool is_horiz = true;
  if (ctx && ctx->parent_box) {
    is_horiz = !ctx->parent_box->isBmode(UMode::IS_VERTICAL);
  }
  if (is_horiz) return this;
  else return vstyle;
  return this;
}
*/

UScrollbuttonStyle::UScrollbuttonStyle() {
  orient = UOrient::INHERIT;
  halign = UHalign::CENTER;
  valign = UValign::CENTER;
  setBgcolor(UOn::ENTERED, UColor::lightblue);
  local.padding.set(0,0);
}

class UScrollbutton: public UButton {
public:
  UCLASS(UScrollbutton)
  UScrollbutton(const UArgs& a = UArgs::none) : UButton(a) {}
  static UStyle* createStyle() {return new UScrollbuttonStyle();}
};

/* ==================================================== [Elc] ======= */

UBox* UScrollbar::createLessButton(bool vertical) {
  UBox* scroller;
  if (vertical) scroller = new UScrollbutton(USymbol::up);  
  else scroller = new UScrollbutton(USymbol::left);
  scroller->setAutoRepeat(true);
  scroller->disableMenuClosing();
  return scroller;
}

UBox* UScrollbar::createMoreButton(bool vertical) {
  UBox* scroller;
  if (vertical) scroller = new UScrollbutton(USymbol::down);
  else scroller = new UScrollbutton(USymbol::right);
  scroller->setAutoRepeat(true);
  scroller->disableMenuClosing();
  return scroller;
}

UBox* UScrollbar::createRail(bool vertical) {
  UBox* rail = new UBox(UBorder::shadowIn);
  rail->setAutoRepeat(true);
  rail->disableMenuClosing();
  rail->ignoreEvents();
  return rail;
}

UBox* UScrollbar::createKnob(bool vertical) {
  UBox* slider = new USliderbutton();
  slider->setArmable(false);
  slider->setCrossable(false);
  slider->disableMenuClosing();
  return slider;
}

/* ==================================================== [Elc] ======= */

struct USbStyle : public UStyle {
  float rail_thickness;
  float rail_alpha;
  UBackground rail_background;
};

struct UScrollbarStyle : public UStyle {
  UScrollbarStyle();
  virtual const USbStyle& getStyle(UScrollbar*) const;
  virtual const UStyle& getStyle(UUpdateContext*) const;
private:
  USbStyle ovstyle, ohstyle, tvstyle, thstyle;
};


const USbStyle& UScrollbarStyle::getStyle(UScrollbar* sb) const {    
  if (sb && !sb->isVertical())
    return (sb && sb->isTransparent()) ? thstyle : ohstyle;
  else
    return (sb && sb->isTransparent()) ? tvstyle : ovstyle;
}

const UStyle& UScrollbarStyle::getStyle(UUpdateContext* ctx) const {
  UElem* obj = (ctx && ctx->obj) ? ctx->obj : null;
  UScrollbar* sb = obj ? dynamic_cast<UScrollbar*>(obj) : null;
  return getStyle(sb);
}

/* ==================================================== ======== ======= */

static void setTransparent(USbStyle& style) {
  style.local.border = &UBorder::none;
  style.rail_alpha = 0.35;
  style.rail_thickness = 6;
  style.rail_background = UColor::grey;
}

static void setOpaque(USbStyle& style) {
  style.local.alpha = 1.0;
  style.rail_alpha = 1.0;
  style.rail_thickness = 5;
  style.rail_background = UColor::grey;
}

static void setVertical(USbStyle& style) {
  style.viewStyle = &UScrollbarView::style;
  // fix size and avoid useless updates (cf. layoutView)
  //style.local.size.set(16,UPX, -1,UAUTO);
  style.setSize(16|UPX, UAUTO);
  style.hspacing = 0;
  style.vspacing = 0;  
  style.orient = UOrient::VERTICAL;
  style.halign = UHalign::CENTER;  // FLEX;
  style.valign = UValign::TOP;
  // DEVRAIT ETRE SUR rail_box pas sur SCROLL !
  style.local.padding.left = 1; //sinon impression rail decentre
}

static void setHorizontal(USbStyle& style) {
  style.viewStyle = &UScrollbarView::style;
  // fix size and avoid useless updates (cf. layoutView)
  //style.local.size.set(-1,UAUTO, 16,UPX);
  style.setSize(UAUTO, 16|UPX);
  style.hspacing = 0;
  style.vspacing = 0;  
  style.orient = UOrient::HORIZONTAL;
  style.halign = UHalign::LEFT;
  style.valign = UValign::CENTER;  //FLEX;
  style.local.padding.top = 1;
}

UScrollbarStyle::UScrollbarStyle() {
  setVertical(ovstyle);
  setOpaque(ovstyle);

  setHorizontal(ohstyle);
  setOpaque(ohstyle);

  setVertical(tvstyle);
  setTransparent(tvstyle);

  setHorizontal(thstyle);
  setTransparent(thstyle);
}

/* ==================================================== [Elc] ======= */

UScrollbar::UScrollbar(const UArgs& a) : pvalue(new UFloat) {
  // !! HACK: on specifie VERTICAL par defaut puis on ecrase par arglist !!
  emodes.IS_VERTICAL = 1;
  add(a);
  constructs(); 
}

UScrollbar::UScrollbar(UFloat& _value, const UArgs& a) : pvalue(_value) {
  emodes.IS_VERTICAL = 1;
  add(a);
  constructs();
}

//UScrollbar& uscrollbar(UFloat& v, const UArgs& a) {return *new UScrollbar(v, a);}
UScrollbar& uhscrollbar(const UArgs& a) {return *new UScrollbar(UOrient::horizontal + a);}

UScrollbar::~UScrollbar() {
  if (ppane) {
    if (this == ppane->getVScrollbar()) ppane->unsetVScrollbar();
    if (this == ppane->getHScrollbar()) ppane->unsetHScrollbar();
    ppane = null;
  }
}

UStyle* UScrollbar::createStyle() {
  return new UScrollbarStyle();
}

/* ==================================================== [Elc] ======= */

void UScrollbar::constructs() {
  unit_increment  = 15;
  block_increment = 75;
  press_rail_goto_pos = false;
  ppane = null;
  pless_btn = createLessButton(isVertical());
  pmore_btn = createMoreButton(isVertical());
  prail = new UBox;
  pknob = createKnob(isVertical());
  pknob_pos = new UPos(0,0);
  show_rail = true;
  is_transparent = false; //(_modes & TRANSP) != 0;
  tracking_mode = true;
  delta_mouse = 0;

  static UScrollbarStyle scrollbar_style;
  USbStyle& style = const_cast<USbStyle&>(scrollbar_style.getStyle(this));

  pknob->add
    (UOn::mdrag / ucall(this, &UScrollbar::dragKnob)
     + UOn::mpress / ucall(this, &UScrollbar::pressKnob)
     + UOn::mrelease / ucall(this, &UScrollbar::releaseKnob)
     + *pknob_pos
     );
  
  if (pless_btn)
    pless_btn->addAttr(UOn::mpress / ucall(this,-1, &UScrollbar::pressScrollButton));
  if (pmore_btn)
    pmore_btn->addAttr(UOn::mpress / ucall(this,+1, &UScrollbar::pressScrollButton));

  UBox* rail = null;
  if (show_rail && (rail = createRail(true))) {
    rail->addAttr(style.rail_background + ualpha(style.rail_alpha));

    // !! A REVOIR AVEC STYLES APPROPRIES !!
    if (isVertical()) {
      //rail->addAttr(uwidth(style.rail_thickness));
      rail->addAttr(new USize(style.rail_thickness, UAUTO));
      prail->add(uvflex() + uhcenter() + rail);
    }
    else {
      //rail->addAttr(uheight(style.rail_thickness));
      rail->addAttr(new USize(UAUTO, style.rail_thickness));
      prail->add(uhflex() + uvcenter() + rail);
    }
  }

  // NB: bug car les scrollbars sont dans border ce qui fait echouer 
  // UEventFlow::mustCloseMenus => disableMenuClosing() compense ca
  prail->disableMenuClosing();

  prail->add
    (UOn::mpress  / ucall(this, &UScrollbar::pressRail)
     + UOn::mrelease / ucall(this, &UScrollbar::releaseRail)
     + *pknob
     );

  if (isVertical()) this->add(uhflex() + uvflex() + *prail + ubottom());
  else this->add(uhflex() + uvflex() + *prail + uright());
    
  if (pless_btn) add(*pless_btn);
  if (pmore_btn) add(*pmore_btn);
}

/* ==================================================== [Elc] ======= */

//bool UScrollbar::isVertical() const {return hasBMode(UMode::IS_VERTICAL);}
//bool UScrollbar::isHorizontal() const {return !hasBMode(UMode::IS_VERTICAL);}

void UScrollbar::setPane(UScrollpane* p) {ppane = p;}

bool UScrollbar::isTracking() const {return tracking_mode;}
UScrollbar& UScrollbar::setTracking(bool state) 
{tracking_mode = state; return *this;}

bool UScrollbar::isTransparent() const {return is_transparent;}
UScrollbar& UScrollbar::setTransparent(bool state) 
{is_transparent = state; return *this;}

float UScrollbar::getUnitIncrement() const {return unit_increment;}
UScrollbar& UScrollbar::setUnitIncrement(float val) 
{unit_increment = val; return *this;}

float UScrollbar::getBlockIncrement() const {return block_increment;}
UScrollbar& UScrollbar::setBlockIncrement(float val) 
{block_increment = val; return *this;}

float UScrollbar::getValue() const {return pvalue->floatValue();}

UScrollbar& UScrollbar::setValue(float _val) {
  if (ppane) {
    // if a pane is attached to this scrollbar, recompute xscroll, yscroll
    // before moving the scrollbar
    
    if (isVertical()) ppane->setScrollImpl(ppane->getXScroll(), _val);
    else ppane->setScrollImpl(_val, ppane->getYScroll());
    
    //if (isVertical()) ppane->setScroll(ppane->getXScroll(), _val);
    //else ppane->setScroll(_val, ppane->getYScroll());
  }
  else setValueImpl(_val, true);
  return *this;
}

//cette fonction fait n'importe quoi
void UScrollbar::setValueImpl(float _val, bool upd_pane) {
  if (_val < 0) _val = 0; else if (_val > 100) _val = 100.0;
  
  //if (_val == value) return;   // !faux: marche pas quand ajout
  *pvalue = _val;
  
  // move the scrollbar, updates display, calls callbacks
  if (upd_pane) {
    // NB: on pourrait optimiser quand on reaffiche pas l'autre scrollbar
    //if (!UAppli::conf.soft_dbf && (is_transparent || show_rail)) upd.softDbf();
    update();
  }
  
  UEvent e(UOn::change, this);  //UElemEvent
  fire(e);
}

/* ==================================================== [Elc] ======= */

float UScrollbar::getPercent(UMouseEvent& e, UView *slider_view, UView *rail_view,
                             float delta_mouse) {
  UPoint ipos = e.getPosIn(*rail_view);
  if (isVertical()) {
    float remaining_h = rail_view->getHeight() - slider_view->getHeight();
    return ipos.y / remaining_h * 100.0;
  }
  else {
    float remaining_w = rail_view->getWidth() - slider_view->getWidth();
    return ipos.x / remaining_w * 100.0;
  }
}

float UScrollbar::getIncrementPercent(UMouseEvent& e, float increment) {
  UView* v = null;
  if (ppane) v = ppane->getScrolledView(ppane->getView(e));
  else v = getView(e);

  if (isVertical()) {
    return (float)increment / v->getHeight() * 100;
  }
  else {
    return (float)increment / v->getWidth() * 100;
  }
}
  
/* ==================================================== ======== ======= */

void UScrollbar::pressKnob(UMouseEvent& e) {
  // delta_mouse : decalage pointe par rapport a slider
  if (isVertical()) delta_mouse = e.getY();
  else delta_mouse = e.getX();
}

void UScrollbar::dragKnob(UMouseEvent& e) {  
  UView *slider_view = e.getView();
  UView *rail_view = slider_view->getParentView();
  float _val = getPercent(e, slider_view, rail_view, delta_mouse);
  
  // ne rien faire si on sort du range (cad si la souris sort du scrollbar)
  // penible: on n'arrive pas a mettre le scrollbar a 0 ou a 100
  // if (_val >= 0 && _val <= 100) setValue(_val);
  if (tracking_mode) setValue(_val);
  else setValueImpl(_val, true);  // devrait etre false ???
}

void UScrollbar::releaseKnob(UMouseEvent& e) {
  // dragSlider(e); plutot penible car bouge le slider qunad on clique
  // dessus sans modifier sa position (a cause de la maniere dont sont
  // calculees les positions)
  // MAIS necessaire dans le cas non incremental
  
  if (!tracking_mode) {
    UView *slider_view = e.getView();
    UView *rail_view = slider_view->getParentView();
    float _val = getPercent(e, slider_view, rail_view, delta_mouse);
    setValue(_val);
  }
  
  //effet: le slider se synchronise avec la pos ou on a lache' la souris
  // meme si on est sorti de la fenetre (car dans ce cas, a moins qu'il
  // n'y ait un xgrab, le scrollbar ne suit plus la souris)
}

/* ==================================================== ======== ======= */

void UScrollbar::pressRail(UMouseEvent& e) {
  UView *rail_view = e.getView();
  UView *slider_view = pknob->getViewInImpl(rail_view /*,null*/);
  float value = pvalue->floatValue();
  float incr = getIncrementPercent(e, block_increment);
  
  // normal_scroll 
  if (!press_rail_goto_pos && e.getButton() == UAppli::conf.getMouseSelectButton()) {
    float pressed_val = getPercent(e, slider_view, rail_view, 0);
    if (pressed_val > value) {
      if (value + incr > pressed_val) setValue(pressed_val);
      else setValue(value + incr);
    }
    else {
      if (value - incr < pressed_val) setValue(pressed_val);
      else setValue(value - incr);
    }
  }
  
  // scroll_to_pos
  else if (press_rail_goto_pos || e.getButton() == UAppli::conf.getMouseAltButton()) {
    setValue(getPercent(e, slider_view, rail_view, 0));
  }
}

void UScrollbar::releaseRail(UMouseEvent& e) {}

/* ==================================================== ======== ======= */

void UScrollbar::pressScrollButton(UMouseEvent& e, int dir) {  
  float incr = getIncrementPercent(e, unit_increment);

  // faire en sorte qu'on arrive toujours au bords en cliquant sur les
  // more & less buttons meme si la valeut tombe pas juste et sort du
  // range 0,100
  float _val = pvalue->floatValue() + incr * dir;
  if (_val < 0) _val = 0;
  else if (_val > 100) _val = 100.0;
  setValue(_val);
}

// ==================================================== [Elc] ===============
// UScrollbarView

UViewStyle UScrollbarView::style(&UScrollbarView::createView, UCONST);

UScrollbarView::UScrollbarView(UBox* box, UView* parview, UHardwinImpl* w)
: UView(box, parview, w) {
  //scrollmin = scrollmax = 0;
}

UView* UScrollbarView::createView(UBox* box, UView* parview, UHardwinImpl* w) {
  return new UScrollbarView(box, parview, w);
}

//NB1: il faudrait un ajustement proportionnel
//NB2: att aux boucles infinies et effet pendulaires, d'ou les deux 'if':
//il ne faut pas qu'une reduction de la taille fasse passer dans l'autre
// cas sinon on boucle

static float adjustSliderWidth(UView* slider_view, UView* rail_view) {
  float remaining_w = rail_view->getWidth() - slider_view->getWidth();

  if (remaining_w < 25) {
    slider_view->width = 16;    // DEPEND DE SCALE ETC: A REVOIR !!!!!!
    remaining_w = rail_view->getWidth() - slider_view->getWidth();
  }
  else if (remaining_w > 45) {
    slider_view->width  = 36;
    remaining_w = rail_view->getWidth() - slider_view->getWidth();
  }
  return remaining_w;
}

static float adjustSliderHeight(UView* slider_view, UView* rail_view) {
  float remaining_h = rail_view->getHeight() - slider_view->getHeight();

  if (remaining_h < 25) {
    slider_view->height = 16;
    remaining_h = rail_view->getHeight() - slider_view->getHeight();
  }
  else if (remaining_h > 45) {
    slider_view->height = 36;
    remaining_h = rail_view->getHeight() - slider_view->getHeight();
  }
  return remaining_h;
}


void UScrollbarView::doUpdate(UUpdateContext& parctx, URect r, URect clip, UViewUpdate& vup)
{
  // l'intersection de clip et de r est affectee dans clip
  if (clip.doIntersection(r) == 0) return;
  
  UScrollbar* sb = (UScrollbar*)getBox();
  
  // il faut appeler DEUX fois doUpdate:
  // - la premiere pour calculer correctement la taille du UScrollbarView
  //   (car doLayout ne suffit pas)
  // - la seconde vraiment pour tout afficher correctement 
  //   a la bonne position (c'est a dire APRES avoir bouge le slider)
  
  // ne rien reafficher juste mettre a jour data
  UViewUpdate updata(UViewUpdate::UPDATE_DATA);
  UView::doUpdate(parctx, r, clip, updata);
  
  UView* rail_view = sb->prail->getViewInImpl(this /*,null*/);
  UView* slider_view = sb->pknob->getViewInImpl(rail_view /*,null*/);
  
  if (sb->isVertical()) {
    float remaining_h = adjustSliderHeight(slider_view, rail_view);
    // repositionner le slider
    // ATT: set(,,false) ==> no update; surtout ne pas appeler move car 
    // ca rappellerait cette meme fonction => boucle infinie
    // NB: centrer horizontalement
    
    sb->pknob_pos->set((getWidth() - slider_view->getWidth()) / 2,
                        (sb->getValue() * remaining_h / 100.0));
                        //false);
  }
  else {  // horizontal
    float remaining_w = adjustSliderWidth(slider_view, rail_view);
    
    sb->pknob_pos->set((sb->getValue() * remaining_w / 100.0),
                        (getHeight() - slider_view->getHeight()) / 2);
                        //false);
  }
  
  // voir remarque ci-dessus sur le fait qu'il faut appeler 2 FOIS doUpdate
  UView::doUpdate(parctx, r, clip, vup);
}

}
