package fr.enst.vreng.data;

import fr.enst.vreng.IMessage;
import java.io.ByteArrayOutputStream;

/**
 * Class used to create outgoing messages.
 */
public class MsgOutputStream extends ByteArrayOutputStream {

  /**
   * Get a byte[] representation of the given message.
   */
  public static byte[] toBytes(IMessage msg) {
    MsgOutputStream mos = new MsgOutputStream(msg.getHeader());
    msg.pack(mos);
    return mos.toByteArray();
  }

  /**
   * Initializes an outgoing message with the given header.
   */
  public MsgOutputStream(Header header) {
    super();
    header.pack(this);
  }

  /**
   * Returns the byte[] representation of this message.
   * The length of data is inserted to the header at this point.
   */
  public byte[] toByteArray() {
    byte[] raw = super.toByteArray();
    int datalen = size() - Header.HDR_LEN;
    DataHelper.put16(raw, Header.DATA_LEN_IDX, datalen);
    return raw;
  }

  /**
   * Writes an 8 bit (signed) value.
   */
  public void write8(int data) {
    byte[] tmp = new byte[DataHelper.int8];
    DataHelper.put8(tmp, 0, data);
    write(tmp, 0, tmp.length);
  }

  /**
   * Writes an 16 bit (signed) value.
   */
  public void write16(int data) {
    byte[] tmp = new byte[DataHelper.int16];
    DataHelper.put16(tmp, 0, data);
    write(tmp, 0, tmp.length);
  }

  /**
   * Writes an 32 bit (signed) value.
   */
  public void write32(int data) {
    byte[] tmp = new byte[DataHelper.int32];
    DataHelper.put32(tmp, 0, data);
    write(tmp, 0, tmp.length);
  }
}
