/************************************************************************
*
*  usource.hpp: file and socket data management
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

#ifndef _usource_hpp_
#define	_usource_hpp_ 1
#include <ubit/uobject.hpp>
namespace ubit {

/** a USource object fires callbacks when a file or a socket gets data.
* Example:
* <pre>
*    int source = ...;
*    uptr<USource> i = new USource(source);
*    i->onAction( ucall(x, y, printArgs) );
*    i->onAction( ucall(obj, val, &Demo::doIt) );
* </pre>
* - callbacks printArgs(x, y) and obj->doIt(val) are fired when 
*   the source receives data
*
* See also: USocket (Ubit simple sockets).
*/
class USource : public UNode {    // !!!@@@ devrait deriver de UObject !!!
public:
  UCLASS(USource)
    
  USource(int source =-1);
  ///< creates a new Source listener and start listening to this source if >= 0.
  
  virtual ~USource();
  
  virtual void onAction(UCall& callback);
  /**< adds a callback that is fired when data is received.
   * the argument is a ucall<> expression (see UCall) that is destroyed when the
   * USource is destroyed (except if "referenced elsewhere"; see UNode).
   */
  
  virtual void onInput(UCall& callback) {onAction(callback);}
  ///< synonym for onAction().

  virtual void onClose(UCall& callback);
  ///< adds a callback that is fired when the source is closed.

  virtual void open(int source);
  ///< starts listening to the source (a file, a socket, a pipe ID).

  virtual void close();
  ///< stops listening to the source.
     
  int getSource() const {return source;}
  ///< returns the OS ID of the the source.

  bool isOpened() const {return is_opened;}
  ///< returns true if the USource is opened.
        
  // - - - impl - - -    
  
  virtual void fireInput();
  virtual void fireClose();

private:
  friend class UAppliImpl;
  bool is_opened;
  int source;
  int gid;  // input ID when GDK is used
};

}
#endif
