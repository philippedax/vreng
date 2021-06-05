package fr.enst.vreng.net;

// VRENG dependencies
import fr.enst.vreng.IMessage;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.data.MsgOutputStream;

// Java dependencies
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;

// Java exceptions
import java.io.IOException;
import java.net.UnknownHostException;

/**
 * Class used to send data to an IfcServer inside
 * a Vreng application.
 */
public class Sender {

  /** Vreng application's host name */
  String destName;
  /** Port number the Vreng IfcServer is listening on. */
  int destPort;
  /** State of this sender object */
  boolean isUp;
    
  /** Socket used to transmit the data */
  protected static DatagramSocket sock;
  /** Vreng IfcServer's address */
  protected InetAddress destAddr;
  
  // Initialize the sending socket.
  static {
    sock = null;
    try {
      sock = new DatagramSocket();
    } catch (IOException ioe) {
      System.err.println("Fatal error: Sender is unable to create a datagram socket.");
      ioe.printStackTrace(System.err);
    }
  }
  
  /**
   * Build a new Sender object to talk to the IfcServer
   * listening on the given host and port.
   * @throws fr.enst.vreng.VrengException if there is an error
   * resolving the hostname, or if the class initalization failed
   * (datagram creation failed).
   */
  public Sender(String destName, int destPort) throws VrengException {
    if (Sender.sock == null) throw new VrengException("Couldn't create datagram socket.");
    this.destName = destName;
    this.destPort = destPort;
    this.destAddr = null;
    try {
      destAddr = InetAddress.getByName(destName);
    } catch(UnknownHostException uhe) {
      throw new VrengException("Unable to resolve host name "+destName+" : "+uhe.getMessage());
    }
    this.isUp = true;
  }

  /**
   * Sends a message to the IfcServer.
   * @throws fr.enst.vreng.VrengException if the operation failed,
   * or if the sender is not open.
   */
  public void send(IMessage msg) throws VrengException {
    if (!isOpen()) throw new VrengException("Sender not open");
    
    byte payload[] = MsgOutputStream.toBytes(msg);
    try {
      DatagramPacket dp = new DatagramPacket(payload, payload.length, destAddr, destPort);
      Sender.sock.send(dp);
    } catch(IOException ioe) {
      throw new VrengException("Unable to send data : "+ioe.getMessage());
    }
  }

  /**
   * Returns true if this sender is ready to send messages.
   */
  public boolean isOpen() {
    return isUp;
  }

  /**
   * Marks this sender as inactive.
   */
  public void close() {
    this.isUp = false;
  }
}
