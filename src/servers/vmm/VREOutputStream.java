package collector;
import java.util.*;
import java.io.*;
import java.awt.*;
import world.*;


/**
 * Class 'VREOutputStream'
 * Provides a specific stream used for writting "vre" file.
 */

public class VREOutputStream {

  /* Constants. */

  public static final int nbDecimals = 2;  // Number of decimal for double representation.


  /* Attributes. */

  private String name;        // File name to write.
  private BufferedWriter bw;  // Stream used for writting.


  /* Static methods. */
  
  /**
   * Trucate double:
   * @param d the double,
   * @return trucated double.
   */
  public static String trunc(double d) {
    StringTokenizer st = new StringTokenizer (d + "", ".");
    String a, b;

    // We parse double.
    if ( !st.hasMoreTokens () )
      return "0";
    a = st.nextToken ();
    if ( !st.hasMoreTokens () )
      return a;
    b = st.nextToken ();

    // We truncate decimals.
    b = b.substring (0, Math.min (nbDecimals, b.length ()));

    if (b == "0")
      return a;
    return a + "." + b;
  }
  

  /* Constructors. */

  /**
   * Main constructors.
   * @param name File name to write.
   */
  public VREOutputStream (String name) {

    String s;
    this.name = name;

    // We open file.
    try {
      bw = new BufferedWriter (new FileWriter (name));
    } catch ( IOException e ) {
      System.err.println ("Impossible to create 'vre' file: " + name);
      bw = null;
    }
    s = "<!-- File generated by VMM -->\n";
    s += "<vre version=\"6.9\">\n\n";
    s += "<head>\n";
    s += "<meta http-equiv=\"refresh\" content=\"3600\" />\n";
    s += "<meta name=\"filename\" content=\"" + name + "\" />\n";
    s += "</head>\n\n";
    s += "<scene>\n\n";

    write (s);
  }


  /* Public methods. */

  
  /**
   * Close stream.
   */
  public void close () {

    // Not opened ?
    if ( bw == null )
      return;

    // We try it...
    try {
      bw.close ();
    } catch ( IOException e ) {
      System.err.println ("Impossible to close 'vre' file: " + name);
    }
  }

  /**
   * Write 'entry' section:
   * @param pos Position.
   */
  public void writeEntry (Quadruplet pos) {
    String s;

    s = "<entry pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t) + "\">";
    s += "</entry>\n";
    
    write (s);
  }

  /**
   * Write 'background' section:
   * @param pos Position.
   */
  public void writeSky (VREColor color) {
    double [] RGB = color.getRGB ();
    String s;

    s = "<bgcolor color=\"";
    s += trunc(RGB[0]) + " " + trunc(RGB[1]) + " " + trunc(RGB[2]) + "\">";
    s += "</bgcolor>\n";
    
    write (s);
  }

  /**
   * Write the ground object:
   * @param pos position of ground,
   * @param dim describes dimensions of ground,
   * @param url for texture.
   */
  public void writeGround (Pair pos, Pair dim, String url) {
    String s;

    s = "\n";
    s += "<ground";
    s += " name=\"mother-board\"";
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + "-0.5 0\"";
    s += "> <solid shape=\"box\" dim=\"" + trunc(dim.y / 2) + " " + trunc(dim.x / 2);
    s += " 0.5\" zp=\"" + url + "\"" + " />";
    s += "</ground>\n";

    write (s);
  }

  /**
   * Write a step object:
   * @param pos describes position of step,
   * @param dim describes dimensions of step,
   * @param url for texture.
   */
  public void writeStep (Quadruplet pos, Triplet dim, String url) {
    String s;

    s = "<step";
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t) + "\"";
    s += "> <solid shape=\"box\" dim=\"" + trunc(dim.y / 2) + " " + trunc(dim.x / 2) + " " + trunc(dim.z / 2);
    s += "\" zp=\"" + url + "\"" + " />";
    s += "</step>\n";

    write (s);
  }

  /**
   * Write a wall object:
   * @param pos describes position of wall,
   * @param dim describes dimensions of wall,
   * @param url for texture,
   */
  public void writeWall (Quadruplet pos, Pair dim, String url) {
    String s;

    s = "<wall";
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t) + "\"";
    s += "> <solid shape=\"box\" dim=\"" + trunc(dim.y / 2) + " " + trunc(dim.x / 2) + " ";
    s += trunc(Collector.properties.wallHeight / 2);
    s += "\" xp=\"" + url + "\" xn=\"" + url + "\" yp=\"" + url + "\" yn=\"" + url + "\"" + " />";
    s += "</wall>\n";

    write (s);
  }

  /**
   * Write a gate object.
   * @param name     the name of gate,
   * @param pos	     Position of object,
   * @param solid    string which defines geometry.
   * @param url      used to specifiate an other "vre" source file,
   * @param maddr    multicast address which define place,
   */
  public void writeGate (String name, Quadruplet pos,
			 String url, MulticastAddress maddr,
			 String solid) {
    String s;

    s = "<gate";
    s += ( ( name == null ) ? "" : " name=\"" + name + "\"" );
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t) + "\"";
    s += " url=\"" + url + "\"";
    s += " channel=\"" + maddr + "\"";
    s += "> " + solid + " />";
    s += "</gate>\n";

    write (s);    
  }

  /**
   * Write a mirage object:
   * @param name     name for solid,
   * @param pos	  its positione,
   * @param solid its geometry.
   */
  public void writeMirage (String name, Quadruplet pos, String solid) {
    String s;

    s = "<mirage";
    s += (((name != null) && !name.equals ("")) ? " name=\"" + name + "\"" : "" );
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " 0 " + trunc(pos.t) + "\"";
    s += " mode=\"refresh\"";
    s += "> " + solid + " />";
    s += "</mirage>\n";

    write (s);    
  }

  /**
   * Write a text object:
   * @param pos position of text,
   * @param vertical vertical if true,
   * @param url of font to apply,
   * @param text to write.
   */
  public void writeText (Quadruplet pos, boolean vertical, String url, String text) {
    String s;

    s = "<text";
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t);
    s += ( vertical ? " 1.57" : " 0" ) + "\"";
    s += " font=\"" + url + "\">";
    s += " " + text + " ";
    s += "</text>\n";
    
    write (s);
  }

  /**
   * Write a host object:
   * @param pos position of host,
   * @param url of font to apply,
   * @param host to write.
   */
  public void writeHost (Quadruplet pos, String url, String solid) {
    String s;

    s = "<host";
    s += " pos=\"";
    s += trunc(pos.y) + " " + trunc(-pos.x) + " " + trunc(pos.z) + " " + trunc(pos.t) + "\"";
    s += " host=\"" + url + "\"";
    s += "> " + solid + " />";
    s += "</host>\n";
    
    write (s);
  }

  /**
   * Write the end of file.
   */
  public void writeEnd () {
    String s;

    s  = "\n</scene>\n</vre>\n";
    
    write (s);
  }

  
  /* Private methods. */

  /**
   * Write a string into file.
   * @param s The string.
   */
  private void write (String s) {

    // Little hack.
    if ( bw == null )
      return;

    // We try it...
    try {
      bw.write (s, 0, s.length ());
    } catch ( IOException e ) {
      System.err.println ("Impossible to write into 'vre' file: " + name);
    }
  }
}