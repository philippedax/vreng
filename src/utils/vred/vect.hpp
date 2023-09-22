#ifndef VECT_HPP
#define VECT_HPP

class Vect {

public:
  Vect(const float x = 0, const float y = 0, const float z = 0);

  float operator[](const int n) const;
  float& operator[](const int n);
  Vect operator+(const Vect&) const;
  Vect& operator+=(const Vect&);
  Vect operator-(const Vect&) const;
  Vect& operator-=(const Vect& v);
  float operator*(const Vect& v) const;
  Vect operator^(const Vect& v) const;
  friend Vect operator*(const float d, const Vect& v);
  Vect& operator*=(const float d);
  bool operator==(const Vect&) const;

  float norm() const {return (x * x + y * y + z * z);}
  void print() const;

  const static Vect null, unit;

private:
  float  x, y, z;

};

#endif
