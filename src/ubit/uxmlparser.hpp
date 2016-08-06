/* ==================================================== ======== ======= *
 *
 *  uxmlparser.hpp
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 / Eric Lecolinet / Telecom ParisTech / http://www.enst.fr/~elc/ubit
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

#ifndef _uxmlparser_hpp_
#define _uxmlparser_hpp_ 1
#include <ubit/udom.hpp>
#include <fstream>
namespace ubit {
    
  class UXmlGrammar;
  
  /** XML parser.
   *
   * @see: use UHtmlParser to parse HTML code (and see and setPermissive()
   * and setCollapseSpaces() for more details)
   */
  class UXmlParser {
  public:
    struct ParseError {};
    
    UXmlParser();
    ///< creates a new XML parser.
    
    virtual ~UXmlParser();
    
    void addGrammar(const UXmlGrammar&);
    /**< adds a grammar to the parser.
     * note that order matters: an element or an attribute name is searched in
     * the first grammar, then is the second one, and so on. 
     */
    
    UXmlDocument* read(const UStr& path);
    /**< reads and parses a XML file and returns the corresponding XML tree.
     * 'path' is the qualified filename of the XML document.
     */
    
    UXmlDocument* readBuffer(const UStr& docname, const UStr& buffer) 
    {return parse(docname,buffer);}
    ///< synonym for parse();
    
    UXmlDocument* parse(const UStr& docname, const UStr& buffer);
    /**< reads and parses a XML buffer and returns the corresponding XML tree.
     * 'buffer' contains the text to parse and 'docname' the anme of this document
     */
    
    int getStatus() {return status;}
    ///< returns the reading/parsing status.
    
    UErrorHandler* getErrorHandler() {return perrhandler;}
    ///< returns the current error handler.
 
    void setErrorHandler(UErrorHandler* eh) {perrhandler = eh;}
    ///< changes the error handler (UAppli default handler used if argument is null).
    
    void setPermissive(bool b) {permissive = b;}
    /**< parses documents in permissive mode (default is false).
     * allows for constructs which are invalid in XML code but OK in HTML code:
     * - attributes value dont need to be quoted, missing values are also accepted.
     * - closing tags can be omitted for EMPTY_ELEMENTs (eg. <img>).
     *   EMPTY_ELEMENT is a mode of UElemClass, @see UClass::getMode()
     * - the textual content of DONT_PARSE_CONTENT elements is not parsed
     *   and their comments are stored as a text element (eg. <style> <script>)
     */
    
    void setCollapseSpaces(bool b) {collapse_spaces = b;}
    /**< collapses whitespaces (and tabs and newlines) in elements (default is false).
     * should be set to false when parsing actual XML code, and true for HTML code.
     * Note that whitespaces are never collapsed for elements which UElemClass
     */  
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    void readElement(UElem* parent);
    void readText(UElem* parent);
    bool readXMLDeclaration();
    void readXMLInstruction(UElem* parent);
    void readSGMLData(UElem* parent);
    
    void skipSpaces();
    UChar readCharEntityReference();
    bool readName(UStr&);
    bool readQuotedValue(UStr&, UChar quoting_char);
    bool readUnquotedValue(UStr&);
    bool readNameValuePair(UStr& name, UStr& value);
    UElem* readElementStartTag(UStr& elem_name, int& stat);
    int  readElementEndTag(const UStr& elem_name);
    
    void error(const char* msg, const UChar* line);
    void error(const char* msg_start, const UStr& name,
               const char* msg_end, const UChar* line);
    void unexpected(const char* msg, const UChar* line);
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
  private:
    static const int INVALID_TAG = 0, END_TAG = 1, END_TAG_AND_ELEM = 2;
    int status;
    bool permissive, collapse_spaces;
    const UChar *text_buffer, *p;
    UXmlDocument* doc;
    UXmlGrammars* parser_grammars;
    uptr<UErrorHandler> perrhandler;
  };
}
#endif



