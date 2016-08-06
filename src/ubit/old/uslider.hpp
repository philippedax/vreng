/************************************************************************
 *
 *  uslider.hpp
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

#ifndef _uslider_hpp_
#define	_uslider_hpp_ 1
#include <ubit/unumber.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ustyle.hpp>
namespace ubit {
  
  struct USliderStyle : public UStyle {
    USliderStyle();
    const UStyle& getStyle(UUpdateContext* ctx) const;
    // default is hstyle
    UStyle vstyle;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Slider widget.
   * the value is a percentage in the range [0., 100.]
   *
   * Geometry:
   *  - horizontal orientation by default 
   *  - addAttr(UOrient::vertical) makes slider vertical
   *  - shortcut functions uhslider() and uvslider() specify the orientation
   *
   * Callbacks
   *  - UOn::action (default callback) is fired when the value is changed
   *  - UOn::change is fired continuously while the knob is dragged
   */
  class USlider: public UBox {
  public:
    UCLASS(USlider)
    
    USlider(const UArgs& = UArgs::none);
    /**< creates a new slider (@see also shortcut uslider(UArgs&)).
     * Default orientation is horizontal. new USlider(UOrient::vertical) creates
     * a vertical slider. The slider's value is in the range [0., 100.]
     */
    
    USlider(UFloat& value, const UArgs& = UArgs::none);
    /**< creates a new slider with a model value (@see also shortcut uslider(UFloat&, UArgs&)).
     * the slider is synchronized with this value. Several widgets (eg. sliders)
     * that share the same value are synchronized.
     * Default orientation is horizontal. new USlider(value, UOrient::vertical) creates
     * a vertical slider. The slider's value is in the range [0., 100.]
     */
  
    float getValue() const {return *pvalue;}
    ///< returns the current value (a float between 0 and 100).
    
    USlider& setValue(float val) {*pvalue = val; return *this;}
    /**< changes the current value (a float between 0 and 100).
     * UOn::change and UOn::action callbacks are fired when
     * the value is changed (see class USlider).
     */
    
    UFloat& value() {return *pvalue;}
    /**< return the internal value model.
     * UOn::change and UOn::action callbacks are fired when
     * the value is changed (see class USlider).
     * this object can be shared but can't be deleted.
     */
    
    // - - - impl.  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static UStyle* createStyle() {return createSliderStyle();}
    static USliderStyle* createSliderStyle() {return new USliderStyle;}
    
    virtual UBox* createKnob();
    ///< [impl] creates the knob; may be redefined by subclasses.
    
    virtual UBox* createRail();
    ///< [impl] creates the rail; may be redefined by subclasses.
    
  protected:
    uptr<UFloat> pvalue;        // corresponding value (percent)
    uptr<UBox> prail, pknob;
    uptr<UPos> pknob_pos;
    uptr<class UPosControl> pknob_ctrl;
    
    void constructs();
    virtual void gotoPosCB(UMouseEvent&);
    virtual void actionCB(UEvent&);  //UNodeEvent
    virtual void changeCB(UEvent&);  //UNodeEvent
  };
  
  
  USlider& uslider(const UArgs& a = UArgs::none);
  ///< shortcut that creates a new slider (that is horizontal by default).
  
  USlider& uslider(UFloat& value, const UArgs& a = UArgs::none);
  ///< shortcut that creates a new slider (that is horizontal by default) with a model value.
  
  USlider& uhslider(const UArgs& a = UArgs::none);
  ///< shortcut that creates a new horizontal slider.
  
  USlider& uvslider(const UArgs& a = UArgs::none);
  ///< shortcut that creates a new vertical slider.
  
}
#endif


