package fr.enst.vreng.data.types;

import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;

/**
 * Represents a Vreng object's position.
 * Data:
 * <UL>
 *  <LI>3*32bit coordinates</LI>
 *  <LI>3*32bit angle position</LI>
 * </UL>
 */
public class ObjPos {

  public Point3D pos = null;
  public Point3D ang = null;

  public ObjPos() {
    pos = new Point3D();
    ang = new Point3D();
  }

  public ObjPos(Point3D pos, Point3D ang) {
    this.pos = pos;
    this.ang = ang;
  }

  /**
   * Reads an object position from an input stream.
   */
  public static ObjPos unpack(MsgInputStream mis) {
    ObjPos ret = new ObjPos();
    ret.pos = Point3D.unpack(mis);
    ret.ang = Point3D.unpack(mis);
    return ret;
  }

  /**
   * Writes an object position to an output stream.
   */
  public void pack(MsgOutputStream mos) {
    pos.pack(mos);
    ang.pack(mos);
  }
}
