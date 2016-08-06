#include "vred.hpp"
#include "ray.hpp"
#include "solid.hpp"
#include "raycaster.hpp"
#include "group.hpp"


Solid* RayCaster::GetSelection(
		const Vect& position, const Vect& watch, 
		GLdouble x, GLdouble y, 
		GLint x0, GLint y0, GLint width, GLint height)
{
  // first, we compute the projection of the 
  // camera position, and then an aim.
  GLdouble l_camera_x, l_camera_y, l_camera_z;
  GLdouble l_aim_z;
  GLint l_viewport[4];
  l_viewport[0] = x0;
  l_viewport[1] = y0;
  l_viewport[2] = width;
  l_viewport[3] = height;
  GLdouble l_modelviewMatrix[16];
  GLdouble l_projectionMatrix[16];

  glGetDoublev(GL_MODELVIEW_MATRIX, l_modelviewMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, l_projectionMatrix);
  gluProject(watch[0] + position[0], watch[1] + position[1], 
             watch[2] + position[2],
             l_modelviewMatrix,
             l_projectionMatrix,
             l_viewport,
             &l_camera_x, &l_camera_y, &l_aim_z);

#ifdef VERBOSE
  cout << "camera position " << position[0] << ", " <<
      position[1] << ", " <<
      position[2] << endl;
  cout << "screen camera position " << l_camera_x << ", " <<
      l_camera_y << ", " <<
      l_camera_z << endl;
  cout << "watching direction " << watch[0] << ", " <<
      watch[1] << ", " <<
      watch[2] << ", " << endl;
  cout << "aim Z : " << l_aim_z << endl;
#endif

  gluProject(position[0], position[1], position[2],
             l_modelviewMatrix,
             l_projectionMatrix,
             l_viewport,
             &l_camera_x, &l_camera_y, &l_camera_z);
  // we hope that aim is aligned with the screen-Z axis !!

  GLdouble l_Endx, l_Endy, l_Endz;
  GLdouble l_Startx, l_Starty, l_Startz;
  gluUnProject(x, y, l_aim_z, l_modelviewMatrix,
               l_projectionMatrix, 
               l_viewport, 
               &l_Endx, &l_Endy, &l_Endz);
  gluUnProject(l_camera_x, l_camera_y, l_camera_z, 
               l_modelviewMatrix, 
               l_projectionMatrix, 
               l_viewport,  
               &l_Startx, &l_Starty, &l_Startz);
  Vect l_Start(l_Startx, l_Starty, l_Startz);
  Vect l_Dir(l_Endx - l_Startx, l_Endy - l_Starty, l_Endz - l_Startz);
  Ray  cast(l_Start, l_Dir); 
  
#ifdef VERBOSE
  cout << "Starting Ray Vector " << 
    l_Start[0] << ", " << l_Start[1] << ", " << l_Start[2] << endl;
  cout << "Direction of Ray Vector " <<
    l_Dir[0] << ", " << l_Dir[1] << ", " << l_Dir[2] << endl;
#endif

  // we have the ray ...
  Solid *result;
  Vect *Nearest;
  GLdouble dist,tmpdist;
  Vect *Temp;

  // now detecting the object ...
  Nearest = 0;
  Temp = 0;
  dist = 0.0;
  tmpdist = 0.0;
  result = 0;
#ifdef VERBOSE
  cout << "TreeRoot is : " << Vred::treeRoot << endl;
#endif
  for (int i=0; i<Vred::treeRoot->GetCard(); ++i) {
    Temp = Vred::treeRoot->GetElement(i)->GetBoundingBox().RayIntersect(cast);
#ifdef VERBOSE
    cout << " Bounding sphere center : " << 
    Vred::treeRoot->GetElement(i)->GetBoundingSphere().Center()[0] << ", " <<
    Vred::treeRoot->GetElement(i)->GetBoundingSphere().Center()[1] << ", " <<
    Vred::treeRoot->GetElement(i)->GetBoundingSphere().Center()[2] << endl;
    cout << " Bounding Sphere radius : " << 
    Vred::treeRoot->GetElement(i)->GetBoundingSphere().Radius() << endl;
#endif

    if (Temp != 0) {
#ifdef VERBOSE
      cout << " non null intersection " << endl;
#endif
      tmpdist = (*Temp - l_Start) * (*Temp - l_Start);
      if (Nearest == 0) {
        result = Vred::treeRoot->GetElement(i);
        Nearest = Temp;
        dist = tmpdist;
      } else {
        if (tmpdist <= dist) {
          result = Vred::treeRoot->GetElement(i);
          delete Nearest;
          Nearest = Temp; 
          dist = tmpdist;
        } else {
          delete Temp;
        }
      }
    } else {
#ifdef VERBOSE
      cout << " NO intersection " << endl;
#endif
    }
  }
  // we have the nearest point, and so the approximately nearest object.
  return result;
}
