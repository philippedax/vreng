package fr.enst.vreng;

import fr.enst.vreng.data.Header;

/**
 * Interface that objets wanting to receive messages from VRENG
 * need to implement.
 */
public interface IVrengProcessor {

  /**
   * Method called when a message arrives from VRENG.
   */
  public void notify(IMessage msg);

  /**
   * Called when an error occured with an incoming packet.
   * @param boolean fatal <B>true</B> if the error has compromised the underlying message source.
   * @param fr.enst.vreng.VrengException the exception raised
   * @param fr.enst.vreng.data.Header the message's header if is had been reconstructed, <B>null</B> otherwise.
   */
   public void notify(boolean fatal, VrengException problem, Header header);
}
