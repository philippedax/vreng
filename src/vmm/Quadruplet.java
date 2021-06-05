package world;
import java.io.*;


/**
 * Class 'Quadruplet'
 * Contains four coordinates.
 */

public class Quadruplet implements Serializable {

  /* Attibutes. */

  public double x, y, z, t;  // Coordinates.

  
  /* Constructors. */


  /**
   * Default constructor.
   */
  public Quadruplet () {
    x = y = z = t = 0.0;
  }

  /**
   * Main constructor :
   * @param x x,
   * @param y y,
   * @param z z,
   * @param t t,
   */
  public Quadruplet (double x, double y, double z, double t) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.t = t;
  }

}
