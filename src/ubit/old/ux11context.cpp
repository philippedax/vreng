/* ***********************************************************************
 *
 *  ugraphX11.cpp: native X11 graphics
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#if (UBIT_WITH_X11 && WITH_2D_GRAPHICS)

#include <iostream>
#include <cmath>
#include <ubit/udefs.hpp>
#include <ubit/uima.hpp>
#include <ubit/upix.hpp>
#include <ubit/udispX11.hpp>
#include <ubit/uhardima.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/ux11context.hpp>
#include <ubit/uhardfont.hpp>
using namespace std;

/* NOTE concernant X11 et le parametre gcval.line_width :
 * width != 0 provoque des resultats quasi aleatoires a cause de pbms d'arrondis
 * et de regles tres pas claires (cf. man pages) ainsi le dernier point apparait
 * ou pas selon les cas ! 
 * DE PLUS: si line_width > 1 le trait deborde en epaisseur tantot a gauche, 
 * tantot a droite AINSI: il est preferable de conserver line_width = 0 et 
 * d'imprimer plusieurs lignes cote a cote quand on veut savoir PRECISEMENT 
 * ce que l'on va obtenir au pixel pres
 */
#define _NAMESPACE_UBIT namespace ubit {  // avoid crazy indentation
_NAMESPACE_UBIT

// polylines and polygons stored in the stack if card<=POLYCARD, heap memory used otherwise.
static const int POLYCARD = 100;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UX11context::UX11context(UDisp* d) : URenderContext(d) 
{  
  sys_disp = ((UDispX11*)d)->getSysDisp();
  UDispX11* _d = (UDispX11*)d;
  XGCValues gcval;
  gcval.function = GXcopy;
  gcval.foreground = BlackPixelOfScreen(_d->getSysScreen());
  gcval.background = WhitePixelOfScreen(_d->getSysScreen());
  // graphics_exposures = true means that events are generated when
  // XCopyArea or XCopyPlanes is called and a part of the view is obscured
  // This is useless here except when scrolling data (the last argument 
  // of the UGraph::copyArea function can be used to change this
  // behavior (see fct. copyArea() in ugraph.hpp for details)
  gcval.graphics_exposures = false;
  
  // Note: The GC can be used with any destination drawable having 
  // the same root and depth as the specified drawable. 
  sys_gc = XCreateGC(sys_disp, _d->getSysFrame(),
                     GCFunction | GCForeground | GCBackground | GCGraphicsExposures, 
                     &gcval);
  //cerr << "!!!! NEW X11 gc: " << this << " sys_gc: "<< sys_gc<<endl;
}

UX11context::~UX11context() {
  XFreeGC(sys_disp, sys_gc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::makeCurrent() const {
}

void UX11context::swapBuffers() {
}

void UX11context::flush() {
  XFlush(sys_disp);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
void UX11context::resetGraph() {
  XGCValues gcval;
  gcval.function = GXcopy;
  gcval.line_width = 0;
  XChangeGC(sys_disp, sys_gc, GCFunction|GCLineWidth, &gcval);  
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
void UX11context::resetClip() {
  XRectangle c = {(int)clip.x, (int)clip.y, (unsigned int)clip.width, (unsigned int)clip.height};
  XSetClipRectangles(sys_disp, sys_gc, 0, 0, &c, 1, Unsorted);
}
*/

void UX11context::setClip(double x, double y, double width, double height) {
  clip.setRect(x, y, width, height);
  XRectangle c = {(int)x, (int)y, (unsigned int)width, (unsigned int)height};
  XSetClipRectangles(sys_disp, sys_gc, 0, 0, &c, 1, Unsorted);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setPaintMode(UGraph& g) {
  XGCValues gcval;
  gcval.foreground = g.color;
  gcval.function = GXcopy;  
  XChangeGC(sys_disp, sys_gc, GCFunction|GCForeground, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setXORMode(UGraph& g, const UColor& bg) {
  g.bgcolor = bg.getPixel(disp);
  // seul le champ 'gcval.foreground' sert pour le XOR
  // cette valeur est elle meme un XOR de foreground ET alternate_color
  XGCValues gcval;
  gcval.foreground = g.color ^ g.bgcolor;
  gcval.function = GXxor;  
  XChangeGC(sys_disp, sys_gc, GCFunction|GCForeground, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setColor(UGraph& g, const UColor& c) {
  g.color = c.getPixel(disp);
  
  XGCValues gcval;        //NB: alpha pas pris en compte sous X11
  if (g.in_xor_mode)
    gcval.foreground = g.color ^ g.bgcolor;
  else 
    gcval.foreground = g.color;
  XChangeGC(sys_disp, sys_gc, GCForeground, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setBackground(UGraph& g, const UColor& c) {
  g.bgcolor = c.getPixel(disp);
  XGCValues gcval;
  gcval.background = g.bgcolor;
  XChangeGC(sys_disp, sys_gc, GCBackground, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setFont(UGraph& g, const UFontDesc& fd) {
  XGCValues gcval;
  gcval.font = g.font->sysf->fid;
  XChangeGC(sys_disp, sys_gc, GCFont, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::setWidth(UGraph&, double w) {  
  XGCValues gcval;
  gcval.line_width = int(w);
  XChangeGC(sys_disp, sys_gc, GCLineWidth, &gcval);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::drawLine(double x1, double y1, double x2, double y2) const {
  XDrawLine(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, 
            int(xwin + x1), int(ywin + y1), int(xwin + x2), int(ywin + y2));  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::drawRect(double x, double y, double w, double h, bool filled) const {
  if (filled)
    XFillRectangle(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, 
                   int(xwin + x), int(ywin + y), int(w), int(h));
  else
    XDrawRectangle(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, 
                   int(xwin + x), int(ywin + y), int(w), int(h));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::drawArc(double x, double y, double w, double h, 
                            double start, double ext, bool filled) const 
{
  if (filled) 
    XFillArc(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc,
             int(xwin + x), int(ywin + y), int(w), int(h), 
             int(start * 64.), int(ext * 64.));  // !!att: au *64 !  
  else
    XDrawArc(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, 
             int(xwin + x), int(ywin + y), int(w), int(h), 
             int(start * 64.), int(ext * 64.));     //att: au *64 !
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::drawString(const UHardFont*, const char* str, int str_len, 
                               double x, double y) const 
{
  XDrawString(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc,
              int(xwin + x), int(ywin + y), str, str_len);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).

void UX11context::drawPolygon(const float* coords2d, int card, int polytype) const {
  if (card <= 0 || coords2d == null) return;
  int card2 = card * 2;
  
  USysPoint pstack[POLYCARD+1];
  USysPoint* pmem = null;
  USysPoint* syspts = card < POLYCARD ? pstack : (pmem = new USysPoint[card+1]);
  for (int k = 0, kk = 0; k < card2; k++, kk += 2) {
    syspts[k].x = int(coords2d[kk] + xwin);
    syspts[k].y = int(coords2d[kk+1] + ywin);
  }
  
  if (polytype == UGraph::FILLED)
    XFillPolygon(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, Complex, CoordModeOrigin);
  else if (polytype == UGraph::LINE_LOOP) {
    syspts[card].x = int(coords2d[0] + xwin);
    syspts[card].y = int(coords2d[1] + ywin);
    card++;
    XDrawLines(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, CoordModeOrigin);
  }
  else XDrawLines(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, CoordModeOrigin);
  
  delete[] pmem;
}


void UX11context::drawPolygon(const std::vector<UPoint>& points, int polytype) const {
  int card = points.size();
  if (card <= 0) return;
  
  USysPoint pstack[POLYCARD+1];
  USysPoint* pmem = null;
  USysPoint* syspts = card < POLYCARD ? pstack : (pmem = new USysPoint[card+1]);
  for (int k = 0; k < card; ++k) {
    syspts[k].x = int(points[k].x + xwin);
    syspts[k].y = int(points[k].y + ywin);
  }
  
  if (polytype == UGraph::FILLED)
    XFillPolygon(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, Complex, CoordModeOrigin);
  else if (polytype == UGraph::LINE_LOOP) {
    syspts[card].x = int(points[0].x + xwin);
    syspts[card].y = int(points[0].y + ywin);
    card++;
    XDrawLines(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, CoordModeOrigin);
  }
  else XDrawLines(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, syspts, card, CoordModeOrigin);
  
  delete[] pmem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 void drawNatIma(const class UHardIma&, float x, float y) const;
 void drawNatIma(const class UHardImaGL&, float x, float y, float scale = 1.) const;
 void drawNatIma(const class UHardIma2D&, float x, float y) const;
 void drawNatPix(const class UHardPix&, float x, float y) const;
 void UX11context::drawNatIma(const UHardIma& ima, float x, float y) const {
 drawNatIma((UHardIma2D&)ima, x, y);     // et si pixmaps ????
 */

void UX11context::drawIma(const UGraph& g, const UIma& ima, double x, double y, double scale) const {
  if (ima.isPix()) {
    UHardPix* np = ((UPix*)&ima)->getOrCreatePix(disp, scale);
    if (np) {
      drawHardPix(np, x, y);
      return;
    }
  }
  
  // recuperer bonne copie a la bonne echelle
  UHardIma* ni = ima.getOrCreateIma(disp, scale);
  if (ni) drawHardIma((UHardIma2D*)ni, x, y);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define DrawImage(D,Wn,G,I,Xs,Ys,Xd,Yd,W,H) XPutImage(D,Wn,G,I,int(Xs),int(Ys),int(Xd),int(Yd),int(W),int(H))

void UX11context::drawHardIma(const UHardIma2D* ni, double x, double y) const {
  // optimisation du clipping: 
  // on ne peut pas se contenter d'utiliser le GC pour le clipping
  // car il y a un transfert client -> serveur de l'image
  // et il faut donc clipper l'image source AVANT le transfert 
  // pour minimiser la quaantite de donnees a transferer
  
  // ima rect: rectangle de la location de l'ima dans target view
  // si l'image n'etait pas clippee
  URect imarect(x, y, ni->width, ni->height);
  
  // intersection d'imarect avec le clip courant
  bool clipstat = imarect.doIntersection(URect(clip.x - xwin, clip.y - ywin,
                                               clip.width, clip.height));
  if (!clipstat) return;  // rien a afficher
  
  // mask => shaped (ie "transparent") image
  /*  PBM: il faut creer le pixmap !
   if (ima->ximashape != None) {
   XSetClipOrigin(SYS_DISP, gc, gelem->xwin + x, gelem->ywin + y);
   XSetClipMask(SYS_DISP, gc, ima->ximashape);
   }
   */
  
  DrawImage(sys_disp, ((UHardwinX11*)dest)->sys_win, sys_gc, 
            ni->sys_ima, 
            imarect.x - x,	             // from in source ima
            imarect.y - y,
            imarect.x+ xwin,          // to in target view 
            imarect.y+ ywin,
            imarect.width, imarect.height);  // clipped size of source
  
  // !att: reset du Clip pour la suite
  /*
   if (ima->ximashape != None) {
   XSetClipRectangles(SYS_DISP, gc, 0,0, &gelem->cliprect, 1, Unsorted);
   }
   */
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::drawHardPix(const UHardPix* pix, double x, double y) const {
  // calculer la zone de clip du pixmap sur la destination
  // (c'est en particulier necessaire s'il y a un mask car XSetClipMask
  // prend le mask entier sans tenir compte du cliprect
  
  float clip_x = clip.x - (x + xwin);
  float clip_y = clip.y - (y + ywin);
  
  // mask => shaped (ie "transparent") image
  if (pix->sys_pixshape != None) {
    XSetClipOrigin(sys_disp, sys_gc, int(xwin+x), int(ywin+y));
    XSetClipMask(sys_disp, sys_gc, pix->sys_pixshape);
  }
  
  XCopyArea(sys_disp,
            pix->sys_pix,  // from
            ((UHardwinX11*)dest)->sys_win, // to
            sys_gc,           
            (int)clip_x, (int)clip_y, (int)clip.width, (int)clip.height, // from
            (int)clip.x, (int)clip.y);  // to
  
  // !att: reset du Clip pour la suite
  if (pix->sys_pixshape != None) {
    const URect& c = clip;
    XRectangle cliprect = {(int)c.x, (int)c.y, (unsigned int)c.width, (unsigned int)c.height};
    XSetClipRectangles(sys_disp, sys_gc, 0, 0, &cliprect, 1, Unsorted);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UX11context::copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                             bool generate_refresh_events_when_obscured) const
{
  XGCValues gcval;
  if (generate_refresh_events_when_obscured) {
    gcval.graphics_exposures = true;
    XChangeGC(sys_disp, sys_gc, GCGraphicsExposures, &gcval);  
  }
  
  XCopyArea(sys_disp, 
            ((UHardwinX11*)dest)->sys_win, // source
            ((UHardwinX11*)dest)->sys_win, // dest
            sys_gc,
            int(xwin + x), int(ywin + y), int(w), int(h),  // source
            int(xwin + x + delta_x), int(ywin + y + delta_y));  // dest
  
  if (generate_refresh_events_when_obscured) {
    gcval.graphics_exposures = false;
    XChangeGC(sys_disp, sys_gc, GCGraphicsExposures, &gcval);  
  }
  
  XFlush(sys_disp);
}

#if 0  // =========================================== [Ubit Toolkit] =========

void UGraph::begin(const URect& clip, bool xdbf) {
  if (!xdbf) gelem = URenderContext::push(URenderContext::WIN, this, clip, _hardwin, 0, 0);
  else {
    // deja en doublebuf: juste incrementer
    if (gelem && gelem->gtype == URenderContext::DBF) {gelem->push_count++; return;}
    gelem = URenderContext::push(URenderContext::DBF, this, clip, true);    // true => alloc buffer    
  }
}

/*
//void beginSubwin(const URect& clip, UHardwin* subwin, float xwin, float ywin);
void UGraph::beginSubwin(const URect& clip, UHardwin* subwin, float _xwin, float _ywin) {
  // cerr << ">>> beginSubwin: syswin " <<  ((UHardwinX11*)subwin)->getSysWin() <<endl;
  gelem = URenderContext::push(URenderContext::SUBWIN, this, clip, subwin, _xwin, _ywin);
}

void UGraph::beginDoublebuf(const URect& clip) {
  // deja en doublebuf: juste incrementer
  if (gelem && gelem->gtype == URenderContext::DBF) {gelem->push_count++; return;}
  gelem = URenderContext::push(URenderContext::DBF, this, clip, true);  // true => alloc buffer
}
*/

void UGraph::end() {
  if (!gelem) {
    UAppli::error("UGraph::end","unbalanced UGraph::begin()/end() requests");
    return;
  }
  switch (gelem->gtype) {
  case URenderContext::NONE:
    break;

  case URenderContext::CLIENT:
  case URenderContext::WIN:
    gelem = URenderContext::pop(this);
    break;

  case URenderContext::SUBWIN:
    gelem = URenderContext::pop(this);
    break;

  case URenderContext::DBF:
    Window dest_win = ((HARDWIN*)gelem->dest)->sys_win;
    
    if (gelem->push_count > 0) {gelem->push_count--; return;}
    // own_dest : dest reelement allouee pour ce gelem
    if (gelem->own_dest && dest_win != GNULL) {
      gelem->setClip(boxview->x, boxview->y, boxview->width, boxview->height);
      CopyArea(SYS_DISP, SYS_GC,
               // from:
               dest_win, 0, 0, (int)gelem->clipbegin.width, (int)gelem->clipbegin.height, 
               // to:
               ((HARDWIN*)_hardwin)->sys_win, (int)-gelem->xwin,(int)-gelem->ywin);
    }
    gelem = URenderContext::pop(this);
    break;

  case URenderContext::BLEND:
    if (gelem->push_count > 0) {gelem->push_count--; return;}
    // faire un blending de layer1 et layer2 et mettre le resultat dans layer1
    if (!gelem->prev) {
      UAppli::internalError("UGraph::end","soft blending: no first layer");
      gelem = URenderContext::pop(this);
      return;
    }
    URenderContext* layer1 = gelem->prev;
    URenderContext* layer2 = gelem;
    if (layer1 == layer2) UAppli::internalError("UGraph::end","soft blending: layer1 = layer2");
    blendLayers(layer1, layer2);
    gelem = URenderContext::pop(this);
    break;
    
  default:
    UAppli::error("UGraph::end","invalid end() requests");
    break;    
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UGraph::beginBlend(const URect& clip, float _alpha) {
  // deja en transpbuf: juste incrementer
  if (gelem && gelem->gtype == URenderContext::BLEND) {gelem->push_count++; return;}
  blend_mode = PSEUDO_BLEND;
  alpha = _alpha;
  gelem = URenderContext::push(URenderContext::BLEND, this, clip, true);  // true => alloc buffer
}

void UGraph::endBlend() {
  ?? alpha = 1.;
  end();
}

void UGraph::fillBackground(const URect& clip, const UColor& c, float _alpha) {
  if (_alpha == 1.) {
    setColor(c);
    fillRect(clip.x, clip.y, clip.width, clip.height);
  }
  else {
    if (gelem && gelem->gtype==URenderContext::BLEND) {gelem->push_count++; return;} //deja en transpbuf: juste incrementer
    setColor(c);
    blend_mode = UNIFORM_BLEND;
    alpha = _alpha;
    gelem = URenderContext::push(URenderContext::BLEND, this, clip, false);
    end();
  }
  /*
   if (curp.bgcolor) g.setColor(*curp.bgcolor);
   if (blend_paint) g.blendColor(clip, curp.local.alpha);
   else g.fillRect(clip.x, clip.y, clip.width, clip.height);
   */
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// X11 only! 
void UGraph::blendLayers(URenderContext* layer1, URenderContext* layer2) {
  USysIma xima1 = null, xima2 = null;
    
  if (((HARDWIN*)gelem->dest)->sys_win != GNULL && layer1->dest && layer2->dest) {
    URect clip2(-layer2->xwin, -layer2->ywin, 
                  layer2->clipbegin.width, layer2->clipbegin.height);
    int clipstat = 0;
    float xshift_in_clip = 0, yshift_in_clip = 0;  //1nov07
    
    if (layer1->gtype != URenderContext::DBF) {
      clipstat = clip2.doIntersection(*boxview); //securite
      xshift_in_clip = boxview->x - -layer2->xwin;
      yshift_in_clip = boxview->y - -layer2->ywin;
    }
    else {
      URect clip1(-layer1->xwin, -layer1->ywin, 
                    layer1->clipbegin.width, layer1->clipbegin.height);
      clipstat = clip2.doIntersection(clip1);
      clip2.x -= -layer1->xwin;
      clip2.y -= -layer1->ywin;
      if (clip2.x < 0) clip2.x = 0;
      if (clip2.y < 0) clip2.y = 0;
      
      xshift_in_clip = -layer1->xwin - -layer2->xwin; 
      yshift_in_clip = -layer1->ywin - -layer2->ywin; 
    }
    
    if (xshift_in_clip < 0) xshift_in_clip = 0;
    if (yshift_in_clip < 0) yshift_in_clip = 0;
    
    if (clipstat != 0) {
      // NB: les xwin ne sont pas forcement mappees et completement visible ce qui peut
      // provoquer une X11 error (mais elles sont toutes interceptees dans unatappli)
      //UXtry xtry;    now useless
      xima1 = GetImage(SYS_DISP, ((HARDWIN*)layer1->dest)->sys_win,
                       (int)clip2.x, (int)clip2.y, (int)clip2.width, (int)clip2.height);
      if (blend_mode != UNIFORM_BLEND)
        xima2 = GetImage(SYS_DISP, ((HARDWIN*)layer2->dest)->sys_win,
                         (int)xshift_in_clip, (int)yshift_in_clip, (int)clip2.width,(int) clip2.height);
    }
    if (!xima1 || (blend_mode != UNIFORM_BLEND && !xima2)) return;
    
    // testing mode: (required by XPutImage)
    // gelem->setClip(boxview->getXwin(), boxview->getYwin(), 
    //                boxview->getWidth(), boxview->getHeight());
    
    layer1->setClip(clip2.x, clip2.y, clip2.width, clip2.height);
    
    /* testing mode:   
      XPutImage(SYS_DISP,win-> natwin->xwin, gc, xima1,
                0, 0,	   //from in source ima
                0, 0,      //to in target view
                xima1->width, xima1->height);
      XPutImage(SYS_DISP, win->natwin->xwin, gc, xima2,
                0, 0,	     //from in source ima
                xima1->width+10,0,      //to in target view
                xima2->width, xima2->height);
    */
    
    if (blend_mode == UNIFORM_BLEND)
      UHardIma2D::blendSysIma(disp, xima1, color, alpha);
    else 
      //le resultat du blend est dans xima1
      UHardIma2D::blendSysIma(disp, xima1, xima2, alpha);
    
    DrawImage(SYS_DISP, ((HARDWIN*)layer1->dest)->sys_win, SYS_GC, 
              xima1, xshift_in_clip, yshift_in_clip,         // from
              clip2.x, clip2.y, clip2.width, clip2.height);  // to
    
    if (xima1) DestroyImage(xima1);
    if (xima2) DestroyImage(xima2); 
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//enum {SHARED_GRAPH = 1, STACKED_GRAPH = 2, CLIENT_GRAPH = 3}; //, SUBWIN, DBF, BLEND};

// pour natwin (TOP ou SUB_WINDOW)

URenderContext* URenderContext::push(int gtype, UGraph* g, const URect& clip, 
                           UHardwin* dest, double xwin, double ywin)
{  
  URenderContext* prev = g->gelem;
  if (!prev) {
    if (!shared_gelem) shared_gelem = create(URenderContext::NONE, g, null);
    prev = shared_gelem;
  }
  URenderContext* ge = create(gtype, g, prev);
  if (!ge) return null;
  ge->dest = dest;
  ge->xwin = -xwin;
  ge->ywin = -ywin;
  ge->setClip(clip.x + xwin, clip.y + ywin, clip.width, clip.height);
  ge->clipbegin.setRect(clip.x + xwin, clip.y + ywin, clip.width, clip.height);
  return ge;
}

// pour DOUBLE ET TRANSP
URenderContext* URenderContext::push(int gtype, UGraph* g, const URect& clip, bool alloc_buffer)
{
  URenderContext* prev = g->gelem;
  if (!prev) {
    if (!shared_gelem) shared_gelem = create(URenderContext::NONE, g, null);
    prev = shared_gelem;
  }  
  URenderContext* ge = create(gtype, g, prev);
  if (!ge) return null;
  
  if (alloc_buffer) {
    ge->dest = null;
#if WITH_2D_GRAPHICS
    if (clip.width > 0 && clip.height > 0) {
      HARDWIN* buf = ((UDispX11*)g->disp)->createWinImpl(null);
      buf->realize(UHardwin::PIXMAP, clip.width, clip.height);
      if (buf->getSysWin() != GNULL) ge->dest = buf;
    }
#else
    UAppli::error("UGraph","can't alloc PIXMAP in GLUT mode");
#endif
    ge->own_dest = (ge->dest != null);
  }  // end(alloc_buffer)
  
  else if (ge->prev) {
    ge->dest = ge->prev->dest;
    ge->own_dest = false;
  }
  
  else  {
    UAppli::fatalError("UGraph::push","wrong operation: empty graphic stack");
    //push(URenderContext::WIN, _g->_hardwin, clip, 0, 0);    // ???? _hardwin
    return null;
  }
  
  if (ge->dest != null) {
    ge->xwin = -clip.x;
    ge->ywin = -clip.y;
    ge->setClip(0, 0, clip.width, clip.height); 
    ge->clipbegin.setRect(0, 0, clip.width, clip.height); 
    return ge;
  }
  else {
    UAppli::fatalError("UGraph::push","wrong operation: null destination");
    return null;
    ***
    // il y a un show() dans uctlmenu.cpp qui provoque cette erreur
    // c'est un bug du show() mais a voir plus tard
    //pas assez de memoire (ou taille nulle) => simple recopie du precedent
    //UAppli::warning("UGraph::push","can't allocate X buffer %d,%d",clip.width, clip.height);
    if (ge->prev) {
      // essayer de sauver la situation en recopiant le precedent
      *ge = *(ge->prev);
      ge->gtype = _gtype;
      ge->push_count = 0;
      ge->own_dest = false; 
    }
    else {
      ge = URenderContext::popElem(gelem);
      push(UGraphElem::WIN, _g->_hardwin, clip, 0, 0);
    }
    ***
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UGraphElem* UGraphElem::pop(UGraph* g) {
  if (!g->gelem) {
    UAppli::fatalError("UGraph::pop","graphic stack out of range");
    return null;
  }
  UGraphElem* ge = g->gelem->prev;
  
  if (g->gelem->own_dest && g->gelem->dest != null) delete g->gelem->dest; // ???&&&
  if (g->gelem->gtype == UGraphElem::NONE)
    ; //cerr << "ERROR: should not delete this UGraphElem object !!!"<<endl; // &&&&&
  else delete g->gelem;
  
  if (ge) ge->resetClip();    //retablir le clip
  g->gelem = ge;
  return ge;
}

#endif

}
#endif
