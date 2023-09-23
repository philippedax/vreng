package collector;
import java.awt.*;
import world.*;


/**
 * Class 'VREColor'
 * Allows to define a color for 'vre' file.
 */

public class VREColor {
  
  /* Constants. */

  public final static VREColor green  = new VREColor (5);   // Green color. 
  public final static VREColor orange = new VREColor (8);   // Orange color.
  public final static VREColor red    = new VREColor (10);  // Red color.


  /* Attributes. */

  private int   index;  // Index of color.
  private Color color;  // Object which defines color.



  /* Static methods. */


  /**
   * Trucate double :
   * @param d the double,
   * @return trucated double.
   */
  public static String trunc (double d) {
    return VREOutputStream.trunc (d);
  }


  /* Constructors. */


  /**
   * Main constructor :
   * @param index indicates a color.
   */
  public VREColor (int index) {

    this.index = index;

    // Selection of color.
    switch ( index ) {
    case 0 :
      color = Color.black ;
      break;
    case 1 :
      color = Color.blue ;
      break;
    case 2 :
      color = Color.cyan ;
      break;
    case 3 :
      color = Color.darkGray ;
      break;
    case 4 :
      color = Color.gray ;
      break;
    case 5 :
      color = Color.green ;
      break;
    case 6 :
      color = Color.lightGray ;
      break;
    case 7 :
      color = Color.magenta ;
      break;
    case 8 :
      color = Color.orange ;
      break;
    case 9 :
      color = Color.pink ;
      break;
    case 10 :
      color = Color.red ;
      break;
    case 11 :
      color = Color.white ;
      break;
    case 12 :
      color = Color.yellow ;
      break;
    default :
      color = Color.lightGray ;
      break;
    }
  }


  /* Methods. */


  /**
   * Accessor : 
   * @return index of color.
   */
  public int getIndex () {
    return index;
  }


  /**
   * Accessor : 
   * @return color RGB composantes.
   */
  public double [] getRGB () {
    double [] RGB = new double [3];

    RGB [0] = (double) color.getRed () / 255.0;
    RGB [1] = (double) color.getGreen () / 255.0;
    RGB [2] = (double) color.getBlue () / 255.0;

    return RGB;
  }


  /**
   * Create a string representation of color for 'vreng' :
   * @return string representation.
   */
  public String toString () {
    double [] RGB = getRGB ();

    return "dif=\"" + trunc (RGB [0]) + " " + trunc (RGB [1]) + " " + trunc (RGB [2]);
  }

}
