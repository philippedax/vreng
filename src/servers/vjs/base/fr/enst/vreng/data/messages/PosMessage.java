package fr.enst.vreng.data.messages;

import fr.enst.vreng.data.types.*;
 
import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.Header;
import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;
import fr.enst.vreng.data.InvalidMessageException;
 
import fr.enst.vreng.MessageTypes;

import java.util.Enumeration;
import java.util.Vector;

/* Message info
type=MSGT_POS

val=MSGV_ASK
NeObjId    Who's position we want

val=MSGV_SET,MSGV_UPD
NetObjId  Who we want to move
ObjPos    New position or position delta
*/

/**
 * Represents POS messages.
 */
public class PosMessage extends Message {
  
  public NetObjId obj = null;
  public ObjPos   pos = new ObjPos();

  public PosMessage(NetObjId id) {
    super(MessageTypes.MSGT_POS, MessageTypes.MSGV_ASK);
    this.obj = id;
  }

  public PosMessage(int type, NetObjId who, ObjPos pos) {
    super(MessageTypes.MSGT_POS, type);
    this.obj = who;
    this.pos = pos;
  }

  public PosMessage(MsgInputStream mis) throws InvalidMessageException {
    super(mis);
    obj = NetObjId.unpack(mis);
    if (getHeader().getVal() != MessageTypes.MSGV_ERROR) {
      pos = ObjPos.unpack(mis);
    } else {
      pos = null;
    }
  }

  public void pack(MsgOutputStream mos) {
    super.pack(mos);
    if (obj == null) {
      // Object is myself
      (new NetObjId()).pack(mos);
    } else {
      // Object is someone else
      obj.pack(mos);
    }
    if (pos != null) {
      // Set or update rather than query
      pos.pack(mos);
    }
  }
}
