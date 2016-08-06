package collector;
import java.util.*;
import world.*;


/**
 * Class 'VRENSubnetWriter'
 * This class generate 'vre' file description for subnet.
 */

public class VRESubnetWriter extends VREWriter {

  /* Attributes. */

  private Subnet subnet;  // Subnetwork to write.


  /* Constructors. */


  /*
   * Main constructor.
   * @param subnet The subnetwork.
   */
  public VRESubnetWriter (Subnet subnet) {

    super (subnet, Collector.properties.subnetDefaultDimension);

    this.subnet = subnet;
  }
  

  /* Methods. */


  /**
   * Main method
   */
  public void write () {    
    write (Collector.properties.subnetEntryPosition,
	   Collector.properties.subnetSkyColor,
	   Collector.properties.subnetFloorTexURL,
	   Collector.properties.subnetWallTexURL);
  }
  

  /* Protected methods. */


  /**
   * Write object.
   * This method is called by 'write'.
   */
  protected void writeObject () {

    // We write machines.
    writeMachines ();

    // We write room gates.
    writeRoomDoors ();
  }


  /* Private methods. */


  /**
   * Write machines contained into subnetwork.
   */
  private void writeMachines () {
    Vector unpositionnedMachines;
    Pair pos;
    Enumeration en;

    // For unpositionned machines.
    unpositionnedMachines = new Vector ();
    pos = new Pair ();

    // Enumerate machines.
    for ( en = subnet.getMachines().elements(); en.hasMoreElements (); ) {
      Machine machine = (Machine) en.nextElement ();
      Pair position = machine.getSubnetPosition ();
      
      // Machine is not positionned.
      if ( position == null ) {
	unpositionnedMachines.addElement (machine);
	continue;
      }

      // We write machine
      writeMachine (machine, position);

      // We update max position.
      pos.y = Math.max (pos.y, position.y);
    }

    // We positionnate unpositionned machines.
    pos.x = Collector.properties.machineSpacing;
    pos.y += Collector.properties.machineSpacing;
    for ( en = unpositionnedMachines.elements (); en.hasMoreElements (); ) {

      // Remains enougth space ?
      if ( pos.x >= (dimension.x - Collector.properties.machineSpacing) ) {
	pos.x = Collector.properties.machineSpacing;
	pos.y += Collector.properties.machineSpacing;
      }
      
      // We can write machine.
      writeMachine ((Machine) en.nextElement (), pos);

      // We update position.
      pos.x += Collector.properties.machineSpacing;
    }
  }

  /**
   * Write gate for accesing to reprensenting rooms
   */
  private void writeRoomDoors () {
    Quadruplet position;

    // For positionning.
    position = new Quadruplet ((dimension.x - Collector.properties.doorDimension.z) / 2,
			       - dimension.y / 2,
			       Collector.properties.doorDimension.y / 2,
			       0);

    // Enumerate rooms.
    for ( Enumeration en = subnet.getRooms().elements(); en.hasMoreElements (); ) {
      Room room = (Room) en.nextElement ();
      String url;
      String solid;

      // Update position.
      position.y += Collector.properties.roomDoorSpacing;
      
      // Generate url.
      //url = Collector.properties.vrengRootURL + "vmm/" + 
      url = Collector.properties.vrengRootURL + Collector.properties.vmmDir + Collector.properties.vreDir + 
	    room.getName () + "." + Collector.properties.vreExt;
      
      // Generate solid (ad hoc).
      solid = "<solid shape=\"box\" dim=\"" + trunc(Collector.properties.doorDimension.x / 2) +
                 " " + trunc (Collector.properties.doorDimension.z / 2) +
                 " " + trunc (Collector.properties.doorDimension.y / 2) +
	         "\" yp=\"" + Collector.properties.doorTexURL +
                 "\"";
      
      // We write it.
      vos.writeGate (room.getName (), position,
		     url, room.getMulticastAddress (), solid);
    }  
  }

}
