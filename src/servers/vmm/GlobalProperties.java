package collector;
import java.util.*;
import java.io.*;
import world.*;


/**
 * Class 'GlobalProperties'
 * Provides customizable features for whole application.
 */

public class GlobalProperties implements Serializable {

  /* Attributes. */

  // Network properties.
  public String subnetMask;               // Netmask.
  public String genericMulticastAddress;  // Generic IP address for multicast.
  public int    multicastPort;            // Port for multicast addresses.
  public int    multicastTTL;             // TTL for multicast addresses.
  public String vrengRootURL;             // Root URL for accessing to vreng homepage.
  public String domainName;               // Name of domain for hosts.

  // IO properties.
  public String statsPath;                // Root path for 'vmm'
  public String vmmDir;                   // 'vmm' directory.
  public String vreDir;                   // Directory where there is 'vre' source file.
  public String vreExt;                   // 'vre' file extension.
  public String worldFilename;            // File name of serialized object 'World'.
  public String collectingFilename;       // File name of infos in text.

  // Collecting properties.
  public String servers;                  // Present servers on machine.

  // Graphic attributes (2D).
  public Pair   subnetDefaultDimension;   // Default dimension of a subnet (in meters).
  public Pair   roomDefaultDimension;     // Default dimension of a room (in meters).
  public Pair   machineDefaultDimension;  // Default dimension of a machine (in meters).
  public double machineSpacing;           // Space between two machines (in meter).

  // Graphic attributes for vreng (3D).
  public Triplet    machineDimension;     // 3D dimension of machine.
  public VREColor   machineColor;         // Color of a machine.
  public Quadruplet subnetEntryPosition;  // Position to avatar when entering in a subnet.
  public Quadruplet machineEntryPosition; // Position to avatar when entering in a machine.
  public Quadruplet roomEntryPosition;    // Position to avatar when entering in a room.
  public double     wallHeight;           // Height for a wall.
  public double     wallDepth;            // Depth for a wall.
  public VREColor   subnetSkyColor;       // Color of sky for a subnetwork.
  public VREColor   roomSkyColor;         // Color of sky for a room.
  public VREColor   machineSkyColor;      // Color of sky for a machine.
  public double     roomDoorSpacing;      // Space between two doors in a subnetwork representation.
  public Triplet    doorDimension;        // Dimension of a door.
  public Triplet    panelDimension;       // Dimension of a panel.
  public double     panelTextHeight;      // Height for text on a panel.
  public double     panelTeeWidth;        // Width of tee for a panel.
  public double     featuresSpacing;      // Spacing between different feature in a machine.  
  public Quadruplet machineInfosPosition; // Position of infos text for a machine.
  public String     machineInfosFontURL;  // Font for a machine.
  public double     userSpacing;          // Spacing between two users.
  public Triplet    userDimension;        // Dimension for user representation.
  public VREColor   userHeadColor;        // Color of head for users.
  public VREColor   userTeeColor;         // Color of tee for each user.
  public double     daySpacing;           // Spacing between two day representations.
  public double     dayRadius;            // Radius of sphere for day representation.
  public VREColor   dayColor;             // Color for a day representation.
  public Pair       meanLoadDimension;    // Dimension of cylinder for mean representation.
  public VREColor   meanLoadColor;        // Color for mean load representation.
  public Pair       swapDimension;        // Dimension of cylinder for swap representation.
  public VREColor   swapColor;            // Color for free space of swap.
  public double     processSpacing;       // Spacing between two process representations.
  public Triplet    processDimension;     // Dimension of each process representation.
  public VREColor   processColor;         // Color for each process representation.
  public double     serverSpacing;        // Spacing between two server representations.
  public Pair       serverDimension;      // Server dimension.
  public VREColor   serverColor;          // Color for a server representation.
  public double     spaceSpacing;         // Spacing between two spaces representation.
  public Pair       spaceDimension;       // Dimension of space cylinder.
  public VREColor   spaceColor;           // Color of free space.

  // Textures.
  public String     subnetFloorTexURL;    // Used texture for subnetwork floor.
  public String     subnetWallTexURL;     // Used texture for subnetwork wall.
  public String     roomFloorTexURL;      // Used texture for room floor.
  public String     roomWallTexURL;       // Used texture for room wall.
  public String     machineFloorTexURL;   // Used texture for machine floor.
  public String     machineWallTexURL;    // Used texture for machine wall.
  public String     doorTexURL;           // Texture for a door.
  public String     panelTexURL;          // Texture for a panel.
  public String     machineTexURL;        // Texture for a machine.

  // Fonts.
  public String     panelFontURL;         // Font used to write on a panel.

  // Private attributes.
  private Properties list;   // Used for loading and saving features.
  private String name;       // Name of loaded file.


  /* Constructors. */


  /**
   * Main default constructor :
   * @param name name of file.
   */
  public GlobalProperties (String name) {

    this.name = name;

    // Creation of an objet 'Properties'.
    list = new Properties ();
  }


  /* Methods. */
  
  
  /**
   * Load from file.
   */
  public void load () {
    FileInputStream fis;

    // We open file.
    try {
      fis = new FileInputStream (name);
    } catch ( IOException e ) {
      fis = null;
    }
    
    // We load file (if exists).
    if ( fis != null ) {
      try {
	list.load (fis);
      } catch ( IOException e ) {
	System.err.println ("Error during properties file loading.");
      }

      // We close.
      try {
	fis.close ();
      } catch ( IOException e ) {
	System.err.println ("Error during properties file closing.");
      }
    } 

    // Get properties.
    getProperties ();
  }


  /**
   * Save into file :
   */
  public void save () {
    FileOutputStream fos;

    // Set properties.
    setProperties ();

    // We open file.
    try {
      fos = new FileOutputStream (name);
    } catch ( IOException e ) {
      System.err.println ("Impossible to create properties file");
      return ;
    }
    
    // We save file.
    try {
      list.store (fos, "World global parameter - generated by 'Collector' (c)");
    } catch ( IOException e ) {
      System.err.println ("Error during properties file saving.");
    }
    
    // We close.
    try {
      fos.close ();
    } catch ( IOException e ) {
      System.err.println ("Error during properties file closing.");
    }
  }


  /**
   * Invoked by GC...
   * Allows to save misc features.
   */
  protected void finalize () {
    save ();
  }


  /* Private methods. */

    
  /**
   * Get all properties.
   */
  private void getProperties () {
    String d1, d2, d3, d4;    

    subnetMask = list.getProperty ("subnet_mask", "255.255.254.0");

    genericMulticastAddress = list.getProperty ("generic_multicast_adress", "239.254.0.0");

    d1 = list.getProperty ("multicast_port", "45678");
    multicastPort = Integer.parseInt (d1);

    d1 = list.getProperty ("multicast_TTL", "15");
    multicastTTL = Integer.parseInt (d1);

    vrengRootURL = list.getProperty ("vreng_root_URL", "http://vreng.enst.fr/");

    domainName = list.getProperty ("domain_name", "enst.fr");

    statsPath           = list.getProperty ("stats_path", "/infres/gervaise/stats/");
    vmmDir              = list.getProperty ("vmm_directory", "vmm/");
    vreDir              = list.getProperty ("vre_directory", "vre/");
    vreExt              = list.getProperty ("vre_extension", "vre");
    worldFilename       = list.getProperty ("world_filename", "world.serialized");
    collectingFilename  = list.getProperty ("collecting_filename", "collecting.txt");

    servers = list.getProperty ("servers", "initd mountd httpd named smbd mrouted routed imapd smtpd printd");
      
    d1 = list.getProperty ("subnet_default_width", "10");
    d2 = list.getProperty ("subnet_default_length", "20");
    subnetDefaultDimension = new Pair (Double.parseDouble (d1),
				       Double.parseDouble (d2));
      
    d1 = list.getProperty ("room_default_width", "10");
    d2 = list.getProperty ("room_default_length", "10");
    roomDefaultDimension = new Pair (Double.parseDouble (d1),
				     Double.parseDouble (d2));

    d1 = list.getProperty ("machine_default_width", "30");
    d2 = list.getProperty ("machine_default_length", "20");
    machineDefaultDimension = new Pair (Double.parseDouble (d1),
					Double.parseDouble (d2));

    d1 = list.getProperty ("machine_spacing", "2");                   
    machineSpacing = Double.parseDouble (d1);
      
    d1 = list.getProperty ("machine_width", "0.3");
    d2 = list.getProperty ("machine_depth", "0.3");
    d3 = list.getProperty ("machine_height", "0.5");
    machineDimension = new Triplet (Double.parseDouble (d1),
				    Double.parseDouble (d2),
				    Double.parseDouble (d3));

    d1 = list.getProperty ("machine_color", "5");
    machineColor = new VREColor (Integer.parseInt (d1));    

    d1 = list.getProperty ("wall_height", "3");
    wallHeight = Double.parseDouble (d1);

    d1 = list.getProperty ("wall_depth", "0.1");
    wallDepth = Double.parseDouble (d1);
    
    d1 = list.getProperty ("subnet_entry_x", "0");
    d2 = list.getProperty ("subnet_entry_y", "0");
    d3 = list.getProperty ("subnet_entry_z", "5");
    d4 = list.getProperty ("subnet_entry_radius", "0");
    subnetEntryPosition = new Quadruplet (Double.parseDouble (d1),
				    Double.parseDouble (d2),
				    Double.parseDouble (d3),
				    Double.parseDouble (d4));

    d1 = list.getProperty ("machine_entry_x", "0");
    d2 = list.getProperty ("machine_entry_y", "0");
    d3 = list.getProperty ("machine_entry_z", "5");
    d4 = list.getProperty ("machine_entry_radius", "0");
    machineEntryPosition = new Quadruplet (Double.parseDouble (d1),
					   Double.parseDouble (d2),
					   Double.parseDouble (d3),
					   Double.parseDouble (d4));

    d1 = list.getProperty ("room_entry_x", "0");
    d2 = list.getProperty ("room_entry_y", "0");
    d3 = list.getProperty ("room_entry_z", "5");
    d4 = list.getProperty ("room_entry_radius", "0");
    roomEntryPosition = new Quadruplet (Double.parseDouble (d1),
					Double.parseDouble (d2),
					Double.parseDouble (d3),
					Double.parseDouble (d4));

    d1 = list.getProperty ("subnet_sky_color", "1");
    subnetSkyColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("room_sky_color", "1");
    roomSkyColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("machine_sky_color", "1");
    machineSkyColor = new VREColor (Integer.parseInt (d1));
    
    d1 = list.getProperty ("room_door_spacing", "1.5");
    roomDoorSpacing = Double.parseDouble (d1);

    d1 = list.getProperty ("door_width", "1");
    d2 = list.getProperty ("door_height", "2");
    d3 = list.getProperty ("door_depth", "0.07");
    doorDimension = new Triplet (Double.parseDouble (d1),
				 Double.parseDouble (d2),
				 Double.parseDouble (d3));

    d1 = list.getProperty ("panel_width", "1");
    d2 = list.getProperty ("panel_height", "1.3");
    d3 = list.getProperty ("panel_depth", "0.07");
    panelDimension = new Triplet (Double.parseDouble (d1),
				  Double.parseDouble (d2),
				  Double.parseDouble (d3));

    d1 = list.getProperty ("panel_text_height", "0.3");
    panelTextHeight = Double.parseDouble (d1);

    d1 = list.getProperty ("panel_tee_width", "0.15");
    panelTeeWidth = Double.parseDouble (d1);

    d1 = list.getProperty ("features_spacing", "2.5");
    featuresSpacing = Double.parseDouble (d1);

    d1 = list.getProperty ("machine_infos_x", "-10");
    d2 = list.getProperty ("machine_infos_y", "0");
    d3 = list.getProperty ("machine_infos_z", "2.5");
    d4 = list.getProperty ("machine_infos_radius", "0");
    machineInfosPosition = new Quadruplet (Double.parseDouble (d1),
					   Double.parseDouble (d2),
					   Double.parseDouble (d3),
					   Double.parseDouble (d4));

    machineInfosFontURL = list.getProperty ("machine_infos_font_URL",
					    "/txf/helvetica-bo-24.txf");

    d1 = list.getProperty ("user_spacing", "1");
    userSpacing = Double.parseDouble (d1);
    
    d1 = list.getProperty ("user_tee_height", "1");
    d2 = list.getProperty ("user_tee_radius", "0.05");
    d3 = list.getProperty ("user_head_radius", "0.25");
    userDimension = new Triplet (Double.parseDouble (d1),
				 Double.parseDouble (d2),
				 Double.parseDouble (d3));
    
    d1 = list.getProperty ("user_tee_color", "3");
    userTeeColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("user_head_color", "2");
    userHeadColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("day_spacing", "0.5");
    daySpacing = Double.parseDouble (d1);

    d1 = list.getProperty ("day_radius", "0.1");
    dayRadius = Double.parseDouble (d1);

    d1 = list.getProperty ("day_color", "10");
    dayColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("swap_height", "2");
    d2 = list.getProperty ("swap_radius", "0.5");
    swapDimension = new Pair (Double.parseDouble (d1),
			      Double.parseDouble (d2));
    
    d1 = list.getProperty ("swap_color", "4");
    swapColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("mean_load_height", "2");
    d2 = list.getProperty ("mean_load_radius", "0.5");
    meanLoadDimension = new Pair (Double.parseDouble (d1),
			      Double.parseDouble (d2));
    
    d1 = list.getProperty ("mean_load_color", "4");
    meanLoadColor = new VREColor (Integer.parseInt (d1));
   
    d1 = list.getProperty ("process_spacing", "0.5");
    processSpacing = Double.parseDouble (d1);
    
    d1 = list.getProperty ("process_width", "0.25");
    d2 = list.getProperty ("process_height", "0.25");
    d3 = list.getProperty ("process_depth", "0.25");
    processDimension = new Triplet (Double.parseDouble (d1),
				    Double.parseDouble (d2),
				    Double.parseDouble (d3));

    d1 = list.getProperty ("process_color", "7");
    processColor = new VREColor (Integer.parseInt (d1));

    d1 = list.getProperty ("server_spacing", "1");
    serverSpacing = Double.parseDouble (d1);

    d1 = list.getProperty ("server_height", "0.75");
    d2 = list.getProperty ("server_radius", "0.3");
    serverDimension = new Pair (Double.parseDouble (d1),
				Double.parseDouble (d2));
    
    d1 = list.getProperty ("server_color", "9");
    serverColor = new VREColor (Integer.parseInt (d1));
    
    d1 = list.getProperty ("space_spacing", "2");
    spaceSpacing = Double.parseDouble (d1);

    d1 = list.getProperty ("space_height_per_giga", "0.5");
    d2 = list.getProperty ("space_radius", "0.5");
    spaceDimension = new Pair (Double.parseDouble (d1),
			       Double.parseDouble (d2));
    
    d1 = list.getProperty ("space_color", "4");
    spaceColor = new VREColor (Integer.parseInt (d1));

    subnetFloorTexURL  = list.getProperty ("subnet_floor_texture_URL",
					   "/gif/marble.gif");
    subnetWallTexURL   = list.getProperty ("subnet_wall_texture_URL",
					   "/gif/brick.gif");
    roomFloorTexURL    = list.getProperty ("room_floor_texture_URL",
					   "/gif/marble.gif");
    roomWallTexURL     = list.getProperty ("room_wall_texture_URL",
					   "/gif/brick.gif");
    machineFloorTexURL = list.getProperty ("machine_floor_texture_URL",
					   "/gif/marble.gif");
    machineWallTexURL  = list.getProperty ("machine_wall_texture_URL",
					   "/gif/brick.gif");
    doorTexURL         = list.getProperty ("door_texture_URL",
					   "/gif/girl.gif");
    panelTexURL        = list.getProperty ("panel_texture_URL",
					   "/gif/wood-256.gif");
    machineTexURL      = list.getProperty ("machine_texture_URL",
					   "/gif/host.gif");


    panelFontURL = list.getProperty ("panel_font_URL",
				     "/txf/fixed-mr-10.txf");
  }


  /**
   * Set all properties.
   */
  private void setProperties () {

    list.setProperty ("subnet_mask", subnetMask);
    list.setProperty ("generic_multicast_adress", genericMulticastAddress);
    list.setProperty ("multicast_port", multicastPort + "");
    list.setProperty ("multicast_TTL", multicastTTL + "");
    list.setProperty ("vreng_root_URL", vrengRootURL);
    list.setProperty ("domain_name", domainName);
    list.setProperty ("stats_path", statsPath);
    list.setProperty ("vmm_directory", vmmDir);
    list.setProperty ("vre_directory", vreDir);
    list.setProperty ("vre_extension", vreExt);
    list.setProperty ("world_filename", worldFilename);
    list.setProperty ("collecting_filename", collectingFilename);
    list.setProperty ("servers", servers);
    list.setProperty ("subnet_default_width", subnetDefaultDimension.x + "");
    list.setProperty ("subnet_default_length", subnetDefaultDimension.y + "");
    list.setProperty ("room_default_width", roomDefaultDimension.x + "");
    list.setProperty ("room_default_length", roomDefaultDimension.y + "");
    list.setProperty ("machine_default_width", machineDefaultDimension.x + "");
    list.setProperty ("machine_default_length", machineDefaultDimension.y + "");
    list.setProperty ("machine_spacing", machineSpacing + "");                   
    list.setProperty ("machine_width", machineDimension.x + "");
    list.setProperty ("machine_depth", machineDimension.y + "");
    list.setProperty ("machine_height", machineDimension.z + "");
    list.setProperty ("machine_color", machineColor.getIndex () + "");
    list.setProperty ("wall_height", wallHeight + "");
    list.setProperty ("wall_depth", wallDepth + "");
    list.setProperty ("machine_entry_x", machineEntryPosition.x + "");
    list.setProperty ("machine_entry_y", machineEntryPosition.y + "");
    list.setProperty ("machine_entry_z", machineEntryPosition.z + "");
    list.setProperty ("machine_entry_radius", machineEntryPosition.t + "");
    list.setProperty ("machine_texture_URL", machineTexURL);
    list.setProperty ("subnet_entry_x", subnetEntryPosition.x + "");
    list.setProperty ("subnet_entry_y", subnetEntryPosition.y + "");
    list.setProperty ("subnet_entry_z", subnetEntryPosition.z + "");
    list.setProperty ("subnet_entry_radius", subnetEntryPosition.t + "");
    list.setProperty ("room_entry_x", roomEntryPosition.x + "");
    list.setProperty ("room_entry_y", roomEntryPosition.y + "");
    list.setProperty ("room_entry_z", roomEntryPosition.z + "");
    list.setProperty ("room_entry_radius", roomEntryPosition.t + "");
    list.setProperty ("room_sky_color", roomSkyColor.getIndex () + "");
    list.setProperty ("subnet_sky_color", subnetSkyColor.getIndex () + "");
    list.setProperty ("machine_sky_color", machineSkyColor.getIndex () + "");
    list.setProperty ("room_door_spacing", roomDoorSpacing + "");
    list.setProperty ("door_width", doorDimension.x + "");
    list.setProperty ("door_height", doorDimension.y + "");    
    list.setProperty ("door_depth", doorDimension.z + "");
    list.setProperty ("panel_width", panelDimension.x + "");
    list.setProperty ("panel_height", panelDimension.y + "");
    list.setProperty ("panel_depth", panelDimension.z + "");
    list.setProperty ("panel_text_height", panelTextHeight + "");
    list.setProperty ("panel_tee_width", panelTeeWidth + "");
    list.setProperty ("features_spacing", featuresSpacing + "");
    list.setProperty ("machine_infos_x", machineInfosPosition.x + "");
    list.setProperty ("machine_infos_y", machineInfosPosition.y + "");
    list.setProperty ("machine_infos_z", machineInfosPosition.z + "");
    list.setProperty ("machine_infos_radius", machineInfosPosition.t + "");
    list.setProperty ("machine_infos_font_URL", machineInfosFontURL + "");
    list.setProperty ("user_spacing", userSpacing + "");
    list.setProperty ("user_tee_height", userDimension.x + "");
    list.setProperty ("user_tee_radius", userDimension.y + "");
    list.setProperty ("user_head_radius", userDimension.z + "");
    list.setProperty ("user_tee_color", userTeeColor.getIndex () + "");
    list.setProperty ("user_head_color", userHeadColor.getIndex () + "");
    list.setProperty ("day_spacing", daySpacing + "");
    list.setProperty ("day_radius", dayRadius + "");
    list.setProperty ("day_color", dayColor.getIndex () + "");
    list.setProperty ("swap_height", swapDimension.x + "");
    list.setProperty ("swap_radius", swapDimension.y + "");
    list.setProperty ("swap_color", swapColor.getIndex () + "");
    list.setProperty ("mean_load_height", meanLoadDimension.x + "");
    list.setProperty ("mean_load_radius", meanLoadDimension.y + "");
    list.setProperty ("mean_load_color", meanLoadColor.getIndex () + "");
    list.setProperty ("process_spacing", processSpacing + "");
    list.setProperty ("process_width", processDimension.x + "");
    list.setProperty ("process_height", processDimension.y + "");
    list.setProperty ("process_depth", processDimension.z + "");
    list.setProperty ("process_color", processColor.getIndex () + "");
    list.setProperty ("server_spacing", serverSpacing + "");
    list.setProperty ("server_height", serverDimension.x + "");
    list.setProperty ("server_radius", serverDimension.y + "");
    list.setProperty ("server_color", serverColor.getIndex () + "");
    list.setProperty ("space_spacing", spaceSpacing + "");
    list.setProperty ("space_height_per_giga", spaceDimension.x + "");
    list.setProperty ("space_radius", spaceDimension.y + "");
    list.setProperty ("space_color", spaceColor.getIndex () + "");
    list.setProperty ("subnet_floor_texture_URL", subnetFloorTexURL);
    list.setProperty ("subnet_wall_texture_URL", subnetWallTexURL);
    list.setProperty ("room_floor_texture_URL", roomFloorTexURL);
    list.setProperty ("room_wall_texture_URL", roomWallTexURL);
    list.setProperty ("machine_floor_texture_URL", machineFloorTexURL);
    list.setProperty ("machine_wall_texture_URL", machineWallTexURL);
    list.setProperty ("door_texture_URL", doorTexURL);
    list.setProperty ("panel_texture_URL", panelTexURL);
    list.setProperty ("panel_font_URL", panelFontURL);
  }
}
