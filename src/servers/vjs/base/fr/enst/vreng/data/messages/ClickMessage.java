package fr.enst.vreng.data.messages;

import fr.enst.vreng.data.types.*;

import fr.enst.vreng.data.Message;
import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;
import fr.enst.vreng.data.InvalidMessageException;

import fr.enst.vreng.MessageTypes;

/* Message info
type=MSGT_CLICK

val=ALL
Point2D  the location where the user clicked
*/

/**
 * Represents CLICK messages.
 */
public class ClickMessage extends Message {

  public Point2D pos = null;

  public ClickMessage(MsgInputStream mis) throws InvalidMessageException {
    super(mis);
    pos = Point2D.unpack(mis);
  }
}
