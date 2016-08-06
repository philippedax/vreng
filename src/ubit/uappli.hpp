/************************************************************************
 *
 *  uappli.hpp: Appplication Context (must be created first)
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef _uappli_hpp_
#define	_uappli_hpp_ 1
#include <cstdio>
#include <cstdarg>
#include <ubit/udisp.hpp>
#include <ubit/uconf.hpp>
namespace ubit {
  
  /** Flow list (@see UEventFlow and UAppli).
   */
  typedef std::vector<UEventFlow*> UFlowList;
  
  
  /** Display lists (@see UDisp and UAppli)
   */
  typedef std::vector<UDisp*> UDispList;
  
      
  /** The Application Context. 
   * A Ubit application must create one (and only one) UAppli object.
   * This UAppli must be created as soon as the program is started by main()
   * No Ubit function should be called before its creation nor after its deletion.
   *
   * A UAppli can be parameterized:
   * - through command line options: see UAppli(argc, arv)
   * - by means of a UConf object: see UAppli(UConf&)
   *
   * A UAppli can open windows on several Displays (that can be controlled
   * by remote machines): see UAppli::openDisp()). Window children can be 
   * shared by windows that belong to different displays. These children are
   * automatically replicated (and synchronized) on the corresponding displays.
   *
   * A UAppli can manage 1 or several Event Flows that are comptelety separated.
   * This is useful for Two-handed interaction or groupware (each user
   * controlling his own pointer on the screen(s)). Alternate event flows are 
   * automatically created when the UMS (Ubit Mouse Server) is running 
   * (see the 'umsd' program in directory ubit/ums). The UMS can manage several
   * sources of events and send them as separate event flows to the UAppli.
   * These event sources can be produced by a mouse or a MIMIo on the serial port
   * or by an application running on a remote machine (see the 'uremote' program
   * in ubit/ums). Finally, Event flows can be uniquely identified in callback 
   * functions through the getFlow() method of their UEvent& argument.
   *
   * Classes UTimer and USource make it possible to fire callback functions after
   * a given timeout or when the application receives data on a file source
   * (such as a socket). UAppli::onTimeout() and UAppli::postpone() provide a 
   * simplified API for using timers. postpone() is used to fire CPU intensive 
   * methods when the event loop becomes idle.
   * 
   * Classes USocket and UServerSocket provide a simple mechanism to handle 
   * sockets. The are based on simple communication protocol that can be
   * implemented in non Ubit programs. In addition, asynchronous messages
   * can be exchanged between Ubit applications and the UMS server through the
   * the UMessage protocol. See: UMessage, UAppli::onMessage() and 
   * UAppli::getMessagePort().
   *
   * NOTE on colors: Ubit applications may not work properly on (obsolete) X11 
   * displays that do not support True Colors. See UDisp::setTrueColors() 
   * and UDisp::setPseudoColors() for details.
   * 
   * See also the UDisp class for inherited methods.
   */
  class UAppli {
  public:
    static UConf conf;
    /**< configuration of the application, must be set BEFORE the UAppli is created.
     * 'conf' is an object that is created when the program is launched. Its fields:
     * - are initialized to default values by UConf::UConf()
     * - can be changed BEFORE the UAppli is created, typically in the main() function
     * - are modified when the UAppli instance is created, according to (argc, argv)
     * @see: UConf, UAppli::UAppli().
     * @see also: UConf::parseOptions(), UConf::readConfFile().
     */
    
    UAppli(int& argc, char** argv,
           UOption* command_line_options = null, 
           const char* configuration_file = null);
    /**< constructor for creating the Application Context.
     * Arguments:
     * - 'argc' and 'argv' : the parameters of the main() function
     * - 'command_line_options' : optional command line options: @see UConf for details
     * - 'configuration_file' : an optional configuration file: @see UConf for details
     *
     * Exceptions:
     * - UError exception thrown if the X server can't be opened.
     *
     * Command line options:
     * - see class UConf for details.
     * Examples:
     * <pre>
     *    $ myappli --help-x
     *    $ myappli --scale 0.75 --display hendrix.tsne.fr:0
     *</pre>
     * the first line prints out the help message. The second line specifies
     * a rendering scale and a X server display name.
     *
     * See also: UAppli::openDisp() and the man of classes UAppli and UDisp.
     */
    
    virtual ~UAppli();
    
    static UAppli& appli() throw (UError);
    /**< returns a reference to the UAppli instance of the program.
     * There is *only one* UAppli for a given program.
     * Throws a UError exception if the UAppli has not yet been created.
     */
    
    static UAppli* getAppli();
    /**< returns a pointer the UAppli instance of the program.
     * same as UAppli::appli() except that no exception is thrown.
     */
    
    static UConf& getConf() {return conf;}
    ///< returns the configuration of the application, @see UConf.
    
    static const char* getVersion();
    ///< returns the version of the Ubit toolkit (for instance "6.0.0").
    
    static bool isUsingGL() {return conf.is_using_gl;}
    ///< returns true if OpenGL is used for rendering graphics.
    
    static bool isRunning();
    ///< returns true if the main loop is running.
    
    static bool isExiting();
    ///< returns true if the applicating is exiting.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static void add(UWin&);
    /**< adds a window (UFrame, UDialog, etc) to the UAppli.
     * Note that:
     * - the first UFrame that is shown or added to the UAppli is said to be the
     *   "Main Frame" of the application. It will be returned by UAppli::getMainFrame()
     *
     * - windows are initially hidden, their show() method must be called to make 
     *   them visible. If a window has not been added to a display by UDisp::add(), 
     *   it is implicitely added to the the UAppli when its show() method is called.
     * 
     * - the size of windows is computed when their show() method is called for the
     *   first time. It won't change afterwards excepts if UWin::adjustSize() is called.
     *
     * @see also: UWin:show(), UWin:update() and UWin, UFrame and UDialog classes.
     */ 
    
    static void add(UWin*);
    ///< calls add(UWin&) if the argument is not null, produces an error otherwise.
    
    static void remove(UWin& win, bool auto_delete = true);
    /**< removes or deletes a window from the UAppli.
     * this function works as follows:
     * - 1) the first occurence of 'win' is removed from the child list
     * - 2) if 'auto_delete' is true, this 'win' is DELETED if it has no other
     *      parent in the scene graph AND if it is not pointed by a UPtr<>.
     * - 3) destruction is RECURSIVE: if this 'win' is deleted, its descendants
     *      that have no other parents and are nor UPtr'ed are also destroyed.
     */
    
    static void remove(UWin*, bool auto_delete = true);
    ///< calls remove(UWin&) if the argument is not null, produces an error otherwise.
    
    static int start();
    /**< starts the event main loop of the application.
     * returns the value given to UAppli::quit()
     */
    
    static void quit(int status = 0);
    /**< quits the main loop of the application.
     * UAppli::start() will return the 'status' value
     */
    
    static UFrame* getMainFrame();
    /**< returns the Main Frame (if any, null otherwise).
     * the main frame is the first UFrame that was added to the UAppli by add().
     */
    
    static const UStr& getName();
    ///< returns argv[0], the name of the application.
    
    static UStr getTitle();
    ///< returns the title of the Main Frame (@see getMainFrame()).
    
    static void setTitle(const UStr& title);
    ///< changes the title of the Main Frame (@see getMainFrame()).
    
    static void setFocus(UView*);
    /**< gives the input focus to this view.
     * no object gets the focus if argument is null. The focus is reset when the user 
     * clicks on a the view of widget that can handle input from the keyboard 
     */
    
    static void updateAll(const UUpdate& mode = UUpdate::layoutAndPaint); 
    /**< updates all windows.
     * see UWin::update() for details.
     */
    
    
    // - - - Errors and warnings - - - - - - - - - - - - - - - - - - - - - - - -
        
    static UErrorHandler& getErrorHandler();
    ///< returns the current error handler (see UErrorHandler).
    
    static void setErrorHandler(UErrorHandler&);
    ///< changes the current error handler (see UErrorHandler).
        
    
    static void error(const char* funcname, const char* format, ...) throw(UError);
    ///< raises an error; shortcut for raiseError(UError::ERROR ...).
    
    static void warning(const char* funcname, const char* format, ...) throw (UError);
    ///< raises a warning; shortcut for raiseError(UError::WARNING, ...).
    
    static void fatalError(const char* funcname, const char* format, ...) throw (UError);
    ///< raises a fatal error; shortcut for raiseError(UError::FATAL_ERROR, ...).
     
    
    static void raiseError(int errnum, const UObject*, 
                           const char* funcname, const char* format, ...) throw (UError);
    /**< raises an error: prints out a message and/or generate an exception.
     * this method:
     * - displays an error message (on std::cerr by default, see below)
     * - throws a UError exception if 'errnum' is < 0.
     *   Predefined errnums are UError::WARNING, ERROR, FATAL_ERROR, INTERNAL_ERROR 
     *   but other values may be used if needed.
     * - 'funcname' should be the name of the function where the error occured
     * - 'format'is a printf-like format
     * - '...' is a variadic list of arguments that correspond to 'format'.
     * 
     * see also:
     * - shortcut functions: error(), warning(), fatalError()
     * - UErrorHandler for configuring error management.
     */
    
    static void raiseError(int errnum, const UObject*, 
                           const char* funcname, const char* format, va_list) throw (UError);
    ///< raises an error; see raiseError().

    
    // - - - Time and Timeouts - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static unsigned long getTime();
    ///< returns the current time.
    
    static void setMotionLag(unsigned long app_lag, unsigned long nat_lag);
    /**< changes the motion lag.
     * motion (drag, move) events are withdrawn if the time lag is hihgher 
     * than these values (default are 15 and 100 ms respectively). 
     */
    
    static void addTimeout(unsigned long msec_delay, int ntimes, UCall& callback);
    /**< fire this callback after a given delay.
     * Args:
     * - 'delay' is in milliseconds. if 'delay' = 0, the timer is
     *   immediately fired when the main loop becomes idle.
     * - 'ntimes' specifies how many times the timer is fired (each time
     *   after the specified 'delay'). The timer will be fired repeatedly
     *   until the completion of the application if ntimes is -1.
     * - 'callback' is a ucall<> expression (see UCall). It is destroyed after
     *    completion, except if referenced by a uptr<> or another node (see UNode).
     * See also: postpone() and class UTimer.
     */
    
    static void postpone(UCall& callback);
    /**< fire this callback when the main loop becomes idle.
     * this function is useful:
     * - to postpone the execution of callbacks that take some time
     * - to request the execution of callbacks in the main thread from another thread
     * 'callback' is a ucall<> expression (see UCall). It is destroyed after
     *  completion, except if referenced by a uptr<> or another node (see UNode).
     * See also: onTimeout() and UTimer.
     */
    
    
    // - - - Displays and Event Flows  - - - - - - - - - - - - - - - - - - - - -
    
    static UDisp* getDisp();
    ///< returns the default display of the appplication.
    
    static UDisp* getDisp(int ID);
    /**< returns the display with this ID.
     * Ubit applications can open windows on multiple displays when X11 is used.
     * this function returns the UDisp that has this ID, and numm otherwise.
     * The ID of the default display is 0.
     * @see UAppli::openDisp().
     */
    
    static const UDispList& getDispList();
    //< returns the list of Displays that are connected with this UAppli.
    
    static UDisp* openDisp(const UStr& display_name);
    /**< opens a connection on another X display.
     * returns null if the connection failed. Otherwise, windows (UFrame,
     * UDialog...) can be added to the returned UDisp by using its add()
     * method. 
     * - 'display' is the name of the X Window server. It follows the syntax:
     *    hostname[:display_no[:screen_no]] such as in: hendrix.tsne.fr:0.0
     * - window children can be shared by windows that belong to different
     *   displays. They are automatically replicated (and synchronized) on
     *   these displays.
     */
    
    static void closeDisp(UDisp*);
    ///< closes a connection with another X display.  
    
    static UEventFlow* getFlow(int ID = 0);
    /**< returns the Event Flow corresponding to this ID (null if not found).
     * A UAppli can manage several separate Event Flows on one or multiple displays.
     * This is useful for two-handed interaction or groupware, see UAppli for details.
     *
     * Each event flow has a unique ID. 0 is the ID of the native event flow on 
     * the main display (ie. the display where the UAppli was launched).
     * @see also: UDisp::getDispFlow()
     */
    
    static const UFlowList& getFlowList();
    ///< returns the list of all event flows on all displays (see getFlow()).
    
    static USelection* getSelection(int ID = 0);
    /**< returns the text selection manager.
     * return the text selection of the native event flow of the main display
     * by default (ie. if ID = 0). However, an application may have several
     * selections (it has as many selections as its number of event flows). The ID
     * number then specifies the requested event flow. @see: getFlow() for details
     */
    
    
    // - - - Inter-client Messages - - - - - - - - - - - - - - - - - - - - - - -
    
    static void onMessage(const UStr& port_name, UCall& callback);
    /**< adds a callback that is fired when data is received on this port.
     * Ubit messages makes it simple to exchange data between applications.
     * See class UMessagePort.
     */
    
    static UMessagePort& getMessagePort(const UStr& name);
    ///< Ubit Messages: @see onMessage() and class UMessagePort.
    
    static UMessagePort* findMessagePort(const UStr& name);
    ///< Ubit Messages: @see onMessage() and class UMessagePort.
    
    static UMessagePortMap* getMessagePortMap();
    ///< [impl] Ubit Messages.
    
    
    // - - - Misc - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    
    static const UStr& getImaPath();
    ///< see setImaPath().
    
    static void setImaPath(const UStr& pathname);
    /**< get/set the default image path.
     * the default image pathn is prefixed to image file names that do not
     * start with / or .
     */
    
    static UStyleSheet& getStyleSheet();
    ///< returns the style sheet of the application.
    
    static const UStyle& getDefaultStyle();
    ///< returns the default element style.
    
    UMenu* getOpenedMenu();
    ///< [impl] returns the menu that is currently opened (null otherwise).
    
    static bool hasTelePointers();
    ///< [impl] returns true if the appli has telepointers.
    
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    
    friend class UEventFlow;
    
    static class UAppliImpl& impl;
    ///< [impl] reference to the object that stores the application data.
    
    UStr& initConf(int& argc, char** argv, UOption*, const char* confile);
    ///< [impl] parses the config specs, inits the global data of the appli, returns the display name.
    
    static void internalError(const char* fun, const char* format, ...) throw (UError);

    static void deleteNotify(UDisp*);
    ///< [impl] notifies that a display is being destroyed.
    
    static void deleteNotify(UElem*);
    ///< [impl] notifies that an element is being destroyed.

    static void deleteNotify(UView*);
    ///< [impl] notifies that a view is being destroyed.
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Error management.
   */
  class UErrorHandler : public UObject {
  public:
    UCLASSDEF("UErrorHandler", UErrorHandler, null)
    
    UErrorHandler(const UStr& label, std::ostream* out_stream = &std::cerr);
    /**< creates a new error handler.
     * - 'label' identifies the handler (it is the name of the application for the
     *    default UAppli error handler).
     * - 'out_stream' specifies on which output stream errors are written. std::cerr is
     *    used by default. No errors are written if this argument is null.
     */
    
    virtual ~UErrorHandler();
    
    virtual void setOutputStream(std::ostream*);
    ///< specifies on which output stream errors are written (errors not written on a stream if argument is null).
    
    virtual void setOutputBuffer(UStr*);
    ///< specifies a string buffer on which errors are written (errors not written on a buffer if argument is null).
    
    UStr& label() {return *plabel;}
    ///< returns the label property of this UErrorHandler.
    
    virtual void warning(const char* funcname, const char* format, ...) const throw (UError);
    ///< raises a warning; shortcut for raiseError(UError::WARNING, null, ...).
    
    virtual void error(const char* funcname, const char* format, ...) const throw (UError);
    ///< raises an error; shortcut for raiseError(UError::ERROR, null, ...).
    
    virtual void error(int errnum, const UObject*, const char* funcname, 
                       const char* format, ...) const throw (UError);
    ///< raises an error; shortcut for raiseError(UError::ERROR, ...).
    
    virtual void parserError(int errnum, const UChar* text_buffer,
                                  const char* msg_start, const UStr& name,
                                  const char* msg_end, const UChar* line) const throw (UError);
    ///< used by XML and Style parsers to raise a formatted error.
    
    virtual void raiseError(int errnum, const UObject*, const char* funcname, 
                            const char* format, va_list) const throw (UError);
    /**< raises an error: prints out a message and/or generate an exception.
     * this method:
     * - displays an error message (on std::cerr by default, see below)
     * - throws a UError exception if 'errnum' is < 0.
     *   Predefined errnums are UError::WARNING, ERROR, FATAL_ERROR, INTERNAL_ERROR 
     *   but other values may be used if needed.
     * - 'funcname' should be the name of the function where the error occured
     * - 'format'is a printf-like format
     * - '...' is a variadic list of arguments that correspond to 'format'.
     */
            
    virtual void raiseError(int errnum, UStr* msg) const throw (UError);

    virtual const char* getErrorName(int errnum) const;
    virtual void formatMessage(UError&, const char* format, va_list) const;
    virtual void printOnStream(const UError&) const;
    virtual void printOnBuffer(const UError&) const;
    
  protected:
    uptr<UStr> plabel;
    uptr<UStr> pbuffer;
    std::ostream* fout;
  };
  
}
#endif

