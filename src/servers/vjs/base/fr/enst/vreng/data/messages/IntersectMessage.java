package fr.enst.vreng.data.messages;

import fr.enst.vreng.data.types.*;

import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.InvalidMessageException;

import fr.enst.vreng.MessageTypes;

/* Message info
type=MSGT_ISEC

val=ALL
ObjPos    Object who triggered the intersection
*/

/**
 * Represents ISEC messages.
 */
public class IntersectMessage extends PosMessage {
  
  public ObjPos old;

  public IntersectMessage(MsgInputStream mis) throws InvalidMessageException {
    super(mis);
    old = ObjPos.unpack(mis);
  }
}
