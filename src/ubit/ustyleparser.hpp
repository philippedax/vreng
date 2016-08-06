/* ==================================================== ======== ======= *
 *
 *  ustyleparser.hpp
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

#ifndef _ustyleparser_hpp_
#define _ustyleparser_hpp_ 1
#include <vector> 
#include <fstream>
#include <ubit/unode.hpp>
#include <ubit/ustr.hpp>
namespace ubit {
  
  typedef char UChar;
  
  /** Style Parser.
   * Abstract class: methods addStyle() and addProp() must be
   * implemented by subclasses.
   */
  class UStyleParser {
  public:
    
    struct StyleMaker {
      std::vector<UStr*> selectors;
      unsigned int count;
      
      StyleMaker();
      virtual ~StyleMaker();
      virtual void begin() {}
      virtual void create() {}
      virtual void addProp(const UStr& name, const UStr& value) {}
      virtual void end(bool ok) {}
    };
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    
    UStyleParser();
    virtual ~UStyleParser();
    
    virtual int parseImpl(StyleMaker&, const UStr& buffer);
    ///< parses the buffer by using this StyleMaker.
    
    int getStatus() {return stat;}
    ///< returns the parsing status.
    
    UErrorHandler* getErrorHandler() {return perrhandler;}
    ///< returns the current error handler.
    
    void setErrorHandler(UErrorHandler* eh) {perrhandler = eh;}
    ///< changes the error handler (UAppli default handler used if argument is null).
    
  protected:
    bool readStyleDef(StyleMaker&);
    ///< reads one style definition (its selectors + its properties).
    
    bool readStyleSelectors(StyleMaker&);
    ///< reads the selector(s) of the style definition.
    
    bool readStyleProps(StyleMaker&);
    ///< reads the properties of the style definition (the {} part after the selector).
    
    void skipSpaces();
    bool readName(UStr& name);
    bool readValue(UStr& value);
    bool readNameValuePair(UStr& name, UStr& value);
    
    virtual void error(const char* msg, const UChar* line);
    virtual void error(const char* msg_start, const UStr& name,
                       const char* msg_end, const UChar* line);
    virtual void unexpected(const char* msg, const UChar* line);
    
    bool permissive;
    int stat;
    const UChar *text_buffer, *p;
    uptr<UErrorHandler> perrhandler;
  };
  
}
#endif

