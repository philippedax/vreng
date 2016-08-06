/************************************************************************
 *
 *  uselection.cpp: Text Selection
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
#include <ubit/ucolor.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ufont.hpp>
#include <ubit/uelem.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uselection.hpp>
#include <ubit/uappli.hpp>
using namespace std;
namespace ubit {

// !!! ATT: cette implementation est DANGEREUSE car il n'est pas sur que les
// iterateurs puissent tjrs etre initialises a null (cf aussi uchild.hpp)
const UChildIter USelection::null_link;

/* ==================================================== ===== ======= */

USelection::USelection(const UColor* fg, const UColor* bg,
                               const UFont* f) {
  pcolor   = fg ? new UColor(*fg) : null;
  pbgcolor = bg ? new UColor(*bg) : null;
  pfont    = f ? new UFont(*f) : null;
  rec_sel  = true;
  in_obj   = null;
  clear();
}

USelection::~USelection() {}

void USelection::copyText(UStr& res) {
  res.clear();
  perform(&res, false);
}

void USelection::cutText(UStr& res) {
  res.clear();
  perform(&res, true);
  clear();
}

void USelection::deleteText() {
  perform(null, true);
  clear();
}

void USelection::clear() {
  being_selected = false;
  pressLink = oldLink = UChildIter();
  endPressLink = UChildIter();
  pressPos = oldPos = 0;
  fromLink = toLink = UChildIter();
  fromPos  = toPos = 0;
  update(null);
  in_obj = null;
}

void USelection::setObj(UElem* obj) {
  in_obj = obj;
}

/* ==================================================== [Elc] ======= */

void USelection::update(UDataContext* p) {
  if (!in_obj) return;
  
  UChildren::iterator newLink = UChildIter(), endLink = UChildIter();
  long newPos = 0;
  
  if (p) {
    newLink = p->it;
    endLink = p->it2;
    newPos = p->strpos;
  }
  
  int refreshFromPos = 0, refreshToPos = 0;

  if (pressLink != null_link && pressLink != endPressLink
      && newLink != null_link && newLink != endLink) {
          
    if (newLink == pressLink) {
      fromLink = toLink = pressLink; 
      if (newPos > pressPos) {fromPos = pressPos; toPos = newPos;}
      else {fromPos = newPos; toPos = pressPos;}
    }
    
    else {
      bool found = false;
      for (UChildren::iterator c = pressLink; c != endPressLink; ++c) {
        if (c == newLink) {
          found = true;
          fromLink = pressLink;
          fromPos  = pressPos;
          toLink   = newLink;
          toPos    = newPos;
          break;
        }
      }
      
      if (!found) {
        for (UChildren::iterator c = newLink; c != endLink; ++c) {
          if (c == pressLink) {
            fromLink = newLink;
            fromPos  = newPos;
            toLink   = pressLink;
            toPos    = pressPos;
            break;
          }
        }
      }
      //NB: dans certains cas newLink appartient a un autre objet
      //et on n'arrive donc pas a trouver fromLink et toLink
      // c'est pas grave on garde les anciens
    }
          
    refreshFromPos = fromPos, refreshToPos = toPos;
      
    if (oldLink == fromLink && newPos == fromPos) {
      if (oldPos < fromPos) {
        refreshFromPos = oldPos;
        refreshToPos   = fromPos;
      }
      else {
        refreshFromPos = fromPos;
        refreshToPos   = oldPos;
      }
    }
    
    else if (oldLink == toLink && newPos == toPos) {
      if (oldPos < toPos) {
        refreshFromPos = oldPos;
        refreshToPos = toPos;
      }
      else {
        refreshFromPos = toPos;
        refreshToPos   = oldPos;
      }
    }
      
    oldLink = newLink;
    oldPos  = newPos;
  }
    
  paint(refreshFromPos, refreshToPos);
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// NB: cette fonction change egalement le mode IN_TEXTSEL des elems

void USelection::paint(long refreshFromPos, long refreshToPos) {
  bool initial_state = false;
  paintImpl(in_obj, initial_state, refreshFromPos, refreshToPos); //recursive);
}

void USelection::paintImpl(UElem* obj, bool state,
                           long refreshFromPos, long refreshToPos) {

  for (UChildIter c = obj->cbegin(); c != obj->cend(); ++c) {
    if (c == fromLink) state = true;
    UData* data = null;
    UElem* chgrp = null;
    
    if ((data = (*c)->toData())) {
          
      if (state || (!state && data->omodes.IS_IN_SELECTION)) {
        // allume ou eteint le mode IN_TEXTSEL de data suivant le cas
        data->omodes.IS_IN_SELECTION = state;
        
        // NO_DELAY evite le flicking (sinon maj faite a posteriori)
        //UUpdate upd(UUpdate::PAINT|UUpdate::NO_DELAY);
        UUpdate upd(UUpdate::PAINT);
    
        // ATT: il faut egalement reafficher les zones qui redeviennent 
        // non selectionneees et repassent en noir        
        UStr* str = data->toStr();
        
        if (!str) upd.setPaintData(data);
        else {
          if (c == fromLink && c == toLink)
            upd.setPaintStr(str, refreshFromPos, refreshToPos);	 
          else if (c == fromLink)
            upd.setPaintStr(str, refreshFromPos, str->length());
          else if (c == toLink)
            upd.setPaintStr(str, 0, refreshToPos);
          else 
            upd.setPaintStr(str, 0, str->length());
          
        }
               
        // update all parents (not only the inBox being selected)
        // doUpdate / NO_DELAY evite le flicking ?
        //obj->doUpdate(upd);
        obj->update(upd);
      }
      
    } // endif(item)
    
    else if (rec_sel && (chgrp = (*c)->toElem())) {
      // (initial) state = false ou true suivant le cas
      paintImpl(chgrp, state, refreshFromPos, refreshToPos); //, recursive);
    }
    
    //!att: a mettre a la fin du corps de boucle!
    if (c == toLink) state = false;
  }
}

/* ==================================================== [Elc] ======= */

static void xxx(UStr* copy, bool del, UStr* str, long frompos, long len) {
  if (copy) copy->insert(-1, *str, frompos, len);
  if (del) str->remove(frompos, len);
}

void USelection::perform(UStr* copy, bool del) {
  bool state = false;
  int refreshFromPos = fromPos, refreshToPos = toPos;
  if (!in_obj) return;
  
  const UStr* sep = in_obj->getTextSeparator();
  if (sep && sep->length() <= 0) sep = null;

  for (UChildIter c = in_obj->cbegin(); c != in_obj->cend(); ++c) {
    if (c == fromLink) state = true;
    
    if (state) {
      UNode* ch = *c;
      UElem* chgrp = null;

      if (ch->toData()) {
        UStr* str = ch->toStr();
        // NB: cas (data && !str) pas traite!
        if (str) {
          if (c == fromLink && c == toLink) {
            if (refreshToPos > refreshFromPos)
              xxx(copy, del, str, refreshFromPos, refreshToPos-refreshFromPos);
          }
          else if (c == fromLink) {
            xxx(copy, del, str, refreshFromPos, str->length());
          }
          else if (c == toLink) {
            if (0 < refreshToPos) xxx(copy, del, str, 0, refreshToPos);
          }
          else {
            // selection.append(str->chars());
            xxx(copy, del, str, 0, str->length());
          }
        } // endif(str)
      } // endif(data)


      else if (rec_sel && copy && (chgrp = ch->toElem())) {
        if (sep) copy->append(*sep);
        copy->append(chgrp->retrieveText(true));
      }
    } // endif(state)
    
    //!att: a mettre a la fin du corps de boucle!
    if (c == toLink) state = false;
  }
}

/* ==================================================== [Elc] ======= */

void USelection::start(UMouseEvent& e) {
  if (e.getSource() && e.getButton() == UAppli::conf.getMouseSelectButton()) {
    
    // effacer selection courante (ce qui necessite ancien obj)
    clear();

    // nouvelle selection
    in_obj = e.getSource();
    if (!in_obj) return;
    being_selected = true;

    UDataContext dc;
    if (!e.getStr(dc)) in_obj = null;
    else {
      pressLink = dc.it;
      endPressLink = dc.it2;
      pressPos = dc.strpos;
    }
  }
}

/* ==================================================== ======== ======= */

void USelection::extend(UMouseEvent& e) {
  if (!in_obj || e.getSource() != in_obj) return;
  // si on a localise une string, changer la selection (sinon on ne fait rien
  // pour ne pas abimer inutilement la selection precedente)
  UDataContext dc;
  if (e.getStr(dc)) update(&dc);
}

/* ==================================================== ======== ======= */

bool USelection::complete(UMouseEvent& e) {
  being_selected = false;   // on n'est plus en train de selectionner le texte

  return (in_obj && fromLink != in_obj->cend() && toLink != in_obj->cend()
          && (fromLink != toLink || fromPos != toPos));
}

/* ==================================================== ======== ======= */

void USelection::keyPress(UKeyEvent& e) {
  // toute entree clavier qui renvoie un char efface la selection
  // et paste doit virer la selection (ou du moins la remplacer)
  if (e.getKeyChar()) clear();
}

}
/* ==================================================== [TheEnd] ======= */

