//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
// bvh.hpp
//---------------------------------------------------------------------------
#ifndef BVH_HPP
#define BVH_HPP

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

#define ROTATE_SLOWNESS 8

class fileNotFound {};
class ChannelError;

/// Three floats in a array, lots of overloaded operators
class vector3f {
 public:
  vector3f() { memset(vertex, 0, sizeof(float[3])); }
  vector3f(float x, float y, float z);
  virtual ~vector3f() {}

  float vertex[3];

  void reset();
  float length();
  float length2();

  vector3f& operator= (const vector3f &v1);

  friend bool operator== (const vector3f &v1, const vector3f &v2);
  friend bool operator<= (const vector3f &v1, const vector3f &v2);
  friend bool operator>= (const vector3f &v1, const vector3f &v2);
  friend bool operator< (const vector3f &v1, const vector3f &v2);
  friend bool operator> (const vector3f &v1, const vector3f &v2);
  friend vector3f operator+ (const vector3f &v1, const vector3f &v2);
  friend vector3f operator+ (const vector3f &v1, const float scalar);
  friend vector3f operator- (const vector3f &v1, const vector3f &v2);
  friend vector3f operator- (const vector3f &v1, const float scalar);
  friend vector3f operator* (const vector3f &v1, const vector3f &v2);
  friend vector3f operator* (const vector3f &v1, const float scalar);
  friend vector3f operator/ (const vector3f &v1, const vector3f &v2);
  friend vector3f operator/ (const vector3f &v1, const float scalar);

  vector3f abs();

  friend float dotProduct(const vector3f &v1, const vector3f &v2);
  friend vector3f crossProduct(const vector3f &v1, const vector3f &v2);

  void set(float x, float y, float z);
};

/// Array of 16 floats in OpenGL conformant style 
/**
  Plenty of useful  overloaded operators,
  including those for interacting with the matrix9f
  and vector3f classes.
 */
class matrix16f {
 public:
  float matrix[16];

  void reset();
  void identity();

  void print(); 

  void translate(float x, float y, float z);
  void scale(float x, float y, float z);

  void RotateX(int deg);
  void RotateY(int deg);
  void RotateZ(int deg);

  void RotateX(float rad);
  void RotateY(float rad);
  void RotateZ(float rad);

  void RotateXYZ(float x, float y, float z);
  void Rotate(matrix16f m1);
  
  void set(vector3f right, vector3f up, vector3f out);
  
  //void eq(float m[16]);
  matrix16f& operator= (const matrix16f &m1);

  vector3f transform(vector3f point);
  matrix16f FlipOrientation();

  void normalize();

  friend matrix16f operator+ (const matrix16f &m1, const matrix16f &m2);
  friend matrix16f operator+ (const matrix16f &m1, const vector3f  &v2);
  friend matrix16f operator- (const matrix16f &m1, const matrix16f &m2);
  friend matrix16f operator* (const matrix16f &m1, const float scalar);
  friend matrix16f operator* (const matrix16f &m1, const matrix16f &m2);
  friend matrix16f operator/ (const matrix16f &m1, const float scalar);
  friend bool operator== (const matrix16f &m1, const matrix16f &m2);

  matrix16f() { memset(matrix, 0, sizeof(float[16])); }
};

/// Smaller matrix for storing orientation but no location information.
/**
  The contents are puhttp://sdldoc.csn.ul.ie/sdlkey.phpblic for now.
  */
class matrix9f {
 public:
  float matrix[9];

  matrix9f() { memset(matrix, 0, sizeof(float[9])); }

  void reset();
  void identity();
  
  void RotateX(int deg);
  void RotateY(int deg);
  void RotateZ(int deg);

  matrix9f& operator= (const matrix9f &m1);

  matrix9f transpose();
  matrix9f inverse();

  friend matrix9f Star(vector3f a);
  friend matrix9f operator+ (const matrix9f &m1, const matrix9f &m2);
  friend matrix16f operator+ (const matrix16f &m1, const matrix9f &m2);
  friend matrix9f operator- (const matrix9f &m1, const matrix9f &m2);
  friend matrix9f operator* (const matrix9f &m1, const float scalar);
  friend matrix9f operator* (const matrix9f &m1, const matrix9f &m2);
  friend vector3f operator* (const matrix9f &m1, const vector3f &v2);
  friend matrix9f operator* (const matrix9f &m1, const matrix16f &m2);
  friend matrix9f operator* (const matrix16f &m1, const matrix9f &m2);
  friend matrix9f operator/ (const matrix9f &m1, const float scalar);
  friend bool operator== (const matrix9f &m1, const matrix9f &m2);
};

float det(float f0, float f1, float f2, float f3);

//float dotProduct(vector3f &v1, vector3f &v2);
vector3f crossProduct(const vector3f &v1, const vector3f &v2);

typedef struct _triangleV {
  vector3f *vertNorm[3];
  vector3f *vertice[3];  
  vector3f norm;      // triangle norm
} triangleV;

//notcalled vector3f findNorm(int index, int numSurfTriangles, vector3f **surfTriangles);


/// Mostly virtual class for any entity in the scene
/**
  It holds a lot of physical information that would be
  better off in a subclass where it would make sense, as
  lights and camera don't really need such properties.
  */
class movable {
 public:
  movable();
  movable(string name);
  virtual ~movable() {}

  void init();

  movable& operator= (const movable& other);

  void update() {}

  void move(int pitch, int turn, int roll, float x, float y, float z);
  void setName(string name);

  /// physical constants
  float mass;
  /// moment of inertia tensor
  matrix9f Ibody;
  /// inverse of moi tensor
  matrix9f IbodyInv; 
  /// state variable
  matrix16f location,    // Rotation + location
        newLocation;
  /// momentum
#if 0 //notused
  vector3f P,newP, 
  /// angular momentum
       L,newL; 
  // derived
  matrix9f Iinv;
  /// velocity of center of mass
  vector3f velocity,
  /// w angular velocity
       omega;    
  /// summed forces per timestep 
  vector3f force,
  /// summed torque per timestep
       torque;

  bool normalize;

  float step;
#endif

  string name;
  
  //bounding box
  void getBoundingBox() {}
  void getAABB();
  void drawAABB();
  void drawBoundingBox();
  bool drawBB;

  // has this been completely tested for collisions?
  //notused bool BBtested;
  bool BBcollided;

  vector3f centerBB;
  vector3f edgesBB;
  vector3f boundingBox[8];
  vector3f oldCenterAABB;
  vector3f centerAABB;
  vector3f edgesAABB;
  vector3f AABB[8];
  
  // is the object moved normally or is
  // it subject to the physics engine?
  //notused bool physical;
};


class triangleInd {
 public:
  triangleInd() {}
  virtual ~triangleInd() {}

  int vertIndices[3];   
  vector3f normal;
  bool collision;  // temp visual debugging member
  
  triangleInd& operator= (const triangleInd &tri);
};


/// Hold location, orientation, and mode of the viewer
/**
  Also some experimental modes of movement 
  like following a given object, and following that
  objects every rotation, same with a delay, or just tracking
  the object without rotating with it.
  */
class camera: public movable {
 public:
  enum mode { FREE, FREEORIENTED, CENTERED };

  camera();
  
  // prototype in movable prevents lookMode= from working
  camera(string name, mode lookMode = FREE);
  virtual ~camera() {}

  void init();
  void move(int pitch,int turn, int roll,float x, float y, float z);
  void look();

  float radius;

  //matrix16f location;

  /// the most recent center locations, the camera follows the last.
  matrix16f delay[20];
  /// extra matrix needed for follow mode
  matrix16f other;
  /// Location for camera to spin around
  matrix16f *center;
  
  vector3f centerPoint;
  vector3f angles;
  
  mode lookMode;
};


/// Simple light point source
/**
  Orientation is also stored even though a rotated point source
  is indistuinguishable from the non-rotated.
  There's also a visible .obj shell around the light in order to 
  make placement possible (rather than guessing the lights position
  by looking at it's effect on normal objects).
  There's no diminishment with distance, that would be interesting to
  implement and though I can think of at least one way to do it 
  (objects would be individually lit with a distance scaling factor,
  and perhaps the distance to the light would also have the object
  be split up for lighting in order to show a differential across
  it's surface), I'm curious what the conventional way is.
 */
class light: public movable {
 public:
  light() {}
  virtual ~light();
  light(camera *viewer, int number, float maxFade = 0, float minFade = 0, float scale = 1.0f);

  void draw();
  void drawDim(vector3f distant);
  void getBoundingBox();

  // Keep track of where the camera is so bitmap can be oriented properly.
  camera *viewer;  

  /// Distance at which the light is completely dim
  /** maxFade = 0 means no linear scaling -> uniform at any difference */
  float maxFade;
  /// Minimum istance at which the light maximally bright
  float minFade;
  /// How to scale the brightness- >1.0 will be clamped, so bright lights will wash out
  float scale;
  
  //objloader *lightobj;

  /** Currently lights aren't directional, so only keep a vector of
     position and orientation doesn't matter.
    vector3f *position;
  */
  GLfloat Ka[4]; // Ambient 
  GLfloat Kd[4]; // Diffuse
  GLfloat Ks[4]; // Specular

  /// From 0-7 (or how ever many lights can be handled at once).
  int GL_LIGHTX;
  bool verbose;
};

/**
  UVs and textures aren't supported, as I'm not really interested
  in them, being literally superficial in addition to me not having
  any easy-to-use UV mappers (lithunwrap is a pain).
 */
class material {
 public:
  material() {}
  virtual ~material() {}

  material& operator= (const material &mat);

  /// material name defined in the parsed .obj files
  string name;
  //int numFaces;
  //int numUVs;
  //uv     **uvs;

  vector<triangleInd> faces;

  // reflectances
  GLfloat Se;    ///< shininess exponent 0-128
  GLfloat Ka[4]; ///< Ambient
  GLfloat Kd[4]; ///< Diffuse  
  GLfloat Ks[4]; ///< Specular
  GLfloat Ke[4]; ///< Emission
  GLfloat opacity;
};


/// Parse in .obj's 

//dax typedef struct uv { float uorv[2]; };

/**
  I could do more parsers for other formats but .objs fit all my
  needs for now (I'll need my own format most likely).
  Perhaps I should further separate obj loading from the physics operations
  for finding the interior points and so forth.
  Probably this shouldn't be a movable subclass anymore.
 */
class objloader: public movable {

  enum objMode {NONE,MTLLIB,VERTEX,NORMAL,FACE};
  enum mtlMode {NONEM,NEWMTL,NS,D,ILLUM,KD,KA,KS};

 public:
  objloader(string objFile);
  virtual ~objloader();

  /// Load .obj file
  bool load(string objFile);
  /// Parse each line of .obj file 
  void process(string line);
  /// Parse .mtl file specified in .obj file
  void loadMtl(string mtlFile);
  /// Parse each line of .mtl file into material objects
  void processMtl(string line, material *mtl);
  /// Match specified material to loaded materials
  bool matchMtl(uint32_t &index, string name);
  /// Change or initialize total mass
  void setMass(float newMass);

  void draw();
  /// Does nothing currently, put scripted movements here
  void update(float seconds);

  objloader& operator= (const objloader &obj1);

  void getBoundingBox();

  bool success;
  int counter; ///< For scripted movements, unused currently

 private:
  ///loading variables
  string subdir;
  string mtlFile;
  uint32_t mtlIndex;
  triangleInd tempTriangle;
  //notused uint32_t kIndex;
  objMode theMode;
  //notused mtlMode theMtlMode;
  vector3f tempVector;
  uint32_t tempVectorIndex;
  bool verbose;

 public:
  std::vector<vector3f> vertices;
  std::vector<vector3f> normals;
  std::vector<material*> mtls;
  
  /// Which opengl call list
  int listNum;
  
  std::vector<vector3f> iStack;

  /// the parts of iStack that are on the surface
  std::vector<vector3f> surface;

  vector3f centerOfMass;
  bool translucent;

  /// Used for finding MoI
  std::vector<vector3f> upperI;
  std::vector<vector3f> lowerI;
};


/// Hold rigid body data, use objloader to load obj's
/**
I thought this would be more memory efficient, to have
specialized classes for loading and holding rigid body data.
*/
class rigid: public movable {
 public:
  rigid(const char *url);
  virtual ~rigid() {}
  
  /// Virtual generic draw member, calls makeList
  void draw();
  /// Adjust light intensity per-vertex
  void drawDim(vector<light*> lights);
  /// Does nothing currently, put scripted movements here
  //dax void update();
  /// Currently not using display lists, this just draws each triangle
  void makeList();
  /// Virtual member that finds the extrema of all the vertices
  void getBoundingBox();
  /// For scripted movements, unused currently
  int counter;

  char *url;

  static void reader(void *_rigid, class Http *http);

 private:
  ///temp
  objloader *theObj;
  
  /// All surface vertices, independent of triangles they are part of.
  std::vector<vector3f> vertices;
  /// Normals for each vertice, same index should be corresponding vertice index
  std::vector<vector3f> normals;
  /// The mtls hold material lighting information as well as indices of vertices in triangles
  std::vector<material*> mtls;
  /// Set by findCenterOfMass in objloader
  vector3f centerOfMass;
  /// Which opengl call list, unsupported currently
  int listNum;
  /// Toggles drawing of the surface triangles
  bool drawSurface;
  /// This is likely obsolete and should be gotten rid of
  bool translucent;

  void download(const char *url);
};

// how should the animation info fit in?
// per part, or globally?
class bvhPart {

 public: 
  enum channelTypes {Xpos,Ypos,Zpos,Zrot,Xrot,Yrot};
  
  bvhPart();

  string name;

  vector3f offset;
  
  vector<matrix16f> motion;
  
  //objloader* objPart;
  rigid* objPart;
  
  bvhPart* parent;
  vector<channelTypes> channels;
  vector<bvhPart*> child;
};

/**
 * Bvh class
 */
class Bvh {
  bool verbose;
  enum mode {NONE,OFFSET,CHANNELS,JOINT,ROOT,End,Site,MOTION,Frames,Frame,Time,MOTIONDATA};

 public:
  bvhPart *root;
  float frameTime;

 private:
  // mostly used just for init/processing- what to do?
  // Have another class with just essential data structures
  // that uses a processing class and then deletes it- more mem efficient
  bvhPart *current;
  vector <bvhPart*> bvhPartsLinear;
  
  mode theMode;
  int vertIndex;
  
  uint32_t channelIndex;
  uint32_t partIndex;

  int data;  
  /// the channels vector will store it's own size, so this is just for error checking.
  uint32_t channelsNum;  

  matrix16f tempMotion;
  matrix16f tempMotionY;
  matrix16f tempMotionX;
  matrix16f tempMotionZ;

 public:
  Bvh(const char *url);
  virtual ~Bvh() {}

  int framesNum;
  char *url;

  void recurs(bvhPart* some);
  void process(string line);
  void init(string bvhFile);

  static void reader(void *_bvh, class Http *http);

 private:
  void download(const char *url);
};


#endif
