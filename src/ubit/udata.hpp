/* ***********************************************************************
 *
 *  udata.hpp: all viewable objects that can be inserted in an element
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

#ifndef _udata_hpp_
#define	_udata_hpp_ 1
#include <ubit/unode.hpp>
namespace ubit {

/** Base class for Viewable Objects.
  *
  * Data objects can have callback functions that are fired when their value is
  * changed. Data parents can also be notified when data value are modified
  * by using the UOn::dataChange condition (see below).
  *
  * see an example in class UStr. The only difference is that UOn::dataChange
  * must be used instead of UOn::strChange for specifing callbacks in parents. 
 */
class UData: public UNode {
public:
  //UCLASS("#data", UData, null);
  UABSTRACT_CLASS(UData);

  UData() {}
  UData(UConst) {omodes.IS_CONST = true;}

  virtual int getDisplayType() const {return DATA;}

  virtual UData* toData() {return this;}
  virtual const UData* toData() const {return this;}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual UData& onChange(UCall&);
  /**< adds a callback that is fired when the value is modified.
    * @see UData doc.
    */
  
  virtual void changed(bool update = true);
  /**< [impl] called when object's content is changed.
    * This function updates graphics (if 'update' is true) then fires 
    * parents' UOn::dataChange callbacks
    */

  virtual void update() = 0;
  ///< updates graphics.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  friend class UArgs;
  friend class UBox;
  virtual void getSize(UUpdateContext&, UDimension&) const = 0;
  virtual void paint(UGraph&, UUpdateContext&, const URect&) const = 0;
#endif
};
}
#endif
