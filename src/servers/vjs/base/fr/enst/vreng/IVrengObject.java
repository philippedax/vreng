package fr.enst.vreng;

import fr.enst.vreng.VrengException;
import fr.enst.vreng.data.types.NetObjId;

/**
 * Interface used to represent an Ifc object in a Vreng scene.
 * It holds the object's identification, and exposes methods
 * to send data to that particular object.
 */
public interface IVrengObject {

	/**
	 * Should return the object's identifier.
	 */
	public NetObjId getId();
	
	/**
	 * Should return true if the object is alive in Vreng and
	 * if a communication link has been established to it.
	 */
	public boolean isUp();
	
	/**
	 * Transmit a message to the object.
	 */
	public void send(IMessage message) throws VrengException;
	
	/**
	 * Called when this object is no longer needed.
	 * Should clean up and release any resources this object
	 * is holding on to.
	 */
	public void close();

}
