package fr.enst.vreng.data;

import fr.enst.vreng.VrengException;

/**
 * Exception used when an invalid message is received from the network.
 */
public class InvalidMessageException extends VrengException {
  public InvalidMessageException() { super(); }
  public InvalidMessageException(String s) { super(s); }
}
