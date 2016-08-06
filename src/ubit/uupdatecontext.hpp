/************************************************************************
 *
 *  uupdatecontext.hpp: [implementation] stack context
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

#ifndef _uupdatecontext_hpp_
#define	_uupdatecontext_hpp_ 1
#include <ubit/udefs.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/ufontImpl.hpp>
#include <ubit/uview.hpp>
#include <vector>
namespace ubit {
  
  // ====[internal implementation]=========================================
  // NOTE: this header is part of the Ubit intrinsics and subject to change
  
  /* [impl] UUpdateContext = a subcontext (not the first layer of the context cstack)
   * and a base for UWinUpdateContext (the first layer of the cstack)
   */
  class UUpdateContext {
  public:  
    UUpdateContext(const UUpdateContext& parent_ctx, UElem*, UView*, UViewUpdateImpl*);
    ///< creates a subcontext (not the first layer of the context stack)
    
    UElem* getObj() {return obj;}
    ///< returns the object (a UElem, a UElem or a UBox) that is related to this context.
    
    UBox* getBoxParent() {return view->getBoxParent();}
    /*< returns the direct or *indirect* box parent of getObj().
     * 'boxes' are UBox objects or objects that derive from UBox.
     */  
    
    UView* getView() {return view;}
    ///< returns the view related to this context.
    
    UView* getWinView() {return view->getWinView();}
    ///< returns the window view that contains getView().
    
    UDisp* getDisp() const {return view->getDisp();}  
    // const necessaire pour certains appels
    
    UGraph* getGraph() {return graph;}

    void rescale();
    ///< rescales the coordinates according to the current scale.
    
    void addFlagdef(const UFlagdef*);
    const UFlagdef* getFlagdef(const UFlag&) const;
    const UFlagdef* getFlagdef(const UFlag*) const;
    const UPropdef* getPropdef(const UFlag&) const;
    const UPropdef* getPropdef(const UFlag*) const;
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    // cette implementation est dangereuse car dans certains cas win_ctx est undef
    const UUpdateContext* parent_ctx;
    UWinUpdateContext* win_ctx;  // not const because of the flags vector
    unsigned short flag_count;   // number of valid flagdefs for this stack level
    
    UElem* obj;
    UView* view;
    UViewUpdateImpl* view_impl;
    const UStyle* obj_style;
    ULocalProps local;
    UPos* pos;                // UPos ou U3dpos, UPos peut etre proportionnelle
    UFontDesc fontdesc;
    bool boxIsHFlex, boxIsVFlex; // true if the object is Horiz or Vert Flexible
    char valign, halign; 
    float xyscale;            // current scale
    float vspacing, hspacing;
    UEdit* edit;
    const UColor *color, *bgcolor;
    const UCursor *cursor;
    UGraph* graph;

  protected:
    friend class UViewFind;
    UUpdateContext() {}  // pour UViewFind
    UUpdateContext(UView* win_view); // for UWinUpdateContext
#endif
  };
  
  /* ==================================================== ======== ======= */
  /* [impl] UWinUpdateContext = the first layer of the context cstack
   */
  class UWinUpdateContext : public UUpdateContext {
  public:
    UWinUpdateContext(UView* winview, UGraph*);
    /**< creates the first layer of the context stack
     * !Warning: 'win_view' must be a valid (NOT null) window view!
     */
        
  private:
    friend class UUpdateContext;
    std::vector<const UFlagdef*> flags;
  };
  
  /* ==================================================== ======== ======= */
  
  // cette implementation est dangereuse car dans certains cas win_ctx est undef
  //inline UView*  UUpdateContext::getWinView() {return win_ctx->view;}
  //inline UDisp*  UUpdateContext::getDisp() const {return win_ctx->view->getDisp();}
  //inline UGraph* UUpdateContext::getGraph() {return win_ctx->graph;}
  
}
#endif
