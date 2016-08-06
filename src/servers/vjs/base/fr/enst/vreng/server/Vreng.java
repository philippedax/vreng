package fr.enst.vreng.server;

import fr.enst.vreng.IMessage;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.data.types.NetObjId;

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

@SuppressWarnings("unchecked")

/**
 * Represents an active Vreng client.
 * This class is the logical equivalent of the IfcServer object
 * inside Vreng. It keeps a list of active Vreng objects (which
 * map to Vreng Ifc objects), and the application they are connected
 * to.
 * To deal with lost connections, this class also keeps track of an
 * 'age' for the Vreng client. The age is incremented (up to a maximum
 * value) every time a ping or a message is received from the Vreng
 * client, and decremented by the client controller periodically. If the
 * age reaches zero, the client is considered dead and is closed by
 * the controller.
 */
public class Vreng {

  /** Maximum age value */
  public static final int MAX_AGE = 5;

  /**
   * Utility class used to store a Vreng object with the application
   * it's connected to.
   */
  public class VrengHolder {
    IVrengObject obj;
    IVrengApp    app;
    public VrengHolder(IVrengObject obj, IVrengApp app) {
      this.obj = obj;
      this.app = app;
    }
    public String toString() {
      return app.toString();
    }
  }

  /** This client's age */
  private int age;
  /** Decrement this client's age */
  public synchronized void decreaseAge() { age--;  }
  /** Increment this client's age */
  public synchronized void increaseAge() { age = MAX_AGE; }
  /** Returns this client's age */
  public synchronized int getAge() { return age; }
  
  /** Structure used to store all the Vreng objects and their corresponding apps. */
  protected HashMap objects;
  
  /** Vreng client's current SSRC */
  protected int ssrc;
  /** Returns the Vreng client's SSRC */
  public int getSSRC() { return ssrc; }
  /** Updates the Vreng client's SSRC */
  public synchronized void update(int newSSRC) { this.ssrc = newSSRC; }

  /**
   * Create a new Vreng client with the given SSRC.
   */
  public Vreng(int ssrc) {
    this.ssrc = ssrc;
    this.age  = MAX_AGE;
    this.objects = new HashMap();
  }

  /**
   * Dispatch an incoming message to the appropriate application.
   * The right Vreng object is looked for in the objects list, and
   * its application is notified of the incoming message.
   */
  public synchronized void dispatch(IMessage msg) {
    increaseAge();
    VrengHolder vh = (VrengHolder) objects.get(msg.getSource());
    if (vh == null) {
      System.out.println("["+this+": Dispatch failed ("+msg.getSource()+")]");
    } else {
      vh.app.incoming(vh.obj, msg);
    }
  }

  /**
   * Register a new Vreng object for this Vreng client.
   * The application it is working with is also stored.
   */
  public synchronized void addObject(IVrengObject obj, IVrengApp a) {
    objects.put(obj.getId(), new VrengHolder(obj, a));
    a.addClient(obj);
  }

  /**
   * Remove a Vreng object from this client.
   * The object is also removed from its application.
   */
  public synchronized void removeObject(NetObjId objid) {
    VrengHolder vh = (VrengHolder) objects.get(objid);
    if (vh == null) return;
    vh.app.removeClient(vh.obj);
    objects.remove(objid);
  }

  /**
   * Remove all this client's Vreng objects.
   * The objects are unregistered from their respective
   * applications in the process.
   */
  public synchronized void removeAll() {
    Iterator i = objects.entrySet().iterator();
    while (i.hasNext()) {
      VrengHolder vh = (VrengHolder) ((Map.Entry) i.next()).getValue();
      vh.app.removeClient(vh.obj);
      i.remove();
    }
    objects.clear();
    age = 0;
  }

  public String toString() {
    return "Vreng "+getSSRC();
  }

  /** Prints some status information to System.out */
  public void dump() {
    System.out.println("<Vreng "+getSSRC()+">");
    Iterator i = objects.entrySet().iterator();
    while (i.hasNext()) {
      Map.Entry me = (Map.Entry) i.next();
      System.out.println("\t"+me.getValue());
    }
    System.out.println("</Vreng "+getSSRC()+">");
  }
}
