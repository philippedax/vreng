package fr.enst.vreng.data;

import fr.enst.vreng.IMessage;
import fr.enst.vreng.data.types.*;

/**
 * Base class for all the Vreng messages.
 */
public class Message implements IMessage {

  /** This message's header. */
  protected Header head = null;
  
  /**
   * Retruns the object id for this message's sender (or intended recipient).
    */
  public NetObjId getSource() { return head.getSource(); }

  /**
   * Returns this message's header.
   */
  public Header getHeader() { return head; }
  
  /**
   * Set this message's header.
   */
  protected void setHeader(Header header) { head = header; }

  /**
   * Default constructor. Used only internally for unpacking.
   */
  protected Message() { };
  
  /**
   * Constructor used to build outgoing messages.
   * The header is initialized with the message type and subtype passed in.
   */
  public Message(int type, int val) {
    setHeader(new Header(type, val));
  }

  /**
   * Constructor for incoming messages.
   * All derived classes should call this constructor since it reads
   * the message's header.
   */
  public Message(MsgInputStream mis) throws InvalidMessageException {
    head = mis.getHeader();
  }

  /**
   * Write a message to an output stream.
   * All derived classes should call this method when overriding it.
   */
  public void pack(MsgOutputStream mos) { }

  public String toString() {
    return this.getClass().getName()+"["+head.getType()+"-"+head.getVal()+"]";
  }
}
