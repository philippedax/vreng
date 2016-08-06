/************************************************************************
 *
 *  usymbol.cpp : graphical symbols that can be embedded in elements
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/usymbol.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uview.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uon.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

// symbol margin
#define SM_GETSIZE 4
#define SM 0

USymbol USymbol::left(S_LEFT, UCONST);
USymbol USymbol::right(S_RIGHT, UCONST);
USymbol USymbol::up(S_UP, UCONST);
USymbol USymbol::down(S_DOWN, UCONST);
USymbol USymbol::check(S_CHECK, UCONST);
USymbol USymbol::radio(S_RADIO, UCONST);
USymbol USymbol::square(S_SQUARE, UCONST);
USymbol USymbol::circle(S_CIRCLE, UCONST);

/* ==================================================== [Elc] ======= */

USymbol::USymbol(const USymbol &o) : UData() {
  ix = o.ix;
  color = o.color;
  frontShadowColor = o.frontShadowColor;
  backShadowColor = o.backShadowColor;
}

USymbol::USymbol(int index, UConst m) : UData(m) {
  ix = index;
  color = null;
  frontShadowColor = null;
  backShadowColor = null;
}

void USymbol::set(const USymbol& o) {  // a completer!
  ix = o.ix;
  color = o.color;
  frontShadowColor = o.frontShadowColor;
  backShadowColor = o.backShadowColor;
  changed(true);
}

void USymbol::update() {
  _parents.updateAutoParents(UUpdate::LAYOUT_PAINT);
}

void USymbol::setColor(const UColor &c) {
  color = &c;
}
void USymbol::setFrontShadowColor(const UColor &c) {
  frontShadowColor = &c;
}
void USymbol::setBackShadowColor(const UColor &c) {
  backShadowColor = &c;
}

void USymbol::getSize(UUpdateContext& ctx, UDimension& dim) const {
  // les tailles des symbols dependent de celles des fonts !
  // on enleve SM_GETSIZE pour eviter que ca aille du haut jusqu'en bas
  dim.height = UFontMetrics(ctx).getHeight() - SM_GETSIZE;
    
  // on rend le truc pair car necessaire pour que les fleches... tombent juste
  //if (h % 2 != 0) (h)--;   ????
  dim.width = dim.height; // format carre
}

void USymbol::paint(UGraph& g, UUpdateContext& ctx, const URect& r) const {
  // xl, yl = length for obtaining the last point (= width-1, height-1)
  // dont't forget to remove the left and right margin (SM)
  float xl = r.width  - 2*SM - 1;
  float yl = r.height - 2*SM - 1;

  bool active;
  if (ctx.obj->isSelected())
    active = (ctx.obj->getInterState() != UOn::ARMED);
  else 
    active = (ctx.obj->getInterState() == UOn::ARMED);

  const UColor *back, *fore, *symcolor;
  symcolor = color ? color : &UColor::darkgrey;

  if (active) {
    fore = backShadowColor ? backShadowColor : &UColor::black;
    back = frontShadowColor ? frontShadowColor : &UColor::white;
  }
  else {
    back = backShadowColor ? backShadowColor : &UColor::black;
    fore = frontShadowColor ? frontShadowColor : &UColor::white;
  }

  vector<UPoint> p(3);

  switch (ix) {
    /*
  case S_HSLIDER:
    g.drawIma(&_hslider, r.x, r.y);
    break;

  case S_VSLIDER:
    if (r.height >= 20)
      g.drawIma(&_vslider, r.x, r.y);
    else {
      g.drawIma(&_vslider_top, r.x, r.y);
      g.drawIma(&_vslider_bottom, r.x, r.y + _vslider_top.getHeight());
    }
    break;
    */
    /*
    px[0] = r.x+SM,    py[0] = r.y+SM;
    px[1] = r.x+xl,  py[1] = r.y+yl;

    if (active) {
      g.setColor(symcolor);
      g.fillRect(px[0], py[0], px[1] - px[0], py[1] - py[0]);
    }
    // backShadow = bottom+right
    g.setColor(back);
    g.drawLine(px[0], py[1], px[1], py[1]); // bottom
    g.drawLine(px[1], py[0], px[1], py[1]); // right

    // frontShadow = top+left
    g.setColor(fore);
    g.drawLine(px[0], py[0], px[1], py[0]); // top
    g.drawLine(px[0], py[0], px[0], py[1]); // left
    */

  case S_DOWN:
    p[0].x = r.x+SM,      p[0].y = r.y+SM;
    p[1].x = r.x+SM+xl/2, p[1].y = r.y+SM+yl;
    p[2].x = r.x+SM+xl,   p[2].y = r.y+SM;
    // dans cet ordre
    g.setColor(*symcolor);
    g.fillPolygon(p);
    g.setColor(*fore);
    g.drawLine(p[0], p[1]);
    g.drawLine(p[0], p[2]);
    g.setColor(*back);
    g.drawLine(p[1], p[2]);
   break;

  case S_UP:
    p[0].x = r.x+SM,      p[0].y = r.y+SM+yl;
    p[1].x = r.x+SM+xl/2, p[1].y = r.y+SM;
    p[2].x = r.x+SM+xl,   p[2].y = r.y+SM+yl;

    // dans cet ordre!!
    g.setColor(*symcolor);
    g.fillPolygon(p);
    g.setColor(*back);
    g.drawLine(p[1], p[2]);
    g.drawLine(p[0], p[2]);
    g.setColor(*fore);
    g.drawLine(p[0], p[1]);
    break;

  case S_RIGHT:
    p[0].x = r.x+SM,     p[0].y = r.y+SM;
    p[1].x = r.x+SM+xl,  p[1].y = r.y+SM+yl/2;
    p[2].x = r.x+SM,     p[2].y = r.y+SM+yl;

    g.setColor(*symcolor);
    g.fillPolygon(p);
    g.setColor(*fore);
    g.drawLine(p[0], p[2]);
    g.drawLine(p[0], p[1]);
    g.setColor(*back);
    g.drawLine(p[1], p[2]);
    break;

  case S_LEFT:
    p[0].x = r.x+SM+xl,  p[0].y = r.y+SM;
    p[1].x = r.x+SM,     p[1].y = r.y+SM+yl/2;
    p[2].x = r.x+SM+xl,  p[2].y = r.y+SM+yl;
    g.setColor(*symcolor);
    g.fillPolygon(p);
    g.setColor(*back);
    g.drawLine(p[1], p[2]);
    g.drawLine(p[0], p[2]);
    g.setColor(*fore);
    g.drawLine(p[0], p[1]);
    break;

  case S_RADIO:	  // diamond
    p[0].x = r.x+SM+xl/2,  p[0].y = r.y+SM;
    p[1].x = r.x+SM,       p[1].y = r.y+SM+yl/2;
    p[2].x = r.x+SM+xl/2,  p[2].y = r.y+yl;
    p.push_back(UPoint(r.x+xl, r.y+SM+yl/2));  // 4 points

    if (active) {
      g.setColor(*symcolor);
      g.fillPolygon(p);
    }
    g.setColor(*fore);
    g.drawLine(p[0], p[1]);
    g.drawLine(p[0], p[3]);

    g.setColor(*back);
    g.drawLine(p[1], p[2]);
    g.drawLine(p[2], p[3]);
   break;

  case S_CHECK:
    p[0].x = r.x+SM+1,             p[0].y = r.y+SM+1;
    p[1].x = r.x+ r.width-(SM+1),  p[1].y = r.y + r.height-(SM+1);

    if (active) {
      g.setColor(*symcolor);
      g.fillRect(p[0].x, p[0].y, (p[1].x - p[0].x), (p[1].y - p[0].y));
    }
    // backShadow = bottom+right
    g.setColor(*back);
    g.drawLine(p[0].x, p[1].y, p[1].x, p[1].y); // bottom
    g.drawLine(p[1].x, p[0].y, p[1].x, p[1].y); // right

    // frontShadow = top+left
    g.setColor(*fore);
    g.drawLine(p[0].x, p[0].y, p[1].x, p[0].y); // top
    g.drawLine(p[0].x, p[0].y, p[0].x, p[1].y); // left
    break;

  case S_SQUARE:
    if (xl-1 <= 0 || yl-1<=0) return;
    g.setColor(*fore);
    g.drawRect(r.x+SM+1, r.y+SM+1, xl-1, yl-1);
    g.setColor(*back);
    g.drawRect(r.x+SM, r.y+SM, xl-1, yl-1);
   break;

      /*
  case S_SMALL_SQUARE:
    if (xl-5 <= 0 || yl-5<=0) return;
    g.setColor(*fore);
    g.drawRect(r.x+SM+6, r.y+SM+6, xl-5, yl-5);
    g.setColor(*back);
    g.drawRect(r.x+SM+5, r.y+SM+5, xl-5, yl-5);
    break;
    */
      
  case S_CIRCLE:
    if (xl-1 <= 0 || yl-1<=0) return;
    g.setColor(*symcolor);
    g.fillArc(r.x+SM+1, r.y+SM+1, xl-1, yl-1, 0, 360);

    g.setColor(*back);
    g.drawArc(r.x+SM+1, r.y+SM+1, xl-1, yl-1, 0, 360);

    g.setColor(*fore);
    g.drawArc(r.x+SM, r.y+SM, xl-1, yl-1, 0, 360);

    break;

  default:
    break;
  }
}

}
