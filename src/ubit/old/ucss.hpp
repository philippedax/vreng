/* ==================================================== ======== ======= *
 *
 *  ucss.hpp
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

#ifndef _ucss_hpp_
#define _ucss_hpp_ 1
#include <map>
#include <ubit/udefs.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/ustyleparser.hpp>
#include <ubit/udom.hpp>
namespace ubit {
  
  /** [impl] CSS parser for XML/HTML documents.
   * this stylesheet parser is used implicitely when reading UXmlDocument objects.
   */
  class UCssParser : public UStyleParser {
  public:
    int parse(const UStr& buffer, class UXmlDocument*);
    int parseAttr(const UStr& buffer, class UXmlDocument*, UAttrList*);
  };
  
  
  /** [impl] precompiled style properties.
   * base for UCssProps.
   */
  class UStyleProps {
  public:
    typedef void (*AddPropFunc)(UXmlDocument*, UAttrList*, const UStr&);
    ~UStyleProps();
    
    AddPropFunc findAddPropFunc(const UStr& prop_name);
    void defProp(const char* prop_name, AddPropFunc);
    void defProp(const UStr& prop_name, AddPropFunc);
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // useful fcts.
    
    static bool parseUrl(const UStr&, UStr& url, UStr& remain);
     
  private:
    struct Comp {
      bool operator()(const UStr*s1, const UStr*s2) const {return s1->compare(*s2)<0;}
    };
    typedef std::map<const UStr*, AddPropFunc, Comp> PropMap;
    PropMap prop_map;
  };
  
  /* ==================================================== ======== ======= */
  /** [impl] precompiled CSS properties.
   * to be completed...
   */
  class UCssProps : public UStyleProps {
  public:
    UCssProps();
    
    static void create_font_family(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_font_size(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_font_weight(UXmlDocument*,UAttrList*, const UStr& v);
    static void create_font_style(UXmlDocument*, UAttrList*, const UStr& v);
    
    static void create_color(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_background_color(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_background_image(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_background(UXmlDocument*, UAttrList*, const UStr& v);
    
    static void create_width(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_height(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_text_align(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_vertical_align(UXmlDocument*, UAttrList*, const UStr& v);
    /*
    static void create_margin_top(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_margin_bottom(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_margin_left(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_margin_right(UXmlDocument*, UAttrList*, const UStr& v);
    */
    static void create_padding_top(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_padding_bottom(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_padding_left(UXmlDocument*, UAttrList*, const UStr& v);
    static void create_padding_right(UXmlDocument*, UAttrList*, const UStr& v);
    
    static void create_border(UXmlDocument*, UAttrList*, const UStr& v);
  };
  
  /* ==================================================== ======== ======= */
  /** [impl] precompiled HTML Elements Styles.
   * to be completed...
   */
  class UCssStyles {
  public:
    static UStyle* create_body_style();
    static UStyle* create_div_style();
    static UStyle* create_p_style();
    
    static UStyle* create_ul_style();
    static UStyle* create_ol_style();
    static UStyle* create_li_style();
    
    static UStyle* create_pre_style();
    static UStyle* create_blockquote_style();
    static UStyle* create_center_style();
    
    static UStyle* create_h1_style();
    static UStyle* create_h2_style();
    static UStyle* create_h3_style();
    static UStyle* create_h4_style();
    static UStyle* create_h5_style();
    static UStyle* create_h6_style();
    
    static UStyle* create_table_style();
    static UStyle* create_tr_style();
    static UStyle* create_td_style();
    static UStyle* create_th_style();
    
    static UStyle* create_span_style();
    static UStyle* create_b_style();
    static UStyle* create_i_style();
    static UStyle* create_em_style();
    static UStyle* create_u_style();
    static UStyle* create_font_style();
    
    static UStyle* create_a_style();
    static UStyle* create_img_style();
    static UStyle* create_br_style();
  };
  
}
#endif



