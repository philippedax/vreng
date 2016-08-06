package fr.enst.vreng.data.messages;

import fr.enst.vreng.data.types.*;
 
import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.Header;
import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;
import fr.enst.vreng.data.InvalidMessageException;

import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.VrengException;

import java.util.Vector;
import java.util.Enumeration;

/* Message info
type=MSGT_ADD,MSGT_DEL

val=MSGV_ADD
int16      Figure tag
Point3D    Color
Point2D*  List of points for this figure

val=MSGV_DEL
int16      Figure tag
*/

/**
 * Represents both ADD and DEL messages.
 */
public class DrawMessage extends Message {
  
  private static int nextTag = 0;
  private static int seed = 0;
  public static void setSeed(int newseed) { seed = 0x000000FF & newseed; }

  private int tag;
  public int getTag() { return tag; }
  public void setTag(int tag) { this.tag = tag; }
  public void setTag() {
    this.tag = ( nextTag << 8 ) + seed;
    nextTag++;
  }

  public Point3D color = new Point3D();
  public Vector pointList = new Vector();

  public DrawMessage(MsgInputStream mis) throws VrengException {
    throw new VrengException("Can't handle incoming draw messages.");
  }
  
  /**
    * Constructs a new Add or Delete message, with a random tag;
   */
  public DrawMessage(int msg_type, int fig_type) {
    super(msg_type, fig_type);
    setTag();
  }
  /**
    * Constructs a new Add or Delete message, with a specific tag
   */
  public DrawMessage(int msg_type, int fig_type, int tag) {
    super(msg_type, fig_type);
    setTag(tag);
  }

  public void addPoint(Point2D point) {
    pointList.addElement(point);
  }
  public void addPoint(int x, int y) {
    pointList.addElement(new Point2D(x, y));
  }

  public void pack(MsgOutputStream mos) {
    super.pack(mos);
    // Check the message type
    mos.write16(tag);
    if (getHeader().getType() == MessageTypes.MSGT_ADD) {
      // ADD message
      color.pack(mos);
      for(Enumeration e = pointList.elements(); e.hasMoreElements();) {
        ((Point2D) e.nextElement()).pack(mos);
      }
    }
  }
}
