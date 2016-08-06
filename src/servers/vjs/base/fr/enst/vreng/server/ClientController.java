package fr.enst.vreng.server;

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

@SuppressWarnings("unchecked")

/**
 * Keeps track of active Vreng clients.
 * This class has a collection mecanism similar to that
 * of the {@link AppController AppController}'s.
 */
public class ClientController implements Runnable {

  /** Structure used to store the active clients. */
  protected HashMap clients = new HashMap();

  /** Build a new client controller. */
  public ClientController() { }

  /** Start the disposal thread. */
  public void start() {
    Thread runner = new Thread(this, "Vreng Client Controller");
    runner.setDaemon(true);
    runner.start();
  }
  
  /** Disposal loop. */
  public void run() {
    boolean cont = true;
    while (cont) {
      try {
        Thread.sleep(8000);
      } catch (InterruptedException ie) {
        ie.printStackTrace();
        cont = false;
      }
      synchronized(clients) {
        Iterator i = clients.entrySet().iterator();
        while (i.hasNext()) {
          Vreng vreng = (Vreng) ((Map.Entry) i.next()).getValue();
          vreng.decreaseAge();
          if (vreng.getAge() <= 0) {
            System.out.println("[Discarding "+vreng+"]");
            vreng.removeAll();
            i.remove();
          }
        }
      }
    }
  }

  /** Returns a client based on its SSRC */
  public Vreng getClient(int ssrc) {
    synchronized(clients) {
      return (Vreng) clients.get(new Integer(ssrc));
    }
  }

  /** Add a client to the list of active clients. */
  public void addClient(Vreng vreng) {
    synchronized(clients) {
      System.out.println("[Addind "+vreng+"]");
      clients.put(new Integer(vreng.getSSRC()), vreng);
    }
  }

  /** Update a client's SSRC */
  public void updateClient(Vreng vreng, int newSSRC) {
    synchronized(clients) {
      System.out.println("[Updating "+vreng+" to "+newSSRC+"]");
      removeClient(vreng);
      vreng.update(newSSRC);
      addClient(vreng);
    }
  }

  /**
   * Remove an active client. The client is not closed.
   */
  public void removeClient(Vreng vreng) {
    synchronized(clients) {
      System.out.println("[Removing "+vreng+"]");
      clients.remove(new Integer(vreng.getSSRC()));
    }
  }

  /**
   * Prints some status info to System.out.
   */
  public void dump() {
    Iterator i = clients.entrySet().iterator();
    while (i.hasNext()) {
      Vreng vreng = (Vreng) ((Map.Entry) i.next()).getValue();
      vreng.dump();
    }
  }
}
