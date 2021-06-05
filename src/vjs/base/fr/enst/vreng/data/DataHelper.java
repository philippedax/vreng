package fr.enst.vreng.data;

/**
 * Class used to pack/unpack data in the correct network format.
 */
public class DataHelper {

  /**
   * Put an 8bit value.
   */
  public static int put8(byte data[], int pos, int val) {
    data[pos] = (byte) (val & 0xff);
    return 1;
  }

  /**
   * Put a 16bit value.
   */
  public static int put16(byte data[], int pos, int val) {
    data[pos]   = (byte) ((val & 0x0000ff00) >> 8);
    data[pos+1] = (byte) ( val & 0x000000ff);
    return 2;
  }

  /**
   * Put a 32bit value.
   */
  public static int put32(byte data[], int pos, int val) {
    data[pos]   = (byte) ((val & 0xff000000) >> 24);
    data[pos+1] = (byte) ((val & 0x00ff0000) >> 16);
    data[pos+2] = (byte) ((val & 0x0000ff00) >> 8);
    data[pos+3] = (byte) ( val & 0x000000ff);
    return 4;
  }

  /**
   * Read an 8bit value.
   */
  public static int read8(byte data[], int pos) {
    byte b = data[pos];
    if ((b & 0x80) == 0) 
      return data[pos] & 0xff;
    else
      return -1 * ( ~data[pos] + 1);
  }

  /**
   * Read a 16bit value.
   */
  public static int read16(byte data[], int pos) {
    if ((data[pos] & 0x80) == 0) {
      return ((0xff & data[pos]) << 8) + (0xff & data[pos+1]);
    } else {
      return -1 * (((0xff & ~data[pos]) << 8) + (0xff & ~data[pos+1]) + 1);
    }
  }

  /**
   * Read a 32bit value.
   */
  public static int read32(byte data[], int pos) {
    if ((data[pos] & 0x80) == 0) {
      return ((0xff & data[pos]) << 24)
        + ((0xff & data[pos+1]) << 16)
        + ((0xff & data[pos+2]) << 8)
        + (0xff & data[pos+3]);
    } else {
      return -1*(((0xff & ~data[pos]) << 24)
             + ((0xff & ~data[pos+1]) << 16)
             + ((0xff & ~data[pos+2]) << 8)
             + (0xff & ~data[pos+3])
             + 1);
    }
  }

  public static final int int8 = 1;
  public static final int int16 = 2;
  public static final int int32 = 4;
}
