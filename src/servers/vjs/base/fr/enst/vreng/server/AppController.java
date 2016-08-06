package fr.enst.vreng.server;

import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.server.VrengApp;

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

@SuppressWarnings("unchecked")

/**
 * Keeps track of running applications.
 * This class is responsible for keeping track
 * of all the running applications, and for
 * disposing of those who no longer have active clients.
 * The disposal mechanisme is a simple poll on the list
 * of active applications every five seconds.
 */
public class AppController implements Runnable {

  /** Structure used to store the applications */
  protected HashMap apps = new HashMap();

  /** Creates a new AppController. */
  public AppController() { }

  /** Starts the disposal thread. */
  public void start() {
    Thread runner = new Thread(this, "Vreng App Controller");
    runner.setDaemon(true);
    runner.start();
  }
  
  /** Disposal loop. */
  public void run() {
    boolean cont = true;
    while (cont) {
      try {
        Thread.sleep(5000);
      } catch (InterruptedException ie) {
        ie.printStackTrace();
        cont = false;
      }
      synchronized(this) {
        Iterator i = apps.entrySet().iterator();
        while (i.hasNext()) {
          IVrengApp a = (IVrengApp) ((Map.Entry) i.next()).getValue();
          if (a.isFree()) {
            a.stop();
            i.remove();
          }
        }
      }
    }
  }

  /** Returns an application based on its name. */
  public synchronized IVrengApp getApp(String name) {
    return (IVrengApp) apps.get(name);
  }

  /** Add an application to the list of active apps. */
  public synchronized void addApp(IVrengApp a) {
    apps.put(a.getName(), a);
  }

  /**
   * Removes an application.
   * The application is not closed.
   */
  public synchronized void removeApp(IVrengApp a) {
    apps.remove(a);
  }

  /**
   * Removes all the applications and closes them.
   */
  public synchronized void removeAll() {
    Iterator i = apps.entrySet().iterator();
    while (i.hasNext()) {
      IVrengApp a = (IVrengApp) ((Map.Entry) i.next()).getValue();
      a.stop();
      i.remove();
    }
  }

  /**
   * Prints some status information to System.out.
   */
  public void dump() {
    Iterator i = apps.entrySet().iterator();
    while (i.hasNext()) {
      IVrengApp a = (IVrengApp) ((Map.Entry) i.next()).getValue();
      if (a instanceof VrengApp) {
        ((VrengApp) a).dump();
      } else {
        System.out.println("<App "+a.getName()+"/>");
      }
    }
  }
}
