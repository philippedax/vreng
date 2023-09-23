package fr.enst.vreng.tester;

import fr.enst.vreng.ObjectTypes;
import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.server.VrengApp;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.net.*;
import fr.enst.vreng.data.*;
import fr.enst.vreng.data.messages.*;
import fr.enst.vreng.data.types.*;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

@SuppressWarnings("unchecked")


/**
 * Sample Vreng application: attractive/repulsive objects.
 * This application interacts with all the users that enter
 * its bounding box by moving them towards/away from its center.
 * The movement is described by the width/heigh parameters of
 * the Vrelet object spec.
 * This sample illustrates Intersect message processing and
 * getting/setting Vreng object positions.
 */
public class Fan extends VrengApp {

  public static double CLOSE = 0.5;

  /** Utility class: holds a Vreng object id and its position */
  public class Target {
    public boolean ready;
    public ObjPos pos;
    public NetObjId id;
  }

  /** List of current targets */
  protected Hashtable targets = new Hashtable();

  /** This object's position within Vreng */
  protected ObjPos me = null;

  /** Set to false when this app is getting closed */
  volatile boolean keepGoing = true;

  /**
   * Main loop: if this object has targets within its bounding box,
   * update their position.
   */
  public void run() {
    try {
      sendPrimary(new PosMessage((NetObjId)null)); // ask for my position
    } catch (VrengException ve) {
      // this would mean that I don't have clients anymore
      ve.fillInStackTrace().printStackTrace();
      return;
    }

    int count = 1;
    while (keepGoing) {
      try {
        Thread.sleep(100);  // 150
      } catch (InterruptedException ie) {
        keepGoing = false;
        return;
      }
      if (me != null) {
        Enumeration e = targets.elements();
        while (e.hasMoreElements()) {
          processTarget((Target) e.nextElement());
        }
      }
      if ((count % 10) == 0) updateTargets();
      count++;
    }
  }

  /**
   * Ask Vreng for the target's positions,
   * in case they moved since the last poll.
   */
  public void updateTargets() {
    Enumeration e = targets.elements();
    while (e.hasMoreElements()) {
      Target t = (Target) e.nextElement();
      try {
        sendPrimary(new PosMessage(t.id));
      } catch (VrengException ve) {
        ve.fillInStackTrace().printStackTrace();
      }
    }
  }

  /**
   * Move the target around if he's within in reach.
   */
  public void processTarget(Target t) {
    if (!t.ready) return;

    Point3D delta = Point3D.sub(t.pos.pos, me.pos);
    delta.z = 0;
    double norm = delta.norm();

    if (norm > CLOSE) {
      delta.x /= width;
      delta.y /= height;
      t.pos.pos.add(delta);
      sendAll(new PosMessage(MessageTypes.MSGV_UPD, t.id,
              new ObjPos(delta, new Point3D())));
    }
  }

  /**
   * Process position updates for the targets.
   */
  protected void processPos(IVrengObject source, PosMessage msg) {
    //pd System.out.println("[Got pos message "+msg+"]");
    if (msg.getHeader().getVal() == MessageTypes.MSGV_SET) {
      if (msg.obj.type == 0) {
        me = msg.pos;
      }
      else {
        Target t = (Target) targets.get(msg.obj);
        if (t != null) {
          t.pos = msg.pos;
        }
      }
    }
    else if (msg.getHeader().getVal() == MessageTypes.MSGV_ERROR) {
      System.out.println("[Got pos error: someone probably left this world]");
      targets.remove(msg.obj);
    }
  }

  /**
   * Used to know what Vreng objects are entering/leaving the bounding box.
   * Objects entering the box are added to the target list, those leaving are
   * removed.
   */
  protected void processIntersect(IVrengObject source, IntersectMessage msg) {
    if (msg.getHeader().getVal() == MessageTypes.MSGV_ISECIN) {
      //pd System.out.println("[Got intersect in]");
      Target t = new Target();
      t.id = msg.obj;
      t.pos = msg.pos;
      t.ready = true;
      targets.put(msg.obj, t);
    }
    else {
      //pd System.out.println("[Got intersect out]");
      targets.remove(msg.obj);
    }
  }

  public void close() {
    keepGoing = false;
    super.close();
  }
}
