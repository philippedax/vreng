package fr.enst.vreng;

/**
 * Interface that applications need to implement.
 */
public interface IVrengApp extends Runnable {

	/**
	 * Method used to initialize the client.
	 */
	public void setup(String name, String url, int width, int height);

	/**
	 * Should return this app's name.
	 */
	public String getName();

	/**
	 * Called to start the app.
	 */
	public void start();

	/**
	 * Called when a new client joins.
	 */
	public void addClient(IVrengObject obj);

	/**
	 * Should return true if no more clients are registered.
	 */
	public boolean isFree();

	/**
	 * Called when a client quits.
	 */
	public void removeClient(IVrengObject obj);

	/**
	 * Called when no more Vreng apps are listening
	 */
	public void stop();

	/**
	 * Called when a message arrives.
	 */
	public void incoming(IVrengObject source, IMessage msg);

}
