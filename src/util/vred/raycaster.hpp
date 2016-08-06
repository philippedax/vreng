#ifndef RAYCASTER_H
#define RAYCASTER_H

class Solid;
class Group;

class RayCaster {

public:
  RayCaster() {}
  virtual ~RayCaster() {}

  static Solid* GetSelection(
                            const Vect& position, const Vect& watch, 
    	                    GLdouble x, GLdouble y, 
    	                    GLint x0, GLint y0,
                            GLint width, GLint height);
  
};

#endif
