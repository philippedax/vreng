package collector;
import java.util.*;
import java.io.*;
import world.*;


/**
 * Class 'MachineCollector'.
 * This class define an object which is able to collect all information about a machine.
 */

public class MachineCollector {
  
  // Attributes.
  
  private Machine machine;  // Machine that collect on.
  

  /* Private static methods. */


  /**
   * Test if a string is in an other :
   * @param s1 container string,
   * @param s2 string to test,
   * @return true if s2 is in s1.
   */
  private static boolean stringContains (String s1, String s2) {
    int len1, len2;

    // Compute length to parse.
    len2 = s2.length ();
    len1 = s1.length () - len2 + 1; 

    // Parse.
    for ( int index = 0; index < len1; index++ )
      if ( s1.regionMatches (index, s2, 0, len2) )
	return true;

    return false;
  }


  /* Constructors. */
  

  /**
   * Main constructor :
   * @param machine Machine that collect on.
   */
  public MachineCollector (Machine machine) {
  
    this.machine = machine;
  }
  

  /* Methods. */


  /**
   * Main method : execute collecting.
   */
  public void collect () {

    // Room name.
    try {
      collectRoomName ();
    } catch ( CollectingException e ) {
      e.printMessage ("room name");
    }

    // IP addresses.
    try {
      collectIPs ();
    } catch ( CollectingException e ) {
      e.printMessage ("IP addresses");
    }

    // OS infos.
    try { 
      collectOSInfos ();
    } catch ( CollectingException e ) {
      e.printMessage ("OS informations");
    }
    
    // Name of users.
    try { 
      collectUsers ();
    } catch ( CollectingException e ) {
      e.printMessage ("name of users");
    }
    
    // Process infos.
    try { 
      collectProcessInfos ();
    } catch ( CollectingException e ) {
      e.printMessage ("process informations");
    }
    
    // State infos.
    try { 
      collectStateInfos ();
    } catch ( CollectingException e ) {
      e.printMessage ("state informations");
    }
    
    // Swap ratio.
    try { 
      collectSwapRatio ();
    } catch ( CollectingException e ) {
      e.printMessage ("swap ratio");
    }
    
    // Space ratio of disks.
    try { 
      collectDisks ();
    } catch ( CollectingException e ) {
      e.printMessage ("space ratio");
    }
  }
  
 
  /* Private methods. */

  
  /**
   * Collect room name for machine.
   */
  private void collectRoomName () throws CollectingException {
    BufferedReader br;
    String line = null;
    boolean error;

    // Set default value.
    machine.setRoomName ("unknown room name");

    // We open file.
    br = openFile ("room/");

    // We read file.
    try {
      error = false;
      line = br.readLine ();
    
    } catch ( IOException e ) {
      error = true;
    }

    // We close file.
    closeFile (br, error);
    
    // We add room name.
    if ( (line == null) || line.equals ("") )
      return;

    // Set room name.
    machine.setRoomName (line);
  }


  /**
   * Collect IPs for machine.
   */
  private void collectIPs () throws CollectingException {
    Vector IPs;
    boolean error;
    BufferedReader br;

    // Vector creation.
    IPs = new Vector ();

    // Set infos on 'machine' object.
    machine.setIPs (IPs);

    // We open file.
    br = openFile ("ifconfig/");

    // We read file.
    try {
      error = false;
      
      for ( String line = br.readLine (); line != null; line = br.readLine () ) {
	MyStringTokenizer st;
	
	// Not tunnel address...
	st = new MyStringTokenizer (line, "inet tunnel");
	if ( st.hasMoreTokens () ) {
	  st.nextToken ();
	  if ( st.hasMoreTokens () )
	    continue;
	}

	// We parse.
	st = new MyStringTokenizer (line, "inet ");
	if ( !st.hasMoreTokens () )
	  continue;
	st.nextToken ();
	if ( !st.hasMoreTokens () )
	  continue;
	st = new MyStringTokenizer (st.nextToken (), " ");
	if ( !st.hasMoreTokens () )
	  continue;
	line = st.nextToken ();
	
	// Not null and not loopback address.
	if ( ( line != null ) && !line.equals ("127.0.0.1") )
	  IPs.addElement (line);
      }
      
    } catch ( IOException e ) {
      error = true;
    }
    
    // We close stream.
    closeFile (br, error);
  }
  
  
  /**
   * Collect OS information.
   */
  private void collectOSInfos () throws CollectingException {
    BufferedReader br;
    String line = null;
    StringTokenizer st;
    boolean error;
    
    // Set default values.
    machine.setOSName ("unknown OS name");
    machine.setMachineType ("unknown machine type");
    
    // We open file.
    br = openFile ("uname/");

    // Read line.
    try {
      error = false;
      line = br.readLine ();

    } catch ( IOException e ) {
      error = true;
    }

    // Close stream.
    closeFile (br, error);
    
    // Nothing to do.
    if ( ( line == null ) || ( line == "" ) )
      return ;

    // We parse machine type.
    st = new StringTokenizer (line, ",");
    if ( !st.hasMoreTokens () )
      return;
    line = st.nextToken ();
    if ( st.hasMoreTokens () )
      machine.setMachineType (st.nextToken ());
    
    // We parse OS name.
    st = new StringTokenizer (line, " ");
    if ( !st.hasMoreTokens () )
      return;
    line = st.nextToken ();
    st.nextToken ();
    machine.setOSName (line + " " + st.nextToken ());
  }

  
  /**
   * Collect users.
   */
  private void collectUsers () throws CollectingException {
    Vector users;
    BufferedReader br;
    boolean error;

    // Vector creation.
    users = new Vector ();

    // Set users on 'machine' object.
    machine.setUsers (users);
    
    // We open file.
    br = openFile ("who/");
      
    // Read file;
    try {
      error = false;
      
      lineReading :
	for ( String line = br.readLine (); line != null; line = br.readLine () ) {
	  StringTokenizer st = new StringTokenizer (line);
	  
	  // User is valid ?
	  if ( st.hasMoreTokens () ) {
	    String user = st.nextToken ();
	    
	    // Search user in vector.
	    for ( Enumeration en = users.elements (); en.hasMoreElements (); )
	      if ( ((String)en.nextElement()).equals (user) )
		continue lineReading;
	    
	    // Add user...
	    users.addElement (user);
	  }
	}
    
    } catch ( IOException e ) {
      error = true;
    }
    
    // We close stream.
    closeFile (br, error);    
  }
  

  /**
   * Collect number of executing processes.
   */
  private void collectProcessInfos () throws CollectingException {
    Vector servers;
    int nbProcesses = 0;
    Vector notPresentServers;
    BufferedReader br;
    boolean error;

    // Create vector.
    servers = new Vector ();

    // Set default values.
    machine.setNbProcesses (0);
    machine.setServers (servers);

    // Create a special vector for searching servers.
    notPresentServers = new Vector ();
    for ( StringTokenizer st = new StringTokenizer (Collector.properties.servers); st.hasMoreTokens (); )
      notPresentServers.addElement (st.nextToken ());
    
    // We open file.
    br = openFile ("ps/");
    
    // We count just lines.
    // And we get servers on machine.
    try {
      String line;

      error = false;
      
      for ( nbProcesses = 0, line = br.readLine ();
	    line != null;
	    nbProcesses++, line = br.readLine () ) {

	// We search server.
	for ( Enumeration en = notPresentServers.elements (); en.hasMoreElements (); ) {
	  String server = (String) en.nextElement ();
	  if ( stringContains (line, server) ) {
	    servers.addElement (server);
	    notPresentServers.removeElement (server);
	  }
	}
      }
      
    } catch ( IOException e ) {
      error = true;
    }

    // We close stream.
    closeFile (br, error);    

    // We have to remove on line.
    if ( nbProcesses > 0 )
      nbProcesses--;
    
    // Set number of executing processes on 'machine' object.
    machine.setNbProcesses (nbProcesses);
  }


  /**
   * Collect mean load of machine
   */
  private void collectStateInfos () throws CollectingException {
    int uptime;
    double meanLoad;
    BufferedReader br;
    String line = null;
    MyStringTokenizer st1;
    StringTokenizer st2;
    boolean error;

    // Set default values.
    machine.setUptime (0);
    machine.setMeanLoad (0.0);
    
    // We open file.
    br = openFile ("w/");
    
    // Read file.
    try {
      error = false;

      // We get just first line.
      line = br.readLine ();
      
    } catch ( IOException e ) {
      error = true;
    }
     
    // We close stream.
    closeFile (br, error);    

    // Not OK ?
    if ( line == null )
      return;

    // We parse string in order to get uptime (if exists)
    if ( stringContains (line, "day") ) {
      st1 = new MyStringTokenizer (line, "up");
      if ( !st1.hasMoreTokens () )
	return;
      st1.nextToken ();
      if ( !st1.hasMoreTokens () )
	return;
      line = st1.nextToken ();
      st2 = new StringTokenizer (line);
      if ( !st2.hasMoreTokens () )
	return;
      uptime = Integer.parseInt (st2.nextToken ());
    } else
      uptime = 1;

    // We parse string in order to get mean load.
    st1 = new MyStringTokenizer (line, "load average: ");
    if ( !st1.hasMoreTokens () )
      return;
    st1.nextToken ();
    if ( !st1.hasMoreTokens () )
      return;
    st2 = new StringTokenizer (st1.nextToken (), ",");
    if ( !st2.hasMoreTokens () )
      return;
    meanLoad = Double.parseDouble (st2.nextToken ());
    
    // Set number of mean load on 'machine' object and uptime.
    machine.setUptime (uptime);
    machine.setMeanLoad (meanLoad);
  }
  

  /**
   * Collect swap ratio of machine
   */
  private void collectSwapRatio () throws CollectingException {
    int usedSize, freeSize;
    BufferedReader br;
    String line = null;
    MyStringTokenizer st1, st2;
    boolean error;

    // Set default values.
    machine.setSwapRatio (0);
    
    // We open file.
    br = openFile ("swap/");
    
    // Read file.
    try {
      error = false;

      // We get the line.
      line = br.readLine ();
    
    } catch ( IOException e ) {
      error = true;
    }

    // We close stream.
    closeFile (br, error);    

    // Not OK ?
    if ( line == null )
      return;

    // We parse string in order to get used size.
    st1 = new MyStringTokenizer (line, "k used, ");
    if ( !st1.hasMoreTokens () )
      return ;
    st2 = new MyStringTokenizer (st1.nextToken (), " = ");
    if ( !st2.hasMoreTokens () )
      return ;
    st2.nextToken ();
    if ( !st2.hasMoreTokens () )
      return ;
    usedSize = Integer.parseInt (st2.nextToken ());

    // We parse string in order to get available size.
    if ( !st1.hasMoreTokens () )
      return ;
    st2 = new MyStringTokenizer (st1.nextToken (), "k available");
    if ( !st2.hasMoreTokens () )
      return ;
    freeSize = Integer.parseInt (st2.nextToken ());

    // Set wap ratio on 'machine' object.
    machine.setSwapRatio ((100 * usedSize) / (freeSize + usedSize));
  }


  /**
   * Collect space ratio of machine for each disk.
   */
  private void collectDisks () throws CollectingException {
    Vector disks;
    BufferedReader br;
    boolean error;

    // Vector creation.
    disks = new Vector ();

    // Set default vector.
    machine.setDisks (disks);
    
    // We open file.
    br = openFile ("df/");
    
    // Read file, and search for 'home' part.
    try {
      String line;
      MyStringTokenizer st1;
      StringTokenizer st2;

      error = false;

      // We read just first line.
      br.readLine ();

      lineReading :
	for ( line = br.readLine (); line != null; line = br.readLine () ) {
	  String name;
	  double capacity;
	  int ratio;
	  
	  // We parse.
	  st2 = new StringTokenizer (line);
	  
	  // Parse for capacity.
	  for ( int n = 0; n < 2; n++ )
	    if ( st2.hasMoreTokens () )
	      line = st2.nextToken ();
	    else
	      continue lineReading;
	  capacity = Double.parseDouble (line) / 1000000.0;

	  // Parse for ratio.
	  for ( int m = 2; m < 5; m++ )
	    if ( st2.hasMoreTokens () )
	      line = st2.nextToken ();
	    else
	      continue lineReading;
	
	  // Get name.
	  if ( !st2.hasMoreTokens () )
	    continue;
	  name = st2.nextToken ();

	  // Get ratio.
	  st1 = new MyStringTokenizer (line, "%");
	  if ( !st1.hasMoreTokens () )
	    continue;
	  ratio = Integer.parseInt (st1.nextToken ());

	  // Add disk.
	  disks.addElement (new Disk (name, capacity, ratio));
	}

    } catch ( IOException e ) {
      error = true;
    }

    // We close stream.
    closeFile (br, error);    
  }


  /**
   * Open file in specified directory :
   * @param directory the directory, 
   * @return a stream to read.
   */
  private BufferedReader openFile (String directory) throws CollectingException {
    BufferedReader br;

    // We try to open.
    try {
      String path = Collector.properties.statsPath + directory + machine.getName ();
      br = new BufferedReader (new FileReader (path));
      
    } catch ( IOException e ) {
      throw new CollectingException ("file opening");
    }

    return br;
  }


  /**
   * Close the stream and manage reading error...
   * @param br    the stream,
   * @param error flag set when reading error is occured. 
   */
  private void closeFile (BufferedReader br, boolean error) throws CollectingException {
    
    // We close stream.
    try {
      br.close ();
      
    } catch ( IOException e ) {
      if ( error )
	throw new CollectingException ("file reading and closing");
      else
	throw new CollectingException ("file closing");
    }
    
    // Just error during reading.
    if ( error )
      throw new CollectingException ("file reading");
  }


  /* Classes internes. */


  /**
   * Class for managing error during a collecting,
   */
  public class CollectingException extends Exception {
    private String error;

    /**
     * Main constructor :
     * @param error type of error.
     */
    public CollectingException (String error) {
      this.error = error;
    }

    /**
     * Print a message :
     * @param context Context of error.
     */
    public void printMessage (String context) {
      System.err.println ("Collecting error for machine <" + machine.getName () +
			  "> : impossible to get <" + context +
			  "> : reason is <" + error +">");
    }
  }
}
