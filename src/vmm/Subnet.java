package world;
import java.util.*;


/**
 * Class 'Subnet'
 */

public class Subnet extends WorldObject {
  
  /* Static attributes. */
  
  private static String netmask;  // Netmask;
  

  /* Attributes. */
  
  // Main attributes.
  private Vector machines;  // List of the machines.
  private Vector rooms;     // Representating rooms for subnetwork.
  

  /* Static methods. */


  /**
   * Init class:
   * @param netmask netmask used to construct.
   */
  public static void init (String netmask) {
    Subnet.netmask = netmask;
  }

  /**
   * Extract subnet name from IP address:
   * @param IP IP address,
   * @return name of subnet.
   */
  public static String extractSubnet (String IP) {
    String result = "";
    StringTokenizer st1, st2;

    // We extract subnet IP address with netmask.
    st1 = new StringTokenizer (IP, ".");
    st2 = new StringTokenizer (netmask, ".");
    for ( int num = 0; num < 4; num++ ) {
      // If error ?
      if ( !st1.hasMoreTokens () || !st2.hasMoreTokens () )
	return "unknown subnetwork";
      
      result += (Integer.parseInt (st1.nextToken ()) &
		 Integer.parseInt (st2.nextToken ())) + ".";
    }

    // We must delete last '.'.
    return result.substring (0, result.length () - 1);
  }


  /* Constructors. */
  
  
  /**
   * Main contructor:
   * @param IP IP address that contains the subnet.
   */
  public Subnet (String IP) {

    super (extractSubnet (IP));
    
    // List of machines and rooms creation.
    machines = new Vector ();
    rooms    = new Vector ();
  }
  

  /* Methods. */


  /**
   * Get machines:
   * @return list of machines in subnet.
   */
  public Vector getMachines () {
    return machines;
  }

  /**
   * Get rooms:
   * @return list of rooms for subnet.
   */
  public Vector getRooms () {
    return rooms;
  }

  /**
   * Add machine into subnetnork:
   * @param machine the machine.
   */
  public void addMachine (Machine machine) {
    machines.addElement (machine);
  }

  /**
   * Add room for subnetnork:
   * @param room the room.
   */
  public void addRoom (Room room) {
    
    // Add if not in.
    if ( !rooms.contains (room) )
      rooms.addElement (room);
  }
  
  /**
   * Test if IP is in subnetwork:
   * @param IP IP to test,
   * @return true if subnet contains IP.
   */
  public boolean containsIP (String IP) {
    return extractSubnet(IP).equals(name);
  }

  /**
   * For displaying a subnetwork...
   * @return string representation.
   */
  public String toString () {
    String machinesString = "";

    // String representation for machine.
    for ( Enumeration en = machines.elements (); en.hasMoreElements (); )
      machinesString += ((Machine) en.nextElement()).getName() + " ";

    return "\nname     : " + name +
           "\nmachines : " + machinesString + "\n";
  }
  
}
