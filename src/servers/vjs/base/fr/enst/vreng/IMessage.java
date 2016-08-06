package fr.enst.vreng;

import fr.enst.vreng.data.Header;
import fr.enst.vreng.data.MsgOutputStream;
import fr.enst.vreng.data.types.NetObjId;

/**
 * Interface all the VRENG messages implement.
 */
public interface IMessage {

	/**
	 * Returns the message's header.
	 */
	public Header getHeader();

	/**
	 * Returns the message's source.
	 */
	public NetObjId getSource();

	/**
	 * Returns the message converted to a byte array for transmission.
	 */
	public void pack(MsgOutputStream mos);
}
