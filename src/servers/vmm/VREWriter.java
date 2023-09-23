package collector;
import java.util.*;
import world.*;


/**
 * Class 'VREWriter'
 * This class allows to generate 'vre' file description.
 */

public abstract class VREWriter {

  /* Attributes. */

  protected WorldObject     object;     // Object which we write for.
  protected VREOutputStream vos;        // Stream used for writting.
  protected Pair            dimension;  // Dimension of main plane.


  /* Static methods. */

  /**
   * Trucate double:
   * @param d the double,
   * @return trucated double.
   */
  public static String trunc (double d) {
    return VREOutputStream.trunc (d);
  }


  /* Constructors. */

  /*
   * Main constructor:
   * @param object file name of file.
   */
  public VREWriter (WorldObject object, Pair defaultDimension) {

    this.object = object;

    // Creation of stream.
    vos = new VREOutputStream (Collector.properties.statsPath +
			       Collector.properties.vreDir +
			       object.getName () + "." +
			       Collector.properties.vreExt);

    // We get dimension.
    if ( (dimension = object.getDimension ()) == null )
      dimension = defaultDimension;
  }


  /* Public methods. */

  /**
   * Main method for writing:
   * @param entryPosition entry point in object,
   * @param skyColor color of background,
   * @param floorTexURL URL for texturing floor,
   * @param wallTexURL URL for texturing walls.
   */
  public void write (Quadruplet entryPosition,
		     VREColor skyColor,
		     String floorTexURL,
		     String wallTexURL) {

    // We write entry position.
    vos.writeEntry (entryPosition);

    // We write sky.
    vos.writeSky (skyColor);

    // We write place (walls, entering gate, floor,...).
    writePlace (floorTexURL, wallTexURL);

    // Method for writting object.
    writeObject ();

    // We write end.
    vos.writeEnd ();

    // We close stream.
    vos.close ();
  }


  /* Protected methods. */


  /**
   * Write object.
   * This method is called by 'write'.
   */
  protected abstract void  writeObject ();


  /**
   * Write a panel:
   * @param position position on ground,
   * @param radius radius for positionning panel,
   * @param text text to write.
   */
  protected void writePanel (Pair position, double radius, String text) {
    Quadruplet position3D;
    String solid;

    // Generate position 3D.
    position3D = new Quadruplet (position.x,
				 position.y,
				 Collector.properties.panelDimension.y -
				 Collector.properties.panelTextHeight / 2,
				 radius);

    // Generate Geometry.
    solid = "<solid shape=\"box\" dim=\"" + trunc (Collector.properties.panelDimension.z / 2) +
               " " + trunc (Collector.properties.panelDimension.x / 2) +
               " " + trunc (Collector.properties.panelTextHeight / 2) +
               "\" xp=\"" + Collector.properties.panelTexURL +
               "\" yp=\"" + Collector.properties.panelTexURL +
               "\" xn=\"" + Collector.properties.panelTexURL +
               "\" yn=\"" + Collector.properties.panelTexURL +
               "\" zp=\"" + Collector.properties.panelTexURL +
               "\"";


    // Write first solid.
    vos.writeMirage ("wall", position3D, solid);

    // Generate position 3D.
    position3D = new Quadruplet (position.x,
				 position.y,
				 (Collector.properties.panelDimension.y -
				  Collector.properties.panelTextHeight) / 2,
				 radius);

    // Generate Geometry.
    solid = "<solid shape=\"box\" dim=\"" + trunc (Collector.properties.panelDimension.z / 2) +
               " " + trunc (Collector.properties.panelTeeWidth / 2) +
               " " + trunc ((Collector.properties.panelDimension.y -
			     Collector.properties.panelTextHeight) / 2) +
               "\" xp=\"" + Collector.properties.panelTexURL +
               "\" yp=\"" + Collector.properties.panelTexURL +
               "\" xn=\"" + Collector.properties.panelTexURL +
               "\" yn=\"" + Collector.properties.panelTexURL +
               "\"";

    // Write second solid.
    vos.writeMirage ("wall", position3D, solid);

    // Generate position 3D for text.
    position3D = new Quadruplet (position.x,
				 position.y,
				 Collector.properties.panelDimension.y,
				 radius);

    // Write string.
    vos.writeText (position3D, true, Collector.properties.panelFontURL, text);
  }


  /**
   * Write a machine into place:
   * @param machine the machine,
   * @param position the position for machine,
   */
  protected void writeMachine (Machine machine, Pair position) {
    Quadruplet position3D;
    String url;
    String solid;

    // Generate 3D position.
    position3D = new Quadruplet (position.x - dimension.x / 2,
				 dimension.y / 2 - position.y,
				 Collector.properties.machineDimension.z / 2,
				 0);

    // Generate url for accessing to machine.
    url = Collector.properties.vrengRootURL + Collector.properties.vmmDir + Collector.properties.vreDir +
          machine.getName () + "." + Collector.properties.vreExt;

    // Generate solid.
    solid = "<solid shape=\"box\" dim=\"" + trunc (Collector.properties.machineDimension.x / 2) +
               " " + trunc (Collector.properties.machineDimension.y / 2) +
               " " + trunc (Collector.properties.machineDimension.z / 2) +
               "\" xp=\"" + Collector.properties.machineTexURL +
               "\" " + Collector.properties.machineColor +
               "\"";

    // We write it.
    vos.writeGate (machine.getName (), position3D,
		   url, machine.getMulticastAddress (), solid);
  }


  /* Private methods. */


  /**
   * Allows to create a place for room,
   */
  private void writePlace (String floorTexURL, String wallTexURL) {


    // We write floor.
    vos.writeGround (new Pair (0.0, 0.0), dimension, floorTexURL);

    // We write walls (N, E, S, W).
    vos.writeWall (new Quadruplet (0.0, (dimension.y + Collector.properties.wallDepth) / 2,
				   Collector.properties.wallHeight / 2, 0.0),
		   new Pair (dimension.x, Collector.properties.wallDepth),
		   wallTexURL);
    vos.writeWall (new Quadruplet ((dimension.x + Collector.properties.wallDepth) / 2, 0.0,
				   Collector.properties.wallHeight / 2, 0.0),
		   new Pair (Collector.properties.wallDepth, dimension.y),
		   wallTexURL);
    vos.writeWall (new Quadruplet (0.0, - (dimension.y + Collector.properties.wallDepth) / 2,
				   Collector.properties.wallHeight / 2, 0.0),
		   new Pair (dimension.x, Collector.properties.wallDepth),
		   wallTexURL);
    vos.writeWall (new Quadruplet (- (dimension.x + Collector.properties.wallDepth) / 2,
				   0.0, Collector.properties.wallHeight / 2, 0.0),
		   new Pair (Collector.properties.wallDepth, dimension.y),
		   wallTexURL);
  }

}
