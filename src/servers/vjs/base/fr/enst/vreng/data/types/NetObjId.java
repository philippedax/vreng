package fr.enst.vreng.data.types;

import fr.enst.vreng.data.MsgInputStream;
import fr.enst.vreng.data.MsgOutputStream;

/**
 * Represents a Vreng object's net object identifier.
 * Data:
 * <UL>
 *  <LI>8bit type id</LI>
 *  <LI>32bit application SSRC</LI>
 *  <LI>16bit port identifier</LI>
 *  <LI>32bit object identifier</LI>
 * </UL>
 */
public class NetObjId {
  
  public int type = 0;
  public int src = 0;
  public int port = 0;
  public int id = 0;

  public NetObjId() {}

  public NetObjId(int type, int src, int port, int id) {
    this.type = type;
    this.src = src;
    this.port = port;
    this.id = id;
  }

  public boolean equals(Object o) {
    if (!(o instanceof NetObjId)) return false;
    NetObjId i = (NetObjId) o;
    return ((type == i.type) && (src == i.src) && (port == i.port) && (id == i.id));
  }

  public int hashCode() {
    return toString().hashCode();
  }

  public String toString() {
    StringBuffer sb = new StringBuffer();
    sb.append(type);
    sb.append(":");
    sb.append(src);
    sb.append(":");
    sb.append(port);
    sb.append(":");
    sb.append(id);
    return sb.toString();
  }

  /**
   * Reads an object identifier from an input stream.
   */
  public static NetObjId unpack(MsgInputStream mis) {
    NetObjId ret = new NetObjId();
    ret.type = mis.read8();
    ret.src = mis.read32();
    ret.port = mis.read16();
    ret.id = mis.read16();
    return ret;
  }

  /**
   * Writes the object identifier to an output stream.
   */
  public void pack(MsgOutputStream mos) {
    mos.write8(type);
    mos.write32(src);
    mos.write16(port);
    mos.write16(id);
  }
}
