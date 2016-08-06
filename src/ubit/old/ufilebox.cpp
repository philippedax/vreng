/************************************************************************
 *
 *  ufilebox.cpp: File Box Element
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
#include <algorithm>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  
#include <dirent.h>
#include <ctime>
#include <ubit/ubit.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
namespace ubit {

UFilebox& ufilebox(const UArgs& a) {return *new UFilebox(a);}

UFilebox::~UFilebox() {
  // objects are implicitely deleted as they are filebox children
}

UFilebox::UFilebox(const UArgs& args) :
  fname(new UStr), fdir2(new UStr), ffilter(new UStr), 
  fpath(new UStr), /*viewport(null), */ autoclose(true) 
{
  static UBorder& btn_border = 
  *new URoundBorder(+UBorder::ETCHED, UColor::darkgrey, UColor::white,2,2,10,10);
    
  fspec = "./";  // => scan du directory courant
  *ffilter = "*.*";

  this->disableMenuClosing();

  show_list = &ubutton(btn_border + uvcenter() + UFont::bold + " = " 
                       + ucall(this, &UFilebox::rescan));
  show_list->setSelectable(true);
  
  show_hidden_files = &ubutton(btn_border + uvcenter() + UFont::bold + " . " 
                               + ucall(this, &UFilebox::rescan));
  show_hidden_files->setSelectable(true);

  static UStr* goto_parent = new UStr("..");
  static UStr* goto_default = new UStr("./");

  UBox& tbar = uhbox
    (uhspacing(1) + upadding(3,3) 
     + uleft()
     + uitem(UPix::bigUp + ucall(this, goto_parent, &UFilebox::changeDirImpl))
     + uitem(UPix::bigRight + ucall(this, goto_default, &UFilebox::setDirImpl))
     + uhflex()
     + utextfield(UPix::ofolder + fspec
		  //ca va pas: on reappellerait rescan a tort et a travers
		  // des qu'on reformatte fspec: ucall(this, &UFilebox::rescan, UOn::change)
     + UOn::ktype / ucall(this, &UFilebox::rescan))
     + uright()
     + uvcenter()
     + " "
     + show_list
     + " "
     + show_hidden_files
     );

  // chaque fois que le filtre est modifier, soit interactivement, 
  // soit par appel de la fct. setFilter(), on rescanne la liste
  ffilter->onChange( ucall(this, &UFilebox::rescan) );

  UButton& ok = ubutton(btn_border + UFont::bold + "OK" 
                        + ucall(this, &UFilebox::okBehavior));
  UButton& cancel = ubutton(btn_border + "Cancel" 
                            + ucall(this, &UFilebox::cancelBehavior));
  UBox& controls = 
  uhbox(upadding(0, 5)
        + uleft()
        + uvbox(UValign::center + UFont::bold
                + ulabel("File Name") + ulabel("Filters"))
        + " " 
        + uhflex()
        + uvbox(uvspacing(4)
                + utextfield(*fname + ucall(this, &UFilebox::okBehavior))
                // NB: ffilter est reactif et associe a rescan() (cf plus haut)
                + utextfield(*ffilter))
        + uright()
        + " " 
        + uvbox(UFont::bold + ok + cancel)
        );
  
  mainbox = new UBox();
  mainbox->addAttr(uhflex() + uvflex());
  
  addAttr(UOrient::vertical + uvspacing(5) + upadding(4,4)
          + new URoundBorder(UBorder::LINE,UColor::black,UColor::white,2,2,15,15));
  
  add(utop()
      + args  //pbm place des args!
      + tbar
      + uvflex()  + mainbox
      + ubottom() + controls
      );
  
  rescan();   //devrait etre a l'apparition du dialog
}

/* ==================================================== ====== ======= */

void UFilebox::showHiddenFiles(bool st) {
  show_hidden_files->setSelected(st);
}

void UFilebox::showList(bool st) {
  show_list->setSelected(st);
}

/* ==================================================== [Elc:] ======= */
// separer le chemin et du repertoire et le reste (remain) qui
// servira de filtre
// -- reduit s_dir si necessaire : enleve filter et / final
// -- renvoie nouvelle longueur de s_dir
// -- met le filter dans *sremain si sremain!= null
//    (il faudra faire un delete de *sremain si != null)

static int separ(char *s_dir, char **s_remain) {
  int l_dir = strlen(s_dir);
  if (s_remain) *s_remain = null;

  //chercher si dir se termine par un / (et l'enlever)
  if (s_dir[l_dir-1] == '/') {
    if (l_dir > 1) {   // ne pas enlever le / de la racine
      s_dir[l_dir-1] = 0;  //securite pour opendir() si / final pas admis
      l_dir--;
    }
  }
  else {  // chercher le / du parent
    char *p = strrchr(s_dir, '/');
    //s_remain = ce qui reste apres le dernier /
    if (p) {
      if (s_remain) {
        *s_remain = new char [strlen(p)];
        strcpy(*s_remain, p+1);
      }

      //!att: ne pas enlever le / de la racine
      if (p > s_dir) *p = 0;
      else *(p+1) = 0;
      l_dir = strlen(s_dir);  // la taille a change!
    }
  }

  return l_dir;
}

/* ==================================================== ======== ======= */
// new_path is relative except if starting by a /
// cette fonction reinitialise fspec sauf si arg null ou == fspec

void UFilebox::changeDirImpl(UStr *argpath_str) {  
  const char* argpath = argpath_str ? argpath_str->c_str() : null;

  if (argpath && argpath[0] == '/') {      // new_path is absolute
    setDir(*argpath_str);
  }
  
  else {			// new_path is relative
    UStr newpath = fspec;
    if (newpath.empty()) newpath = "/";

    if (!argpath || !argpath[0])
      /*nop*/;

    else if (strcmp(argpath, "..")==0 || strcmp(argpath, "../")==0) {
      // trouver le path du parent (et skip du dernier /)
      int ll = newpath.length();
      //char *pc = null;
      int ix = -1;
      
      if (ll > 0) {
        newpath.remove(-1, 1);
        ix = newpath.rfind('/');
      }
      
      // garder le /
      if (ix >= 0) newpath.remove(ix+1, UStr::npos);
      else newpath = "/";    // racine
    }

    else { //ajouter le subdirectory
      //concatener (mais apres avoir enleve un filtre eventuel)
      int ll = separ((char*)newpath.c_str(), null);

      char *pc = new char[ll + strlen(argpath) + 4];
      sprintf(pc, "%s/%s/", newpath.c_str(), argpath);
      newpath = pc; 
    }
    
    // ne pas oublier de remettre a jour fspec
    fspec = newpath;
    rescan();
  }
}

/* ==================================================== ======== ======= */

void UFilebox::rescan() {
  bool want_file_attributes = show_list->isSelected();
  bool want_hidden_files = show_hidden_files->isSelected();
  bool want_normalfiles = true;
  
  // creer le nouveau vieport (on detruira l'ancien a la fin)
  UBox* viewport = &uhbox(upadding(3,3) + uhspacing(3)+ uvspacing(3));
  
  UStr dirpath = fspec.dirname(true); 
  UStr prefix  = fspec.basename(true);
  UFileDir dir;
  dir.read(dirpath, prefix, *ffilter, want_hidden_files);
  
  //NB: UDir normalise dirpath qui peut donc avoir change
  fspec = dir.getPath();
  *fdir2 = fspec.dirname(true); 
  if (!prefix.empty()) fspec.append(prefix);

  const UFileInfos& entries = dir.getFileInfos(); 

  if (want_file_attributes) {     // vertical list with attributes
    UBox& listing = uvbox(utop() + upadding(3,3) + uhspacing(3) + uvspacing(1));
    viewport->add(listing);
    int n = 0;
    static UPadding& pad = upadding(10,UIGNORE);

    for (UFileInfos::const_iterator pe = entries.begin(); pe != entries.end(); ++pe) {
       const UFileInfo& e = *(*pe);
      
      if (e.isDir()) {	// cas directory
        UArgs call = ucall(this, e.pname(), &UFilebox::changeDirImpl);
        UStr* infos = new UStr();
        e.getInfoString(*infos);
        listing.add(uitem(new_sel + call + pad
                          + e.getMiniIconImage() 
                          + uelem(UFont::monospace + infos)
                          + uelem(UFont::bold + e.pname)
                          ));
        n++;
      }
      else if (want_normalfiles) {    // cas ordinary file or whatever
        // NB:UOn::select la cond la plus appropriee car :
        // - selectBehavior fires select
        // - pas de rappel si meme btn clique  plusieurs fois
        // - coherent si selection changee par programme (typiquement via
        //   le USelect) et non par l'utilisateur
        
        UArgs call = UOn::select / ucall(this, e.pname(), &UFilebox::selectBehavior)
                     + UOn::doubleClick / ucall(this, &UFilebox::okBehavior);
        UStr* infos = new UStr();
        e.getInfoString(*infos);
        listing.add(uitem(new_sel + call + pad
                          + e.getMiniIconImage() 
                          + uelem(UFont::monospace + infos)
                          + uelem(UFont::bold + e.pname)
                          ));
        n++; 
      }
    } //endfor(int k)
  } //endif(want_attributes)
  
  else {   // no attributes -> matrix
    UBox* col = &uvbox(utop());  //premiere colonne verticale du viewport
    viewport->add(*col);
    int n = 0;
    
    for (UFileInfos::const_iterator pe = entries.begin(); pe != entries.end(); ++pe) {
      const UFileInfo& e = *(*pe);

      if (e.isDir()) {	// cas directory
        col->add(uitem(new_sel + e.getMiniIconImage() + *e.pname
                       + UOn::action/ucall(this, e.pname(), &UFilebox::changeDirImpl))
         );
        n++; 
      }
      
      else if (want_normalfiles) {    // cas ordinary file or whatever
        // NB:UOn::select la cond la plus appropriee car :
        // - selectBehavior fires select
        // - pas de rappel si meme btn clique  plusieurs fois
        // - coherent si selection changee par programme (typiquement via
        //   le USelect) et non par l'utilisateur
        col->add
        (uitem(new_sel + e.getMiniIconImage()
               + e.pname
               + UOn::select / ucall(this, e.pname(), &UFilebox::selectBehavior)
               + UOn::doubleClick / ucall(this, &UFilebox::okBehavior)
               )
         );
        n++; 
      }
      
      // nouvelle colonne verticale
      if (!want_file_attributes && n % UAppli::conf.filebox_line_count == 0) {
        col = &uvbox(utop());
        viewport->add(*col /*, false*/);
      }
    } //endfor(int k)
  } //endelse(want_attributes)    
  
  UScrollpane* scrollpane = 
  want_file_attributes ? new UScrollpane(true,false) : new UScrollpane(false,true);
 
  scrollpane->addAttr(usize(UAppli::conf.filebox_width, UAppli::conf.filebox_height)
                      + UBackground::white);
  scrollpane->add(viewport);
  
  bool auto_up = mainbox->isAutoUpdate();
  mainbox->setAutoUpdate(false);  // utile ?
  mainbox->removeAll();
  mainbox->setAutoUpdate(auto_up);
  mainbox->add(scrollpane);
}

/* ==================================================== ======== ======= */

void UFilebox::selectBehavior(UEvent& _e, UStr* filename) {
  *fname = *filename;
  *fpath = *fdir2 & *fname;
  UEvent e2(UOn::change, this, _e.getSource());    //UElemEvent
  fire(e2);
}

void UFilebox::okBehavior(UInputEvent& e) {
  *fpath = *fdir2 & *fname;
  if (autoclose) {
    closeWin(e, 1);   // returns 1 to lock()
    e.getFlow()->closeAllMenus();
  }
  UEvent e2(UOn::action, this, e.getSource());    //UElemEvent
  fire(e2);
}

void UFilebox::cancelBehavior(UInputEvent& e) {
  closeWin(e,0);   // returns 0 to lock()
  e.getFlow()->closeAllMenus();
}

/* ==================================================== ======== ======= */

void UFilebox::setName(const UStr& s) {
  *fname = s;  
  *fpath = *fdir2 & *fname;
}

void UFilebox::setDir(const UStr& s) {
  fspec = s;
  // rajouter un / a la fin si necessaire
  if (fspec.at(-1) != '/') fspec.append("/");
  rescan();
}

void UFilebox::setDirImpl( UStr* s) {
  if (s) setDir(*s);
}

void UFilebox::setFilter(const UStr& s) {
  *ffilter = s;
}

}

