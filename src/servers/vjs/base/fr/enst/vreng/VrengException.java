package fr.enst.vreng;

/**
 * Root class for all Vreng exceptions.
 */
public class VrengException extends Exception {

	public VrengException() {
		this("Unspecified exception");
	}

	public VrengException(String s) {
		super(s+" ("+ Thread.currentThread().getName()+")");
	}

}
