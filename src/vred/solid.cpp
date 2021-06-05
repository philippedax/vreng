#include "vred.hpp"
#include "solid.hpp"
#include "group.hpp"


Solid::Solid(const char* _name,
	     const Vect& _center, const Vect& _orient, const Vect& _size,
	     const int _renderStyle, const Color& _color, const Tex& _tex,
	     const App& _app):
  Item(_name), center(_center), orient(_orient), size(_size),
  modelView(Matrix(_center, _orient, _size)),
  renderStyle(_renderStyle), color(_color), father(NULL), tex(_tex), app(_app)
{
}

Vect Solid::getCenter() const
{
  return Vect(getAbsoluteModelView()[12],
	      getAbsoluteModelView()[13],
	      getAbsoluteModelView()[14]);
}

void Solid::setCenter(const Vect& center)
{
  modelView = Matrix(center, getOrient(), getSize());
  changeRef(father->getModelView());
  updateBB(0);
}

Vect Solid::getOrient() const
{
  Matrix m = getAbsoluteModelView();
  double sx = sqrt(Vect(m[0], m[1], m[2]).norm());
  double sy = sqrt(Vect(m[4], m[5], m[6]).norm());
  double cos = m[0] / sx, sin = m[1] / sy, alpha = 0;
  if (sin >= 0)
    alpha = ACOS(cos);
  else
    alpha = - ACOS(cos);
  return Vect(0.0, 0.0, alpha);
}

void Solid::setOrient(const Vect& orient)
{
  modelView = Matrix(getCenter(), orient, getSize());
  changeRef(father->getModelView());
  updateBB(0);
}

Vect Solid::getSize() const
{
  Matrix m = getAbsoluteModelView();
  return Vect(sqrt(Vect(m[0], m[1], m[2]).norm()),
	      sqrt(Vect(m[4], m[5], m[6]).norm()),
	      m[10]);
}

void Solid::setSize(const Vect& size)
{
  modelView = Matrix(getCenter(), getOrient(), size);
  changeRef(father->getModelView());
  updateBB(0);
}

void Solid::setStyle(const int _renderStyle)
{
  renderStyle = _renderStyle;
}

Group& Solid::getRoot()
{
  if (father == NULL)
    return *(Group*)this;
  else
    return father->getRoot();
}

int Solid::collide(Solid& moving) const
{ 
  if (!getBoundingSphere().collide(moving.getBoundingSphere())) return 0;
  if (!getBoundingBox().collide(moving.getBoundingBox())) return 0;
  return 1;
}

void Solid::Transform(const Vect& translation,
		      const Vect& rotation,
		      const Vect& scaling) 
{
  modelView *= Matrix(translation, rotation, scaling);
  updateBB(0);
}

Matrix Solid::getAbsoluteModelView() const
{
  if (father == NULL)
    return modelView;
  else
    return father->getAbsoluteModelView() * modelView;
}

void Solid::changeRef(const Matrix& m)
{
  modelView = m * modelView;
}




