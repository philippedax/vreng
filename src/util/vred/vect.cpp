#include "vred.hpp"
#include "vect.hpp"


Vect::Vect(const double x, const double y, const double z):
  x(x), y(y), z(z) {
}

double Vect::operator[](const int n) const {
  switch (n) {
  case 0:
    return x;
  case 1:
    return y;
  default:
    return z; // TODO: if (n >= 3) raise Exception;
  }
}

double& Vect::operator[](const int n) {
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

double Vect::operator*(const Vect& v) const {
  return (x * v.x + y * v.y + z * v.z);
}

Vect Vect::operator^(const Vect& v) const {
  return Vect(y * v.z -z * v.y, z * v.x - x * v.z, x * v.y - x * v.y);
}

void Vect::Print() const {
  cout << "(" << x << ", " << y << ", " << z << ")" << endl;
}

Vect operator*(const double d, const Vect& v) {
  return Vect(d * v.x, d * v.y, d * v.z);
}    

Vect& Vect::operator*=(double d) {
  x *= d; y *= d; z *= d;
  return *this;
}

bool Vect::operator==(const Vect& v) const {
  return ((x == v.x) && (y == v.y) && (z == v.z));
}

const Vect Vect::null(0.0, 0.0, 0.0);
const Vect Vect::unit(1.0, 1.0, 1.0);
