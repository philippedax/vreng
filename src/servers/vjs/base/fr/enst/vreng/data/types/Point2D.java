package fr.enst.vreng.data.types;

import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;

/**
 * Represents a 2D coordinate.
 * Data:
 * <UL>
 *  <LI>2*32bit coordinates</LI>
 * </UL>
 */

public class Point2D {

  public int x;
  public int y;

  public Point2D() { }

  public Point2D(int x, int y) {
    this.x = x;
    this.y = y;
  }

  public static Point2D unpack(MsgInputStream mis) {
    Point2D ret = new Point2D();
    ret.x = mis.read32();
    ret.y = mis.read32();
    return ret;
  }

  public void pack(MsgOutputStream mos) {
    mos.write32(x);
    mos.write32(y);
  }
}
