#include "vred.hpp"
#include "solid.hpp"
#include "group.hpp"


Solid::Solid(char* _name,
	     const Vect& _center, const Vect& _orientation, const Vect& _size,
	     const int _renderStyle, const Color& _color, const Tex& _tex,
	     const App& _app):
  Item(_name), center(_center), orientation(_orientation), size(_size),
  modelView(Matrix(_center, _orientation, _size)),
  renderStyle(_renderStyle), color(_color), father(NULL), tex(_tex), app(_app) {
}

Vect Solid::GetCenter() const {
  return Vect(GetAbsoluteModelView()[12],
	      GetAbsoluteModelView()[13],
	      GetAbsoluteModelView()[14]);
}

void Solid::SetCenter(const Vect& center) {
  modelView = Matrix(center, GetOrientation(), GetSize());
  ChangeRef(father->GetModelView());
  UpdateBoundingVolumes(0);
}

Vect Solid::GetOrientation() const {
  Matrix m = GetAbsoluteModelView();
  double sx = sqrt(Vect(m[0], m[1], m[2]).Norm());
  double sy = sqrt(Vect(m[4], m[5], m[6]).Norm());
  double cos = m[0] / sx, sin = m[1] / sy, alpha = 0;
  if (sin >= 0)
    alpha = ACOS(cos);
  else
    alpha = - ACOS(cos);
  return Vect(0.0, 0.0, alpha);
}

void Solid::SetOrientation(const Vect& orientation) {
  modelView = Matrix(GetCenter(), orientation, GetSize());
  ChangeRef(father->GetModelView());
  UpdateBoundingVolumes(0);
}

Vect Solid::GetSize() const {
  Matrix m = GetAbsoluteModelView();
  return Vect(sqrt(Vect(m[0], m[1], m[2]).Norm()),
	      sqrt(Vect(m[4], m[5], m[6]).Norm()),
	      m[10]);
}

void Solid::SetSize(const Vect& size) {
  modelView = Matrix(GetCenter(), GetOrientation(), size);
  ChangeRef(father->GetModelView());
  UpdateBoundingVolumes(0);
}

void Solid::SetRenderStyle(const int _renderStyle) {
  renderStyle = _renderStyle;
}

Group& Solid::GetRoot() {
  if (father == NULL)
    return *(Group*)this;
  else
    return father->GetRoot();
}

int Solid::Collide(Solid& moving) const
{ 
  if (!GetBoundingSphere().Collide(moving.GetBoundingSphere())) return 0;
  if (!GetBoundingBox().Collide(moving.GetBoundingBox())) return 0;
  return 1;
}

void Solid::Transform(const Vect& translation,
		      const Vect& rotation,
		      const Vect& scaling) 
{
  modelView *= Matrix(translation, rotation, scaling);
  UpdateBoundingVolumes(0);
}

Matrix Solid::GetAbsoluteModelView() const {
  if (father == NULL)
    return modelView;
  else
    return father->GetAbsoluteModelView() * modelView;
}

void Solid::ChangeRef(const Matrix& m) {
  modelView = m * modelView;
}




