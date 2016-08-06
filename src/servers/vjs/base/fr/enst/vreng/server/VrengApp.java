package fr.enst.vreng.server;

// Vreng dependencies
import fr.enst.vreng.IMessage;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.messages.*;
// Collections
import java.util.HashSet;
import java.util.Iterator;
// Classloader
import java.net.URL;
import java.net.URLClassLoader;
import java.net.MalformedURLException;

@SuppressWarnings("unchecked")

/**
 * Base class to be used to create Vreng applications.
 */
public abstract class VrengApp implements IVrengApp {

  /** Thread that runs this app */
  protected Thread runner = null;

  /** Set of IVrengObjects that are client of this application */
  protected HashSet clients = new HashSet();
  /** Returns the number of activ clients */
  public int numClients() { return clients.size(); }
  /** Returns true if no more clients are registered. */
  public boolean isFree() { return clients.isEmpty(); }

  /** Primary client (the one that started this app */
  protected volatile IVrengObject primary;
  /** Returns the primary client */
  public IVrengObject getPrimary() { return primary; }

  /** Application name (first token in the .vre file) */
  protected String name;
  /** Returns this application's name */
  public String getName() { return name; }
  /** URL from which this class was loaded */
  protected String url;
  /** Returns this application's url */
  public String getURL() { return url; }

  /** VRENG obj's width (x in (-width;width) */
  protected int width;
  /** VRENG obj's height (y in (-height;height) */
  protected int height;

  protected class QLock {
    boolean busy;
    public QLock() {
      busy = false;
    }
    public synchronized boolean isLocked() { return busy; }

    public synchronized void lock()
      throws InterruptedException {
      while ((isLocked()) && (!closing)) {
        wait(500);
      }
      busy = true;
    }

    public synchronized void unlock() {
      busy = false;
      notifyAll();
    }
  }

  private QLock queryLock = new QLock();
  private QueryMessage queryMessage = null;
  private volatile boolean closing = false;

  public VrengApp() { }

  /**
   * Stores the data passed in. Does not start the app.
   */
  public void setup(String name, String url, int width, int height) {
    this.name = name;
    this.url = url;
    this.width = width;
    this.height = height;
    this.primary = null;
  }
  /**
   * Send a message to all the clients.
   */
  public void sendAll(Message msg) {
    Iterator i = clients.iterator();
    while (i.hasNext()) {
      try {
        send((IVrengObject) i.next(), msg);
      } catch (VrengException ve) {
        // TODO: Figure out what to do with this
      }
    }
  }

  /**
   * Send a message to the primary client only.
   */
  public synchronized void sendPrimary(Message msg) throws VrengException {
    while ((closing == false) && (primary == null)) {
      try {
        System.out.println("[Waiting for primary]");
        wait(500);
      } catch (InterruptedException ie) {
        return;
      }
    }
    if ((primary != null) && (closing == false)) send(primary, msg);
  }

  /**
   * Send a message to a specific VrengObject.
   */
  public void send(IVrengObject source, Message msg) throws VrengException {
    source.send(msg);
  }

  /**
   * Send a query message. This call is blocking until
   * an answer has been emitted by the vreng client.
   * The query is sent only to the primary client.
   */
  public synchronized QueryMessage sendQuery(Message msg) throws VrengException {
    synchronized(queryLock) {
      int count = 0;
      try {
        queryLock.lock();
      } catch (InterruptedException ie) {
        return null;
      }
      if (closing) return null;
      sendPrimary(msg);
      while ((closing == false)
          && (queryMessage == null)
          && (count < 4)           ) {
        try {
          queryLock.wait(500);
        } catch (InterruptedException ie) {
          queryLock.unlock();
          return null;
        } 
        count++;
      }
      QueryMessage ret = queryMessage;
      queryMessage = null;
      queryLock.unlock();
      return ret;
    }
  }

  /**
   * Override this if you need special handling when a new 
   * client joins this app.
   */
  public void onJoin(IVrengObject obj) { }

  /**
   * Override this if you need special handling when a
   * client leaves this app.
   */
  public void onLeave(IVrengObject obj) { }

  /** Override this to process intersection messages */
  protected void processIntersect(IVrengObject obj, IntersectMessage mm) { }
  /** Override this to process query (reply) messages */
  protected void processQuery(IVrengObject obj, QueryMessage mm) { }
  /** Override this to process click messages */
  protected void processClick(IVrengObject obj, ClickMessage mm) { }
  /** Override this to process position messages */
  protected void processPos(IVrengObject obj, PosMessage mm) { }

  /** Dispatches incoming messages to the various process functions */
  public final void incoming(IVrengObject obj, IMessage msg) {
    if (msg instanceof IntersectMessage) {
      processIntersect(obj, (IntersectMessage) msg);
    } else if (msg instanceof PosMessage) {
      processPos(obj, (PosMessage) msg);
    } else if (msg instanceof ClickMessage) {
      processClick(obj, (ClickMessage) msg);
    } else if (msg instanceof QueryMessage) {
      QueryMessage qm = (QueryMessage) msg;
      if (qm.getHeader().getVal() == MessageTypes.MSGV_QANS) {
        synchronized(queryLock) {
          queryMessage = qm;
          queryLock.notifyAll();
        }
      } else {
        processQuery(obj, qm);
      }
    }
  }

  /**
   * Close this app. The clients are all closed. 
   * Interrupts threads that were waiting on q query result
   */
  public synchronized void close() {
    closing = true;
    closeAll();
    clients.clear();
    notifyAll();
    synchronized(queryLock) {
      queryMessage = null;
      queryLock.notifyAll();
    }
  }

  protected synchronized void setPrimary(IVrengObject obj) {
    primary = obj;
    notifyAll();
  }

  /** Add a client to this app */
  public synchronized void addClient(IVrengObject obj) {
    System.out.println("["+name+": Adding client "+obj+"]");
    clients.add(obj);
    if (primary == null) setPrimary(obj);
    onJoin(obj);
  }

  /** Remove a client from this app */
  public synchronized void removeClient(IVrengObject obj) {
    System.out.println("["+name+": Removing client "+obj+"]");
    obj.close();
    clients.remove(obj);
    if ((obj == primary) && (!clients.isEmpty())) {
      setPrimary((IVrengObject) clients.iterator().next());
    } else if (obj == primary) {
      setPrimary(null);
    }
    onLeave(obj);
  }

  /** Terminate all clients */
  public synchronized void closeAll() {
    Iterator i = clients.iterator();
    while (i.hasNext()) {
      ((IVrengObject) i.next()).close();
    }
  }

  /** Start this app. */
  public final void start() {
    runner = new Thread(this, "VRENG App Thread - "+name);
    runner.start();
  }

  /** Stop this app. Calls close() */
  public final void stop() {
    System.out.println("["+this+" going down]");
    close();
    try {
      runner.interrupt();
      runner.join();
    } catch (InterruptedException ie) {
    }
    System.out.println("["+this+" down]");
  }

  public String toString() {
    StringBuffer sb = new StringBuffer("<App ");
    sb.append(getName());
    if (clients.isEmpty()) {
      sb.append("/>");
      return sb.toString();
    }
    sb.append(">\n");
    Iterator i = clients.iterator();
    while (i.hasNext()) {
      sb.append("\t\t");
      sb.append(i.next());
      sb.append("\n");
    }
    sb.append("\t</App ");
    sb.append(getName());
    sb.append(">");
    return sb.toString();
  }

  public void dump() {
    System.out.println("<App "+getName()+">");
    Iterator i = clients.iterator();
    while (i.hasNext()) {
      System.out.println("\t"+i.next());
    }
    System.out.println("</App "+getName()+">");
  }

  /**
   * Loads a VrengApp from the given data, and initialises it.
   * The app needs to be started afterwards.
   */
  public static IVrengApp load(String name, String classname, String url, int width, int height)
    throws VrengException {
    IVrengApp app = load(classname, url);
    app.setup(name, url, width, height);
    return app;
  }

  /**
   * Loads an IVrengApp from its classname and codebase URL.
   * A URLClassLoader is used for the resolution.
   * @throws VrengException if anything bad happens during the class loading
   * or the object instanciation.
   */
  protected static IVrengApp load(String classname, String url)
    throws VrengException {

    URL[] urls = null;
    IVrengApp app = null;
    try {
      urls = new URL[] { new URL(url) };
    } catch (MalformedURLException mfue) {
      throw new VrengException("URL "+url+" is malformed.");
    }
    URLClassLoader urlcl = new URLClassLoader(urls);
    Class clazz = null;
    try {
      clazz = Class.forName(classname, true, urlcl);
    } catch (ClassNotFoundException cnfe) {
      throw new VrengException("Class "+classname+" not found.");
    }
    try {
      app = (IVrengApp) clazz.newInstance();
    } catch (Throwable t) {
      throw new VrengException("Couldn't load class "+classname+" : "+t.getMessage());
    }
    System.out.println("[Loaded "+classname+" through "+url+"]");
    return app;
  }
}
