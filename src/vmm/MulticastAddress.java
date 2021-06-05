package world;
import java.util.*;
import java.io.*;


/**
 * Class 'MulticastAddress'
 * Defines a multicast address.
 */

public class MulticastAddress implements Serializable {

  /* Static attributes. */

  private static int      multicastPort;   // Port for addresses.
  private static int      multicastTTL;    // Port for addresses.
  private static int      maxNb;           // Max number of addresses.
  private static int      num;             // Number of generated addresses.
  private static short [] genericAddress;  // Used for generating new addresses


  /* Attributes. */

  private short [] address;  // IPv4 address.
  private int      port;     // Its port.
  private int      TTL;      // Time to live.


  /* Static methods. */


  /**
   * Initialize class :
   * @param genericMulticastAddress is a special address used for generation,
   * @param multicastPort port for addresses,
   * @param multicastTTL TTL for addresses.
   */
  public static void init (String genericMulticastAddress,
			   int multicastPort, int multicastTTL) {
    StringTokenizer st;

    MulticastAddress.multicastPort = multicastPort;
    MulticastAddress.multicastTTL  = multicastTTL;

    // Creation of generic address.
    genericAddress = new short [4];
    st = new StringTokenizer (genericMulticastAddress, ".");
    for ( int n = 0; st.hasMoreTokens (); n++ )
      genericAddress [n] = (short) Integer.parseInt (st.nextToken ());

    // We compute max number of addresses and number of bytes for generation.
    maxNb = 1;
    for ( int m = 3; m >= 0; m-- )
      if ( genericAddress [m] == 0 )
	maxNb *= 256;
      else
	break;
    
    // No addresses yet.
    num = 0;
  }

  /**
   * Generate a new multicast address.
   */
  public static MulticastAddress getNew () {
    MulticastAddress maddr;

    // Creation of instance.
    maddr = new MulticastAddress (genericAddress [0],
				  genericAddress [1],
				  genericAddress [2],
				  genericAddress [3],
				  multicastPort,
				  multicastTTL);

    // Test if it remains free addresses.
    if ( num == maxNb ) {
      System.err.println ("Impossible to create a new uniq multicast address.");
      return maddr;
    }
	
    // We update address.
    for ( int n = 3, m = num; n >= 0; n-- ) {
      maddr.address [n] = (short) (m % 256);
      m /= 256;
      if ( m == 0 )
	break;
    }
    
    // One more address.
    num++;

    return maddr;
  }


  /* Constructors. */


  /**
   * Constructor :
   * @param a1 first byte of address,
   * @param a2 second byte of address,
   * @param a3 third byte of address,
   * @param a4 fourth byte of address,
   * @param port its port,
   * @param TTL time to live.
   */
  private MulticastAddress (short a1, short a2, short a3, short a4, int port, int TTL) {
    
    address = new short [4];
    address [0] = a1;
    address [1] = a2;
    address [2] = a3;
    address [3] = a4;
    this.port = port;
    this.TTL  = TTL;
  }


  /* Methods. */


  /**
   * Create a string representation of multicast address :
   * @return string representation.
   */
  public String toString () {

    return address [0] + "." + address [1] + "." + address [2] + "." + address [3] + 
           "/" + port + "/" + TTL;
  }

}
