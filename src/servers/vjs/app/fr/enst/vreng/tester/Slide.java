package fr.enst.vreng.tester;

import fr.enst.vreng.IMessage;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.server.VrengApp;
import fr.enst.vreng.net.*;
import fr.enst.vreng.data.*;
import fr.enst.vreng.data.types.*;
import fr.enst.vreng.data.messages.*;
import fr.enst.vreng.ObjectTypes;
import fr.enst.vreng.MessageTypes;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

@SuppressWarnings("unchecked")


/**
 * Sample Vreng application: impose user translation.
 * This application imposes a fixed movement on any user
 * entering its bounding box. The translation vector is
 * described by the width/heigh parameters of the Vrelet object.
 * It works just like the {@link Fan Fan} application.
 */
public class Slide extends VrengApp {

  class Target {
    IVrengObject source;
    ObjPos pos;
    public Target(IVrengObject o, ObjPos p) {
      this.source = o;
      this.pos = p;
    }
  }

  public Hashtable targets = new Hashtable();

  volatile boolean keepGoing = true;

  public void run() {
    while (keepGoing) {
      try {
        Thread.sleep(50);  // 100
      } catch (InterruptedException ie) {
        keepGoing = false;
        return;
      }
      Vector todel = new Vector();
      Enumeration e = targets.keys();
      while (e.hasMoreElements()) {
        NetObjId id = (NetObjId) e.nextElement();
        Target t = (Target) targets.get(id);
        if (t.source.isUp()) {
          sendAll(new PosMessage(MessageTypes.MSGV_UPD, id, t.pos));
        } else {
          System.out.println("[Discaring stale client "+t.source+"]");
          todel.addElement(id);
        }
      }
      e = todel.elements();
      while (e.hasMoreElements()) {
        targets.remove(e.nextElement());
      }
    }
  }

  protected void processPos(IVrengObject source, PosMessage msg) {
    if (msg.getHeader().getVal() == MessageTypes.MSGV_ERROR) {
      Target t = (Target) targets.get(msg.obj);
      if ((t != null) && (t.source == source)) {
        System.out.println("[Target error : probably left the world]");
        targets.remove(msg.obj);
      }
    }
  }

  protected void processIntersect(IVrengObject source, IntersectMessage msg) {
    if (msg.getHeader().getVal() == MessageTypes.MSGV_ISECIN) {
      //pd System.out.println("[Got isec in]");
      Target t = new Target(source,
        new ObjPos(
        new Point3D(width/100.0, height/100.0, 0),
        new Point3D(0, 0, 0)));

      targets.put(msg.obj, t);
    } else {
      //pd System.out.println("[Got isec out]");
      targets.remove(msg.obj);
    }
  }

  public void close() {
    keepGoing = false;
    super.close();
  }
}
