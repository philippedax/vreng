/* ==================================================== ======== ======= *
 *
 *  udom.hpp
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

#ifndef _udom_hpp_
#define _udom_hpp_ 1
#include <map>
#include <ubit/udoc.hpp>
#include <ubit/uclassImpl.hpp>  //UStyleSheet
#include <ubit/ustr.hpp>
#include <fstream>
namespace ubit {
  
  class UXmlGrammar;
  class UXmlGrammars;
   
  /* ==================================================== ======== ======= */
  /** XML Comment.
   */
  class UComment : public UNode {
  public:
    UCLASSDEF("comment", UComment, new UComment)

    UComment(const UStr& = "");  
    virtual int getNodeType() const {return COMMENT_NODE;}
    virtual const UStr& getNodeName() const;
    virtual UStr getNodeValue() const {return *data;}
    virtual const UStr& getData() const {return *data;}
    
  private:
    uptr<UStr> data;
  };
  
  /* ==================================================== ======== ======= */
  /** XML CDATASection.
   */
  class UCDATASection : public UNode { // pas bon: va afficher cdata
  public:
    UCLASSDEF("CDATASection", UCDATASection, new UCDATASection)
    
    UCDATASection(const UStr& = "");
    virtual int getNodeType() const {return CDATA_SECTION_NODE;}
    virtual const UStr& getNodeName() const;
    virtual UStr getNodeValue() const {return *data;}
    virtual const UStr& getData() const {return *data;}
    
  private:
    uptr<UStr> data;
  };
  
  /* ==================================================== ======== ======= */
  /** XML ProcessingInstruction.
   */
  class UProcessingInstruction : public UNode {
  public:
    UCLASSDEF("processinginstruction", UProcessingInstruction, new UProcessingInstruction("",""))
    
    UProcessingInstruction(const UStr& target, const UStr& data);
    virtual int getNodeType() const {return PROCESSING_INSTRUCTION_NODE;}
    virtual const UStr& getNodeName()  const {return getTarget();}
    virtual UStr getNodeValue() const {return getData();}
    virtual const UStr& getTarget() const {return *target;}
    virtual const UStr& getData() const {return *data;}
    
  private:
    uptr<UStr> target, data;
  };
  
  /* ==================================================== ======== ======= */
  /** XML DocType.
   */
  class UXmlDocType : public UNode {
  public:
    UCLASSDEF("doctype", UXmlDocType, new UXmlDocType("","",""))
    
    UXmlDocType(const UStr& name, const UStr& public_id, const UStr& system_id);
    virtual ~UXmlDocType();  
    virtual int getNodeType() const {return DOCUMENT_TYPE_NODE;}
    virtual const UStr& getNodeName() const {return *name;}
    virtual const UStr& getName()     const {return *name;}
    virtual const UStr& getPublicId() const {return *public_id;}
    virtual const UStr& getSystemId() const {return *system_id;}
    
  private:
    friend class UXmlDocument;
    uptr<UStr> name, public_id, system_id;
    //UXmlDocType();
  };
  
  
  /* ==================================================== ======== ======= */
  /** XML Doc.
   */
  class UXmlDocument : public UDoc {
  public:
    UCLASSDEF("document", UXmlDocument, new UXmlDocument)
    
    UXmlDocument();
    UXmlDocument(const UStr& pathname);
    virtual ~UXmlDocument();
    
    virtual int getNodeType() const {return DOCUMENT_NODE;}
    virtual const UStr& getNodeName() const;
    virtual UStr getNodeValue() const {return "";}
    
    virtual UElem* getDocumentElement() {return doc_elem;}
    virtual const UElem* getDocumentElement() const {return doc_elem;}
    virtual const UXmlDocType* getDoctype() const {return doc_type;}
    virtual const UStr& getXmlVersion()  const {return *xml_version;}
    virtual const UStr& getXmlEncoding() const {return *xml_encoding;}
    virtual bool  isXmlStandalone()      const {return xml_standalone;}
    
    virtual UAttr* createAttribute(const UStr& name);
    virtual UElem* createElement(const UStr& name);
    virtual UStr* createTextNode(const UStr& data);
    virtual UComment* createComment(const UStr& data);
    virtual UCDATASection* createCDATASection(const UStr& data);
    virtual UProcessingInstruction* createProcessingInstruction(const UStr& target, const UStr& data);
    
    virtual void initElement(UElem*);
    virtual void setClassStyle(UElem*, const UStr& name, const UStr& value);
    virtual void setIdStyle(UElem*, const UStr& name, const UStr& value);
    
    virtual void addGrammar(const UXmlGrammar&);
    ///< adds a grammar to the document.
    
    virtual void print(std::ostream& fout);
    ///< prints the XML tree on this stream (eg. std::cout).
    
    virtual void print(UStr& buffer);
    ///< prints the XML tree on this buffer.
    
    virtual const UDocAttachments* getAttachments() const;
    ///< returns the list of attachments.
    
    virtual int  loadAttachments(bool reload = false);
    virtual bool loadAttachment(UDocAttachment*, bool reload = false);
    virtual void addAttachment(UDocAttachment*);
    ///< impl.
    
    const UStyleSheet& getStyleSheet() const {return doc_stylesheet;}
    UStyleSheet& getStyleSheet() {return doc_stylesheet;}
    ///< impl.
    
  protected:
    friend class UXmlParser;
    static const UStr NodeName;
    uptr<UStr> xml_version, xml_encoding;
    bool xml_standalone;
    UXmlDocType* doc_type;
    UXmlGrammars* grammars;
    UStyleSheet doc_stylesheet;
    UElem* doc_elem;
    UDocAttachments attachments;
    void constructs();
    virtual void setClassIdStyle(UElem*, const UStr& name, const UStr& value);
  };
  
  /* ==================================================== ======== ======= */
  /** [impl] XML Creator.
   */
  struct UXmlCreator : public UDocCreator {
    UXmlCreator();
    virtual UDoc* create(UDocSource& doc);
    
    bool load_objects;  ///< load HTML objects (eg. images) [default=true].
    bool keep_comments; ///< keep comments [default=false].    
    bool permissive;    ///< parse documents in permissive mode [default=false].
    bool collapse_spaces;
  };
  
  /** [impl] HTML Creator.
   */
  struct UHtmlCreator : public UXmlCreator {
    UHtmlCreator();
  };
  
  /* ==================================================== ======== ======= */
  
  struct UImgAttachment : public UDocAttachment {
    const UStr url, type;
    UIma* ima;
    
    UImgAttachment(const UStr& _url, const UStr& _type, UIma* _ima)
    : url(_url), type(_type), ima(_ima) {}
    
    virtual const UStr& getUrl() const {return url;}
    virtual const UStr& getType() const {return type;}
    virtual bool isLoaded() const;
    virtual int  load(UDoc*);
  };
  
  /* ==================================================== ======== ======= */
  
  struct UCssAttachment : public UDocAttachment {
    const UStr url, type;
    UXmlDocument* doc;
    int stat;
    
    UCssAttachment(const UStr& _url, const UStr& _type, UXmlDocument* _doc)
    : url(_url), type(_type), doc(_doc), stat(0) {}
    
    virtual const UStr& getUrl() const {return url;}
    virtual const UStr& getType() const {return type;}
    virtual bool isLoaded() const {return stat > 0;}
    virtual int  load(UDoc*);
  };
  
}
#endif

