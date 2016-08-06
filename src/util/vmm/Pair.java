package world;
import java.io.*;


/**
 * Class 'Pair'
 * Contains two coordinates.
 */

public class Pair implements Serializable {

  /* Attibutes. */

  public double x, y;  // Coordinates.

  
  /* Constructors. */


  /**
   * Default constructor.
   */
  public Pair () {
    x = y = 0.0;
  }

  /**
   * Main constructor :
   * @param x x,
   * @param y y.
   */
  public Pair (double x, double y) {
    this.x = x;
    this.y = y;
  }

}
