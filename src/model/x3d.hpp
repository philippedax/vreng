//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// x3d.hpp
//---------------------------------------------------------------------------
#ifndef X3D_HPP
#define X3D_HPP

#include <iostream>
#include <string>
#include <vector>	// index of gllist

#include "../ext/xmlParser/xmlParser.hpp"


//The classes we have below
class X3d;
class X3dShape;
class X3dVectors;
class X3dInterpolator;
class X3dTimeSensor;
class X3dRoute;


/**
 * Provided you extend the X3d::knownPrimitives array and
 * the X3d::drawPrimitive function
 */
enum Primitives {
  X3DNONE=0,
  X3DSPHERE,
  X3DCYLINDER,
  X3DCONE,
  X3DBOX,
  X3DDISK,
  X3DRECT,
  X3DTORUS,
  X3DPYRAMID,
  X3DLINE,
  KNOWNPRIMITIVESNUMBER
};

struct ShapeToId {
  XMLCSTR shape;
  Primitives id;
};


/**
 * Temporary structure used to build the IndexedFaceSet meshes
 */
struct MeshInfos
{
  bool colorPerVertex;
  std::vector<std::vector<float> > coordIdx, texCoordIdx, colorIdx, Coord, TextureCoord, Color;
};


/**
 * X3dVectors class
 */
class X3dVectors {
 public:
  static bool parseFloats(const std::string str, float* outputs, uint32_t number);
  /**< returns true if succeeded, false else */

  static bool parseFloats(const std::string str, std::vector<float>* output);

  static bool parseCertifiedVectors(const std::string str, std::vector<std::vector<float> >* outputs, uint32_t vectorLength=0, uint32_t numVector=0);

  static bool parseVectors(const std::string str, std::vector<std::vector<float> >* outputs);
  /**< extracts vectors of floats,
   *  spearated by commas. Detects only errors due to non-digit characters
   */

  static void echoVector(std::vector<std::vector<float> >* outputs);
  static void echoVector(std::vector<float>* outputs);
};


/**
 * X3dShape class
 * Node of the X3D tree, containing the data for a shape or a transormation
 */
class X3dShape {
 public:
  std::vector<X3dShape*> childShapes; ///< the sons of that node

  int level;	///< niveau croissant dans l'arbre x3d, root->0

  std::string name; ///< for the ROUTES

  std::string getName() { return name; } //just in case

  float ambientIntensity; bool ambientIntensityOn;
  float shininess; bool shininessOn;
  float transparency; bool transparencyOn;

  float specular[4]; bool specularOn; //4 fields : last one is tranparency
  float diffuse[4]; bool diffuseOn;
  float emissive[4]; bool emissiveOn;

  GLuint texture;

  //applied in this order, so inverted in opengl : scaled, rotated, and then translated
  bool translationOn; float translation[4]; //last float unused
  bool rotationOn; float rotation[4]; //ax,ay,az,angle in rad !
  bool scaleOn; float scale[4]; //last float unused

  MeshInfos meshInfos; //to store the data, in order to build indexFaceSets whenever possible
  std::vector<GLuint> meshes; //index of the glCallLists of that mesh (for primitives and IndexedFaceSets)

  X3dShape(int _level) {
    level = _level;
    scaleOn=translationOn = rotationOn = false;
    transparencyOn = shininessOn = ambientIntensityOn = specularOn = diffuseOn = emissiveOn = false;
    ambientIntensity = 0;
    shininess = 0;
    transparency = 0;
    texture=0;

    for (int i=0;i<4;i++) {
      specular[i] = 0;
      diffuse[i] = 0;
      emissive[i] = 0;

      translation[i] = 0; //last float unused
      rotation[i] = 0;
      scale[i] = 1; //last float unused
    }
  }
};


/**
 * X3dTimeSensor class
 * Object taking care of time for interpolators
 */
class X3dTimeSensor {
 private:
  struct timeval previousTime;
  float fraction;	///< current percentage of the loop

  float cycleInterval;	///< length of the loop (i.e speed of the animation)
  bool loop; 		///< if true : animation loops
  std::string name;

  std::vector<X3dInterpolator*> targets;	///< targets of the time event

 public:
  X3dTimeSensor() {
    loop = true;
    cycleInterval = 0;
    gettimeofday(&previousTime, NULL);
    fraction = 0;
  }

  virtual ~X3dTimeSensor() {}

  bool initSensor(XMLNode* xmlnode);
  /**< gets the attributes of the sensor */

  void initTarget(X3dInterpolator* interpolator);
  /**< adds an interpolator to the targets of the time event */

  void updateFraction(bool anim);
  /**< update the time for this timer */

  void resetFraction();
  /**< resets animations to the initial state */

  std::string getName() { return name; }
};


/**
 * different ways of treating target fields
 */
enum InterpolatorType {
  INTERPOLATORTYPENONE=0,
  SCALEINTERPOLATOR,
  POSITIONINTERPOLATOR,
  ROTATIONINTERPOLATOR,
  COLORINTERPOLATOR,
  SCALARINTERPOLATOR //fo singl float value (transparency tc.)
};


//FIELDS THAT CAN APPEAR IN ROUTE NODES

enum X3DOUTField {
  NOOUTX3DFIELD=0,
  FRACTION_CHANGED,	/* Out for TimeSensor*/
  VALUE_CHANGED		/* Out for Interpolators */
};

enum X3DINField {
  NOINX3DFIELD=0,
  SET_FRACTION,		/* Out for Interpolators */
  //BEWARE: THE ORDER OF THESE MACROS IS IMPORTANT FOR TESTS IN OTHER FUNCTIONS
  TRANSLATION,
  SCALE,
  ROTATION,
  DIFFUSECOLOR,
  SPECULARCOLOR,
  EMISSIVECOLOR,
  AMBIENTINTENSITY,
  SHININESS,
  TRANSPARENCY
};


/**
 * X3dInterpolator class
 */
class X3dInterpolator {
 private:
  std::string name;

  InterpolatorType type; ///< what kind of field does this animate ??

  std::vector<float> keys; ///< key frames used, value in percentage of the loop

  std::vector<std::vector<float> > keyValues; ///< values coresponding to these key frames

  std::vector<float*> targets; ///< float targets (rotation, scale, shininess...)

 public:
  X3dInterpolator() { type = INTERPOLATORTYPENONE; }

  bool initArrays(XMLNode* xmlnode);
  /**< gets the attributes of the interpolator */

  bool initTarget(X3dShape* targetShape, X3DINField field);
  /**< adds a target field for the animated values */

  virtual ~X3dInterpolator() {}

  std::string getName() { return name; }

  void updateValue(float newFraction);
  /**< called by the timeSensor to update the interpolated values */
};


/**
 * X3dRoute class
 * temporary structure to prepare the initialization
 * of the sensors and interpolators
 */
class X3dRoute {
 public:
  std::string fromNode, toNode;
  X3DOUTField fromField;
  X3DINField toField;

  X3dRoute() {
   fromField = NOOUTX3DFIELD;
   toField = NOINX3DFIELD;
  }

  virtual ~X3dRoute() {}

  bool initRoute(XMLNode* xmlnode);
  /**< inits the attributes of the temp Route */
};


/**
 * X3d class
 */
class X3d {
 public:
  bool anim; 			///< is it animated or not ?

  X3d(const char *url);		///< Constructor.
  virtual ~X3d();		///< Destructor.

  void displayShape(X3dShape* mySHape);
  void render();		///< rendering.
  void resetAnimations();	///< for the "stop" button of the GUI.

  void setFlashy();
  /**< Sets flashy the X3d object */
  void stopFlashy();

  static bool isEqual(const char*, const char*);
  /**< case insensitive equality betwen strings */

 private:
  char* url; //url of th x3d model

  X3dShape rootShape;	///< root of the X3D tree

  std::vector<X3dInterpolator> interpolators;
  std::vector<X3dTimeSensor> timeSensors;
  std::vector<X3dRoute> routes; ///< temporary structures to create links

  bool flashy;	///< flashy or not

  void defaults();
  /**< default values for class attributes */

  bool loadFromFile(char* f);
  /**< calls the functions to load the x3d tree and initialize everything */

  void browseX3dTree(XMLNode* xmlnode, X3dShape* shapeNode);
  /**< parses the whole xml file */

  void setupInterpolators();
  /**< initializes the animations thanks to the ROUTE objects */

  GLuint drawPrimitive(Primitives Id);
  /**< draws the primitive and returns the calllist number */

  GLuint drawMesh(MeshInfos* meshInfos);
  /**< draws the complex shape and returns the calllist number */

  X3dShape* findShape(X3dShape* root, std::string name);
  /**< browses the X3d tree to find the name shape */

  template <class T> T* findItem(std::vector<T>* tab, std::string name);
  /**< to find a named item in a vector */

  Primitives isKnownPrimitive(XMLCSTR vredata);
  /**< sends back the  vreng "solid" primitive corresponding to x3d */

  static void reader(void *x3d, class Http *http);
  /**< loader of the web file, manages the cache... */

  static ShapeToId knownPrimitives[KNOWNPRIMITIVESNUMBER];
  /**< array to do this */

};

#endif
