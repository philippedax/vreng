#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

class Solid;
class Group;

class RayCaster {

public:
  RayCaster() {}
  virtual ~RayCaster() {}

  static Solid* getSelection(
                            const Vect& position, const Vect& watch, 
    	                    GLdouble x, GLdouble y, 
    	                    GLint x0, GLint y0,
                            GLint width, GLint height
                            );
  
};

#endif
