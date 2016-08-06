#include "vred.hpp"
#include "color.hpp"


Color::Color(const GLfloat* _color) {
  for (int i = 0; i < 4; ++i)
    color[i] = _color[i];
}

Color::Color(const float r, const float g, const float b, const float a) {
  color[0] = r; color[1] = g; color[2] = b; color[3] = a;
}

void Color::Print() const {
  cout << "(" << color[0] << ", " << color[1] << ", "
       << color[2] << ", " << color[3] << ")" << endl;
}

bool Color::operator==(const Color& c) const {
  return ((color[0] == c.color[0]) 
	  && (color[1] == c.color[1])
	  && (color[2] == c.color[2])
	  && (color[3] == c.color[3]));
}

const Color Color::black(0.0, 0.0, 0.0, 1.0);
const Color Color::white(1.0, 1.0, 1.0, 1.0);
const Color Color::red(1.0, 0.0, 0.0, 1.0);
const Color Color::green(0.0, 1.0, 0.0, 1.0);
const Color Color::blue(0.0, 0.0, 1.0, 1.0);
