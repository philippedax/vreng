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
 * Sample Vreng application: TicTacToe-like game.
 * Two players at most. Try and get three identical
 * symbols aligned.
 */
public class TicTacToe extends VrengApp {

  volatile boolean keepRunning = true;
  public static final int bw = 33;
  public static final int ox = -100;
  public static final int oy = 50;

  int[] board = new int[9];
  IVrengObject player[] = new IVrengObject[2];

  Button[] buttons = new Button[10];
  int winner = -1;
  int turn = -1;

  class Button {
    Point2D tl;
    Point2D br;

    public Button(Point2D tl, Point2D br) {
      this.tl = tl;
      this.br = br;
    }

    public boolean inside(Point2D p) {
      return ((p.x>tl.x) && (p.x<br.x)
           && (p.y>tl.y) && (p.y<br.y));
    }
  }

  /**
   * Create the buttons and reset the board.
   */
  public void setup(String name, String url, int width, int height) {
    super.setup(name, url, width, height);
    createButtons();
    for (int i=0; i<2; i++) player[i] = null;
    resetBoard();
  }

  /**
   * Create the "buttons" (board boxes and new game)
   */
  void createButtons() {
    // board boxes
    for (int i=0; i<9; i++) {
      buttons[i] = new Button(
            new Point2D(ox+(i%3)*bw,     oy-((i/3)+1)*bw),
            new Point2D(ox+((i%3)+1)*bw, oy-(i/3)*bw));
    }
    // "new game
    buttons[9] = new Button(
      new Point2D(ox+140, oy-70),
      new Point2D(ox+210, oy-40));
  }

  /**
   * Clean the board and reset the appropriate state variables.
   */
  public void resetBoard() {
    winner = -1;
    for (int i=0; i<9; i++) board[i] = -1;
    DrawMessage em = new DrawMessage(MessageTypes.MSGT_DEL,
                                     MessageTypes.MSGV_DELALL, 0);
    sendAll(em);
    for (int i=0; i<2; i++) {
      if (player[i] != null) {
        try {
          send(player[i], markAt(-1, i));
        } catch (VrengException vre) {
          vre.printStackTrace();
          player[i] = null;
        }
      }
    }
  }

  /**
   * Returns the center of a board box.
   */
  protected Point2D boxCenter(int num) {
    return new Point2D(ox+(num%3)*bw+bw/2, oy-((num/3)+1)*bw+bw/2);
  }

  /**
   * Returns a DrawMessage to put a mark in the
   * given box for that user. Box number -1 is
   * to draw the user's mark on his screen.
   */
  protected DrawMessage markAt(int num, int player) {
    DrawMessage msg = null;
    Point2D center = (num > -1 ? boxCenter(num) : new Point2D(ox+165, oy-90));
    if (player == 1) {
      msg = new DrawMessage(MessageTypes.MSGT_ADD, MessageTypes.MSGV_CIRCLE);
      msg.addPoint(center);
      msg.addPoint(12, 12);
    } else {
      msg = new DrawMessage(MessageTypes.MSGT_ADD, MessageTypes.MSGV_LOOP);
      msg.addPoint(center.x-12, center.y-12);
      msg.addPoint(center.x+12, center.y+12);
      msg.addPoint(center.x-12, center.y+12);
      msg.addPoint(center.x+12, center.y-12);
    }
    return msg;
  }

  /**
   * When a user leaves.
   */
  public void onLeave(IVrengObject source) {
    int play = isPlayer(source);
    if (play > -1) {
      player[play] = null;
      resetBoard();
      turn = -1;
    }
  }

  /**
   * Returns the index of the player if he is part of the current game.
   */
  public int isPlayer(IVrengObject obj) {
    for (int i=0; i<2; i++) {
      if ((player[i] != null) && (player[i].getId().equals(obj.getId()))) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Process the user clicks.
   * Process the "new game" button first,
   * then the board clicks.
   */
  public boolean processButtons(Point2D p, int player) {
    for (int i=0; i<10; i++) {
      if (buttons[i].inside(p)) {
        System.out.println("B"+i+", w="+winner+", turn="+turn+"n, player="+player);
        if (i == 9) {
          resetBoard();
          turn = player;
        } else if ((winner == -1) && (turn == player) && (i < 9) && (board[i] == -1)) {
          board[i] = player;
          sendAll(markAt(i, player));
          turn = 1 - turn;
        }
        winner = checkWinner();
        return true;
      }
    }
    return false;
  }

  /**
   * Check to see if a given alignment is owned
   * by the same player or not. If it is, draw a line
   * over it and return the index of the winner.
   * If a winner was found, new players will have to be found.
   * This is done by clicking on the "new game" button. The first
   * two who click it are the players.
   */
  protected int checkWinner(int a, int b, int c) {
    if ((board[a] != -1)
     && (board[a] == board[b])
     && (board[a] == board[c])) {
      DrawMessage dm = new DrawMessage(MessageTypes.MSGT_ADD, MessageTypes.MSGV_LINE);
      dm.addPoint(boxCenter(a));
      dm.addPoint(boxCenter(c));
      sendAll(dm);
      player[0] = null;
      player[1] = null;
      turn = -1;
      return board[a];
    }
    return -1;
  }

  /**
   * Check to see if we have a winner.
   */
  public int checkWinner() {
    int won = -1;
    // rows
    for (int i=0; ((i<3) && (won == -1)); i++)
      won = checkWinner(3*i, 3*i+1, 3*i+2);
    if (won>-1) return won;

    // colomns
    for (int i=0; ((i<3) && (won == -1)); i++)
      won = checkWinner(i, i+3, i+6);
     if (won>-1) return won;

    // diagonal 1
    won = checkWinner(0, 4, 8);
    if (won>-1) return won;

    // diagonal 2
    won = checkWinner(2, 4, 6);
    if (won>-1) return won;

    // no winner
    return -1;
  }

  /**
   * Deal with incoming clicks.
   * First check that the player who clicked was actually in the game.
   * If he was, check out where the user clicked.
   */
  protected void processClick(IVrengObject source, ClickMessage msg) {
    int play = isPlayer(source);
    if ((play == -1) && (buttons[9].inside(msg.pos))) {
      // Not a legal player, but he clicked on "new game"
      boolean newuser = false;
      for (int i=0; ((newuser == false) && (i<2)); i++) {
        // There is a free spot
        if (player[i] == null) {
          newuser = true;
          player[i] = source;
          play = i;
        }
      }
    }
    if (play == -1) {
      // The board is busy
      System.out.println("[Can't play]");
    } else {
      processButtons(msg.pos, play);
    }
  }

  /**
   * Main loop: do nothing but wait for the app to be closed.
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

  public void close() {
    keepRunning = false;
    super.close();
  }
}
