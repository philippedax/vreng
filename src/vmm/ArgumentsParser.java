package collector;
import world.*;


/**
 * Provides a mechanism for parsing command line.
 */

public class ArgumentsParser {

  /* Attributes. */

  private String application;         // Name of application.
  private String [] args;             // Command line.
  private int nbArgs;                 // Number of arguments in command line.
  private String longOptionsString;   // For printinting usage.
  private String shortOptionsString;  // For printinting usage.


  /* Constructors. */

  /**
   * Main constructor: 
   * @param application its name,
   * @param args command line.
   */
  public ArgumentsParser (String application, String [] args) {
    
    this.application = application;
    this.args = args;
    
    nbArgs = args.length;
    
    longOptionsString = "\t--help\tdisplay usage\n";
    shortOptionsString = "\t-h\tdisplay usage\n";
  }
  

  /* Public methods. */


  /**
   * Specifiate and test long option:
   * @param name name of option,
   * @param message for help,
   * @return true if option appears in command line.
   */ 
  public boolean getOption (String name, String message) {
    return getOption (name, '\0', message);
  }

  /**
   * Specifiate and test short option:
   * @param alpha  of option,
   * @param message for help,
   * @return true if option appears in command line.
   */ 
  public boolean getOption (char alpha, String message) {
    return getOption ("", alpha, message);
  }

  /**
   * Specifiate and test long and short option:
   * @param name name of option,
   * @param alpha char for option,
   * @param message for help,
   * @return true if option appears in command line.
   */ 
  public boolean getOption (String name, char alpha, String message) {
    boolean isLong  = !name.equals ("");
    boolean isShort = ( alpha != '\0' );
    String key  = "--" + name;
    String skey = "-" + alpha;
    
    // Construction of strings.
    if ( isLong )
      longOptionsString += "\t" + key + "\t" + message + "\n";
    if ( isShort )
      shortOptionsString += "\t" + skey + "\t" + message + "\n";

    // Search option...
    for ( int num = 0; num < nbArgs; num++ )
      if ( ( args [num] != null ) &&
	   ( isLong  && args[num].equals (key) ||
	     isShort && args[num].equals (skey) ) ) {	
	args [num] = null;
	return true;
      }

    return false;
  }

  /**
   * Specifiate and test long option with value:
   * @param name name of option,
   * @param notNull force presence of value,
   * @param message for help,
   * @return value of option.
   */ 
  public String getOptionValue (String name, boolean notNull, String message) {
    return getOptionValue (name, '\0', notNull, message);
  }

  /**
   * Specifiate and test long option with value:
   * @param alpha char of option,
   * @param notNull force presence of value,
   * @param message for help,
   * @return value of option.
   */ 
  public String getOptionValue (char alpha, boolean notNull, String message) {
    return getOptionValue ("", alpha, notNull, message);
  }

  /**
   * Specifiate and test long option with value:
   * @param name name of option,
   * @param alpha char of option,
   * @param notNull force presence of value,
   * @param message for help,
   * @return value of option.
   */ 
  public String getOptionValue (String name, char alpha, boolean notNull, String message) {
    boolean isLong  = !name.equals ("");
    boolean isShort = ( alpha != '\0' );
    String key  = "--" + name;
    String skey = "-" + alpha;
    String value = "";

    // Construction of strings.
    if ( isLong )
      longOptionsString += "\t" + key + "\t" + message + "\n";
    if ( isShort )
      shortOptionsString += "\t" + skey + "\t" + message + "\n";
    
    // Search option...
    for ( int num = 0; num < nbArgs; num++ )
      if ( ( args [num] != null ) &&
	   ( isLong  && args[num].equals (key) ||
	     isShort && args[num].equals (skey) ) ) {
	if ( num == (nbArgs - 1) ) {
	  if ( notNull )
	    error ("option '" + name + "' requires an argument");
	  else
	    value = "";
	} else {
	  if ( args[num+1].startsWith ("-") ) {
	    if ( notNull )
	      error ("option '" + name + "' requires an argument");
	    else
	      value = "";
	  } else {
	    value = args [num + 1];
	    args [num + 1] = null;
	  }
	}
	
	args [num] = null;
	return value;
      }

    return value;
  }

  /**
   * Achieve parser job...
   */
  public void end () {

    // Test if command line is good.
    for ( int num = 0; num < nbArgs; num++ )
      if ( args [num] != null ) {
	if ( args [num].equals ("--help") || args [num].equals ("-h") )
	  usage ();
	else
	  error("invalid command line: \n\ttype '--help' or '-h' for more help");
      }
  }


  /* Private methods. */
  
  /**
   * Print error message and quit:
   * @param message the message.
   */
  private void error (String message) {
    
    System.err.println (application + " : " + message + ".");
    System.exit (1);
  }
  
  /**
   * Print help and quit.
   */
  private void usage () {
    
    System.out.println ("Usage:\n" + application + " arguments ...\n\n" + longOptionsString + "\n" + shortOptionsString);
    System.exit (0);
  }

}
