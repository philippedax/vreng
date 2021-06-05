package fr.enst.vreng.data;

import fr.enst.vreng.data.types.NetObjId;

import java.net.DatagramPacket;

/**
 * Vreng message header.
 * Fields:
 * <UL>
 *  <LI><b>16bit</b> protocol identifier</LI>
 *  <LI><b>8bit</b> protocol version number</LI>
 *  <LI><b>32bit</b> application SSRC</LI>
 *  <LI><b>8bit</b> message type</LI>
 *  <LI><b>8bit</b> message sub-type</LI>
 *  <LI><b>16bit</b> payload length field</LI>
 *  <LI><b>72bit</b> sender identifier (see {@link fr.enst.vreng.data.types.NetObjId NetObjId})</LI>
 * </UL>
 */
public class Header {

    public static final int PROTO = 0xabcd;
    public static final int VERSION = 0;
    public static final int HDR_LEN = 20;
    public static final int DATA_LEN_IDX = 9;

    public static int sAppSSRC = 0;

    int proto;
    int version;
    int app_ssrc;
    int msgType;
    int msgVal;
    int dataLen;

    NetObjId source;

    public int getSSRC() { return app_ssrc; }
    public int getType() { return msgType; }
    public int getVal() { return msgVal; }
    public int getDataLen() { return dataLen; }
    public NetObjId getSource() { return source; }
    public void setSource(NetObjId src) { source = src; }

    /**
     * Constructs a new header whith the given type and subtype.
     */
    public Header(int type, int val) {
        proto = PROTO;
        version = VERSION;
        app_ssrc = sAppSSRC;
        msgType = type;
        msgVal = val;
        dataLen = 0;
        source = new NetObjId();
    }

    /**
     * Constructs a full header with the given parameters.
     */
    public Header(int proto, int version, int app_ssrc, int type, int val, int dataLen, NetObjId source) {
        sAppSSRC = app_ssrc;

        this.proto = proto;
        this.version = version;
        this.app_ssrc = app_ssrc;
        this.msgType = type;
        this.msgVal = val;
        this.dataLen = dataLen;
        this.source = source;
    }

    public String toString() {
        StringBuffer stringbuffer = new StringBuffer("Message header (");
        stringbuffer.append(proto).append(", ");
        stringbuffer.append(version).append(", ");
        stringbuffer.append(app_ssrc).append(", ");
        stringbuffer.append(msgType).append(", ");
        stringbuffer.append(msgVal).append(", ");
        stringbuffer.append(dataLen).append(")");
        return stringbuffer.toString();
    }

    /**
     * Reads a message header from a message input stream.
     */
    public static Header unpack(MsgInputStream mis)
        throws InvalidMessageException {
        Header h =  new Header(mis.read16(),    // magic number
        mis.read8(),    // protocol version
        mis.read32(),    // app's SSRC
        mis.read8(),    // message type
        mis.read8(),    // message val
        mis.read16(),    // data length
        NetObjId.unpack(mis));
        return h;
    }

    /**
     * Writes a message header to an output stream.
     */
    public void pack(MsgOutputStream mos) {
        mos.write16(proto);
        mos.write8(version);
        mos.write32(app_ssrc);
        mos.write8(msgType);
        mos.write8(msgVal);
        mos.write16(dataLen);
        source.pack(mos);
    }
}
