package world;
import java.util.*;
import java.io.*;


/**
 * Class WorldVisualisator
 */

public class World implements Serializable {

  /* Attributes. */

  private Vector subnets;   // The subnetworks of world.
  private Vector rooms;     // The rooms of our world (Note each subnetwork is composed by many rooms).
  private Vector machines;  // The machines of the world.


  /* Static methods. */


  /**
   * Load world from file :
   * @param name name of file,
   * @return new loaded world.
   */
  public static World load (String name) {
    World world;

    try {
      FileInputStream fis = new FileInputStream (name);
      ObjectInputStream ois = new ObjectInputStream (fis);

      // We read 'World' object.
      world = (World) ois.readObject ();

      // Close stream.
      fis.close ();

    } catch ( Exception e ) {
      System.err.println ("Impossible to get 'world' description from file...");
      return null;
    }

    return world;
  }


  /**
   * Save world into file :
   * @param name name of file.
   */
  public static void save (World world, String name) {

    try {
      FileOutputStream fos = new FileOutputStream (name);
      ObjectOutputStream oos = new ObjectOutputStream (fos);

      // We write 'World' object.
      oos.writeObject (world);

      // Flush and close streams.
      oos.flush ();
      fos.close ();

    } catch ( IOException e ) {
      System.err.println ("Impossible to save 'world' description...");
      return ;
    }
  }


  /* Constructors. */


  /**
   * Main contructor :
   * @param machines list of machines in the world.
   */
  public World (Vector machines) {

    this.machines = machines;
  }


  /* Methods. */


  /**
   * Get machines :
   * @return list of machines.
   */
  public Vector getMachines () {
    return machines;
  }

  /**
   * Get rooms :
   * @return list of rooms.
   */
  public Vector getRooms () {
    return rooms;
  }

  /**
   * Get subnetworks :
   * @return list of subnetworks.
   */
  public Vector getSubnets () {
    return subnets;
  }

  /**
   * Create rooms in using machine infos.
   */
  public void createRooms () {

    // Vector creation.
    rooms = new Vector ();

    // Enumerate machines.
    for ( Enumeration en = machines.elements(); en.hasMoreElements (); ) {
      Machine machine = (Machine) en.nextElement ();
      String roomName = machine.getRoomName ();
      Room room;

      // We search if room for machine already exists.
      room = null;
      for ( Enumeration en1 = rooms.elements (); en1.hasMoreElements (); ) {
	room = (Room) en1.nextElement();
	if ( room.containsRoomName (roomName) )
	  break;
	room = null;
      }

      // The room doesn't exist, let's create it !
      if ( room == null ) {
	room = new Room (roomName);
	rooms.addElement (room);
      }

      // Attach room to machine.
      machine.attachRoom (room);

      // Add machine in room.
      room.addMachine (machine);
    }
  }

  /**
   * Create subnets in using machine infos.
   */
  public void createSubnets () {

    // Vector creation.
    subnets = new Vector ();

    // Enumerate machines.
    for ( Enumeration en = machines.elements(); en.hasMoreElements (); ) {
      Machine machine = (Machine) en.nextElement ();
      Vector IPs = machine.getIPs ();

      // Enumerate all IP addresses of machine.
      for ( Enumeration en1 = IPs.elements (); en1.hasMoreElements (); ) {
	String IP = (String) en1.nextElement ();
	Subnet subnet = null;

	// We search a subnet which contains IP address.
	for ( Enumeration en2 = subnets.elements (); en2.hasMoreElements (); ) {
	  subnet = (Subnet) en2.nextElement ();
	  if ( subnet.containsIP (IP) )
	    break;
	  subnet = null;
	}
	
	// No subnet corresponds to this IP, let's create one.
	if ( subnet == null ) {
	  subnet = new Subnet (IP);
	  subnets.addElement (subnet);
	}

	// We add machine in subnet.
	subnet.addMachine (machine);
      }
    }
  }

  /**
   * Create rooms in a subnetwork.
   */
  public void createRoomsInSubnets () {

    // Enumerate subnets.
    for ( Enumeration en = subnets.elements (); en.hasMoreElements (); ) {
      Subnet subnet = (Subnet) en.nextElement ();

      // Enumerate machine in subnet.
      for ( Enumeration en1 = subnet.getMachines().elements(); en1.hasMoreElements (); )
	subnet.addRoom (((Machine)en1.nextElement()).getRoom ());
    }
  }

  /**
   * For displaying a world...
   * @return string representation.
   */
  public String toString () {
    String subnetsString = "";
    String roomsString = "";
    String machinesString = "";
    Enumeration en;

    // We create subnetworks representation.
    for ( en = subnets.elements (); en.hasMoreElements (); )
      subnetsString += ((Subnet) en.nextElement ()).toString ();

    // We create rooms representation.
    for ( en = rooms.elements (); en.hasMoreElements (); )
      roomsString += ((Room) en.nextElement ()).toString ();

    // We create machines representation.
    for ( en = machines.elements (); en.hasMoreElements (); )
      machinesString += ((Machine) en.nextElement ()).toString ();

    return " World collecting produces by 'Collector' (c) \n" +
           "\n\n S U B N E T W O R K S\n" + subnetsString +
           "\n\n R O O M S\n" + roomsString +
           "\n\n M A C H I N E S\n" + machinesString;
  }

}
