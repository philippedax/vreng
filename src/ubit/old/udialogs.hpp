/************************************************************************
 *
 *  udialogs.hpp: dialog boxes
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

#ifndef _udialogs_hpp_
#define	_udialogs_hpp_ 1
#include <ubit/uwin.hpp>
namespace ubit {

  /** Dialog box.
   * Dialogs can't be iconified and are always located on the top of the
   * "Main Frame" (@see UFrame). Also, Dialogs are hidden when the Main Frame
   * is iconified
   *
   * @see class UWin for inherited methods and other important remarks.
   */
  class UDialog : public UWin {
  public:
    UCLASS(UDialog)
    
    UDialog(UArgs nodelist = UArgs::none);
    ///< creates a new Dialog Window; @see also shortcut function udialog().
        
    virtual ~UDialog() {destructs();}
    
    virtual UDialog& setTitle(const UStr& title) {UWin::setTitle(title); return *this;}
    ///< changes the title of the dialog box.

    virtual void show(bool state, UDisp*); // redefined
    ///< shows/hides the dialog box on this display; @see: show(bool state);
    
    virtual void show(bool state = true) {UDialog::show(state, null);} // redefined
    /**< shows/hides this dialog box.
     * dialogs are not shown by default; show() must be called to make them visible.
     * Besides, their size do not change after the first call to show(), except if
     * the setSize() or adjustSize() method is called.
     *
     * @see also: showModal() to open a modal dialog.
     */
    
    static void showAlertDialog(const UStr& message_string);
    ///< pops up an Alert dialog on the screen (see showMessageDialog()).
    
    static void showAlertDialog(UArgs message_nodes);
    ///< pops up an Alert dialog on the screen (see showMessageDialog()).
    
    static void showErrorDialog(const UStr& message_string);
    ///< pops up an Error dialog on the screen (see showMessageDialog()).
    
    static void showErrorDialog(UArgs message_nodes);
    ///< pops up an Error dialog on the screen (see showMessageDialog()).
    
    static void showWarningDialog(const UStr& message_string);
    ///< pops up a Warning dialog on the screen (see showMessageDialog()).
    
    static void showWarningDialog(UArgs message_nodes);
    ///< pops up a Warning dialog on the screen (see showMessageDialog()).
    
    static void showMessageDialog(const UStr& title, const UStr& message_string,
                                  UArgs icon = UArgs::none);
    /**< pops up a Message dialog on the screen.
     * opens a modal dialog that display this message.
     * - the 'message_string' argument is copied (and can thus be freed).
     * - no 'icon' is displayed by defaut. 'icon' is an arbitrary arglist of nodes
     *   (see UArgs) that must not be destroyed (they won't appear otherwise)
     */
    
    static void showMessageDialog(const UStr& title, UArgs message_nodes,
                                  UArgs icon = UArgs::none);
    /**< pops up a Message dialog on the screen.
     * opens a modal dialog that display this message.
     * - 'message_nodes' is an arbitrary arglist of nodes (see UArgs) that must 
     *   not be destroyed (they won't appear otherwise)
     * - no 'icon' is displayed by defaut. 'icon' is an arbitrary arglist of nodes
     *   (see UArgs) that must not be destroyed (they won't appear otherwise)
     */
    
    static UStyle* createStyle();  // redefined
    virtual bool realize(); // redefined

#ifndef NO_DOC
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);  
  private:
    UCall* open_call;
    static class UOptionDialog* messdialog;
#endif
  };
  
  inline UDialog& udialog(UArgs arglist = UArgs::none) {return *new UDialog(arglist);}
  ///< shortcut function that returns *new UDialog(args).
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** UFrame: toplevel and main window.
   * The main window of the application should usually be a UFrame. This window, 
   * called the "Main Frame", has a special role for dialog boxes (@see UDialog)
   *
   * In contrast with Dialogs, Frames can be iconified separately. Most applications
   * have only 1 frame (the "Main Frame"). However, applications that require several 
   * iconfiable windows can create as many Frames as needed (the "Main Frame" is then
   * the first UFrame that was added to the UAppli)
   *
   * @see class UWin for inherited methods and other important remarks.
   */
  class UFrame : public UDialog {
    friend class UAppli;
  public: 
    UCLASS(UFrame)
    
    UFrame(UArgs nodelist = UArgs::none);
    ///< creates a new Frame Window; @see also shortcut function uframe().
    
    bool isMainFrame() const {return wmodes.IS_MAINFRAME;}
    ///< is this frame the "main frame"? (@see: UAppli)
    
    virtual UFrame& setTitle(const UStr& title) {UWin::setTitle(title); return *this;}
    ///< changes the title of the dialog box.

    virtual void show(bool state, UDisp*);  // redefined
    virtual void show(bool state = true) {UFrame::show(state, null);}  // redefined

    static UStyle* createStyle();  // redefined
    virtual bool realize();  // redefined
  };
  
  inline UFrame& uframe(const UArgs& a = UArgs::none) {return *new UFrame(a);}
  ///< shortcut that returns *new UFrame(args).
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Predefined dialog box with optional content, icon and buttons area.
   * @see UDialog.
   */
  class UOptionDialog : public UDialog {
  public:
    UCLASS(UOptionDialog)

    UOptionDialog(UArgs message_nodes = UArgs::none);
    UOptionDialog(const UStr& title, UArgs message_nodes, UArgs icon, UArgs buttons);
       
    UOptionDialog& setTitle(const UStr& title);
    UOptionDialog& setMessage(const UStr& string);
    UOptionDialog& setMessage(UArgs nodelist);
    UOptionDialog& setIcon(UArgs icon);
    UOptionDialog& setButtons(UArgs buttons);

  protected:
    uptr<UBox> picon, pmessage, pbuttons;
    void constructs(UArgs message);
  };
  
}
#endif
