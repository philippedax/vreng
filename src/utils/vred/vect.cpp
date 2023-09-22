#include "vred.hpp"
#include "vect.hpp"


Vect::Vect(const float x, const float y, const float z):
  x(x), y(y), z(z) {
}

float Vect::operator[](const int n) const {
  switch (n) {
  case 0:
    return x;
  case 1:
    return y;
  default:
    return z; // TODO: if (n >= 3) raise Exception;
  }
}

float& Vect::operator[](const int n) {
  switch (n) {
  case 0:
    return x;
  case 1:
    return y;
  default:
    return z; // TODO: if (n >= 3) raise Exception;
  }
}

Vect Vect::operator+(const Vect& v) const {
  return Vect(x + v.x, y + v.y, z + v.z);
}

Vect& Vect::operator+=(const Vect& v) {
  x += v.x; y += v.y; z += v.z;
  return *this;
}

Vect Vect::operator-(const Vect& v) const {
  return Vect(x - v.x, y - v.y, z - v.z);
}

Vect& Vect::operator-=(const Vect& v) {
  x -= v.x; y -= v.y; z -= v.z;
  return *this;
}

float Vect::operator*(const Vect& v) const {
  return (x * v.x + y * v.y + z * v.z);
}

Vect Vect::operator^(const Vect& v) const {
  return Vect(y * v.z -z * v.y, z * v.x - x * v.z, x * v.y - x * v.y);
}

void Vect::print() const {
  cout << "(" << x << ", " << y << ", " << z << ")" << endl;
}

Vect operator*(const float d, const Vect& v) {
  return Vect(d * v.x, d * v.y, d * v.z);
}    

Vect& Vect::operator*=(float d) {
  x *= d; y *= d; z *= d;
  return *this;
}

bool Vect::operator==(const Vect& v) const {
  return ((x == v.x) && (y == v.y) && (z == v.z));
}

const Vect Vect::null(0.0, 0.0, 0.0);
const Vect Vect::unit(1.0, 1.0, 1.0);
