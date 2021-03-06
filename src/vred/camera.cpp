#include "vred.hpp"
#include "camera.hpp"


Camera::Camera(const char* _name, const Vect& position, const Vect& aim):
  Solid(_name, position, aim, Vect::unit, WIRED, Color::black) 
{
  myBoundingBox.set(position,2.0,2.0,2.0,0.0);
  myBoundingSphere.set(position,1.0);

  look(position, aim);
}

void Camera::look(const Vect& position, const Vect& aim) const
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated(aim[1], 1, 0, 0);
  glRotated(-aim[2] + 90, 0, 1, 0);
  glRotated(-90, 1, 0, 0);
  glTranslated(-position[0], -position[1], -position[2]);
}

void Camera::move(const Vect& translation, const Vect& rotation)
{
// - run * COS(heading) * COS(pitch)
//   - strafe * SIN(heading)
//   - jetpack * SIN(pitch),
// + run * SIN(heading) - strafe * COS(heading),
// - run * SIN(pitch) - jetpack * COS(pitch)
  center += Vect(+ translation[0] * COS(orient[2]) * COS(orient[1])
		 - translation[1] * SIN(orient[2])
		 + translation[2] * SIN(orient[1]),

		 + translation[0] * SIN(orient[2])
		 + translation[1] * COS(orient[2]),

		 - translation[0] * SIN(orient[1])
		 + translation[2] * COS(orient[1]));
  orient += rotation;
  look(center, orient);
}

Vect Camera::getWatch() const
{
  // (cos(-pitch) * cos(heading), sin(heading), sin(-pitch) * cos(heading))
  Vect watch(COS(orient[2]) * COS(orient[1]), SIN(orient[2]), - SIN(orient[1]) * COS(orient[2]));
  return (1.0 / sqrt(watch.norm())) * watch;
}
