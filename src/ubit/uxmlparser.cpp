/************************************************************************
*
*  uxmlparser.cpp
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <stdexcept>
#include <clocale>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ubit/uappli.hpp>
#include <ubit/ustyleparser.hpp>
#include <ubit/udom.hpp>
#include <ubit/uxmlparser.hpp>
#include <ubit/uxmlgrammar.hpp>
using namespace std;
namespace ubit {

/* ==================================================== ===== ======= */

void UXmlParser::skipSpaces() {
  while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t'))  p++;
}

/* Definition: A Name is a token beginning with a letter or one of a few punctuation characters, and continuing with letters, digits, hyphens, underscores, colons, or full stops, together known as name characters.
 */
// starts on the 1st char of the name
// ends on the first non (alpha || - || _) character

bool UXmlParser::readName(UStr& name) {
  if (!isalpha(*p) && *p!='_' && *p!=':') return false; // 1st char must be in (alpha _ :)
  
  const UChar* begin = p;
  p++;

  while (isalnum(*p) || *p == '-' || *p == '_' || *p == ':'|| *p == '.') p++;
  if (!*p) return false;

  name.append(begin, p-begin);
  if (permissive) name.lower();
  return true;
}

/* ==================================================== ===== ======= */
// starts on starting " or '
// ends on ending " or ' or the first control character

bool UXmlParser::readQuotedValue(UStr& value, UChar quoting_char) {
  if (*p != quoting_char) return false;
  
  const UChar* begin = p;
  p++;

  while (*p /*&& !iscntrl(*p)*/ && *p != quoting_char) p++;
  //if (!*p || iscntrl(*p)) return false;

  value.append(begin+1, p-begin-1);
  p++;       // skip the final "
  return true;
}

bool UXmlParser::readUnquotedValue(UStr& value) {
  if (iscntrl(*p)) return false; 

  const UChar* begin = p;

  while (*p /*&& !iscntrl(*p)*/ && *p != ' ' && *p != '\n' 
	 && *p != '\r' && *p != '\t' && *p != '>') p++;
  //if (!*p || iscntrl(*p)) return false;

  value.append(begin, p-begin);
  return true;
}

  // ======================================================== [Elc] ===========
// starts on the 1st char of the name

bool UXmlParser::readNameValuePair(UStr& name, UStr& value) {
  const UChar* begin = p;

  if (!readName(name)) {
    error("invalid attribute name", begin);
    return false;
  }

  skipSpaces();
  
  if (*p == '=') p++;
  else {
    if (permissive) {
      value.append(name);   // ?? plutot ""
      return true;
    }
    else {
      error("invalid attribute '",name,
            "': no equal sign after attribute name",begin);
      return false;
    }
  }
  
  skipSpaces();

  if (*p == '"' || *p == '\'') {
    if (readQuotedValue(value, *p))      // *p == quote_char == ' or "
      return true;
    else {
      error("invalid attribute '",name,"': incorrect value", begin);
      return false;
    }
  }

  else if (permissive) {
    if (readUnquotedValue(value)) return true;
    else {
      error("invalid attribute '",name,"': incorrect value", begin);
      return false;
    }
  }

  else {
    error("invalid attribute '", name,
          "': value should be a quoted string", begin);
    return false;
  }
}

// ======================================================== [Elc] ===========
// starts on the 1st char of the name
// ends on the >

  UElem* UXmlParser::readElementStartTag(UStr& name, int& stat) {
  stat = false;
  const UChar* begin = p;

  if (!readName(name)) {
    error("invalid element name", begin-1);
    return null;
  }

  UElem* e = doc->createElement(name);
  if (!e) {
    error("unknown element", begin-1);
    return null;
  }
  
  while (true) {
    skipSpaces();

    if (*p == '>') {         // end of starting tag: <tag>
      if (permissive
          && (e->getClass().getParseModes() & UClass::EMPTY_ELEMENT))
        stat = END_TAG_AND_ELEM;
      else
        stat = END_TAG;
      return e;
    }

    else if (*p == '/' && *(p+1) == '>') {  // end of tag and elem: <tag/>
      p++;   // skip ending_char
      stat = END_TAG_AND_ELEM;
      return e;
    }

    else if (isalpha(*p)) {
      UStr name, value;

      if (!readNameValuePair(name, value)) {
        delete e;
        return null;
      }
      else {
        // may return null if attribute is unknown (if checked)
        UAttr* attr = doc->createAttribute(name);
        if (attr) {
          attr->setValue(value);
          e->setAttr(*attr);
          attr->initNode(doc, e);
        }
      }
    }
    
    else {
      unexpected("in starting tag", begin-1);
      delete e;
      return null;
    }  
  }
}

  // ======================================================== [Elc] ===========
// starts on the 1st char of the name
// ends on the >

int UXmlParser::readElementEndTag(const UStr& elem_name) {
  const UChar* begin = p;
  UStr ending_name;
  
  if (!readName(ending_name)) {
    error("invalid element name in ending tag", begin-1);
    return false;
  }

  skipSpaces();

  if (*p == '>') {  // end of ending tag: case </tag>
    if (ending_name.compare(elem_name) == 0)
      return END_TAG_AND_ELEM;
    else {
      error("tag mismatch: final tag should be </",elem_name,">", begin-2);
      return false;
    }
  }

  else {
    unexpected("in ending tag", begin);
    return false;  // invalid char
  }
}

  // ======================================================== [Elc] ===========
// starts on <
// ends on the final >

void UXmlParser::readElement(UElem* parent) {
  if (*p != '<') {
    error("element should start with a < sign", p);
    throw ParseError();
  }
  const UChar* begin = p;
  p++;

  if (*p == '!') {
    readSGMLData(parent);
    return;
  }

  else if (*p == '?') {
    readXMLInstruction(parent);
    return;
  }

  UStr name;
  int stat = false;
  UElem* e = readElementStartTag(name, stat);

  if (!e) throw ParseError();
  parent->add(e);

  // end of a terminal elem, case: />
  if (stat == END_TAG_AND_ELEM) {
    //e->docInit(doc);
    doc->initElement(e);
    return;  
  }

  p++;  // skip the > of the starting tag
    
  while (true) {
    skipSpaces();

    if (!*p || iscntrl(*p)) {
      unexpected("whithin element" /*'",name,"'"*/, begin);
      throw ParseError();
    }

    else if (*p == '<') {  // ending tag or nested tag

      if (*(p+1) == '/') {  // end elem </tag>
        p += 2;  // skip </
        if (!readElementEndTag(name)) throw ParseError();
        else {
          //e->docInit(doc);
          doc->initElement(e);
          return;
        }
      }

      else {
        readElement(e);     // nested tag <tag>
        p++;   // skip the final > of the elem
      }
    }
    
    else readText(e); 
  }
}

  // ======================================================== [Elc] ===========

static const char* findEndTag(const char* p, const UStr& end) {
  const char c = end[0];
  int l = end.length();
  while(*p) {
    if (tolower(*p) == c) {
      // if (UCstr::strncasecmp(p, w, l) == 0) return p;
      if (end.compareN(p, l) == 0) return p;   // !! no case !!
    }
    p++;
  }
  return null;
}

// starts on text beginning, ends on following <
void UXmlParser::readText(UElem* parent) {
  const UChar* begin = p;
  UStr text;

  // <script> and <style> tags: no parsing 'till the corresponding
  // </script> or </style> tag

  if (parent->getClass().getParseModes() & UClass::DONT_PARSE_CONTENT) {
    UStr endtag = "</"; 
    //endtag &= parent->getNodeName() ? *parent->getNodeName() : "";
    endtag &= parent->getNodeName();
    endtag.lower();

    const char* pend = findEndTag(p, endtag);
    if (!pend) {
      error("final tag ", endtag, "> is missing; aborting", begin);
      throw ParseError();
    }

    p = pend;
    text.append(begin, p-begin);
    if (!text.empty()) parent->add(doc->createTextNode(text));
    return;
  }

  bool in_token = false;
  bool collapse_spaces = false;

  // NB: selon la norme il faut toujours preserver les spaces et les
  // enlever uniquement au rendu mais on triche en mode html
  // (sauf pour les <pre>)
  if (collapse_spaces 
      && !(parent->getClass().getParseModes() & UClass::PRESERVE_SPACES)) {
    collapse_spaces = true;
  }
  
  while (true) {
    if (!*p /*|| iscntrl(*p)*/) {
      unexpected("in text element", begin);
      throw ParseError();
    }
    
    else if ((collapse_spaces && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t'))
             || *p == '<' || *p == '&') {
      
      if (in_token) {
        in_token = false;
        text.append(begin, p-begin);
        
        if (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t') {
          if (collapse_spaces) text.append(" ");
          else text.append(*p); //NB: il faudrait gerer les TABs
        }
      }
      
      if (*p == '&') {
        begin = p;
        UChar charval = readCharEntityReference();
        
        if (charval != 0) text.append(charval);
        // inserer la sequence telle quelle en cas d'erreur
        else text.append(begin, p-begin);
        
        // si &code; suivie d'un blanc: l'inserer
        if (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t') {
          text.append(" ");
          p++;
        }
      }
      
      else if (*p == '<') {
        if (!text.empty()) parent->add(doc->createTextNode(text));
        return;
      }
      
      else p++;
    }
    
    else {
      if (!in_token) {
        in_token = true;
        begin = p;
      }
      p++;
    }
  }
}
  
  /* ==================================================== ===== ======= */
  
  UChar UXmlParser::readCharEntityReference() {
  if (*p != '&') {
    error("character entity reference should start with an &", p);
    throw ParseError();
  }

  const UChar* begin = p;
  p++;

  if (*p=='#') {    // numrical entity
    p++;
    while (isdigit(*p)) p++;
  }
  else {   // char entity ref
    while (isalnum(*p) || *p=='_' || *p=='-') p++;
  }
  
  if (!*p) {
    unexpected("expected entity name for reference", begin);
    throw ParseError();
  }

  if (*p == ';') p++;
  else {
    if (permissive) return 0;  // will just insert the text "as it"
    else {
      error("expected entity name for reference", begin);
      throw ParseError();
    }
  }

  if (p-begin-2 <= 0) return 0;
  UStr code;
  code.append(begin+1, p-begin-2);

  if (code[0] == '#') {
    code.remove(0,1);
    UInt val = code;
    return val.intValue();
  }
  else {
    UChar val = doc->grammars->getCharEntityRef(code);
    if (val == 0) error("unknown character entity reference: ", code, "", begin);
    return val;
  }
}

  // ======================================================== [Elc] ===========
// XML declaration:  <?xml version="1.0" encoding="iso-8859-1"?>
// starts on '?'
// ends on '>'

bool UXmlParser::readXMLDeclaration() {
  const UChar* begin = p;
  p++;

  UStr name;
  if (!readName(name)) {
    error("invalid XML Declaration", begin-1);
    throw ParseError();
  }

  // xml is a reserved keyword for document declaration
  if (!(name == "xml")) {
    error("invalid XML Declaration: tag name should be 'xml'", begin-1);
    throw ParseError();
  }

  while (true) {
    skipSpaces();

    if (*p == '>') {         // end of starting tag: <tag>
      return true;
    }

    else if (*p == '?' && *(p+1) == '>') {  // end of tag and elem: <tag/>
      p++;   // skip ending_char
      return true;
    }

    else if (isalpha(*p)) {
      UStr name, value;

      if (!readNameValuePair(name, value)) {
        return false;
      }
      else {
        if (name == "version")
          *doc->xml_version = value;
        else if (name == "encoding")
          *doc->xml_encoding = value;
        else if (name == "standalone") {
          doc->xml_standalone = (value == "yes");
        }
        else {
          error("invalid attribute in XML declaration", name, null, begin-1);
          return false;
        }
      }
    }
    
    else {
      unexpected("in XML declaration", begin-1);
      return false;
    }
  }
}

/* ==================================================== ======== ======= */
// Processing instruction:  <?xxx ... ?>
// starts on '?'
// ends on '>'

void UXmlParser::readXMLInstruction(UElem* parent) {
  //if (*p != '?') {
  //  error("XML instruction should start with <?", p-1);
  //  throw ParseError();
  //}

  const UChar* begin = p;
  p++;

  UStr name;
  if (!readName(name)) {
    error("invalid XML instruction", begin-1);
    throw ParseError();
  }

  // xml is a reserved keyword for document declaration
  if (name == "xml") {
    error("the XML declaration must start at line 1 column 1", begin-1);
    throw ParseError();
  }

  while (*p) {
    if (*p == '?' && *(p+1) == '>') break;
    else p++;
  }

  if (*p == '?' && *(p+1) == '>') {
    // a completer: ...
    //cerr << "PROCESSING_INSTRCTION: not implemented yet" << endl;

    //node = doc->createProcessingInstruction(name, target);
    //if (!node) throw ParseError();
    //parent->appendChild(node);
    p += 2;
  }
  else {
    unexpected("in XML processing instruction", begin);
    throw ParseError();
  }
}
  
/* ==================================================== ======== ======= */
// cases:
//    <!-- comment -->
//    <!DOCTYPE HTML>
//    <!DOCTYPE HTML SYSTEM "-//W3C//DTD HTML 4.01 Transitional//EN">
//    <!CDATA ...>
//    <!ENTITY ...>
//
// starts on '?'
// ends on '>'

void UXmlParser::readSGMLData(UElem* parent) {
  if (*p != '!') {
    error("SGML element should start with <!", p-1);
    throw ParseError();
  }

  const UChar* begin = p;
  p++;     // skip the !
  
  if (*p=='-' && *(p+1)=='-') {     // <-- comment
    p += 2;     // skip --
    
    const UChar* final = strstr(p, "-->");   // --> is the end the comment
    if (!final) {
      error("Unterminated comment", begin-1);
      throw ParseError();
    }

    UStr data;
    if (p < final) data.append(p, final-p);
    parent->add(doc->createComment(data));
    p = final+2;      // end of -->
  }

  else {
    UStr name;
    
    if (!readName(name)) {
      error("invalid SGML element", begin-1);
      throw ParseError();
    }
    
    const UChar* final = strchr(p, '>');
    if (!final) {
      error("Unterminated element ", name,"", begin-1);
      throw ParseError();
    }

    if (name.equals("CDATA",permissive)) {
      //cerr << "CDATA: not implemented" << endl;
      UStr data;
      if (p < final) data.append(p, final-p);
      parent->add(doc->createCDATASection(data));
    }
    
    else if (name.equals("ENTITY",permissive)) {
      cerr << "ENTITY: not implemented" << endl;
    }
    
    else if (name.equals("DOCTYPE",permissive)) {
      cerr << "DOCTYPE: not implemented" << endl;
    }
    
    else {
      error("Unknown element ",name,"", begin-1);
    }
    
    p = final;  // end of >
  }
}

// ======================================================== [Elc] ===========

void UXmlParser::error(const char* msg, const UChar* line) {
  error(msg, "", 0, line);
}

void UXmlParser::error(const char* msg1, const UStr& name,
                       const char* msg2, const UChar* line) {
  perrhandler->parserError(UError::XML_ERROR, text_buffer, msg1, name, msg2, line);
}

void UXmlParser::unexpected(const char* msg, const UChar* line) {
  if (!*p) {
    error("premature end of file ", "", msg, line);
  }
  else if (iscntrl(*p)) {
    char s[] = "invalid character ' '";
    s[sizeof(s)-3] = *p;
    // NB: + avec un char deconne avec g++
    error(s, "", msg, line);
    p++;
  }
  else  {
    char s[] = "unexpected character ' '";
    s[sizeof(s)-3] = *p;
    error(s, "", msg, line);
    p++;
  }
}

/* ==================================================== [Elc] ======= */
 // NB: pour un "vrai" parsing XML il faut : permissive(false), 

UXmlParser::UXmlParser() :
status(0),
permissive(false),
collapse_spaces(false),
text_buffer(null),
p(null),
doc(null),
parser_grammars(new UXmlGrammars()),
perrhandler(UAppli::getErrorHandler()) {
}
  
UXmlParser::~UXmlParser() {
  delete parser_grammars;
}

void UXmlParser::addGrammar(const UXmlGrammar& g) {
  parser_grammars->addGrammar(g);
}

/* ==================================================== ===== ======= */

UXmlDocument* UXmlParser::read(const UStr& _pathname) {
  UStr buf;
  status = buf.read(_pathname);

  if (status <= 0) return null;
  else {
    parse(_pathname, buf);
    return doc;
  }
}

/* ==================================================== ===== ======= */

UXmlDocument* UXmlParser::parse(const UStr& _name, const UStr& _buffer) {
  status = 0;
  p = text_buffer = _buffer.c_str();
  if (!p || !*p) return null;

  status = 1;
  doc = new UXmlDocument(_name);
  if (parser_grammars) doc->grammars->addGrammars(*parser_grammars);
    
  // ATTENTION: ne doit pas etre cree avant un changement de Grammar !!
  // (sinon la classe sera indefinie)
  UElem* root = doc->doc_elem;
   
  try {
    // the XML declaration must start at line 1 column 1
    if (*p == '<' && *(p+1) == '?') {
      p++;           // skip <
      readXMLDeclaration();
      p++;           // skip >
    }

    while (*p) {
      const UChar* begin = p;
      skipSpaces();

      if (*p == '<') {
        readElement(root);
        p++;           // skip >
      }

      else if (*p) {
        unexpected("at first level", begin);
        throw ParseError();
      }
    }
  }

  catch (ParseError) {
    const char* msg = "XML Parser: Syntax error, parsing aborted";
    perrhandler->error(UError::XML_ERROR, null/*object*/, null/*funcname*/, msg);
    //if (sout) sout->append(msg);
    //if (fout) *fout << msg;
  }
  /*
  catch (std::out_of_range) {
    const char* msg = "XML Parser: Out of range error, parsing aborted";
    perrhandler->error(UError::XML_ERROR, null, null, msg);
    //if (sout) sout->append(msg);
    //if (fout) *fout  << msg;
  }
  catch (std::bad_alloc) {
    const char* msg = "XML Parser: Out of memory error, parsing aborted";
    perrhandler->error(UError::XML_ERROR, null, null, msg);
    //if (sout) sout->append(msg);
    //if (fout) *fout << msg;
  }
  catch (...) {
    const char* msg = "XML Parser: unknown error, parsing aborted";
    perrhandler->error(UError::XML_ERROR, null, null, msg);
    //if (sout) sout->append(msg);
    //if (fout) *fout << msg;
  }
   */

  p = text_buffer = null;  // !!
  return doc;
}

}
/*
 interface NodeList {
   Node               item(in unsigned long index);
   readonly attribute unsigned long   length;
 };

 interface Entity : Node {
   readonly attribute DOMString       publicId;
   readonly attribute DOMString       systemId;
   readonly attribute DOMString       notationName;
   // Introduced in DOM Level 3:
   readonly attribute DOMString       actualEncoding;
   // Introduced in DOM Level 3:
   attribute DOMString       xmlEncoding;
   // Introduced in DOM Level 3:
   attribute DOMString       xmlVersion;
 };

 interface EntityReference : Node {
 };

class XmlNotation : public XmlNode {
  virtual const UStr& getPublicId() const;
  virtual const UStr& getSystemId() const;
  virtual const UStr& getNodeName() const {return getName();}
  virtual UStr       getNodeValue()      {return 0;}
  virtual unsigned short   getNodeType() const {return NOTATION_NODE;}
};

class DOMConfiguration {
   void setParameter(in DOMString name, in DOMUserData value) raises(DOMException);
   DOMUserData getParameter(in DOMString name) raises(DOMException);
   boolean canSetParameter(in DOMString name, in DOMUserData value);
   
   cdata-sections
   comments
   error-handler
   validate
   validate-if-schema
   well-formed
  whitespace-in-element-content
 };
*/

