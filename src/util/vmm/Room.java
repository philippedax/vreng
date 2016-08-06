package world;
import java.util.*;


/**
 * Class Room
 */

public class Room extends WorldObject {

  /* Attributes. */

  // Features.
  private Vector machines;  // List of the machines.


  /* Static methods. */


  /**
   * Extract room name from a whole one:
   * @param roomName whole name,
   * @return simplified name for room.
   */
  public static String extractName (String roomName) {
    StringTokenizer st;

    // We parse.
    st = new StringTokenizer (roomName, "-");

    if ( st.hasMoreTokens () )
      return st.nextToken ();
    else
      return roomName;
  }


  /* Constructors. */


  /**
   * Main contructor:
   * @param name name of the room.
   */
  public Room (String name) {

    super (extractName (name));

    // list of machines creation.
    machines = new Vector ();
  }


  /* Methods. */


  /**
   * Get the name:
   * @return the name.
   */
  public String getName () {
    return name;
  }

  /**
   * Get machines:
   * @return list of machines in room.
   */
  public Vector getMachines () {
    return machines;
  }

  /**
   * Add a machine in the room:
   * @param machine The machine.
   */
  public void addMachine (Machine machine) {
    machines.addElement (machine);
  }

  /**
   * Test if room name is in this room:
   * @param roomName name to test,
   * @return true if this room is contained.
   */
  public boolean containsRoomName (String roomName) {
    return extractName(roomName).equals(name);
  }

  /**
   * For displaying a room...
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
