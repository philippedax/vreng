/************************************************************************
 *
 *  uhtml.hpp: HTML parser and renderer
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

#ifndef _uhtml_hpp_
#define _uhtml_hpp_
#include <ubit/udefs.hpp>
#include <ubit/uxmlgrammar.hpp>
#include <ubit/uxmlparser.hpp>

namespace ubit {
  
  /** HTML Grammar.
   */ 
  class UHtmlGrammar : public UXmlGrammar {
  public:
    UHtmlGrammar();
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  /** HTML Parser.
   * this parser should be used instead of UXmlParser for parsing HTML text.
   * It automatically:
   * - adds the UHtmlGrammar grammar to the grammar list 
   * - sets the parser in Permissive and CollapseSpaces modes.
   * @see class XmlParser for details.
   */ 
  class UHtmlParser : public UXmlParser {
  public:
    UHtmlParser();
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  /** HTML Attribute.
   */ 
  struct UHtmlAttribute : public UAttr {
    UCLASSDEF("uhtmlattribute", UHtmlAttribute, null)
    
    virtual void setValue(const UStr& val) {setValueImpl(pvalue, val);}
    virtual bool getValue(UStr& val) const {return getValueImpl(pvalue, val);}
    
    static void setValueImpl(uptr<UStr>& pvalue, const UStr& value);
    static bool getValueImpl(const uptr<UStr>& pvalue, UStr& value);

    uptr<UStr> pvalue;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  struct UHtmlStyle : public UAttrList {
    UCLASSDEF("style", UHtmlStyle, new UHtmlStyle)
    void initNode(UDoc* doc, UElem* parent);    
    void setValue(const UStr& val) {UHtmlAttribute::setValueImpl(pvalue, val);}
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;  
  };
  
  struct UHtmlClass : public UHtmlAttribute {
    UCLASSDEF("class", UHtmlClass, new UHtmlClass)
    void initNode(UDoc* doc, UElem* parent);
  };

  struct UHtmlId : public UHtmlAttribute {
    UCLASSDEF("id", UHtmlId, new UHtmlId)
    void initNode(UDoc* doc, UElem* parent);
  };
  
  struct UHtmlRel : public UHtmlAttribute {
    UCLASSDEF("rel", UHtmlRel, new UHtmlRel)
  };
  
  struct UHtmlType : public UHtmlAttribute {
    UCLASSDEF("type", UHtmlType, new UHtmlType)
  };
  
  struct UHtmlAlt : public UHtmlAttribute {
    UCLASSDEF("alt", UHtmlAlt, new UHtmlAlt)
  };
  
  struct UHtmlName : public UHtmlAttribute {
    UCLASSDEF("name", UHtmlName, new UHtmlName)
  };
  
  struct UHtmlSrc : public UHtmlAttribute {
    UCLASSDEF("src", UHtmlSrc, new UHtmlSrc)
  };
  
  struct UHtmlHref : public UHtmlAttribute {
    UCLASSDEF("href", UHtmlHref, new UHtmlHref)
  };
  
  struct UHtmlColor : public UColor {
    UCLASSDEF("color", UHtmlColor, new UHtmlColor)
    void setValue(const UStr& value);    
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;
  };
  
  struct UHtmlBgcolor : public UBackground {
    UCLASSDEF("bgcolor", UHtmlBgcolor, new UHtmlBgcolor)
    void setValue(const UStr&);
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;
  }; 
  
  struct UHtmlBorder : public UBorder {
    UCLASSDEF("border", UHtmlBorder, new UHtmlBorder)
    void setValue(const UStr& v);
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;
  };
  
  struct UHtmlFontFace : public UFont {
    UCLASSDEF("face", UHtmlFontFace, new UHtmlFontFace)
    void setValue(const UStr&);    
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;
  };
  
  struct UHtmlFontSize : public UFont {
    UCLASSDEF("size", UHtmlFontSize, new UHtmlFontSize)    
    void setValue(const UStr&);
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:
    uptr<UStr> pvalue;
  };
  
  
  struct UHtmlUWidth : public UHtmlAttribute {
    UCLASSDEF("width", UHtmlUWidth, new UHtmlUWidth)
    //void setValue(const UStr&);
    //bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue,val);}
  private:
    void initNode(UDoc*, UElem* parent);
  };
  
  struct UHtmlHeight : public UHtmlAttribute {
    UCLASSDEF("height", UHtmlHeight, new UHtmlHeight)
    //void setValue(const UStr&);
    //bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue,val);}
  private:
    void initNode(UDoc*, UElem* parent);
  };
  
  struct UHtmlAlign : public UHalign {
    UCLASSDEF("align", UHtmlAlign, new UHtmlAlign)
    void setValue(const UStr&);
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:    
    uptr<UStr> pvalue;
  };
  
  struct UHtmlValign : public UValign {
    UCLASSDEF("valign", UHtmlValign, new UHtmlValign)
    void setValue(const UStr&);
    bool getValue(UStr& val) const {return UHtmlAttribute::getValueImpl(pvalue, val);}
  private:    
    uptr<UStr> pvalue;
  };

  struct UHtmlColspan : public UHtmlAttribute {
    UCLASSDEF("colspan", UHtmlColspan, new UHtmlColspan)
    void initNode(UDoc* doc, UElem* parent);
    void setValue(const UStr&);
  };
  
  struct UHtmlRowspan : public UHtmlAttribute {
    UCLASSDEF("rowspan", UHtmlRowspan, new UHtmlRowspan)
    void initNode(UDoc* doc, UElem* parent);
    void setValue(const UStr&);
  };
    
  struct UHtmlCellspacing : public UHtmlAttribute {
    UCLASSDEF("cellspacing", UHtmlCellspacing, new UHtmlCellspacing)
    void setValue(const UStr&);
  };
    
  struct UHtmlCellpadding : public UHtmlAttribute {
    UCLASSDEF("cellpadding", UHtmlCellpadding, new UHtmlCellpadding)
    void setValue(const UStr&);
  };  
    
  struct UHtmlNowrap : public UHtmlAttribute {
    UCLASSDEF("nowrap", UHtmlNowrap, new UHtmlNowrap)
    void setValue(const UStr&);
  };
  
}
#endif




