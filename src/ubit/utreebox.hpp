/************************************************************************
*
*  utreebox.cpp
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

#ifndef _utreebox_hpp_
#define	_utreebox_hpp_  1
#include <ubit/ulistbox.hpp>
namespace ubit {

/** Tree widget.
  * @see also: UTreenode.
*/
class UTreebox : public UListbox {
public:
  UCLASS(UTreebox)
  
  UTreebox(const UArgs& = UArgs::none);
  ///< creates a new tree widget (@see also shortcut utreebox()).

  virtual ~UTreebox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual UTreenode* getSelectedNode() const;
  ///< returns the selected node; null if there is no selected node.
  
  virtual void setSelectedNode(UTreenode&);
  ///< selects this node.
  
  virtual int getSelectedIndex() const;
  ///< returns the index of the selected node; -1 if there is no selected node.
  
  virtual UTreebox& setSelectedIndex(int n);
  ///< selects the Nth node; selects the last node if n = -1.
  
private:
    friend class UTreenode;
};

UTreebox& utreebox(const UArgs& = UArgs::none);
///< shortcut that returns *new UTreebox().

/* ==================================================== ===== ======= */
/** Node in a Tree widget.
* @see also: UTreebox.
*/
class UTreenode : public UBox {
public:
  UCLASS(UTreenode)

  UTreenode(const UArgs& label = UArgs::none);
  ///< creates a new node in a UTreebox (@see also shortcut utreenode()).

  UTreenode(const UArgs& label, const UArgs& subnode_list);
  ///< creates a new node in a UTreebox (@see also shortcut utreenode()).
 
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  virtual void addNode(UTreenode&);
  virtual void removeNode(UTreenode&);

  virtual void expand(bool = true);
  virtual void collapse() {expand(false);}
  ///< expand/collapse the treenode.

  //virtual void setOffset(UHmargin&);

  UBox& label() {return *plabel;}
  const UBox& label() const {return *plabel;}
  ///< the box that contains the label of the treenode.
  
  UBox& subnodes() {return *psubnodes;}
  const UBox& subnodes() const {return *psubnodes;}
  ///< the box that contains the subnodes of the treenode.
  
private:
  friend class UTreebox;
  uptr<UBox> plabel, psubnodes;
  uptr<UBox> pexpander;
  void constructs(const UArgs& label, const UArgs& subnode_list);
};

UTreenode& utreenode(const UArgs& label = UArgs::none);
///< shortcut that returns *new UTreenode().

UTreenode& utreenode(const UArgs& label, const UArgs& subnode_list);
///< shortcut that returns *new UTreenode().

} 
#endif

