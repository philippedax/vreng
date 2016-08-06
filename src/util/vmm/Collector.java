package collector;
import java.util.*;
import java.io.*;
import world.*;


/**
 * Class 'Collector'.
 */

public class Collector {

  /* Static attributes. */

  static public GlobalProperties properties;  // Global properties for worlds.


  /* Attributes. */

  private World world;                 // World (set of rooms, machines, subnetworks...).
  private boolean verbose;             // For verbose...
  private boolean forceWorldCreation;  // Force creation of world if it already exists.


  /* Constructors. */


  /**
   * Main constructor:
   * @param verbose true if verbosing...
   * @param forceWorldCreation true if recreate world.
   */
  public Collector (boolean verbose, boolean forceWorldCreation) {

    this.verbose = verbose;
    this.forceWorldCreation = forceWorldCreation;

    // We specify attributes for class.
    MulticastAddress.init (properties.genericMulticastAddress,
			   properties.multicastPort,
			   properties.multicastTTL);
    Subnet.init (properties.subnetMask);
  }


  /* Methods. */


  /**
   * Execute the whole job of collecting.
   */
  public void execute () {

    // Does the world already exist ?
    if ( worldExists () && !forceWorldCreation )
      loadWorld ();
    else
      createWorld ();

    // Update infos.
    updateInfos ();

    // Create rooms and subnetworks.
    createRoomsSubnets ();

    // Create 'vre' file for each subnetworks.
    createVRESubnets ();

    // Create 'vre' file for each rooms.
    createVRERooms ();

    // Create 'vre' file for each machines.
    createVREMachines ();

    // Save world into disk.
    saveWorld ();

    // Write a readable file for infos.
    saveCollecting ();
  }


  /* Private methods. */


  /**
   * Test if world already exists.
   * @return true if 'world' file is found.
   */
  private boolean worldExists () {
    File file;

    printVerbose ("testing if world description file already exists");

    // Create the file...
    file = new File (properties.statsPath +
                     properties.vmmDir +
                     properties.worldFilename);

    return file.exists ();
  }

  /**
   * Load world from disk.
   */
  private void loadWorld () {

    printVerbose ("loading world description from file");

    // We load world...
    world = World.load (properties.statsPath +
                        properties.vmmDir +
                        properties.worldFilename);

    // If problem, creation.
    if ( world == null ) {
      printVerbose ("creating world description");
      createWorld ();
    }
  }

  /**
   * Save world into disk.
   */
  private void saveWorld () {

    printVerbose ("saving world description into file");

    // We save world...
    World.save (world, properties.statsPath +
                       properties.vmmDir +
                       properties.worldFilename);
  }

  /**
   * Create a new world with set of machines.
   * To get machine name, we read 'ping' directory.
   */
  private void createWorld () {
    Vector machines;
    File [] files;

    printVerbose ("creating a new world description");

    // Vector creation.
    machines = new Vector ();

    // Open the 'ping' directory and get files inside.
    files = (new File (properties.statsPath + "ping")).listFiles ();

    // We read the directory.
    for ( int num = 0; num < files.length; num++ ) {

      System.out.println ("ping:" + files[num].getName());
      // Add machine.
      machines.addElement (new Machine (files[num].getName()));

    }
    // 'World' instance creation.
    world = new World (machines);
  }

  /**
   * Collect infos for each machine.
   * For collecting, use a 'MachineCollector' object for each machine.
   */
  private void updateInfos () {

    printVerbose ("collecting informations for each machine");

    // Enumerate machines.
    for ( Enumeration en = world.getMachines().elements(); en.hasMoreElements (); )

      // Collects Info about machine.
      (new MachineCollector ((Machine) en.nextElement ())).collect ();
  }

  /**
   * Create rooms and subnetworks in the world.
   */
  private void createRoomsSubnets () {

    printVerbose ("creating subnetworks");
    world.createSubnets ();
    printVerbose ("creating rooms");
    world.createRooms ();
    printVerbose ("creating rooms for each subnetwork");
    world.createRoomsInSubnets ();
  }

  /**
   * Write 'vre' file description for each subnetwork.
   * For writing, use a 'VRENSubnetWriter' object for each subnetwork.
   */
  private void createVRESubnets () {

    printVerbose ("creating 'vre' file for each subnetwork");

    // Enumerate subnetworks.
    for ( Enumeration en = world.getSubnets().elements(); en.hasMoreElements(); )
      (new VRESubnetWriter ((Subnet) en.nextElement())).write ();
  }

  /**
   * Write 'vre' file description for each room.
   * For writing, use a 'VRENRoomWriter' object for each room.
   */
  private void createVRERooms () {

    printVerbose ("creating 'vre' file for each room");

    // Enumerate rooms.
    for ( Enumeration en = world.getRooms().elements(); en.hasMoreElements(); )
      (new VRERoomWriter ((Room) en.nextElement())).write ();
  }

  /**
   * Write 'vre' file description for each machine.
   * For writing, use a 'VREMachineWriter' object for each machine.
   */
  private void createVREMachines () {

    printVerbose ("creating 'vre' file for each machine");

    // Enumerate rooms.
    for ( Enumeration en = world.getMachines().elements(); en.hasMoreElements(); )
      (new VREMachineWriter ((Machine) en.nextElement())).write ();
  }

  /**
   * Save infos in a readable format.
   */
  public void saveCollecting () {

    printVerbose ("saving collecting in a text file");

    try {
      FileWriter fw = new FileWriter (properties.statsPath +
                                      properties.vmmDir +
                                      properties.collectingFilename);
      BufferedWriter bw = new BufferedWriter (fw);
      String s = world.toString ();

      // We write...
      bw.write (s, 0, s.length ());

      // Close stream.
      bw.close ();

    } catch ( IOException e ) {
      System.err.println ("error during infos writing...");
    }
  }

  /**
   * Manages verbose message:
   * @param message the message.
   */
  private void printVerbose (String message) {

    if ( !verbose )
      return;

    // Print message.
    System.out.println ("=> collector: " + message + "...");
  }

  /* Entry point. */

  /**
   * Main fumction:
   * @param args the arguments.
   */
  public static void main (String [] args) {
    Collector collector;
    ArgumentsParser parser;
    boolean verbose;
    boolean forceWorldCreation;
    String preferences;

    // Parse parameters.
    parser = new ArgumentsParser ("collector", args);
    verbose = parser.getOption ("verbose", 'v', "active verbose mode");
    forceWorldCreation = parser.getOption ("force-world-creation", 'c', "force creation of world if it already exists");
    preferences = parser.getOptionValue ("preferences", 'p', true, "specify preferences file");
    parser.end ();

    // Properties manager.
    properties = new GlobalProperties (preferences);
    properties.load ();

    // Creation of collector object.
    collector = new Collector (verbose, forceWorldCreation);

    // Let's go !!!
    System.out.println ("collecting...");
    collector.execute ();
  }
}
