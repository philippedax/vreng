package fr.enst.vreng;

/**
 * Generated from ../../../src/world/vjc.hpp at Mon Apr 15 12:48:52 2024
 */
public class MessageTypes {

	public static final int MSGT_CTRL       = 0;
	public static final int MSGT_CLICK      = 1;
	public static final int MSGT_ADD        = 2;
	public static final int MSGT_DEL        = 3;
	public static final int MSGT_POS        = 4;
	public static final int MSGT_ISEC       = 5;
	public static final int MSGT_QUERY      = 6;
	public static final int MSGV_REGISTER   = 0;
	public static final int MSGV_UNREGISTER = 1;
	public static final int MSGV_INITIATE   = 2;
	public static final int MSGV_TERMINATE  = 3;
	public static final int MSGV_UPDATE     = 4;
	public static final int MSGV_PING       = 5;
	public static final int MSGV_ASK        = 0;
	public static final int MSGV_SET        = 1;
	public static final int MSGV_UPD        = 2;
	public static final int MSGV_ERROR      = 3;
	public static final int MSGV_QTYPE      = 0;
	public static final int MSGV_QANS       = 1;
	public static final int MSGV_CIRCLE     = 0;
	public static final int MSGV_LINE       = 1;
	public static final int MSGV_LOOP       = 2;
	public static final int MSGV_FILL       = 3;
	public static final int MSGV_DELALL     = 4;
	public static final int MSGV_CLICK      = 0;
	public static final int MSGV_ISECIN     = 0;
	public static final int MSGV_ISECOUT    = 1;

	public static final String[] MSG_CLICK = new String[] {
		"Click",
	};

	public static final String[] MSG_CTRL = new String[] {
		"Register",
		"Unregister",
		"Inititate",
		"Terminate",
		"Update",
		"Ping",
	};

	public static final String[] MSG_DRAW = new String[] {
		"Circle",
		"Line",
		"Loop",
		"Fill",
		"DeleteAll",
	};

	public static final String[] MSG_ISEC = new String[] {
		"GoingIn",
		"GoingOut",
	};

	public static final String[] MSG_POS = new String[] {
		"Ask",
		"Set",
		"Update",
		"Error",
	};

	public static final String[] MSG_QUERY = new String[] {
		"TypeQuery",
		"QueryAnswer",
	};

	public static final String[] MSG_TYPES = new String[] {
		"Control",
		"Click",
		"Add",
		"Delete",
		"Position",
		"Intersect",
		"Query",
	};

}