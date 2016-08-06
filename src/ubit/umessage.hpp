/************************************************************************
 *
 *  umessage.hpp: Ubit Inter-Client Messages: see umservice.hpp 
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

#ifndef _umessage_hpp_
#define	_umessage_hpp_
#include <map>
#include <string>
#include <ubit/uelem.hpp>
#include <ubit/uattr.hpp>
namespace ubit {

/** Ubit Message.
*/
class UMessage {
public:
  static void send(UHardwinImpl&, const char* message);
  static void send(UHardwinImpl&, const UStr& message);
};

/* ==================================================== ====== ======= */
/** Ubit Message Port.
*/
class UMessagePort : public UElem {
public:
  UMessagePort(const UStr& name);
  const UStr& getName()  {return name;}
  const UStr& getValue() {return value;}

private:
  friend class UMessagePortMap;
  UStr name, value;
};

/* ==================================================== ====== ======= */
/** Impl.
 */
class UMessagePortMap {
public:
  UMessagePort& getMessagePort(const UStr& name);
  UMessagePort* findMessagePort(const UStr& name);
  void fireMessagePort(const char* data);

private:
  struct Comp {
    bool operator()(const UStr* s1, const UStr* s2) const
    {return s1->compare(*s2) < 0;}
  };
  typedef std::map<const UStr*, UMessagePort*, Comp> MessMap;
  MessMap mess_map;
  void _fireMessagePort(UMessageEvent&, const char* buf);
};

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

