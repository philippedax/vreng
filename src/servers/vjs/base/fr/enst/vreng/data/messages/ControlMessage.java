package fr.enst.vreng.data.messages;

import fr.enst.vreng.MessageTypes;

import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.InvalidMessageException;

/* Message info
type=MSGT_CTRL
val=MSGV_REGISTER
String  Host name
int16    Port number
String  Application name (Ifc given_name)
String  Class name
int32    Width
int32    Height
val=MSGV_UPDATE
int32    Vreng's new SSRC
val=OTHER
*/

/**
 * Represents CONTROL messages.
 */
public class ControlMessage extends Message {
    
  public String host = null;
  public int port = 0;
  public String name = null;
  public String url = null;
  public String classname = null;
  public int width, height;
  public int newSSRC = 0;

  public ControlMessage(int val) {
    super(MessageTypes.MSGT_CTRL, val);
  }

  public ControlMessage(MsgInputStream mis) throws InvalidMessageException {
    super(mis);
    if (getHeader().getVal() == MessageTypes.MSGV_REGISTER) {
      host = mis.readString();
      port = mis.read16();

      name = mis.readString();
      classname = mis.readString();
      url = mis.readString();
      width = mis.read32();
      height = mis.read32();
    } else if (getHeader().getVal() == MessageTypes.MSGV_UPDATE) {
      newSSRC = mis.read32();
    }
  }

  public String toString() {
    if (name == null) {
      return super.toString();
    } else {
      return super.toString()
        +" "+host
        +" "+port
        +" "+name
        +" "+classname
        +" "+url
        +" "+width
        +" "+height;
    }
  }
}
