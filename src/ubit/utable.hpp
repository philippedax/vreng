/************************************************************************
 *
 *  utable.hpp
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

#ifndef _utable_hpp_
#define	_utable_hpp_ 1
#include <vector>
#include <ubit/ubox.hpp>
#include <ubit/uboxes.hpp>
namespace ubit {
  
  /** UTable = class for creating (simplified) HTML like tables.
   * See: UTrow, UTcell
   */
  class UTable: public UBox {
  public:
    UCLASS(UTable)
    
    UTable(UArgs a = UArgs::none): UBox(a) {}
    ///< creates a new table widget (@see also shortcut utable()).
    
    static UStyle* createStyle();
  };
  
  inline UTable& utable(UArgs a = UArgs::none) {return *new UTable(a);}
  ///< shortcut that returns *new UTable().
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Table Row.
   *  NOTE: MUST be a child of a UTable object
   *  - See also: UTable, UTcell
   */
  class UTrow: public UElem {
  public:
    UCLASS(UTrow)
    
    UTrow(UArgs a = UArgs::none): UElem(a) {}
    ///< creates a new table row (@see also shortcut utrow()).
    
    static UStyle* createStyle();
  };
  
  inline UTrow& utrow(UArgs a = UArgs::none) {return *new UTrow(a);}
  ///< shortcut that returns *new UTrow().
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Table Cell.
   *  NOTE: MUST be a child of a UTrow object
   *  - See also: UTable, UTrow
   */
  class UTcell: public UFlowbox {
  public:
    UCLASS(UTcell)
    
    UTcell(UArgs a = UArgs::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    UTcell(short colspan, UArgs a = UArgs::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    UTcell(short colspan, short rowspan, UArgs a = UArgs::none);
    ///< creates a new table cell (@see also shortcut utcell()).
    
    static UStyle* createStyle();
    
    void  setColspan(short);
    void  setRowspan(short);
    short getColspan() {return colspan;}
    short getRowspan() {return rowspan;}
    
  private:
    short colspan, rowspan;
  };
  

  inline UTcell& utcell(UArgs a = UArgs::none) {return *new UTcell(a);}
  ///< shortcut that returns *new UTcell().
  
  inline UTcell& utcell(short colspan, UArgs a = UArgs::none) {return *new UTcell(colspan,a);}
  ///< shortcut that returns *new UTcell().
  
  inline UTcell& utcell(short colspan, short rowspan, UArgs a = UArgs::none) {return *new UTcell(colspan,rowspan,a);}
  ///< shortcut that returns *new UTcell().
  
  
  /* ==================================================== ===== ======= */
  
  struct UViewCell {
    UViewCell();
    float d, min_d, max_d, spec_d, percent;
    int rowspan, colspan;
  };
  
  class UTableView: public UView {
  public:
    static UViewStyle style;  // renderer
    virtual UViewStyle* getViewStyle() {return &style;}
    
    UTableView(UBox*, UView* parview, UHardwinImpl*);
    virtual ~UTableView();
    
    static UView* createView(UBox*, UView* parview, UHardwinImpl*);
    virtual UTableView* toTableView() {return this;}
    
#ifndef NO_DOC
    std::vector<UViewCell> cols, lines;
    int lcur, ccur; 
    int ccount, lcount;
    virtual bool doLayout(UUpdateContext&, class UViewLayout&);
    static void tableDoLayout(class UTableLayoutImpl&, UUpdateContext& ctx, 
                              UElem& grp, UViewLayout&);
    static void rowDoLayout(UView* row_view, class UTableLayoutImpl&, 
                            UUpdateContext& parctx, UElem& grp, UViewLayout&);
#endif
  };

}
#endif
