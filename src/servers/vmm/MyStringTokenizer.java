package collector;
import world.*;


/**
 * Class 'MyStringTokenizer'
 * Provide a mean for parsing strings.
 */

public class MyStringTokenizer {

  /* Attributes */

  private String string;   // String to parse. 
  private String delim;    // String used for delimitting.
  private int    delimLen; // Length of delimiter string.


  /* Constructors. */

  
  /**
   * Main constructor.
   * @param string String to parse.
   * @param delim  String used for delimitting.
   */
  public MyStringTokenizer (String string, String delim) {

    this.string = string;
    this.delim  = delim;

    // Length.
    delimLen = delim.length ();
  }


  /* Methods. */


  /**
   * Does it remain tokens ?
   * @return true if remain token.
   */
  public boolean hasMoreTokens () {

    return ( string != null ) && ( string.length () > 0 );
  }


  /**
   * Parse string.
   * @return next token.
   */
  public String nextToken () {
    String token;

    // Is there more token ?
    if ( !hasMoreTokens () )
      return "";
    
    // If string begins with delimiter.
    if ( string.startsWith (delim) )
      string = string.substring (delimLen);

    // Parse...
    for ( int index = 0; index < string.length () - delimLen + 1; index++ )
      if ( string.regionMatches (index, delim, 0, delimLen) ) {
	token = string.substring (0, index);
	string = string.substring (index + delimLen);
	return token;
      }
    
    // There is not deliters any more.
    token = string;
    string = null;
    return token;
  }
}
