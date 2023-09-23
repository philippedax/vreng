package world;
import java.util.*;


/**
 * Class 'Machine'
 * Define datas used to modelise a machine.
 */

public class Machine extends WorldObject {

  /* Attributes. */

  // Features of the machine.
  private String roomName;     // Its room name.
  private Room   room;         // Room which the machine is linked with.
  private String OSName;       // OS used on.
  private String machineType;  // Machine type.
  private int    uptime;       // Number of uptime days.
  private Vector IPs;          // List of IP address for machine.
  private Vector users;        // List of users on the machine.
  private Vector servers;      // Present servers on machine.g
  private int    nbProcesses;  // Number of executing processes.
  private double meanLoad;     // Load of system.
  private int    swapRatio;    // Used swap memory ratio (in percent).
  private Vector disks;        // Used disk.

  // Graphic data.
  private Pair subnetPosition;  // Machine position in subnet.
  private Pair roomPosition;    // Machine position in room.


  /* Constructors. */

  /**
   * Main constructor:
   * @param name its name.
   */
  public Machine (String name) {

    super (name);

    // Init graphic attributes.
    subnetPosition = null;
    roomPosition   = null;
  }


  /* Methods. */

  /**
   * Accessor:
   * @return Its room name.
   */
  public String getRoomName () {
    return roomName;
  };

  /**
   * Accessor:
   * @return Its room.
   */
  public Room getRoom () {
    return room;
  }

  /**
   * Accessor:
   * @return Its name.
   */
  public Vector getIPs () {
    return IPs;
  };

  /**
   * Accessor:
   * @return uptime days.
   */
  public int getUptime () {
    return uptime;
  };

  /**
   * Accessor:
   * @return OS used on.
   */
  public String getOSName () {
    return OSName;
  };

  /**
   * Accessor:
   * @return Machine type.
   */
  public String getMachineType () {
    return machineType;
  };

  /**
   * Accessor:
   * @return List of users on the machine.
   */
  public Vector getUsers () {
    return users;
  };

  /**
   * Accessor:
   * @return present servers on machine.
   */
  public Vector getServers () {
    return servers;
  };

  /**
   * Accessor:
   * @return Number of executing processes.
   */
  public int getNbProcesses () {
    return nbProcesses;
  };

  /**
   * Accessor:
   * @return Load of system.
   */
  public double getMeanLoad () {
    return meanLoad;
  };

  /**
   * Accessor:
   * @return Used swap memory ratio (in percent).
   */
  public int getSwapRatio () {
    return swapRatio;
  };

  /**
   * Accessor:
   * @return Used disk space ratio (in percent) for each disk.
   */
  public Vector getDisks () {
    return disks;
  };

  /**
   * Accessor:
   * @return Machine position in subnet.
   */
  public Pair getSubnetPosition () {
    return subnetPosition;
  };

  /**
   * Accessor:
   * @return Machine position in room.
   */
  public Pair getRoomPosition () {
    return roomPosition;
  };

  /**
   * Modifier:
   * @param roomName the name of room.
   */
  public void setRoomName (String roomName) {
    this.roomName = roomName;
  }

  /**
   * Modifier:
   * @param room attached room for machine.
   */
  public void attachRoom (Room room) {
    this.room = room;
  }

  /**
   * Modifier:
   * @param uptime the uptime.
   */
  public void setUptime (int uptime) {
    this.uptime = uptime;
  }

  /**
   * Modifier:
   * @param IPs List of IPs used on.
   */
  public void setIPs (Vector IPs) {
    this.IPs = IPs;
  }

  /**
   * Modifier:
   * @param OSName OS used on.
   */
  public void setOSName (String OSName) {
    this.OSName = OSName;
  }

  /**
   * Modifier:
   * @param OSName OS used on.
   */
  public void setMachineType (String machineType) {
    this.machineType = machineType;
  }

  /**
   * Modifier:
   * @param users List of users on the machine.
   */
  public void setUsers (Vector users) {
    this.users = users;
  }

  /**
   * Modifier:
   * @param servers present servers on machine.
   */
  public void setServers (Vector servers) {
    this.servers = servers;
  }

  /**
   * Modifier:
   * @param nbProcesses Number of executing processes.
   */
  public void setNbProcesses (int nbProcesses) {
    this.nbProcesses = nbProcesses;
  }

  /**
   * Modifier:
   * @param meanLoad Load of system.
   */
  public void setMeanLoad (double meanLoad) {
    this.meanLoad = Math.min (1.0, meanLoad);
  }

  /**
   * Modifier:
   * @param swapRatio Used swap memory ratio (in percent).
   */
  public void setSwapRatio (int swapRatio) {
    this.swapRatio = swapRatio;
  }

  /**
   * Modifier:
   * @param disks Used disk space ratio (in percent).
   */
  public void setDisks (Vector disks) {
    this.disks = disks;
  }

  /**
   * Modifier:
   * @param New position for machine
   */
  public void setSubnetPosition (Pair position) {
    subnetPosition = position;
  }

  /**
   * Modifier:
   * @param New position for machine
   */
  public void setRoomPosition (Pair position) {
    roomPosition = position;
  }

  /**
   * For displaying a machine...
   * @return string representation.
   */
  public String toString () {
    String IPsString = "";
    String usersString = "";
    String serversString = "";
    String disksString = "";
    Enumeration en;

    // String representation for IPs.
    for ( en = IPs.elements (); en.hasMoreElements (); )
      IPsString += (String) en.nextElement () + " ";

    // String representation for users.
    for ( en = users.elements (); en.hasMoreElements (); )
      usersString += (String) en.nextElement () + " ";

    // String representation for servers.
    for ( en = servers.elements (); en.hasMoreElements (); )
      serversString += (String) en.nextElement () + " ";

    // String representation for disks.
    for ( en = disks.elements (); en.hasMoreElements (); ) {
      Disk disk = (Disk) en.nextElement ();
      disksString += "[" + disk.getName () + ", " + disk.getSpaceRatio () + "%] ";
    }

    return "\nName            : " + name +
           "\nRoom name       : " + roomName +
           "\nType            : " + machineType +
           "\nOSName          : " + OSName +
           "\nIP addresses    : " + IPsString +
           "\nUsers           : " + usersString +
           "\nNb of processes : " + nbProcesses +
           "\nPresent servers : " + serversString +
           "\nUptime (days)   : " + uptime +
           "\nMean load [0-1] : " + meanLoad +
           "\nSwap ratio (%)  : " + swapRatio +
           "\nSpace ratio (%) : " + disksString + "\n";
  }

}
