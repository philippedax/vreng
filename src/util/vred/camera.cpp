#include "vred.hpp"
#include "camera.hpp"


Camera::Camera(const char* _name, const Vect& position, const Vect& aim):
  Solid(_name, position, aim, Vect::unit, WIRED, Color::black) 
{
  myBoundingBox.Set(position,2.0,2.0,2.0,0.0);
  myBoundingSphere.Set(position,1.0);

  Look(position, aim);
}

void Camera::Look(const Vect& position, const Vect& aim) const
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated(aim[1], 1, 0, 0);
  glRotated(-aim[2] + 90, 0, 1, 0);
  glRotated(-90, 1, 0, 0);
  glTranslated(-position[0], -position[1], -position[2]);
}

void Camera::Move(const Vect& translation, const Vect& rotation)
{
// - run * COS(heading) * COS(pitch)
//   - strafe * SIN(heading)
//   - jetpack * SIN(pitch),
// + run * SIN(heading) - strafe * COS(heading),
// - run * SIN(pitch) - jetpack * COS(pitch)
  center += Vect(+ translation[0] * COS(orientation[2]) * COS(orientation[1])
		 - translation[1] * SIN(orientation[2])
		 + translation[2] * SIN(orientation[1]),

		 + translation[0] * SIN(orientation[2])
		 + translation[1] * COS(orientation[2]),

		 - translation[0] * SIN(orientation[1])
		 + translation[2] * COS(orientation[1]));
  orientation += rotation;
  Look(center, orientation);
}

Vect Camera::GetWatch() const
{
  // (cos(-pitch) * cos(heading), sin(heading), sin(-pitch) * cos(heading))
  Vect watch(COS(orientation[2]) * COS(orientation[1]),
	      SIN(orientation[2]),
	      - SIN(orientation[1]) * COS(orientation[2]));
  return (1.0 / sqrt(watch.Norm())) * watch;
}
