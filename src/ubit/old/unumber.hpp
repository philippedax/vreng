/************************************************************************
 *
 *  unumber.hpp
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

#ifndef _unumber_hpp_
#define	_unumber_hpp_ 1
#include <iostream>
#include <ubit/unode.hpp>
namespace ubit {
 
// !!! should not inherit from UNode

/** Active Number base class.
  */
class UNumber: public UNode {
public:
  //UCLASS("#number", UNumber, null)
  UABSTRACT_CLASS(UNumber)
    
  virtual int    intValue()    const = 0;
  virtual float  floatValue()  const = 0;
  virtual double doubleValue() const = 0;
  virtual UStr   toString()    const = 0;

  virtual UNumber& onChange(UCall&);
  ///< adds callbacks that are activated when the objects' value changes.

  virtual void changed(bool = true);
  ///< called when object's content is changed.
};
  
  
/* ==================================================== ===== ======= */
/** Active Integer
*/
class UInt : public UNumber {
public:
  UCLASS(UInt)
  
  UInt(int v = 0) : value(v) {}
  UInt(const UInt& v) : value(v.value) {}
  UInt(const UStr&);
  UInt(const std::string&);

  operator int() const {return value;}
  ///< type conversion: converts UInt to int.

  friend std::istream& operator>>(std::istream&, UInt&);
  ///< prints value on output stream.

  int    intValue()    const {return value;}
  float  floatValue()  const {return float(value);}
  double doubleValue() const {return double(value);}
  UStr   toString()    const;
  //static int parseInt(String s);
  //static int parseInt(String s, int radix);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UInt& operator=(int v)          {return setImpl(v);}
  UInt& operator=(const UInt& v)  {return setImpl(v.value);}

  UInt& operator=(const char* s)  {return setImpl(s);}
  UInt& operator=(const UStr& s);
  UInt& operator=(const std::string& s);

  friend std::ostream& operator<<(std::ostream&, const UInt&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(int v)         const {return value == v;}
  bool operator==(double v)      const {return value == v;}
  bool operator==(const UInt& v) const {return value == v.value;}

  bool operator!=(int v)         const {return value != v;}
  bool operator!=(double v)      const {return value != v;}
  bool operator!=(const UInt& v) const {return value != v.value;}

  bool operator<(int v)          const {return value < v;}
  bool operator<(double v)       const {return value < v;}
  bool operator<(const UInt& v)  const {return value < v.value;}

  bool operator<=(int v)         const {return value <= v;}
  bool operator<=(double v)      const {return value <= v;}
  bool operator<=(const UInt& v) const {return value <= v.value;}

  bool operator>(int v)          const {return value > v;}
  bool operator>(double v)       const {return value > v;}
  bool operator>(const UInt& v)  const {return value > v.value;}

  bool operator>=(int v)         const {return value >= v;}
  bool operator>=(double v)      const {return value >= v;}
  bool operator>=(const UInt& v) const {return value >= v.value;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UInt& operator++() {return setImpl(value+1);}
  UInt  operator++(int);
  UInt& operator--() {return setImpl(value-1);}
  UInt  operator--(int);

  UInt& operator+=(int v) {return setImpl(value+v);}
  UInt& operator+=(const UInt& v) {return setImpl(value+v.value);}
  UInt& operator-=(int v) {return setImpl(value-v);}
  UInt& operator-=(const UInt& v) {return setImpl(value-v.value);}
  UInt& operator*=(int v) {return setImpl(value*v);}
  UInt& operator*=(const UInt& v) {return setImpl(value*v.value);}
  UInt& operator/=(int v) {return setImpl(value/v);}
  UInt& operator/=(const UInt& v) {return setImpl(value/v.value);}
  
  // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual UInt& setImpl(int, bool call_callbacks = true);
  virtual UInt& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.
  
private:
  friend class UFloat;
  friend class UDouble;
  int value;
};

  
/* ==================================================== ===== ======= */
/** Active Float.
*/
class UFloat : public UNumber {
public:
  UCLASS(UFloat)
  
  UFloat(float v = 0.0)     : value(v) {}
  UFloat(const UFloat& v)   : value(v.value) {}
  UFloat(const UInt& v) : value(v.value) {}
  //UFloat(const char*);  ambigu
  UFloat(const UStr&);
  UFloat(const std::string&);

  operator float() const {return value;}
  ///< type conversion: converts UFloat to float.

  friend std::ostream& operator<<(std::ostream&, const UFloat&);
  ///< prints value on output stream.

  int    intValue()    const {return int(value);}
  float  floatValue()  const {return value;}
  double doubleValue() const {return double(value);}
  UStr   toString()    const;
  
   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UFloat& operator=(float v)           {return setImpl(v);}
  UFloat& operator=(const UFloat& v)   {return setImpl(v.value);}
  UFloat& operator=(const UInt& v) {return setImpl(v.value);}
  UFloat& operator=(const char* s)     {return setImpl(s);}
  UFloat& operator=(const UStr& s);
  UFloat& operator=(const std::string& s);

  friend std::istream& operator>>(std::istream&, UFloat&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(float v)          const {return value == v;}
  bool operator==(double v)         const {return value == v;}
  bool operator==(const UFloat& v)  const {return value == v.value;}

  bool operator!=(float v)          const {return value != v;}
  bool operator!=(double v)         const {return value != v;}
  bool operator!=(const UFloat& v)  const {return value != v.value;}

  bool operator<(float v)           const {return value < v;}
  bool operator<(double v)          const {return value < v;}
  bool operator<(const UFloat& v)   const {return value < v.value;}

  bool operator<=(float v)          const {return value <= v;}
  bool operator<=(double v)         const {return value <= v;}
  bool operator<=(const UFloat& v)  const {return value <= v.value;}

  bool operator>(float v)           const {return value > v;}
  bool operator>(double v)          const {return value > v;}
  bool operator>(const UFloat& v)   const {return value > v.value;}

  bool operator>=(float v)          const {return value >= v;}
  bool operator>=(double v)         const {return value >= v;}
  bool operator>=(const UFloat& v)  const {return value >= v.value;}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UFloat& operator++() {return setImpl(value+1);}
  UFloat  operator++(int);
  UFloat& operator--() {return setImpl(value-1);}
  UFloat  operator--(int);

  UFloat& operator+=(int v) {return setImpl(value+v);}
  UFloat& operator+=(const UFloat& v) {return setImpl(value+v.value);}
  UFloat& operator-=(int v) {return setImpl(value-v);}
  UFloat& operator-=(const UFloat& v) {return setImpl(value-v.value);}
  UFloat& operator*=(int v) {return setImpl(value*v);}
  UFloat& operator*=(const UFloat& v) {return setImpl(value*v.value);}
  UFloat& operator/=(int v) {return setImpl(value/v);}
  UFloat& operator/=(const UFloat& v) {return setImpl(value/v.value);}

  // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual UFloat& setImpl(float, bool call_callbacks = true);
  virtual UFloat& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.
  
private:
  friend class UInt;
  friend class UDouble;
  float value;
};

  
/* ==================================================== ===== ======= */
/** Active Double.
*/
class UDouble : public UNumber {
public:
  UCLASS(UDouble)

  UDouble(double v = 0.0) : value(v) {}
  UDouble(const UDouble& v) : value(v.value) {}
  UDouble(const UFloat& v) : value(v.value) {}
  UDouble(const UInt& v) : value(v.value) {}
  UDouble(const UStr&);
  UDouble(const std::string&);

  operator double() const {return value;}
  ///< type conversion: converts UDouble to double.

  friend std::ostream& operator<<(std::ostream&, const UDouble&);
  ///< prints value on output stream.

  int    intValue()    const {return int(value);}
  float  floatValue()  const {return float(value);}
  double doubleValue() const {return value;}
  UStr   toString()    const;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UDouble& operator=(double v)           {return setImpl(v);}
  UDouble& operator=(const UDouble& v)   {return setImpl(v.value);}
  UDouble& operator=(const UFloat& v)    {return setImpl(v.value);}
  UDouble& operator=(const UInt& v)  {return setImpl(v.value);}

  UDouble& operator=(const char* s)      {return setImpl(s);}
  UDouble& operator=(const UStr& s);
  UDouble& operator=(const std::string& s);

  friend std::istream& operator>>(std::istream&, UDouble&);
  ///< reads value from input stream.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool operator==(float v)          const {return value == v;}
  bool operator==(double v)         const {return value == v;}
  bool operator==(const UDouble& v) const {return value == v.value;}

  bool operator!=(float v)          const {return value != v;}
  bool operator!=(double v)         const {return value != v;}
  bool operator!=(const UDouble& v) const {return value != v.value;}

  bool operator<(float v)           const {return value < v;}
  bool operator<(double v)          const {return value < v;}
  bool operator<(const UDouble& v)  const {return value < v.value;}

  bool operator<=(float v)          const {return value <= v;}
  bool operator<=(double v)         const {return value <= v;}
  bool operator<=(const UDouble& v) const {return value <= v.value;}

  bool operator>(float v)           const {return value > v;}
  bool operator>(double v)          const {return value > v;}
  bool operator>(const UDouble& v)  const {return value > v.value;}

  bool operator>=(float v)          const {return value >= v;}
  bool operator>=(double v)         const {return value >= v;}
  bool operator>=(const UDouble& v) const {return value >= v.value;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UDouble& operator++() {return setImpl(value+1);}
  UDouble  operator++(int);
  UDouble& operator--() {return setImpl(value-1);}
  UDouble  operator--(int);

  UDouble& operator+=(int v) {return setImpl(value+v);}
  UDouble& operator+=(const UDouble& v) {return setImpl(value+v.value);}
  UDouble& operator-=(int v) {return setImpl(value-v);}
  UDouble& operator-=(const UDouble& v) {return setImpl(value-v.value);}
  UDouble& operator*=(int v) {return setImpl(value*v);}
  UDouble& operator*=(const UDouble& v) {return setImpl(value*v.value);}
  UDouble& operator/=(int v) {return setImpl(value/v);}
  UDouble& operator/=(const UDouble& v) {return setImpl(value/v.value);}

  // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual UDouble& setImpl(double, bool call_callbacks = true);
  virtual UDouble& setImpl(const char*, bool call_callbacks = true);
  ///< [impl] assignment.

private:
  friend class UInt;
  friend class UFloat;
  double value;
};

}
#endif

