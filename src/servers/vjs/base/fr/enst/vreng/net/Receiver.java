package fr.enst.vreng.net;

// VRENG dependencies
import fr.enst.vreng.IMessage;
import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.IVrengProcessor;
import fr.enst.vreng.VrengException;

import fr.enst.vreng.data.Header;
import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.messages.*;

// Java dependencies
import java.net.DatagramSocket;
import java.net.DatagramPacket;

// VRENG Exceptions
import fr.enst.vreng.data.InvalidMessageException;

// Java exceptions
import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.SocketException;

/**
 * Listens for messages coming in from a Vreng Vjc client.
 * A receiver needs to be {@link #start started} to be operational.
 * This will create a new thread that will wait for data from Vreng.
 * The socket reads are set to time out so that the thread
 * can be disposed off when the receiver is no longer needed.
 * Once unpacked, the messages are passed on to the
 * the Receiver was initialized with.
 */
public class Receiver implements Runnable {

  /** Object that will receive the incoming messages */
  private IVrengProcessor client;

  /** Socket the receiver will listen on */
  protected DatagramSocket sock;

  /** Used to terminate the runner thread */
  private volatile boolean stayAlive;

  /** Used to store the runner thread */
  private Thread runner = null;

  /**
   * Constructs a new sender and binds to the receiver port.
   * @throws fr.enst.vreng.VrengException if the bind failed.
   */
  public Receiver(IVrengProcessor client, int port) throws VrengException {
    this.stayAlive = true;
    this.client = client;
    this.sock = null;
    try {
      sock = new DatagramSocket(port);
      sock.setReuseAddress(true);
      sock.setSoTimeout(1000);
    } catch(SocketException se) {
      throw new VrengException("Unable to initialize receiver : "+se.getMessage());
    }
  }

  /**
   * Main processing loop.
   * Waits for incoming data and transmits it to the IVrengProcessor.
   */
  public void run() {
    byte packdata[] = new byte[1024];
    while(stayAlive) {
      DatagramPacket dp = new DatagramPacket(packdata, packdata.length);
      try {
        sock.receive(dp);
        MsgInputStream mis = null;
        IMessage received = null;
        // Build the input stream that reads the message's header
        try {
          mis = new MsgInputStream(dp.getData());
        } catch (InvalidMessageException ime) {
          client.notify(false, ime, null);
        }
        if (mis != null) {
          // The header as good. Try and unack the message completly
          try {
            received = messageFromStream(mis);
          } catch(VrengException ve) {
            client.notify(false, ve, mis.getHeader());
          }
        }
        if (received != null) {
          // We just received a valid message
          client.notify(received);
        }
      } catch(InterruptedIOException iioe) {
        // Ignore this. normal receive timeout. Used to have this thread die by itself.
      } catch(IOException ioe) {
        client.notify(true, new VrengException(ioe.getMessage()), null);
        stayAlive = false;
      }
    }
    System.out.println("[Receiver for "+client+" died]");
  }

  /**
   * Returns a constructed message from the given stream.
   */
  public IMessage messageFromStream(MsgInputStream mis) throws VrengException {
    int type = mis.getHeader().getType();
    switch (type) {
    case MessageTypes.MSGT_CTRL:
      return new ControlMessage(mis);
    case MessageTypes.MSGT_CLICK:
      return new ClickMessage(mis);
    case MessageTypes.MSGT_POS:
      return new PosMessage(mis);
    case MessageTypes.MSGT_ADD:
    case MessageTypes.MSGT_DEL:
      return new DrawMessage(mis);
    case MessageTypes.MSGT_ISEC:
      return new IntersectMessage(mis);
    case MessageTypes.MSGT_QUERY:
      return new QueryMessage(mis);
    default:
      return new Message(mis);
    }
  }
  /**
   * Initiates the listening process by starting a new thread with the main
   * processing loop.
   */
  public synchronized void start() {
    runner = new Thread(this, "Vreng Receiver for "+client);
//    runner.setDaemon(true);
    runner.start();
  }

  /**
   * Stops the receiver.
   */
  public synchronized void stop() {
    sock.close();
    sock = null;
    stayAlive = false;
    try {
      runner.join();
    } catch (InterruptedException ie) {
    }
    client = null;
  }
}
