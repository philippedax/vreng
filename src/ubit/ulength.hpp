/************************************************************************
 *
 *  ulength.hpp: lengths and units
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

#ifndef _ulength_hpp_
#define	_ulength_hpp_ 1
#include <ubit/uobject.hpp>
namespace ubit {
  
  class UUnit {
  public:
    enum Type {PX, EM, EX, IN, CM, MM, PT, PC, PERCENT, PERCENT_CTR, AUTO, IGNORE};
    
    UUnit(const UUnit& u) : type(u.type) {}
    explicit UUnit(UUnit::Type t) : type(t) {}
    
    UUnit& operator=(const UUnit& u) {type = u.type; return *this;}
    UUnit& operator=(UUnit::Type t) {type = t; return *this;}

    UUnit::Type getType() const {return (UUnit::Type)type;}

    bool operator==(UUnit u) const {return u.type == type;}
    bool operator==(UUnit::Type t) const {return t == type;}

    bool operator!=(UUnit u) const {return u.type != type;}
    bool operator!=(UUnit::Type t) const {return t != type;}
    
    unsigned char type;
  };
  
  extern const UUnit 
  UPX,  ///< Pixel unit.
  UEM,  ///< EM unit (1em is equal to the current font size).
  UEX,  ///< EX unit (1ex is the x-height of a font, which is about half the font-size)
  UIN,  ///< Inch unit.
  UCM,  ///< Centimeter unit.
  UMM,  ///< Millimeter unit.
  UPT,  ///< Point unit (1pt is the same as 1/72 inch).
  UPC,  ///< Pica unit (1pc is the same as 12pt).
  UPERCENT,     ///< percentage of the parent size in range [0f,100f].
  UPERCENT_CTR; ///< same as UPERCENT but coordinates correspond to the widget center.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Length of positions (UPos), sizes (USize) and padding (UPadding) 
   */
  class ULength {
  public:
    struct Modes {
      enum {NONE=0};
      unsigned char val;
      explicit Modes(unsigned char m) : val(m) {}
    };
         
    ULength(float value = 0.0) : val(value), unit(UUnit::PX), modes(0) {}
    ///< creates a new length with the specified value, unit = PX and modes = 0.
    
    ULength(float value, UUnit u) : val(value), unit(u), modes(0) {}
    ///< creates a new length with the specified value and unit (modes = 0).
    
    ULength(float value, UUnit u, Modes m) : val(value), unit(u), modes(m) {}
    ///< creates a new length with the specified value, unit and modes.
    
    ULength(const UStr& text_spec); 
    ///< creates a new length with a value and a unit taken from a textual specification (modes = NONE).

    bool operator==(const ULength&) const;
    bool operator!=(const ULength&) const;
    
    operator float() const {return val;}
    
    float getValue() const {return val;}
    UUnit getUnit()  const {return unit;}
    Modes getModes() const {return modes;}
    
    ULength& setValue(float value) {val = value; return *this;}
    ULength& setUnit(UUnit u) {unit = u; return *this;}
    ULength& setUnit(const char* unit);
    ULength& setModes(Modes m) {modes = m; return *this;}
    
    ULength& set(const UStr& text_spec); 
    ///< changes the value and the unit from a textual specification (modes are set to NONE).
        
    UStr toString() const;
    
    float toPixels(UDisp*, const UFontDesc&, float view_len, float parview_len) const;
    
    friend std::ostream& operator<<(std::ostream& s, const ULength& l);
    friend ULength operator|(float len, UUnit u);
    friend ULength operator|(ULength len, UUnit u);
    friend ULength operator|(ULength len, ULength::Modes m);
    
    float val;
    UUnit unit;
    Modes modes;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  std::ostream& operator<<(std::ostream&, const ULength&);
  ///< prints out this ULength on an output stream.
  
  inline ULength operator|(float len, UUnit u) {return ULength(len, u);}
  /**< | makes its possible to specify the Unit of a Length in simple way (see exemples).
   * Exemples: 
   * - 50|UPX means a length of 50 pixels
   * - 50|UPERCENT means a length of 50% of the parent
   * Impl notes:
   * operator|(float, enum) wouldn't work properly because C++ silently converts enums to ints
   */
  
  inline ULength operator|(ULength len, UUnit u) {return ULength(len).setUnit(u);}
  
  inline ULength operator|(ULength len, ULength::Modes m) {return ULength(len).setModes(m);}
  
  extern const ULength
  UIGNORE,    ///< ignore this specification.
  UAUTO;      ///< let Ubit compute this value.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  struct UPosSpec {    
    UPosSpec(ULength _x, ULength _y) : x(_x), y(_y) {}
    ULength x, y;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  struct USizeSpec {
    USizeSpec(ULength w, ULength h) : width(w), height(h) {}
    ULength width, height;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Box padding.
   */
  struct UPaddingSpec {
    UPaddingSpec(ULength horiz, ULength vert) : top(vert), right(horiz), bottom(vert), left(horiz) {}
    // attention: doit etre refenie si cette classe est heritee
    void set(ULength horiz, ULength vert) {left = right = horiz; top  = bottom = vert;}
    void add(const UPaddingSpec&);
    ULength top, right, bottom, left;
  };
  
}
#endif

