package world;
import java.io.*;


/**
 * Class 'WorldObject'
 * Represents a basis element of world.
 */

public abstract class WorldObject implements Serializable {

  /* Attributes. */

  // Main attributes.
  protected String           name;   // Name of object.
  protected MulticastAddress maddr;  // Multicast address.

  // Graphic attributes.
  protected Pair dimension;  // Dimension of object.

  /* Constructors. */


  /**
   * Main constructor:
   * @param name its name.
   */
  public WorldObject (String name) {

    this.name = name;

    // Generation of new multicast address.
    maddr = MulticastAddress.getNew ();
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
   * Get multicast address:
   * @return the multicast address.
   */
  public MulticastAddress getMulticastAddress () {
    return maddr;
  }

  /**
   * Get dimension:
   * @return dimension of object.
   */
  public Pair getDimension () {
    return dimension;
  }

  /**
   * Set room dimensions:
   * @param dimension New dimensions of object.
   */
  public void setDimension (Pair dimension) {
    this.dimension = dimension;
  }

}
