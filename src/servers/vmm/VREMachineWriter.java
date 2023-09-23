package collector;
import java.util.*;
import world.*;


/**
 * Class 'VREMachineWriter'
 * This class generate 'vre' file description for machine.
 */

public class VREMachineWriter extends VREWriter {
  
  /* Attributes. */

  private Machine machine;   // The machine which we have to generate 'vre' file for.
  private Triplet position;  // Used for positionning objects.


  /* Constructors. */

  /**
   * Main constructor:
   * @param machine the machine.
   */
  public VREMachineWriter (Machine machine) {
    
    super (machine, Collector.properties.machineDefaultDimension);

    this.machine = machine;
    
    // For placing objects.
    position = new Triplet ();
  }
  

  /* Public methods. */

  /**
   * Write machine.
   */
  public void write () {
    write (Collector.properties.machineEntryPosition,
	   Collector.properties.machineSkyColor,
	   Collector.properties.machineFloorTexURL,
	   Collector.properties.machineWallTexURL);
  }
  

  /* Protected methods. */

  /**
   * Write object.
   * This method is called by 'write'.
   */
  protected void writeObject () {

    // Write informations for machine.
    writeInformations ();
    
    // Write host for telnet connection.
    position.x = Collector.properties.featuresSpacing;
    position.y = Collector.properties.featuresSpacing;
    writeHost ();

    // Write users.
    position.y += Collector.properties.featuresSpacing;
    writeUsers ();

    /*
    // Write uptime.
    position.y += Collector.properties.featuresSpacing;
    writeUptime ();
    */

    // Write mean load
    position.x = Collector.properties.featuresSpacing;
    position.y += Collector.properties.featuresSpacing;
    writeMeanLoad ();

    // Write swap.
    position.x += Collector.properties.featuresSpacing;
    writeSwapRatio ();

    /*
    // Write number of processes.
    position.y += Collector.properties.featuresSpacing;
    writeNbProcesses ();
    */

    // Write servers.
    position.y += Collector.properties.featuresSpacing;
    writeServers ();
    
    // Write disks.
    position.y += Collector.properties.featuresSpacing;
    writeDisks ();
  }


  /* Private methods. */

  
  /**
   * Write Informations for machine.
   */
  private void writeInformations () {
    
    // write just a text.
    vos.writeText (Collector.properties.machineInfosPosition,
		   true,
		   Collector.properties.machineInfosFontURL,
		   machine.getName () + ", " +
		   machine.getOSName () + ", " + machine.getUptime () + " days, " +
		   machine.getNbProcesses () + " processes");
  }

  /**
   * Write a host.
   */
  private void writeHost () {
    Quadruplet position3D;
    String solid;

    // Generate position.
    position3D = new Quadruplet (position.x - dimension.x / 2,
				 dimension.y / 2 - position.y,
				 Collector.properties.machineDimension.z / 2,
				 0);

    // Generate solid.
    solid = "<solid shape=\"box\" dim=\"" + trunc(Collector.properties.machineDimension.x / 2) +
               " " + trunc (Collector.properties.machineDimension.y / 2) +
               " " + trunc (Collector.properties.machineDimension.z / 2) +
               "\" xp=\"" + Collector.properties.machineTexURL +
               "\" yp=\"" + Collector.properties.machineTexURL +
               "\" xn=\"" + Collector.properties.machineTexURL +
               "\" yn=\"" + Collector.properties.machineTexURL +
               "\" " + Collector.properties.machineColor +
               "\" />";
    
    // write host.
    vos.writeHost (position3D, machine.getName () + "." +
		   Collector.properties.domainName, solid);
  }

  /**
   * Write users representation.
   */
  private void writeUsers () {

    // Enumerate users.
    position.x = Collector.properties.featuresSpacing;
    for ( Enumeration en = machine.getUsers().elements(); en.hasMoreElements (); ) {
      String name = (String) en.nextElement (); 
      Quadruplet position3D;
      String solid;

      // Generate position of tee.
      position3D = new Quadruplet (position.x - dimension.x / 2,
			           dimension.y / 2 - position.y,
			           0, 0);

      // Generate solid of tee.
      solid = "<solid shape=\"cone\" rb=\"" + trunc(Collector.properties.userDimension.y) +
                 "\" rt=\"" + trunc(Collector.properties.userDimension.y) +
	         "\" h=\"" + trunc(Collector.properties.userDimension.x) +
	         "\" " + Collector.properties.userTeeColor +
                 "\" />";
      
      // Write solid for tee.
      vos.writeMirage (name, position3D, solid);

      // Generate position of head.
      position3D = new Quadruplet (position.x - dimension.x / 2,
			           dimension.y / 2 - position.y,
			           Collector.properties.userDimension.x / 2 +
			           Collector.properties.userDimension.z, 0);
      
      // Generate solid of tee.
      solid = "<solid shape=\"sphere\" r=\"" + trunc(Collector.properties.userDimension.z) +
	         "\" " + Collector.properties.userHeadColor +
                 "\" />";
      
      // Write solid for tee.
      vos.writeMirage (name, position3D, solid);

      // Update position.
      position.x += Collector.properties.userSpacing;
    }
  }

  /**
   * Write uptime representation.
   */
  private void writeUptime () {

    // Enumerate days.
    position.x = 0;
    position.z = Collector.properties.daySpacing;
    for ( int num = 1; num <= machine.getUptime (); num++ ) {
      Quadruplet position3D;
      String solid;

      // New line of days.
      if ( position.x > (dimension.x / 2 - Collector.properties.featuresSpacing) ) {
	position.x  = 0;
	position.z += Collector.properties.daySpacing;
      }

      // Generate position.
      position3D = new Quadruplet (position.x, dimension.y / 2 - position.y, position.z, 0);
      
      // Generate solid.
      solid = "<solid shape=\"sphere\" r=\"" + trunc(Collector.properties.dayRadius) +
                 "\" " + Collector.properties.dayColor +
                 "\" />";
     
      // Write solid.
      vos.writeMirage ("day " + num, position3D, solid);

      // Update position.
      position.x += Collector.properties.processSpacing;
    }
  }

  /**
   * Write mean load representation at current position.
   */
  private void writeMeanLoad () {

    // write ratio.
    writeRatio (Collector.properties.meanLoadDimension,
		Collector.properties.meanLoadColor,
		"load=" + machine.getMeanLoad (),
		machine.getMeanLoad ());
  }

  /**
   * Write swap representation at current position.
   */
  private void writeSwapRatio () {

    // write ratio.
    writeRatio (Collector.properties.swapDimension,
		Collector.properties.swapColor,
		"swap=" + machine.getSwapRatio () + "%",
		(double) machine.getSwapRatio () / 100);
  }

  /**
   * Write processes representation at current position.
   */
  private void writeNbProcesses () {

    // Enumerate processes.
    position.x = 0;
    position.z = Collector.properties.processSpacing;
    for ( int num = 1; num <= machine.getNbProcesses (); num++ ) {
      Quadruplet position3D;
      String solid;

      // New line of processes.
      if ( position.x > (dimension.x / 2 - Collector.properties.featuresSpacing) ) {
	position.x  = 0;
	position.z += Collector.properties.processSpacing;
      }

      // Generate position.
      position3D = new Quadruplet (position.x, dimension.y / 2 - position.y, position.z, 0);
      
      // Generate solid.
      solid = "<solid shape=\"box\" dim=\"" + trunc(Collector.properties.processDimension.x / 2) +
	         " " + trunc(Collector.properties.processDimension.z / 2) +
	         " " + trunc(Collector.properties.processDimension.y / 2) +
                 "\" " + Collector.properties.processColor +
                 "\" />";
     
      // Write solid.
      vos.writeMirage ("process " + num, position3D, solid);

      // Update position.
      position.x += Collector.properties.processSpacing;
    }
  }

  /**
   * Write servers representation.
   */
  private void writeServers () {

    // Enumerate all servers.
    position.x = Collector.properties.featuresSpacing;
    for ( Enumeration en = machine.getServers().elements(); en.hasMoreElements (); ) {
      String server = (String) en.nextElement ();
      Quadruplet position3D;
      String solid;

      // Generate position.
      position3D = new Quadruplet (position.x - dimension.x / 2,
				   dimension.y / 2 - position.y,
				   0, 0);

      // Generate solid.
      solid = "<solid shape=\"cone\" rb=\"" + trunc(Collector.properties.serverDimension.y) +
	         "\" h=\"" + trunc(Collector.properties.serverDimension.x) +
	         "\" " + Collector.properties.serverColor +
                 "\" />";

      // Write object.
      vos.writeMirage (server, position3D, solid);

      // Update position.
      position.x += Collector.properties.featuresSpacing;
    }
  }

  /**
   * Write disks representation at current position.
   */
  private void writeDisks () {

    // Enumerate all disks.
    position.x = Collector.properties.featuresSpacing;
    for ( Enumeration en = machine.getDisks().elements(); en.hasMoreElements (); ) {
      Disk disk = (Disk) en.nextElement ();

      // write ratio.
      writeRatio (new Pair (Collector.properties.spaceDimension.x * disk.getCapacity (),
			    Collector.properties.spaceDimension.y),
		  Collector.properties.spaceColor,
		  disk.getName (), //BUG!+ ": " + disk.getSpaceRatio () + "%",
		  (double) disk.getSpaceRatio () / 100);

      // Update position.
      position.x += Collector.properties.spaceSpacing;
    }
  }

  /**
   * Write ratio to current position:
   * @param dimension its dimension,
   * @param color its color when empty,
   * @param label name for object,
   * @param value the ratio.
   */
  private void writeRatio (Pair dimension, VREColor color,
			   String label, double value) {
    Quadruplet position3D;
    VREColor loadColor;
    double height;
    String solid;
      
    // manage color for displaying.
    if ( value < 0.5 )
      loadColor = VREColor.green;
    else if ( value < 0.75 )
      loadColor = VREColor.orange;
    else
      loadColor = VREColor.red;

    // Compute height of ratio.
    height = dimension.x * value;
    
    // Generate position for lower cylinder.
    position3D = new Quadruplet (position.x - this.dimension.x / 2,
			         this.dimension.y / 2 - position.y,
			         0, 0);
    
    // Generate solid for lower cylinder.
    solid = "<solid shape=\"cone\" rb=\"" + trunc(dimension.y) +
               "\" rt=\"" + trunc(dimension.y) +
               "\" h=\"" + trunc(height) +
               "\" " + loadColor +
               "\" />";
    
    // Write lower cylinder.
    vos.writeMirage (label, position3D, solid);
    
    // Generate position for upper cylinder.
    position3D = new Quadruplet (position.x - this.dimension.x / 2,
			         this.dimension.y / 2 - position.y,
			         height, 0);
    
    // Generate solid for upper cylinder.
    solid = "<solid shape=\"cone\" rb=\"" + trunc(dimension.y) +
               "\" rt=\"" + trunc(dimension.y) +
               "\" h=\"" + trunc(dimension.x - height) +
               "\" " + color +
               "\" />";
    
    // Write upper cylinder.
    vos.writeMirage (label, position3D, solid);
  }

}
