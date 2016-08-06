package world;
import java.io.*;


/**
 * Class 'Disk'
 * Contains a name of disk and its space ratio
 */

public class Disk implements Serializable {

  /* Attibutes. */

  private String name;        // Its name.
  private double capacity;    // Its capacity in giga.
  private int    spaceRatio;  // The space ratio.

  
  /* Constructors. */

  /**
   * Main constructor:
   * @param name its name,
   * @param capacity capacity of disk,
   * @param spaceRatio its free space.
   */
  public Disk (String name, double capacity, int spaceRatio) {

    this.name = name;
    this.capacity = capacity;
    this.spaceRatio = spaceRatio;
  }


  /* Methods. */

  /**
   * Accessor:
   * @return name.
   */
  public String getName () {
    return name;
  }

  /**
   * Accessor:
   * @return capacity.
   */
  public double getCapacity () {
    return capacity;
  }

  /**
   * Accessor:
   * @return space ratio
   */
  public int getSpaceRatio () {
    return spaceRatio;
  }
}
