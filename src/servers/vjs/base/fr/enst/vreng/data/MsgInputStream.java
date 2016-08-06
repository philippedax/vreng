package fr.enst.vreng.data;

/**
 * Class used to help unpack incoming messages from VRENG.
 * It stores a byte array and a current position inside that
 * array, plus the message's header that is read when the
 * constructor is called.
 */
public class MsgInputStream {

  /** Current position in the byte array */
  protected int cursor = 0;
  /** The raw data */
  protected byte[] raw = null;

  /** The message's header */
  protected Header header = null;
  /**
   * Returns the message's header.
   */
  public Header getHeader() { return header; }

  /**
   * Constructs a new object, and reads the header.
   */
  public MsgInputStream(byte[] data)
    throws InvalidMessageException {
    this.raw = data;
    readHeader();
  }

  /**
   * Reads the message's header.
   */
  protected void readHeader()
    throws InvalidMessageException {
    header = Header.unpack(this);
    if (header.dataLen > (raw.length-cursor)) {
      throw new InvalidMessageException("Packet too short ("+header.dataLen+" - "+cursor+" "+raw.length+")");
    }
  }

  /**
   * Read an 8 bit int (signed).
   */
  public int read8() {
    int ret = DataHelper.read8(raw, cursor);
    cursor += DataHelper.int8;
    return ret;
  }

  /**
   * Read an 32 bit int (signed).
   */
  public int read16() {
    int ret = DataHelper.read16(raw, cursor);
    cursor += DataHelper.int16;
    return ret;
  }

  /**
   * Read an 32 bit int (signed).
   */
  public int read32() {
    int ret = DataHelper.read32(raw, cursor);
    cursor += DataHelper.int32;
    return ret;
  }

  /**
   * Read a String from the message data.
   */
  public String readString() {
    int len = read16();
    byte[] str = new byte[len];
    System.arraycopy(raw, cursor, str, 0, len);
    cursor += len;
    return new String(str);
  }
}
