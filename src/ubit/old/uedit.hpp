/************************************************************************
 *
 *  uedit.cpp: Text Edition Attribute
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

#ifndef _uedit_hpp_
#define	_uedit_hpp_ 1
#include <ubit/uattr.hpp>
namespace ubit {
  
  /** Text editing and caret controller.
   * exemple: 
   * <pre>
   *   UStr& str = ustr("abcd");
   *   UBox& box = ubox(uedit() + str);
   * </pre>
   * makes'str' editable inside container 'box'
   */
  class UEdit : public UAttr {
  public:
    UCLASS(UEdit)
    
    UEdit();
    ///< creates a new UEdit property; @see also shortcut uedit().
    
    virtual ~UEdit(); // NOTE: removingFrom() requires a destructor.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    bool isEditable() const;
    UEdit& setEditable(bool = true);
    ///< get/set text editing mode
    
    bool isCaretVisible() const;
    UEdit& setCaretVisible(bool = true);
    ///< get/set caret visibility.
    
    UColor* getCaretColor() const;
    UEdit& setCaretColor(UColor*);
    /**< get/set the color used to render the caret.
     * a value of null means that the caret has the same color as the text 
     */
    
    long getCaretPos() const;
    /**< returns the caret position.
     * NOTE that \n chars are not visible but count for 1 char!
     */
    
    UEdit& setCaretPos(long pos);
    /**< changes the caret position.
     * NOTE that \n chars are not visible but count for 1 char!
     */
    
    long getCaretPos(UElem& container) const;
    UEdit& setCaretPos(long pos, UElem& container);
    /**< get/sets the caret position of a shared UEdit.
     * when the UEdit is shared by several containers (a strange idea
     * actually) it is necessary to specify which container we are
     * talking about
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    UStr* getCaretStr() const;
    UStr* getCaretStr(int& pos_in_str) const;
    /**< returns the UStr that contains the caret and the position of the caret in this string.
     * details:
     * - returns null if the caret is not set
     * - 'pos_in_str' is relative to the string that contains the caret
     * - Note that a container (UBox or subclass) can contain several UStr
     *   and that a UEdit may (rarely) be included in several containers.
     */
    
    UEdit& setCaretStr(UStr* str);
    UEdit& setCaretStr(UStr* str, int pos_in_str);
    /**< sets the UStr that contains the caret and the position of the caret in this string.
     * details:
     * - 'pos_in_str' is relative to 'str', the string that will contain 
     *   the caret. 'pos_in_str' = 0 corresponds to the beginning of the 
     *   string; 'pos_in_str' = -1 to the end of the string.
     * - the 'str' must (obvioulsy) be in the same container as the UEdit
     * - Note that a container (UBox or subclass) can contain several UStr.
     *   and that a UEdit may be included in several containers.
     */
    
    static UStr* getPreviousStr(UStr* from, UElem& container);
    static UStr* getNextStr(UStr* from, UElem& container);
    /**< [unstable] returns the string before/after 'from' in this 'container'.
     * returns null if no previous/next string.
     * [unstable] this fct is not defined at the right place!
     */
    
    static UStr* getParagraphFirstStr(UStr* from, int& pos, UElem& container);
    static UStr* getParagraphLastStr(UStr* from, int& pos, UElem& container);
    /**< [unstable] returns the first/last string in the same paragraph than 'from'.
     * returns the beginning/ending string contained in the same line 
     * than 'from' in this parent.
     * returns 'from' itself if it is the beginning/ending string.
     * [unstable] this fct is not defined at the right place!
     */
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
    
    USelection* getSelection(UInputEvent& e, UStr& sel_text);
    void deleteSelection(USelection* sel, UStr& sel_text, UElem& container);
    void deletePreviousChar(UElem& container, UView* view);
    void deleteChar(UElem& container, UView* view);
    bool nextChar(UElem& container, UView* view);
    bool previousChar(UElem& container, UView* view);
    
    void beginningOfLine(UElem& container, UView* view);
    void endOfLine(UElem& container, UView* view);
    void previousLine(UElem& container, UView* view);
    void nextLine(UElem& container, UView* view);
    ///< [impl] moves the caret in the expected way.
    
    virtual void update();
    
#ifndef NO_DOC
    virtual void putProp(UUpdateContext*, UElem&);
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    // NOTE: removingFrom() requires a destructor.
    
    void setCaretStr(UStr*, int pos_in_str, bool update, UView* view);
    
    // prototypes for non-warped text (UView)
    virtual float getXpos(UUpdateContext&, const URect& r) const;
    virtual void  paint(UGraph&, UUpdateContext&, const URect &r) const;
    
    // prototypes for warped text (UFlowview)
    virtual float getXpos(UUpdateContext&, const URect&, int offset, int cellen) const;
    virtual void  paint(UGraph&, UUpdateContext&, const URect&, int offset, int cellen) const;
    
  private:
    uptr<UCall>  calls;        // callback object
    uptr<UCall>  calls2;        // callback object
    uptr<UColor> caret_color;  // specific color for displaying the caret (if any)
    uptr<UStr>   caret_str;    // the string that contains the caret (if any)
    long caret_pos;             // the position of the caret in 'caret_str'
    bool is_editable, is_visible;
    mutable bool repainted;
    virtual void inputCB(UInputEvent&);
    virtual void kpressed(UKeyEvent&);
    virtual void mpressed(UMouseEvent&);
    virtual void mreleased(UMouseEvent&);
    virtual void callbacks2(UMouseEvent&);
#endif
  };
  
  inline UEdit& uedit() {return *new UEdit();}
  ///< creator shortcut that returns *new UEdit().
  
}
#endif
/* ==================================================== [TheEnd] ======= */
