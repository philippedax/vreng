package fr.enst.vreng.tester;

import fr.enst.vreng.ObjectTypes;
import fr.enst.vreng.MessageTypes;
import fr.enst.vreng.VrengException;
import fr.enst.vreng.net.*;
import fr.enst.vreng.data.*;
import fr.enst.vreng.data.types.*;
import fr.enst.vreng.data.messages.*;
import fr.enst.vreng.IVrengApp;
import fr.enst.vreng.IVrengObject;
import fr.enst.vreng.server.VrengApp;

import java.io.InputStream;
import java.io.IOException;
import java.util.Random;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.NoSuchElementException;

@SuppressWarnings("unchecked")


/**
 * Sample Vreng application: shared drawing board.
 * This application enables multiple Vreng clients
 * to draw on the same Vrelet object.
 * The application does change any position, but
 * processes click messages and sends ADD and DEL
 * messages.
 */
public class Board extends VrengApp {

  public static final int B_SIZE = 20;
  public static final int S_SIZE = 2;

  private DrawMessage[] buttons = new DrawMessage[2];

  /** Gets set to false whenever this app needs to stop. */
  volatile boolean keepRunning = true;

  /** List of the last points that got clicked */
  protected Vector points = new Vector();
  /** List of images that are currently drawn on the surface */
  protected Vector figures = new Vector();

  /** Creates the "buttons" */
  public void setup(String name, String url, int width, int height) {
    super.setup(name, url, width, height);

    buttons[0] = new DrawMessage(MessageTypes.MSGT_ADD, MessageTypes.MSGV_LOOP);
    buttons[0].addPoint(-width+B_SIZE, height);
    buttons[0].addPoint(-width+B_SIZE, height-B_SIZE);
    buttons[0].addPoint(-width, height-B_SIZE);
    buttons[0].addPoint(-width, height);

    buttons[1] = new DrawMessage(MessageTypes.MSGT_ADD, MessageTypes.MSGV_LOOP);
    buttons[1].addPoint(-width+B_SIZE, height);
    buttons[1].addPoint(-width+B_SIZE, height-B_SIZE);
    buttons[1].addPoint(-width+B_SIZE*2, height-B_SIZE);
    buttons[1].addPoint(-width+B_SIZE*2, height);
  }

  /**
   * Update's a new user's display.
   * When a user joins, we need to send him the
   * draw messages so that he sees the buttons
   * and the shapes that had already been drawn.
   */
  public void onJoin(IVrengObject source) {
    try {
      // Send the buttons
      send(source, buttons[0]);
      send(source, buttons[1]);
      // Send the drawings
      Enumeration e = figures.elements();
      while (e.hasMoreElements()) {
        send(source, (DrawMessage) e.nextElement());
      }
    } catch (VrengException ve) {
      // Not a big deal - the user already left, probably
      ve.fillInStackTrace().printStackTrace();
    }
  }

  /**
   * Used when the board is cleared: resend the buttons to everyone.
   */
  public void createButtons() {
    sendAll(buttons[0]);
    sendAll(buttons[1]);
  }

  /**
   * Checks whether a button was pressed, and deals with it if one was.
   */
  public boolean processButtons(int x, int y) {
    if (y<height-B_SIZE) return false;

    if (x<-width+B_SIZE) {
      // first button: clear all
      deleteAll();
      return true;
    } else if (x<-width+2*B_SIZE) {
      // Second button: undo
      if (figures.size()==0) return true;
      DrawMessage last = (DrawMessage) figures.lastElement();
      deleteOne(last.getTag());
      return true;
    } else {
      return false;
    }
  }

  /**
   * Clear the board.
   * Once the point list has been cleared, we also need to recreate the buttons.
   */
  public void deleteAll() {
    DrawMessage em = new DrawMessage(MessageTypes.MSGT_DEL,
                                     MessageTypes.MSGV_DELALL, 0);
    sendAll(em);
    createButtons();
    points.clear();
  }

  /**
   * Delete the last figure drawn.
   */
  public void deleteOne(int tag) {
    Enumeration e = figures.elements();
    while (e.hasMoreElements()) {
      // Find the figure in the list
      DrawMessage dm = (DrawMessage) e.nextElement();
      if (dm.getTag() == tag) {
        figures.remove(dm);
        DrawMessage em = new DrawMessage(MessageTypes.MSGT_DEL,
                                         dm.getHeader().getVal(),
                                         dm.getTag());
        sendAll(em);
        return;
      }
    }
    System.out.println("[Not found "+tag+"]");
  }

  /**
   * Main loop: does nothing but wait.
   * This application does not need to do anything
   * special aside from processing Vreng clicks
   */
  public void run() {
    while (keepRunning) {
      try {
        Thread.sleep(5000);
      } catch (InterruptedException ie) {
        keepRunning = false;
        return;
      }
    }
  }

  /**
   * Deal with an incoming click message.
   * Create a new circle at the click point,
   * then draw a line connecting the last five points
   * if necessary.
   */
  protected void processClick(IVrengObject source, ClickMessage msg) {
    // Check if the user clicked on a button
    if (processButtons(msg.pos.x, msg.pos.y)) return;

    // Draw a circle
    DrawMessage dm = new DrawMessage(MessageTypes.MSGT_ADD,
                                     MessageTypes.MSGV_CIRCLE);
    dm.addPoint(msg.pos);
    dm.addPoint(width/30, width/30);

    points.add(msg.pos);
    figures.addElement(dm);

    sendAll(dm);

    // Draw the connecting line
    if (points.size() > 4) {
      DrawMessage dm2 = new DrawMessage(MessageTypes.MSGT_ADD,
                                        MessageTypes.MSGV_LINE);
      Enumeration e = points.elements();
      while (e.hasMoreElements()) {
        dm2.addPoint((Point2D) e.nextElement());
      }
      figures.addElement(dm2);
      sendAll(dm2);

      points.removeAllElements();
    }
  }

  /**
   * Called when no connected users are left.
   */
  public void close() {
    keepRunning = false;
    super.close();
  }
}
