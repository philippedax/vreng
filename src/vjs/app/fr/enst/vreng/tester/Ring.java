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

import java.util.Random;
import java.util.Vector;
import java.util.Enumeration;

@SuppressWarnings("unchecked")


/**
 * Sample Vreng application: mobile object.
 * This application will try to move its Vreng Vrelet object
 * to the center of a random user in the Vreng scene.
 * It demonstrates user query messages and self rotation/translation.
 */
public class Ring extends VrengApp {

  ObjPos me, target;

  volatile boolean keepGoing = true;

  public void setup(String name, String url, int width, int height) {
    super.setup(name, url, width, height);
    me = null;
    target = null;
  }

  public void run() {
    try {
      sendPrimary(new PosMessage((NetObjId)null)); // ask for my position
    } catch (VrengException ve) {
      ve.fillInStackTrace().printStackTrace();
    }
    updateTarget();

    // Delay effective start by a random amount, so multiple rings
    // in the same world are out of sync.
    try {
      Thread.sleep((new Random()).nextInt(500) + 50);
    } catch (InterruptedException ie) {
      keepGoing = false;
      return;
    }

    int cnt = 1;
    boolean gotit = false;
    while (keepGoing) {
      try {
        Thread.sleep(200);  // 400
      } catch (InterruptedException ie) {
        keepGoing = false;
        return;
      }
      if (keepGoing && (me != null) && (target != null)) {
        ObjPos delta = new ObjPos();
        synchronized(me) {
          delta.pos = Point3D.sub(target.pos, me.pos);
          delta.pos.scale(0.05);
          if (delta.pos.norm() <= 0.08) {
            me.pos = target.pos;
            me.ang.add(new Point3D(0.1, 0.1, 0));
            if (!gotit)
              sendAll(new PosMessage((NetObjId)null)); // ask for my position
            gotit = true;
          }
          else {
            me.pos.add(delta.pos);
            gotit = false;
          }
          sendAll(new PosMessage(MessageTypes.MSGV_SET, null, // move myself
            me));
        }
        if ((cnt % 20) == 0) updateTarget();
        cnt++;
      }
    }
  }

  public void updateTarget() {
    if (!keepGoing)
      return;

    QueryMessage qm = new QueryMessage(MessageTypes.MSGV_QTYPE,
                                       ObjectTypes.OBJ_User);
    QueryMessage ans = null;
    try {
      ans = sendQuery(qm);
    } catch (VrengException ve) {
      ve.fillInStackTrace().printStackTrace();
    }
    if (ans == null)
      return;

    if (ans.answers.size() > 0) {
      //pd System.out.println("["+getName()+": got a target!]");
      try {
        sendPrimary(new PosMessage((NetObjId) ans.answers.elementAt(0)));
      } catch (VrengException ve) {
        ve.fillInStackTrace().printStackTrace();
      }
    }
    else {
      System.out.println("["+getName()+": no target found]");
    }
  }

  protected void processPos(IVrengObject source, PosMessage msg) {
    int val = msg.getHeader().getVal();
    if (val == MessageTypes.MSGV_SET) {
      if (msg.obj.type == 0) {
        me = msg.pos;
      }
      else {
        target = msg.pos;
      }
    }
    else if (val == MessageTypes.MSGV_ERROR) {
      synchronized(me) {
        //pd System.out.println("["+getName()+": Need a target update]");
        target = null;
        updateTarget();
      }
    }
    else {
      System.out.println("["+getName()+": Can't handle this : "+msg+"]");
    }
  }

  public void close() {
    keepGoing = false;
    super.close();
  }
}
