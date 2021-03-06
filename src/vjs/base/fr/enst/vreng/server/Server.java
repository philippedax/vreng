package fr.enst.vreng.server;

import fr.enst.vreng.IMessage;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengProcessor;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.net.Sender;
import fr.enst.vreng.net.Receiver;
import fr.enst.vreng.data.Header;
import fr.enst.vreng.data.messages.*;

import java.io.*;

@SuppressWarnings("unchecked")

/**
 * Vreng Server class.
 * This is the main class for this project. It deals
 * with connecting Vreng clients with Ifc applications
 * running inside this server.
 */
public class Server implements IVrengProcessor {

  /** Port number this server is listening on */
  protected int port;
  /** Client controller */
  protected ClientController clients;
  /** Application controller */
  protected AppController apps;
  /** Receiver object */
  protected Receiver receiver;

  /**
   * Constructs a Server and starts it up on the given port.
   * @throws fr.enst.vreng.VrengException if the receiver could
   * not be created.
   */
  public Server(int port) throws VrengException {
    // Create the receiver
    this.port = port;
    receiver = null;
    try {
      receiver = new Receiver(this, port);
    } catch (VrengException vre) {
      throw (VrengException) vre.fillInStackTrace();
    }
    
    // Create and start the client controller
    clients = new ClientController();
    clients.start();
    
    // Create and start the application controller
    apps = new AppController();
    apps.start();

    // Start the receiver
    receiver.start();
    
    // We're good to go
    System.out.println("Vjs listening on port "+port);
  }
  
  /**
   * Simple command loop. Waits on System.in for an "x", and dumps the
   * client and application controller when it gets one.
   */
  public void processInput() {
    BufferedReader reader = new BufferedReader(new InputStreamReader(System.in)); 
    String line;
    try {
      while ((line = reader.readLine()) != null) {
        if (line.equals("x")) {
          Thread.currentThread().getThreadGroup().list();
          apps.dump();
          clients.dump();
        } else if (line.equals("q")) {
          receiver.stop();
          apps.removeAll();
          return;
        }
      }
    } catch (IOException ioe) {
      ioe.printStackTrace();
    }
  }

  /**
   * Process incoming messages.
   * Control messages are processed by the server itself,
   * but all the other messages are transmitted to the
   * appropriate client.
   */
  public void notify(IMessage msg) {
    if (msg instanceof ControlMessage) {
      processControl((ControlMessage) msg);
    } else {
      Vreng vr = clients.getClient(msg.getHeader().getSSRC());
      if (vr == null) {
        System.out.println("[Can't locate vreng for "+msg.getHeader().getSSRC()+"]");
        return;
      }
      vr.dispatch(msg);
    }
  }
  
  /**
   * Process receiver errors.
   */
  public void notify(boolean fatal, VrengException problem, Header header) {
    System.out.println((fatal ? "Fatal" : "Non-fatal")+" error from receiver:");
    problem.printStackTrace();
    if (header != null) {
      System.out.println("Header was: "+header);
    }
  }

  /**
   * Deal with incoming control messages.
   * This method dispatches the message to the
   * register/unregister methods, and processes update and ping messages.
   */
  public void processControl(ControlMessage cm) {
    Vreng vreng = null;
    switch (cm.getHeader().getVal()) {
    case MessageTypes.MSGV_REGISTER:
      registerObject(cm);
      break;
    case MessageTypes.MSGV_UNREGISTER:
      unregisterObject(cm);
      break;
    case MessageTypes.MSGV_INITIATE:
      registerClient(cm);
      break;
    case MessageTypes.MSGV_TERMINATE:
      unregisterAll(cm);
      break;
    case MessageTypes.MSGV_UPDATE:
      vreng = clients.getClient(cm.getHeader().getSSRC());
      if (vreng == null) {
        System.out.println("[Update failed. "+cm.getHeader().getSSRC()+" unknown.]");
      } else {
        clients.updateClient(vreng, cm.newSSRC);
        }
      break;
    case MessageTypes.MSGV_PING:
      vreng = clients.getClient(cm.getHeader().getSSRC());
      if (vreng != null) {
        vreng.increaseAge();
      } else {
        System.out.println("<Got bad ping from "+cm.getHeader().getSSRC()+">");
      }
      break;
    }
  }

  /**
   * Adds a new Vreng client if it doesn't already exist.
   */
  public void registerClient(ControlMessage cm) {
    System.out.println("[Got an initiate message]");
    Vreng vreng = null;
                synchronized (clients) {
                        vreng = clients.getClient(cm.getHeader().getSSRC());
                        if (vreng == null) {
        System.out.println("[Got an initiated source]");
                                vreng = new Vreng(cm.getHeader().getSSRC());
                                clients.addClient(vreng);
                        }
                }
  }

  /**
   * Add a new Vreng object to a Vreng client, and make sure
   * the application it's asking for is up and running.
   * Registers the new vreng object with the application.
   */
  public void registerObject(ControlMessage cm) {
    Vreng vreng = null;
    synchronized (clients) {
      vreng = clients.getClient(cm.getHeader().getSSRC());
      if (vreng == null) {
        System.out.println("[Got a register from a non initiated source]");
        return;
      }
    }
    VrengObject vo = null;
    try {
      vo = new VrengObject(cm.getSource(), cm.host, cm.port);
    } catch (VrengException ve) {
      System.err.println("[Got bad initiate request - unable to resolve host]");
      ve.printStackTrace(System.err);
      return;
    }
    synchronized(apps) {
      IVrengApp a = apps.getApp(cm.name);
      if (a == null) {
        try {
          a = VrengApp.load(cm.name, cm.classname, cm.url, cm.width, cm.height);
        } catch (VrengException vre) {
          vre.printStackTrace();
          return;
        }
        apps.addApp(a);
        a.start();
      }
      vreng.addObject(vo, a);
    }
  }

  /**
   * Unregisters all objects from a Vreng client, and removes the client
   * from the active client list.
   */
  public void unregisterAll(ControlMessage cm) {
    Vreng vreng = clients.getClient(cm.getHeader().getSSRC());
    if (vreng == null) {
      System.out.println("Can't find client "+cm.getHeader());
      return;
    }
    // System.out.println("[Terminating "+vreng+"]");
    clients.removeClient(vreng);
    vreng.removeAll();
  }

  /**
   * Unregister a single Vreng object from a Vreng client.
   */
  public void unregisterObject(ControlMessage cm) {
    Vreng vreng = clients.getClient(cm.getHeader().getSSRC());
    if (vreng == null) {
      System.out.println("Can't find client"+cm.getHeader());
      return;
    }
    vreng.removeObject(cm.getSource());
  }

  public String toString() {
    return "Vreng Server ("+port+")";
  }
  
  public static void usage() {
    System.out.println("Usage: java fr.enst.vreng.server.Server port");
  }

  public static void main(String[] args) {
    if (args.length != 1) {
      usage();
      return;
    }
    int port;
    try {
      port = Integer.parseInt(args[0]);
    } catch (NumberFormatException nfe) {
      System.out.println("("+args[0]+") is not a number.");
      usage();
      return;
    }
    Server s = null;
    try {
      s = new Server(port);
      s.processInput();
    } catch (VrengException vre) {
      vre.printStackTrace();
    }
  }
}
