/************************************************************************
 *
 *  ustr.hpp: active strings (can have callbacks)
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

#ifndef _ustr_hpp_
#define	_ustr_hpp_ 1
#include <ubit/udata.hpp>
namespace ubit {
  
  /** Ubit String.
   *
   * Strings can have callback functions that are fired when their value is
   * changed (see an example below). String parents can also be notified when
   * string are modified by using the UOn::strChange condition:
   *
   * Example:
   * <pre> 
   *    class Demo {
   *    public:
   *       void strCB(UStr* s);   // this method can have an optional UEvent& parameter
   *       void textCB(UEvent&);  // the UEvent& parameter is optional
   *       ....
   *    };
   *
   *    Demo* d = new Demo();
   *    UStr* s = new UStr("Hello");
   *
   *    // d->strCB(s) will be called when the content of 's' is changed
   *    s->onChange( ucall(d, s, &Demo::strCB) );
   *
   *    // 's' is displayed in (and edited by) the textfield
   *    // (as expected, the strCB() callback will be called when 's' is modified)
   *    UBox* tf = new UTextfield(*s);
   *    ....
   * </pre>
   *
   * Alternatively:
   * <pre>
   *    // utextfield(...) is a shortcut for *new UTextfield(...)
   *    UBox& tf = utextfield(s + UOn::strChange / ucall(d, &Demo::textCB));
   * </pre>
   *
   * d->textCB(UEvent&) will be called when the content of 's' is changed. The UEvent&
   * parameter is optional. It makes it possible to retrieve the string as follows:
   * <pre>
   *    void Demo::textCB(UEvent& e) {
   *      UStr* s = 0;
   *      if (e.getTarget()) s = e.getTarget()->toStr();
   *      ...etc...
   *      // NB: one could also write:
   *      // if (e.getTarget()) s = dynamic_cast<UStr*>(e.getTarget());
   *   }
   * </pre>
   *
   * Note strCB() could also have a UEvent& parameter that would make it possible
   * to retrieve the string in the same way.
   *  
   * @see: UCall for more info on callback methods/functions and their parameters
   */
  class UStr: public UData {
  public:
    UCLASS(UStr)
    static const unsigned int npos = static_cast<unsigned int>(-1);
    
    virtual int getNodeType() const {return TEXT_NODE;}
    virtual const UStr& getNodeName() const;
    virtual UStr getNodeValue() const {return *this;}
    virtual const UStr& getData() const {return *this;}
    
    virtual UStr* toStr() {return this;}
    virtual const UStr* toStr() const {return this;}
    
    virtual UStr& onChange(UCall& c);
    /**< adds a callback that is fired when the string is modified.
     * @see an example in the UStr doc.
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    UStr();
    ///< creates a new empty string; @see also shortcut ustr().
    
    UStr(const char*);
    ///< creates a new string; @see also shortcut ustr(const char*).
    
    UStr(const UStr&);
    ///< creates a new string; @see also shortcut ustr(const UStr&).
    
    UStr(const std::string&);
    ///< creates a new string; @see also shortcut ustr(const std::string&).
    
    virtual ~UStr();
    
    int length() const {return len;}
    ///< returns the char count of the string.

    bool empty() const {return (len == 0);}
    ///< returns true if the string is empty.
    
    bool isEmpty() const {return (len == 0);}
    ///< returns true if the string is empty.
        
    virtual UStr& clear();
    ///< clears the string's content.
    
    virtual UStr& operator=(const char*);
    virtual UStr& operator=(const UStr&);
    virtual UStr& operator=(const std::string&);
    
    UStr& setInt(int);          // NB: a numeral, such as 111 is an int
    UStr& setLong(long);
    UStr& setFloat(float);
    UStr& setDouble(double);    // NB: un numeral 1.1 est double  
    
    int    toInt() const;
    long   toLong() const;
    float  toFloat() const;
    double toDouble() const;
    
    int  scan(const char* format, ...) const;
    
    bool scanValue(float& val, UStr& unit) const;
    bool scanValue(float& val, const char*& unit) const;

    const char* c_str() const {return s;}
    ///< returns an unmutable C string.
    
    char at(int pos) const;
    /**< returns the character at this position.
     * - the last char is returned if pos = -1
     * - 0 is returned if the position is invalid (no exception thrown)
     */
    
    char operator[](int pos) const;
    ///< same as at(int).
    
    char setCharAt(int pos, char c, bool update = true);
    /**< changes the character at offset 'pos'.
     * - pos = -1 refers to the last char of the string
     * - returns the character or 0 if the position is invalid
     * - does not throw exception (check the returned value)
     */
    
    friend std::ostream& operator<<(std::ostream&, const UStr&);
    ///< prints the string on an output stream (e.g. cout << str).
    
    friend std::istream& operator>>(std::istream&, UStr&);
    ///< reads the string from an input stream (eg. cin >> str).
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // comparisons
    
    bool operator==(const char* s2) const        {return equals(s2);}
    bool operator==(const UStr& s2) const        {return equals(s2);}
    bool operator==(const std::string& s2) const {return equals(s2);}
    
    bool operator!=(const char* s2) const        {return !equals(s2);}
    bool operator!=(const UStr& s2) const        {return !equals(s2);}
    bool operator!=(const std::string& s2) const {return !equals(s2);}
    
    virtual bool equals(const char*, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual bool equals(const UStr&, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual bool equals(const std::string&, bool ignore_case =false) const;
    ///< checks if string contents are the same, ignores case if last argument is true.
    
    virtual int compare(const char*, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compare(const UStr&, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compare(const std::string&, bool ignore_case =false) const;
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    virtual int compareN(const char*, unsigned int n, bool ignore_case =false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    virtual int compareN(const UStr&, unsigned int n, bool ignore_case =false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    virtual int compareN(const std::string&, unsigned int n, bool ignore_case=false) const;
    ///< compare the N first chars of strings, ignores case if last argument is true.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual int find(char s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const char* s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const UStr& s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int find(const std::string& s, int pos = 0) const;
    ///< search for the first occurrence of 's' starting at 'pos', return -1 if not found. 
    
    virtual int rfind(char s) const;
    ///< search for the last occurrence of 's', return -1 if not found. 
    
    UStr substring(int from_pos=0, unsigned int nbchars = npos) const;
    ///< returns a substring.
    
    std::string toString(int pos=0, unsigned int nbchars = npos) const;
    /**< UStr to std::string conversion.
     * nbchars = npos means "copy to the end of *this string".
     * the second variant appends the substring to the 'to' argument.
     */
    
    bool toString(std::string& to, int pos=0, unsigned int nbchars = npos)const;
    /**< UStr to std::string conversion.
     * nbchars = npos means "copy to the end of *this string".
     * the second variant appends the substring to the 'to' argument.
     */
    
    UStr toUpper() const;
    ///< returns a upercase copy.
    
    UStr toLower() const;
    ///< returns a lowercase copy.
    
    void upper();
    ///< converts to uppercase characters.

    void lower();
    ///< converts to lowercase characters.

    void capitalize();
    ///< capitalizes this string.
    
    UStr split(int pos, bool delete_char_at_pos = false);
    /**< splits this UStr
     * removes chars after 'pos' in this UStr and returns the remaining chars
     * in another UStr
     */
    
    void trim(bool trim_beginning = true, bool trim_end = true);
    ///< removes white spaces, tabs and newlines at the beginning and/or the end of this UStr.
    
    int tokenize(std::vector<UStr*>& tokens, const UStr& separators,
                 bool trim_begin = true, bool trim_end = true) const;
    /**< parses this UStr and stores the tokens separated by a separator in the vector.
     * - return the number of valid tokens, which may differ from 
     * tokens.size() as explained below.
     * - the separator is one character from 'separators'
     * - this function augments the 'tokens' vector and creates its UStr elements.
     *   NOTE THAT if the vector is not empty the existing UStr elements are 
     *   REUSED or unchanged if tokens.size() > the number of  valid tokens.
     *   (this may be potentially dangerous but CPU efficient for tokenizing 
     *   a large number of strings)
     * - trim() is called for each token with arguments trim_begin and trim_end.
     * - the content of this string is unchanged
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void append(char);
    virtual void append(const char*);
    virtual void append(const char*, unsigned int nbchars);
    virtual void append(const UStr&);
    virtual void append(const UStr&, unsigned int nbchars);
    virtual void append(const std::string&);
    virtual void append(const std::string&, unsigned int nbchars);
    virtual void append(int);
    virtual void append(double);
    
    UStr& operator&=(const char c)           {append(c); return *this;}
    UStr& operator&=(const char* s2)         {append(s2); return *this;}
    UStr& operator&=(const UStr& s2)         {append(s2); return *this;}
    UStr& operator&=(const std::string& s2)  {append(s2); return *this;}
    UStr& operator&=(int i)                  {append(i); return *this;}
    UStr& operator&=(double d)               {append(d); return *this;}
    /**< appends the argument to the end of this UStr.
     * usage: str &= arg;
     * see also: append() and operator &
     */
    
    friend UStr operator&(const UStr& s1, const UStr& s2);
    friend UStr operator&(const UStr& s1, const char* s2);
    friend UStr operator&(const char* s1, const UStr& s2);
    friend UStr operator&(const UStr& s1, const std::string& s2);
    friend UStr operator&(const std::string& s1, const UStr& s2);
    friend UStr operator&(const UStr&, int);
    friend UStr operator&(int, const UStr&);
    friend UStr operator&(const UStr&, double);
    friend UStr operator&(double, const UStr&);
    /**< creates a new string that collates the 2 arguments.
     * usage: str = arg1 & arg2;
     * NB: the + operator has another meaning: it is used to create widget child lists
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual bool insert(int to_pos, char);
    virtual bool insert(int to_pos, const char* from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    virtual bool insert(int to_pos, const UStr& from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    virtual bool insert(int to_pos, const std::string& from,
                        int from_pos = 0, unsigned int from_nbchars = npos);
    /**< inserts 'from' string at offset 'to_pos' in *this string.
     * details:
     * - pos = 0 specifies the beginning of the string (BEFORE the first char)
     * - pos = -1 specifies the end of the string (AFTER the last char)
     * - from_nbchars = npos means "to the end of the string"
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void remove(int pos, unsigned int nbchars);
    /**< removes 'nbchars' characters from offset 'pos'
     * details:
     * - pos = 0 specifies the first char of the string
     * - pos = -1 specifies the last char of the string
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void replace(int pos, unsigned int nbchars, const UStr&);
    virtual void replace(int pos, unsigned int nbchars, const char*);
    virtual void replace(int pos, unsigned int nbchars, const std::string&);
    ///< replaces 'nbchars' characters from offset 'pos' in this string by the argument string.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // filenames.
    
    virtual UStr basename(bool with_suffix = true) const;
    /**< returns the base name of a pathname (omitting the directory).
     * this string is supposed to contain the pathname.
     * the suffix (= the pathname extension) is removed if 'with_suffix' is false.
     */
    
    virtual UStr dirname(bool with_final_slash = true) const;
    /**< returns the directory of a pathname (omitting the file name).
     * this string is supposed to contain the pathname.
     * the returned dirname is ended by a / if 'with_final_slash' is true.
     */
    
    virtual UStr suffix() const;
    ///< returns the suffix of a pathname.
    
    virtual UStr expand() const;
    ///< expands a pathname that contains a ~ into an absolute path.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // I/O.
    
    virtual int read(const UStr& pathname);
    /**< reads this file and loads its content into this UStr.
     * pathname can start with a ~
     * returns the reading status (see UFilestat)
     */
    
    virtual int write(const UStr& pathname);
    /**< writes the content of this UStr in this file.
     * pathname can start with a ~
     * returns the writing status (see UFilestat)
     */
    
    static int readLines(const UStr& filename, std::vector<UStr*>& lines,
                         bool trim_beginning = true, bool trim_end = true);
    /**< reads this file and store its lines as a UStr vector.
     * filename can start with  a ~
     * returns the reading status (see UFilestat).
     * apply trim() to each line with arguments trim_beginning and trim_end
     */
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // impl.
    
    virtual void changed(bool update = true);
    /**< [impl] called when object's content is changed.
     * This function updates graphics (if 'update' is true) then fires 
     * parents' UOn::strChange callbacks
     */
    
    virtual void update();
    ///< updates grahics.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // implementation
    
#ifndef NO_DOC
    UStr(const char*, UConst);
    ///< creates a new CONSTANT string; @see UObject::UCONST.
    
    virtual void syncVals(char*,int);
    
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    ///< NOTE that this function require a specific destructor.
    
    virtual bool checkFormat(int pos, int newchar);
    virtual bool checkFormat(int pos, const char*);
    ///< checks input conformance to format model (if any).
    
    virtual void initImpl(const char*, int len);
    virtual void setImpl(const char*, int len);
    virtual void setImplNoCopy(char *_s, int _len);
    virtual bool insertImpl(int to_pos, char newchar, bool upd);
    virtual bool insertImpl(int to_pos, const char* s2, int from_pos,
                            unsigned int from_len, bool upd);
    virtual bool replaceImpl(int pos, unsigned int nbchars,
                             const char* s2, bool upd);
    
    // prototype for non-warped text (UView)
    virtual void getSize(UUpdateContext&, UDimension&) const;
    
    // prototype for warped text (UFlowview)
    virtual void getSize(UUpdateContext&, UDimension&, float available_width, 
                         int offset, int& str_sublen, int& change_line) const;
    
    // prototype for non-warped text (UView)
    virtual void paint(UGraph&, UUpdateContext&, const URect&) const;
    
    // prototype for warped text (UFlowview)
    virtual void paint(UGraph&, UUpdateContext&, const URect&, int offset, int cellen) const;
  private:
    char* s;
    int len;  
#endif
  };
  
  
  std::ostream& operator<<(std::ostream&, const UStr&);
  ///< prints the string on an output stream (e.g. cout << str).
  
  std::istream& operator>>(std::istream&, UStr&);
  ///< reads the string from an input stream (eg. cin >> str).  
  
  inline UStr& ustr() {return *new UStr();}
  ///< creator shortcut that return a new empty UStr string.  
  
  inline UStr& ustr(const char* s) {return *new UStr(s);}
  ///< creator shortcut that return a new UStr string.
  
  inline UStr& ustr(const UStr& s) {return *new UStr(s);}
  ///< creator shortcut that return a new UStr string.
  
  inline UStr& ustr(const std::string& s) {return *new UStr(s);}
  ///< creator shortcut that return a new UStr string.
  
  
  /* === useful functions =============================== ======== ======= */
  /** useful functions for C strings.
   * these functions may already exist on some OS.
   */
  struct UCstr {
    static char* dup(const char* s);
    ///< creates a duplicate of s.
    
    static char* dup(const char* s1, const char* s2);
    ///< creates a duplicate of s1 + s2.
    
    static char* dup(const char* s1, char sep, const char* s2);
    ///< creates a duplicate of s1 + sep + s2.
    
    static bool equals(const char* s1, const char* s2, bool ignore_case =false);
    ///< checks if string contents are the same, ignores case if last argument is true.

    static int compare(const char* s1, const char* s2, bool ignore_case =false);
    ///< compare strings lexicographically, ignores case if last argument is true.
    
    static int compareN(const char* s1, const char* s2, unsigned int n, bool ignore_case =false);
    ///< compare the N first chars of strings, ignores case if last argument is true.
        
    static const char* suffix(const char *pathname);
    /**< finds the suffix in a pathname.
     * the returned value is null or points inside the original string
     *  (its is not duplicated and thus, can't be freed)
     */
    
    static void parsePath(char* path_name, int path_len,
                          char*& dir_name, int& dir_len,
                          char*& base_name, int& base_len,
                          char*& suffix, int& suffix_len);
    /**< transforms a path name into its dirname, basename and suffix components.
     * the returned pointers points inside the path name and should not be freed.
     */
  };
  
}
#endif
