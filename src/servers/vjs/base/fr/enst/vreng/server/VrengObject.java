package fr.enst.vreng.server;

import fr.enst.vreng.IMessage;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.net.Sender;
import fr.enst.vreng.data.types.NetObjId;

@SuppressWarnings("unchecked")

/**
 * Represents a Vreng Ifc object.
 */
public class VrengObject implements IVrengObject {

  /** The object's net identifier */
  protected NetObjId id = null;
  /** Returns the object's net identifier */
  public NetObjId getId() { return id; }
  
  /** Object to use to send data back to Vreng */
  protected Sender sender = null;

  /**
   * Create a new Vreng object and open a Sender back to Vreng.
   * @throws VrengException if the sender failed to open.
   */
  public VrengObject(NetObjId id, String host, int port)
    throws VrengException {
    this.id = id;
    this.sender = new Sender(host, port);
  }
  
  /**
   * Returns <b>true</b> if the Ifc object inside Vreng is still reachable.
   */
  public boolean isUp() {
    return sender.isOpen();
  }
  
  /**
   * Sends a message back to Vreng.
   * @throws VrengException if the message could not be sent.
   */
  public void send(IMessage msg) throws VrengException {
    msg.getHeader().setSource(this.id);
    sender.send(msg);
  }
  
  /**
   * Closes the connection to Vreng.
   * This is used when the Vreng object notified that it was getting
   * destroyed (change of scene for example).
   */
  public void close() {
    sender.close();
  }
  
  public String toString() {
    return "<VrengObject "+id.toString()+"/>";
  }

  public int hashCode() {
    return id.hashCode();
  }
}
