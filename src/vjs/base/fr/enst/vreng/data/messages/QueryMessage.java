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
type=MSGT_QUERY

val=MSGV_TYPE
int32      Object type for query

val=MSGV_QANS
int32      Number of answers
NetObjId*  Object ids that matched
*/

/**
 * Represents QUERY messages.
 */
public class QueryMessage extends Message {
  
  public int ask_type;
  public Vector answers = null;

  public QueryMessage(int query_type, int ask_type) {
    super(MessageTypes.MSGT_QUERY, query_type);
    this.ask_type = ask_type;
  }

  public QueryMessage(MsgInputStream mis) throws InvalidMessageException {
    super(mis);

    if (getHeader().getVal() != MessageTypes.MSGV_QANS) {
      throw new InvalidMessageException("Can't process queries");
    }

    int count = mis.read32();
    answers = new Vector();

    for (int i=0; i<count; i++) {
      answers.addElement(NetObjId.unpack(mis));
    }
  }

  public void pack(MsgOutputStream mos) {
    super.pack(mos);
    mos.write32(ask_type);
  }
}
