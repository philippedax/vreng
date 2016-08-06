package world;
import java.io.*;


/**
 * Class 'Triplet'
 * Contains three coordinates.
 */

public class Triplet implements Serializable {

  /* Attibutes. */

  public double x, y, z;  // Coordinates.

  
  /* Constructors. */


  /**
   * Default constructor.
   */
  public Triplet () {
    x = y = z = 0.0;
  }

  /**
   * Main constructor:
   * @param x x,
   * @param y y,
   * @param z z.
   */
  public Triplet (double x, double y, double z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

}
