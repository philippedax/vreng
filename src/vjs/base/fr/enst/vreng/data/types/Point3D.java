package fr.enst.vreng.data.types;

import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;

/**
 * Represents a 3D coordinate.
 * Data:
 * <UL>
 *  <LI>3*32bit coordinates</LI>
 * </UL>
 */
public class Point3D {

  public double x;
  public double y;
  public double z;

  public Point3D() { }

  public Point3D(double x, double y, double z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  public static Point3D unpack(MsgInputStream mis) {
    Point3D ret = new Point3D();
    ret.x = mis.read32() / 1000.0;
    ret.y = mis.read32() / 1000.0;
    ret.z = mis.read32() / 1000.0;
    return ret;
  }
  
  public void pack(MsgOutputStream mos) {
    mos.write32((int) (x * 1000));
    mos.write32((int) (y * 1000));
    mos.write32((int) (z * 1000));
  }

  /**
   * Substracts the two vectors passed in an returns the result.
   */
  public static Point3D sub(Point3D p1, Point3D p2) {
    return new Point3D(
      p1.x - p2.x,
      p1.y - p2.y,
      p1.z - p2.z);
  }

  /**
   * Scales a vector by the given factor.
   */
  public void scale(double f) {
    x *= f;
    y *= f;
    z *= f;
  }

  /**
   * Adds the given vector to this one.
   */
  public void add(Point3D p) {
    x += p.x;
    y += p.y;
    z += p.z;
  }

  /**
   * Returns a vector's length.
   */
  public double norm() {
    return Math.sqrt(x*x + y*y + z*z);
  }
}
