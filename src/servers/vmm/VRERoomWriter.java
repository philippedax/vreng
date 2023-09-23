package collector;
import java.util.*;
import world.*;


/**
 * Class 'VRENRoomWriter'
 * This class generate 'vre' file description for room.
 */

public class VRERoomWriter extends VREWriter {
  
  /* Attributes. */

  private Room room;  // The room which we have to generate 'vre' file for.


  /* Constructors. */


  /*
   * Main constructor.
   * @param room The room.
   */
  public VRERoomWriter (Room room) {

    super (room, Collector.properties.roomDefaultDimension);

    this.room = room;
  }
  

  /* Methods. */


  /**
   * Main method...
   */
  public void write () {
    
    // We write it with good parameters.
    write (Collector.properties.roomEntryPosition,
	   Collector.properties.roomSkyColor,
	   Collector.properties.roomFloorTexURL,
	   Collector.properties.roomWallTexURL);
  }
  

  /* Protected methods. */
  

  /**
   * Write object.
   * This method is called by 'write'.
   */
  protected void writeObject () {

    // We write machines.
    writeMachines ();
  }


  /* Private methods. */


  /**
   * Write machines contained into roomwork.
   */
  private void writeMachines () {
    Vector unpositionnedMachines;
    Pair pos;
    Enumeration en;

    // For unpositionned machines.
    unpositionnedMachines = new Vector ();
    pos = new Pair ();

    // Enumerate machines.
    for ( en = room.getMachines().elements(); en.hasMoreElements (); ) {
      Machine machine = (Machine) en.nextElement ();
      Pair position = machine.getRoomPosition ();
      
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
  
}
