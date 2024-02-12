//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
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
#include "vreng.hpp"
#include "x3d.hpp"
#include "http.hpp"	// httpOpen
#include "draw.hpp"	// Draw
#include "texture.hpp"	// open
#include "cache.hpp"	// open, close

using namespace std;


void X3d::defaults()
{
  anim = true;
  flashy = false;
}

X3d::X3d(const char *_url) : rootShape(0)
{
  defaults();

  url = new char[strlen(_url) + 1];
  strcpy(url, _url);

  Http::httpOpen(url, reader, this, 0);
}

X3d::~X3d()
{
  if (url) delete[] url;
}

void X3d::reader(void *_x3d, class Http *http)
{
  X3d* x3d = static_cast<X3d *>(_x3d);
  if (! x3d) return;

  FILE *f;
  char filename[PATH_LEN] = {0};
  Cache::setCachePath(http->url, filename);

  Cache *cache = new Cache();
  if ((f = cache->open(filename, http)) == NULL) {
    char buf[BUFSIZ];
    int len;
    while ((len = http->httpRead(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, len, f);	// into cache
    }
    cache->close();
    delete cache;
  }
  x3d->loadFromFile(filename);
}

bool X3d::loadFromFile(char *filename)
{
  // this opens and parses the XML file:
  XMLNode xMainNode = XMLNode::openFileHelper(filename);

  browseX3dTree(&xMainNode, &rootShape);

  setupInterpolators();

  // initializing at time 0 animated fields
  for (int i=0; i < interpolators.size(); i++) {
    interpolators[i].updateValue(0);
  }

  //affichage de l'arbre xml - useless now
  //XMLSTR result = xMainNode.createXMLString(1);
  //printf("xml tree: %s", result);

  return true;
}

void X3d::browseX3dTree(XMLNode* xmlnode, X3dShape* shape)
{
  if (!xmlnode || !shape) return;

  XMLCSTR nodename = xmlnode->getName();

  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (isEqual(nodename, "Shape")) {  //new shape
    //echo("add a child shape at level %d", shape->level + 1);
    X3dShape* child = new X3dShape(shape->level + 1);

    shape->childShapes.push_back(child);
    shape = child; // we change of node

    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "DEF")) {
        //echo("we have an DEF name: %s", xmlnode->getAttributeValue(i));
	shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodename, "Transform")) {
    if (xmlnode->nChildNode() != 0) {
      //echo("add a child transform at level %d", shape->level + 1);

      X3dShape* child = new X3dShape(shape->level + 1);

      shape->childShapes.push_back(child);
      shape = child; // we change of node
    }

    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "translation")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->translation,3))
          shape->translationOn = true;
        else error("Translation not correct !");
      }
      else if (isEqual(attr, "rotation")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->rotation,4))
          shape->rotationOn = true;
        else error("Rotation not correct !");
      }
      else if (isEqual(attr, "scale")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->scale,3))
          shape->scaleOn = true;
        else error("Scale not correct !");
      }
      else if (isEqual(attr, "DEF")) {
        //echo("we have an object DEF name: %s", xmlnode->getAttributeValue(i));
        shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodename, "Material")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "emissiveColor")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->emissive,3))
          shape->emissiveOn = true;
      }
      else if (isEqual(attr, "diffuseColor")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->diffuse,3))
          shape->diffuseOn = true;
      }
      else if (isEqual(attr, "specularColor")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), shape->specular,3))
          shape->specularOn = true;
      }
      else if (isEqual(attr, "ambientIntensity")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), &shape->ambientIntensity,1))
          shape->ambientIntensityOn = true;
      }
      else if (isEqual(attr, "shininess")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), &shape->shininess,1))
          shape->shininessOn = true;
      }
      else if (isEqual(attr, "transparency")) {
        if (X3dVectors::parseFloats(xmlnode->getAttributeValue(i), &shape->transparency,1))
          shape->transparencyOn = true;
      }
      else if (isEqual(attr, "DEF")) {
        //echo("We have an DEF name: %s", xmlnode->getAttributeValue(i));
        shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodename, "ImageTexture")) {
    // we modify the texture
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "url"))
        shape->texture = Texture::open(xmlnode->getAttributeValue(i));
    }
  }
  else if (isEqual(nodename, "Coordinate")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "point"))
        X3dVectors::parseCertifiedVectors(xmlnode->getAttributeValue(i), &shape->meshInfos.Coord,3);
    }
  }
  else if (isEqual(nodename, "TextureCoordinate")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "point"))
        X3dVectors::parseCertifiedVectors(xmlnode->getAttributeValue(i), &shape->meshInfos.TextureCoord,2);
    }
  }
  else if (isEqual(nodename, "Color")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "color"))
        X3dVectors::parseCertifiedVectors(xmlnode->getAttributeValue(i), &shape->meshInfos.Color,3);
    }
  }
  else if (isEqual(nodename, "ColorInterpolator") ||
           isEqual(nodename, "PositionInterpolator") ||
           isEqual(nodename, "ScaleInterpolator") ||
           isEqual(nodename, "OrientationInterpolator") ||
           isEqual(nodename, "ScalarInterpolator")) {
    X3dInterpolator interpolator;
    if (interpolator.initArrays(xmlnode)) {
      interpolators.push_back(interpolator);
    }
    else error("interpolator rejected !");
  }
  else if (isEqual(nodename, "TimeSensor")) {
    X3dTimeSensor timesensor;
    if (timesensor.initSensor(xmlnode)) {
      timeSensors.push_back(timesensor);
    }
    else error("timesensor rejected !");
  }
  else if (isEqual(nodename, "Route")) {
    X3dRoute route;
    if (route.initRoute(xmlnode)) {
      routes.push_back(route);
    }
    else error("route rejected !");
  }
  else {        // is it a PRIMITIVE ?
    Primitives vrengPrimitiveIdx = X3DNONE;
    if (X3DNONE != (vrengPrimitiveIdx = isKnownPrimitive(xmlnode->getName()))) {
      GLuint primitive = drawPrimitive(vrengPrimitiveIdx);
      shape->meshes.push_back(primitive);
    }
  }

  // we take care of the children nodes
  int nchild = xmlnode->nChildNode();
  for (int i=0; i < nchild; i++) {
    XMLNode mynode = xmlnode->getChildNode(i);
    browseX3dTree(&mynode, shape);
  }
  if (isEqual(nodename, "IndexedFaceSet")) {
    shape->meshInfos.colorPerVertex = false;

    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "colorPerVertex")) {
        if (isEqual(xmlnode->getAttributeValue(i), "true"))
          shape->meshInfos.colorPerVertex = true;
      }
      else if (isEqual(attr, "coordIndex")) {
        string temp(xmlnode->getAttributeValue(i));
        uint32_t offset = temp.find("-1");
        while (offset != (uint32_t) string::npos) {
          temp.replace(offset, 2, ", ");
          offset = temp.find("-1");
        }
        X3dVectors::parseVectors(temp, &shape->meshInfos.coordIdx);
      }
      else if (isEqual(attr, "texCoordIndex")) {
        string temp(xmlnode->getAttributeValue(i));
        uint32_t offset = temp.find("-1");
        while (offset != (uint32_t) string::npos) {
          temp.replace(offset, 2, ", ");
          offset = temp.find("-1");
        }
        X3dVectors::parseVectors(temp, &shape->meshInfos.texCoordIdx);
      }
      else if (isEqual(attr, "colorIndex")) {
        string temp(xmlnode->getAttributeValue(i));
        uint32_t offset = temp.find("-1");
        while (offset != (uint32_t) string::npos) {
          temp.replace(offset, 2, ", ");
          offset = temp.find("-1");
        }
        X3dVectors::parseVectors(temp, &shape->meshInfos.colorIdx);
      }
    }
    // we draw the complex mesh with the stored data
    GLuint primitive = drawMesh(&shape->meshInfos);
    shape->meshes.push_back(primitive);

    // we reset everything for the next indexedFaceSet
    shape->meshInfos.coordIdx.clear();
    shape->meshInfos.texCoordIdx.clear();
    shape->meshInfos.colorIdx.clear();
    shape->meshInfos.Coord.clear();
    shape->meshInfos.TextureCoord.clear();
    shape->meshInfos.Color.clear();
  }
}

void X3d::setupInterpolators()
{
  for (vector<X3dRoute>::iterator route = routes.begin(); route != routes.end(); route++) {
    X3DOUTField fromfield = route->fromField;
    X3DINField tofield = route->toField;

    // timer to interpolator route
    if (fromfield == FRACTION_CHANGED && tofield == SET_FRACTION) {
      X3dTimeSensor* sensor = findItem<X3dTimeSensor>(&timeSensors, route->fromNode);
      X3dInterpolator* interpolator = findItem<X3dInterpolator>(&interpolators, route->toNode);
      if (sensor && interpolator)
        sensor->initTarget(interpolator);
      else
        error("route sensor->interpolator bugged: de %p a %p", sensor, interpolator);
    }

    else if (fromfield == VALUE_CHANGED && tofield >= TRANSLATION && tofield <= TRANSPARENCY) {
      // interpolator to shape field
      X3dInterpolator* interpolator = findItem<X3dInterpolator>(&interpolators, route->fromNode);
      X3dShape* targetShape = findShape(&rootShape, route->toNode);
      if (interpolator && targetShape) {
        interpolator->initTarget(targetShape, tofield);
      }
      else
        error("route interpolator->x3dshape bugged: de %p a %p", interpolator, targetShape);
    }
    else
      error("bugged route with improper fields from %s to %s", route->fromNode.c_str(), route->toNode.c_str());
  } //end loop on routes

  // initialisation des liens d'animation done
  routes.clear();
}

X3dShape* X3d::findShape(X3dShape* root, string name)
{
  if (root->getName() == name) return root;

  X3dShape* shape = NULL;
  for (int i=0; i < root->childShapes.size(); i++) {
    shape = findShape(root->childShapes[i], name); //recursivity
    if (shape) return shape;
  }
  return NULL;
}

template <class T> T* X3d::findItem(vector<T>* tab, string name)
{
  for (typename vector<T>::iterator j = tab->begin(); j != tab->end(); j++) {
    if (j->getName() == name) return &(*j);
  }
  return NULL;
}

GLuint X3d::drawPrimitive(Primitives id)
{
  GLuint dlist = glGenLists(1);

  switch (id) {
  case X3DCYLINDER:
    glNewList(dlist, GL_COMPILE);
    Draw::disk(0, 0.5, 16, 2, Draw::STYLE_FILL);
    Draw::cylinder(0.5, 0.5, 1, 16, 4, Draw::STYLE_FILL);
    glTranslatef(0, 0, 1);
    glRotatef(180, 0, 1, 0);
    Draw::disk(0, 0.5, 16, 2, Draw::STYLE_FILL);
    glEndList();
    break;

  case X3DCONE:
    glNewList(dlist, GL_COMPILE);
    Draw::disk(0, 0.5, 16, 2, Draw::STYLE_FILL);
    Draw::cylinder(0.5, 0, 1, 16, 4, Draw::STYLE_FILL);
    glEndList();
    break;

  case X3DSPHERE:
    glNewList(dlist, GL_COMPILE);
    Draw::sphere(0.5, 16, 16, Draw::STYLE_FILL);
    glEndList();
    break;

  case X3DDISK:
    glNewList(dlist, GL_COMPILE);
    Draw::disk(0, 0.5, 16, 2, Draw::STYLE_FILL);
    glRotatef(90, 0, 1, 0);
    glEndList();
    break;

  case X3DBOX:
    {
    GLfloat x = .5, y = .5, z = 1;

    glNewList(dlist, GL_COMPILE);
    glBegin(GL_QUADS);

    glNormal3f(0, 0,-1);
    glVertex3f(-x,-y,0);
    glVertex3f(-x, y,0);
    glVertex3f( x, y,0);
    glVertex3f( x,-y,0);

    glNormal3f(0, 0,1);
    glVertex3f(-x,-y,z);
    glVertex3f( x,-y,z);
    glVertex3f( x, y,z);
    glVertex3f(-x, y,z);

    glNormal3f(0, 1,0);
    glVertex3f( x,y,0);
    glVertex3f(-x,y,0);
    glVertex3f(-x,y,z);
    glVertex3f( x,y,z);

    glNormal3f(0, -1,0);
    glVertex3f(-x,-y,0);
    glVertex3f( x,-y,0);
    glVertex3f( x,-y,z);
    glVertex3f(-x,-y,z);

    glNormal3f(-1, 0,0);
    glVertex3f(-x, y,0);
    glVertex3f(-x,-y,0);
    glVertex3f(-x,-y,z);
    glVertex3f(-x, y,z);

    glNormal3f(1, 0, 0);
    glVertex3f(x, y,0);
    glVertex3f(x, y,z);
    glVertex3f(x,-y,z);
    glVertex3f(x,-y,0);

    glEnd();
    glEndList();
    }
    break;

  default:
    error("X3d - primitive %d not drawable", id);
    dlist = 0;
    break;
  }
  return dlist;
}

GLuint X3d::drawMesh(MeshInfos* meshInfos)
{
#if 0 // debug
  // we display what we have in the temporary structure
  echo("New mesh in creation:");
  printf("coordindex: ");
  X3dVectors::echoVector(& meshInfos->coordIdx);
  printf("texcoordindex: ");
  X3dVectors::echoVector(& meshInfos->texCoordIdx);
  printf("colordindex: ");
  X3dVectors::echoVector(& meshInfos->colorIdx);
  printf("coordinate: ");
  X3dVectors::echoVector(& meshInfos->Coord);
  printf("texturecoordinate: ");
  X3dVectors::echoVector(& meshInfos->TextureCoord);
  printf("color: ");
  X3dVectors::echoVector(& meshInfos->Color);
#endif
  GLuint dList = glGenLists(1);
  glNewList(dList, GL_COMPILE);

  // we browse the vectors containing a polygon each
  for (int polnum=0; polnum < meshInfos->coordIdx.size(); polnum++) {
    glBegin(GL_POLYGON);	// create a polygon
    // one color per polygon
    if (meshInfos->colorPerVertex == false) {
      // we really have at least a single list of colors
      if (0 < meshInfos->colorIdx.size()) {
        // if a color exists at that index
        if (polnum < meshInfos->colorIdx.front().size()) {
          // this is the color index we retrieve from the index list
          uint32_t tempColor = meshInfos->colorIdx.front()[polnum];
          if (tempColor >= meshInfos->Color.size())
            error("color index out of bounds !");
          else {
            uint32_t color =  tempColor;
            // if the color is well formed
            if (meshInfos->Color[color].size() == 3) {
              glColor3f(
                meshInfos->Color[color][0],
                meshInfos->Color[color][1],
                meshInfos->Color[color][2]);
            }
          }
        }
      }
    }
    // we browse the vertexes of one polygon
    for (int vernum=0; vernum < meshInfos->coordIdx[polnum].size(); vernum++) {
      // this is the vertex index we retrieve from the index list
      uint32_t vertex = meshInfos->coordIdx[polnum][vernum];
      if (meshInfos->Coord.size() == 0) break;	//dax
      // if a vertex exists at that index
      if (vertex < meshInfos->Coord.size()) {
        // if the vertex is well formed
        if (meshInfos->Coord[vertex].size() == 3) {
          // one color per polygon
          if (meshInfos->colorPerVertex == true) {
            // we really have a list of colors for that polygon
            if (polnum < meshInfos->colorIdx.size()) {
              // if a color index exists for that vertex in the polygon
              if (vernum < meshInfos->colorIdx[polnum].size()) {
                // this is the color index we retrieve from the index list
                uint32_t tempColor = meshInfos->colorIdx[polnum][vernum];
                if (tempColor >= meshInfos->Color.size())
                  error("color index out of bounds !");
                else {
                  uint32_t color = tempColor;
                  // if the color is well formed
                  if (meshInfos->Color[color].size() == 3) {
                    glColor3f(
                      meshInfos->Color[vertex][0],
                      meshInfos->Color[vertex][1],
                      meshInfos->Color[vertex][2]);
                  }
                }
              }
            }
          }
          // texture coords to apply ?
          if (polnum < meshInfos->texCoordIdx.size() &&
              vernum < meshInfos->texCoordIdx[polnum].size()) {
            uint32_t tex = meshInfos->texCoordIdx[polnum][vernum];
            // if a texturecoord exists at that index
            if (tex < meshInfos->TextureCoord.size()) {
              // if the texcoord is well formed
              if (meshInfos->TextureCoord[tex].size() == 2)
                glTexCoord2f(meshInfos->TextureCoord[tex][0], meshInfos->TextureCoord[tex][1]);
            }
          }
          // we put a vertex, after all that
          glVertex3f(
            meshInfos->Coord[vertex][0],
            meshInfos->Coord[vertex][1],
            meshInfos->Coord[vertex][2]);
        }
        else error("coords of vertex incorrect");
      }
      else {
        error("index of vertex out of bounds %d>%d", vertex, meshInfos->Coord.size());
      }
    }
    glEnd();
  }
  glEndList();
  return dList;
}

void X3d::displayShape(X3dShape* myShape) //NOT RECURSIVE !!
{
  //echo("display shape: %d at level %d", myShape, myShape->level);
  float ambdef[4] = { 0,0,0,1 };
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambdef);

  if (myShape->ambientIntensityOn) {
    float ambient[4];
    ambient[0] = myShape->ambientIntensity * myShape->diffuse[0];
    ambient[1] = myShape->ambientIntensity * myShape->diffuse[1];
    ambient[2] = myShape->ambientIntensity * myShape->diffuse[2];
    ambient[3] = myShape->transparencyOn ? myShape->transparency : 1;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  }
  if (myShape->shininessOn)
    glMaterialf(GL_FRONT, GL_SHININESS, 128 * myShape->shininess);
  if (myShape->diffuseOn) {
    myShape->diffuse[3] = myShape->transparencyOn ? myShape->transparency : 1;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, myShape->diffuse);
  }
  if (myShape->specularOn) {
    myShape->specular[3] = myShape->transparencyOn ? myShape->transparency : 1;
    glMaterialfv(GL_FRONT, GL_SPECULAR, myShape->specular);
  }
  if (myShape->emissiveOn) {
    myShape->emissive[3] = myShape->transparencyOn ? myShape->transparency : 1;
    glMaterialfv(GL_FRONT, GL_EMISSION, myShape->emissive);
  }
  if (myShape->translationOn)
    glTranslatef(myShape->translation[0], myShape->translation[1], myShape->translation[2]);
  if (myShape->rotationOn)
    glRotatef(RAD2DEG(myShape->rotation[3]), myShape->rotation[0], myShape->rotation[1], myShape->rotation[2]);
  if (myShape->scaleOn)
    glScalef(myShape->scale[0], myShape->scale[1], myShape->scale[2]);

  for (vector<GLuint>::iterator i = myShape->meshes.begin(); i != myShape->meshes.end(); i++) {
    glCallList(*i);
    if (flashy) {  //flashy mesh
      glPushMatrix();
      glLineWidth(1);
      glDisable(GL_POLYGON_OFFSET_FILL);
      glEnable(GL_COLOR_MATERIAL);
      glColor3f(1,1,0);
      glScalef(1.03, 1.03, 1.03);  // 3%100 more
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glEnable(GL_CULL_FACE);
      glEnable(GL_BLEND);
      glCallList(*i);
      glDisable(GL_BLEND);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDisable(GL_COLOR_MATERIAL);
      glPopMatrix();
    }
  }
}

void X3d::render()
{
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);

  // default values for materials
  float shiny = 1;
  float black[4] = {0,0,0,1};
  float white[4] = {1,1,1,1};
  glColor4f(0,0,0,1); // track the default ambient wanted else problems with gl_color_material
  glMaterialfv(GL_FRONT, GL_AMBIENT, black);
  glMaterialfv(GL_FRONT, GL_SPECULAR, white);
  glMaterialfv(GL_FRONT, GL_SHININESS, &shiny);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
  glMaterialfv(GL_FRONT, GL_EMISSION, white);

  // we update the timers for animation
  for (vector<X3dTimeSensor>::iterator t = timeSensors.begin(); t != timeSensors.end(); t++) {
    t->updateFraction(anim);
  }

  // list to make an iterative treatment of the tree
  vector<X3dShape*> nextShapes;
  nextShapes.push_back(&rootShape);

  X3dShape* shape;
  int prevlevel = -1;

  while (! nextShapes.empty()) {
    // we take the next node of the tree
    shape = nextShapes.back();
    nextShapes.pop_back();

    // we change the matrix stack according to the new change of level in the X3D tree
    // and if we switch to a child, we don't touch the current (parent) matrix
    if (shape->level <= prevlevel) {
      glPopMatrix(); // we remove the matrix of the current shape
      //glPopAttrib();	//dax1
      if (shape->level < prevlevel) {
        glPopMatrix(); // remove the matrix of the previous shape because we went down
        //glPopAttrib();	//dax1
      }
    }
    prevlevel = shape->level;

    glPushMatrix();
    //glPushAttrib(GL_ALL_ATTRIB_BITS);	//dax1

    if (shape->texture) {  //texture is there in priority
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture(GL_TEXTURE_2D, shape->texture);
    }
    else if (!shape->diffuseOn) {  // we'll use glColors instead of diffuse material
      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }

    displayShape(shape); // call the display of the shape

    // Reset
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    for (vector<X3dShape*>::iterator it = shape->childShapes.begin(); it != shape->childShapes.end(); it++) {
      nextShapes.push_back(*it);
    }
  }

  // we remove the remaining matrixes after having browsed the complete tree
  for (int k=-1; k < prevlevel; k++) {
    glPopMatrix();
  }
}

/* returns the X3DOBJECT index of the primitive */
Primitives X3d::isKnownPrimitive(XMLCSTR vredata)
{
  for (int i=0; i < KNOWNPRIMITIVESNUMBER; i++) {
    if (isEqual(vredata, knownPrimitives[i].shape))
      return knownPrimitives[i].id;
  }
  return X3DNONE;
}

/* case insensitive comparison */
bool X3d::isEqual(const char* str1, const char* str2)
{
  if (!str1 || !str2) return false;

  int l1 = strlen(str1);
  int l2 = strlen(str2);
  if (l1 != l2) return false;

  for (int i=0; i < l1; i++) {
    if (tolower(str1[i]) != tolower(str2[i])) return false;
  }
  return true;
}

/* link between the name parsed in x3d file and the primitive we can draw */
ShapeToId X3d::knownPrimitives[KNOWNPRIMITIVESNUMBER] = {
  {"Sphere", X3DSPHERE},
  {"Cylinder", X3DCYLINDER},
  {"Cone", X3DCONE},
  {"Disk", X3DDISK},
  {"Box", X3DBOX}
};

/* Sets flashy the X3d object */
void X3d::setFlashy()
{
  flashy = true;
}

void X3d::resetFlashy()
{
  flashy = false;
}

void X3d::resetAnimations()
{
  for (vector<X3dTimeSensor>::iterator t = timeSensors.begin(); t != timeSensors.end(); t++) {
    t->resetFraction();
  }
}


/*
 * X3dVectors methods
 */

bool X3dVectors::parseFloats(const string str, float* outputs, uint32_t number)
{
  vector<vector<float> > temp;

  if (parseCertifiedVectors(str, &temp, number, 1)) {
    vector<float> myVector = temp.front();
    for (int i=0; i<number; i++) {
      //outputs[i]=myVector.at(i);
      outputs[i] = myVector[i];
    }
    return true;
  }
  return false;
}

/* returns true if succeeded, false else */
bool X3dVectors::parseFloats(const string str, vector<float>* output)
{
  vector<vector<float> > temp;

  if (parseCertifiedVectors(str, &temp, 0, 1)) {
    vector<float> myVector = temp.front();
    vector<float>::iterator i = myVector.begin();
    vector<float>::iterator j = myVector.end();
    output->assign(i, j);
    return true;
  }
  return false;
}

bool X3dVectors::parseCertifiedVectors(const string str, vector<vector<float> >* outputs, uint32_t vectorLength, uint32_t numVector)
{
  bool res = parseVectors(str, outputs);

  if (numVector > 0 && outputs->size() != numVector) {
    error("too many vectors");
    return false;
  }
  if (vectorLength > 0) {
    for (vector<vector<float> >::iterator i=outputs->begin(); i!=outputs->end();i++) {
      if (i->size() != vectorLength) {
        error("vectors too long");
        return false;
      }
    }
  }
  return res;
}

bool X3dVectors::parseVectors(const string str, vector<vector<float> > *outputs)
{
  int limit = str.size();
  int in = 0;
  int out = 0;
  float newFloat;

  char* temp = new char[limit+1]; //to store chars before conversion to float

  vector<float> tempout;

  while (in < limit) {
    while (in < limit &&
           (str[in] == '\n' ||
            str[in] == '\r' ||
            str[in] == ' ' ||
            str[in] == ',')) {
      in++;  // we skip initial spaces and semicolons
      if (str[in] == ',') {  //end of a vector
        if (!tempout.empty()) {
          outputs->push_back(tempout);
          tempout.clear();
        }
      }
    }
    while (str[in] != '\n' &&
           str[in] != '\r' &&
           str[in] != ' ' &&
           str[in] != ',' &&
           in < limit) {  //on recupere les symboles du float
      if (!isdigit(str[in]) &&
          str[in] != '-' &&
          str[in] != '+' &&
          str[in] != '.') {
        error("non digit character '%c' detected ! digit parsing aborted !", str[in]);
        return false;
      }
      temp[out] = str[in];
      in++;
      out++;
    }
    temp[out] = '\0';
    out = 0;  // we begin again forming a float
    if (strlen(temp)) {		// we have something to convert
      sscanf(temp, "%f", &newFloat); // 0.0 if conversion error
      tempout.push_back(newFloat);
    }
    if (str[in] == ',' || in >= limit) {  // end of a vector
      if (!tempout.empty()) {
        outputs->push_back(tempout);
        tempout.clear();
      }
    }
  }
  delete[] temp;
  return true;
}

void X3dVectors::echoVector(vector<vector<float> >* outputs)
{
  for (vector<vector<float> >::iterator i = outputs->begin(); i != outputs->end(); i++) {
    for (vector<float>::iterator j = i->begin(); j != i->end(); j++) {
      printf("%.1f ", *j);
    }
  }
  printf("\n");
}

void X3dVectors::echoVector(vector<float>* outputs)
{
  for (vector<float>::iterator i = outputs->begin(); i != outputs->end(); i++) {
    printf("%.1f ", *i);
  }
  printf("\n");
}


/*
 * X3dInterpolator methods
 */

bool X3dInterpolator::initArrays(XMLNode* xmlnode)
{
  XMLCSTR nodename = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;
  int vectorsize = 0;

  if (X3d::isEqual(nodename, "PositionInterpolator")) {
    type = POSITIONINTERPOLATOR;
    vectorsize = 3;
  }
  else if (X3d::isEqual(nodename, "OrientationInterpolator")) {
    type = ROTATIONINTERPOLATOR;
    vectorsize = 4;
  }
  else if (X3d::isEqual(nodename, "ScaleInterpolator")) {
    type = SCALEINTERPOLATOR;
    vectorsize = 3;
  }
  else if (X3d::isEqual(nodename, "ColorInterpolator")) {
    type = COLORINTERPOLATOR;
    vectorsize = 3;
  }
  else if (X3d::isEqual(nodename, "ScalarInterpolator")) {
    type = SCALARINTERPOLATOR;
    vectorsize = 1;
  }
  else {
    error("%s is not an interpolator", nodename);
    return false;
  }

  for (int i=0; i < nattr; i++) {
    attr = xmlnode->getAttributeName(i);

    if (X3d::isEqual(attr, "DEF"))
      name = xmlnode->getAttributeValue(i);
    else if (X3d::isEqual(attr, "key"))
      X3dVectors::parseFloats(xmlnode->getAttributeValue(i), &keys);
    else if (X3d::isEqual(attr, "keyValue"))
      X3dVectors::parseCertifiedVectors(xmlnode->getAttributeValue(i), &keyValues, vectorsize);
  }

  if (name == "") {
    error("no name for the interpolator %s", name.c_str());
    return false;
  }
  if (keys.size() < 2) {
    error("less than 2 keys in the interpolator %s", name.c_str());
    return false;
  }
  if (keys.size() != keyValues.size()) {
    error("numbers of keys and values aren't the same in interpolator %s", name.c_str());
    return false;
  }
  if (keys.front() != 0 || keys.back() != 1) {
    X3dVectors::echoVector(&keys);
    error("interpolator bounding keys must be 0 and 1, not %.1f and %.1f, in %s", keys.front(), keys.back(),name.c_str());
    return false;
  }

  for (int i=0; i < keys.size()-1; i++) {
    if (keys[i] >= keys[i+1]) {
      error("interpolator keys must be increasing in %s", name.c_str());
      return false;
    }
  }
  //echo("interpolator added: %s of type %d", name.c_str(), type);

  //printf("Key: ");
  //X3dVectors::echoVector(&keys);
  //printf("KeyValues: ");
  //X3dVectors::echoVector(&keyValues);

  return true;
}

bool X3dInterpolator::initTarget(X3dShape* target, X3DINField field)
{
  float *temp = NULL;

  switch (type) {

  case SCALEINTERPOLATOR:
    switch (field) {
    case SCALE:
      target->scaleOn = true;
      temp = target->scale;
    default:
      break;
    }
    break;

  case POSITIONINTERPOLATOR:
    switch (field) {
    case TRANSLATION:
      target->translationOn = true;
      temp = target->translation;
    default:
      break;
    }
    break;

  case ROTATIONINTERPOLATOR:
    switch (field) {
    case ROTATION:
      target->rotationOn = true;
      temp = target->rotation;
    default:
      break;
    }
    break;

  case COLORINTERPOLATOR:
    switch (field) {
    case DIFFUSECOLOR:
      target->diffuseOn = true;
      temp = target->diffuse;
      break;
    case SPECULARCOLOR:
      target->specularOn = true;
      temp = target->specular;
      break;
    case EMISSIVECOLOR:
      target->emissiveOn = true;
      temp = target->emissive;
    default:
      break;
    }
    break;

  case SCALARINTERPOLATOR:
    switch (field) {
    case AMBIENTINTENSITY:
      target->ambientIntensityOn = true;
      temp = &target->ambientIntensity;
      break;
    case SHININESS:
      target->shininessOn = true;
      temp = &target->shininess;
      break;
    case TRANSPARENCY:
      target->transparencyOn = true;
      temp = &target->transparency;
    default:
      break;
    }
  default:
    break;
  }

  if (temp) {
    targets.push_back(temp);
    //echo("route added between interpolator -%s- and shape -%s- for field %d", name.c_str(), target->name.c_str(), field);
    return true;
  }
  else {
    error("interpolator: unproper field type %d for interpolator type %d", field, type);
    return false;
  }
}

/* updates the target values in the shapes according to the time fraction we receive */
void X3dInterpolator::updateValue(float newFraction)
{
  int index = -1;	// left index of the interpolation key
  float percentage = 0; // % of interpolation between keyValues index and index+1

  if (targets.size() == 0) {
    error("No targets for the updated interpolator %s", name.c_str());
    return;
  }

  for (int i=0; i < keys.size()-1; i++) {
    if (keys[i] <= newFraction && keys[i+1] >= newFraction) index = i;
  }
 
  if (index < 0) error("Error while interpolating in %s: fraction unknown", name.c_str());

  percentage = (newFraction-keys[index]) / (keys[index+1]-keys[index]);

  switch (type) {

  case SCALEINTERPOLATOR:
  case POSITIONINTERPOLATOR:
  case COLORINTERPOLATOR:
      {
      float t[3];

      for (int i=0; i<3; i++) {
        t[i] = (1-percentage) * keyValues[index][i] + percentage*keyValues[index+1][i];
        for (int j=0; j < targets.size(); j++) {
          targets[j][i] = t[i];
        }
      }
      //echo("vector interpolation: %.1f %.1f %.1f", t[0],t[1],t[2]);
    }
    break;

  case SCALARINTERPOLATOR:
      {
      float scalar;

      scalar = (1-percentage)*keyValues[index][0] + percentage*keyValues[index+1][0];
      for (int j=0; j < targets.size(); j++) {
        *targets[j] = scalar;
      }
      //echo("scalar interpolation: %.1f", scalar);
      }
      break;

  case ROTATIONINTERPOLATOR:
      {
      float tempVect[4];

      for (int i=0; i<3; i++) {
      tempVect[i] = (1-percentage)*keyValues[index][i] + percentage*keyValues[index+1][i];
        for (int j=0; j < targets.size(); j++) {
          targets[j][i] = tempVect[i];
        }
      }

      float targetAngle;

      if ((keyValues[index+1][3] - keyValues[index][3]) > 3.1416)
        targetAngle = keyValues[index+1][3]-2*3.1416;
      else if ((keyValues[index+1][3] - keyValues[index][3]) < -3.1416)
        targetAngle = keyValues[index+1][3]+2*3.1416;
      else
        targetAngle = keyValues[index+1][3];

      tempVect[3] = (1-percentage)*keyValues[index][3] + percentage*targetAngle;

      for (int j=0; j < targets.size(); j++) {
        targets[j][3] = tempVect[3];
      }

      //echo("ROTATION: fraction=%.1f, pourcentage=%.1f, entre %.1f et %.1f",
      //newFraction,percentage,keyValues[index][3],targetAngle);
      //echo("Vector 3 interpolation: %.1f %.1f %.1f", t[0],t[1],t[2]);

#if 0 //QUATERNION
      // QUATERNION INTERPOLATION HERE INSTEAD ????
      float rot[4];

      float q;
      for (int i=0; i<3; i++) {
        for (int j=0; j < targets.size(); j++) {
          targets[j][i] = rot[i];
        }
      }
      //echo("scalar interpolation: %.1f", scalar);
#endif //QUATERNION
    }
    break;

  default:
    error("unproper interpolator type in %s", name.c_str());
    break;
  }
}


/*
 * X3dTimeSensor methods
 */

bool X3dTimeSensor::initSensor(XMLNode* xmlnode)
{
  XMLCSTR nodename = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (! X3d::isEqual(nodename, "TimeSensor")) {
    error("this node is not a timeSensor");
    return false;
  }

  for (int i=0; i < nattr; i++) {
    attr = xmlnode->getAttributeName(i);
    if (X3d::isEqual(attr, "DEF")) {
      name = strdup(xmlnode->getAttributeValue(i));
    }
    else if (X3d::isEqual(attr, "cycleInterval")) {
      X3dVectors::parseFloats(xmlnode->getAttributeValue(i), &cycleInterval, 1);
      cycleInterval *= 1000;
    }
    else if (X3d::isEqual(attr, "loop")) {
      if (X3d::isEqual(xmlnode->getAttributeValue(i), "false"))
        loop = false;
    }
  }
  if (name == "") {
    error("no name for the sensor");
    return false;
  }
  if (cycleInterval <= 0) {
    error("cycleInterval <= 0 in sensor");
    return false;
  }

  //echo("TimeSensor added: %s", name.c_str());
  //echo("cycleInterval: %.1f", cycleInterval);
  //echo("loop: %d", loop);

  return true;
}

void X3dTimeSensor::initTarget(X3dInterpolator* interpolator)
{
  targets.push_back(interpolator);
  //echo("route added between initSensor -%s- and interpolator -%s-", name.c_str(), interpolator->getName().c_str());
}

void X3dTimeSensor::resetFraction()
{
  fraction = 0;

  for (vector<X3dInterpolator*>::iterator it = targets.begin(); it != targets.end(); it++) {
    (*it)->updateValue(fraction);
  }
}

void X3dTimeSensor::updateFraction(bool _anim)
{
  if (fraction < 0) {
    error("Fraction is negative in timer");
    fraction = 0;
  }

  struct timeval currTime;
  gettimeofday(&currTime, NULL);

  if (_anim) {
    int32_t diffSec = currTime.tv_sec - previousTime.tv_sec;
    int32_t diffMilliSec = (currTime.tv_usec - previousTime.tv_usec)/1000;
    int diffms = (int) (1000*diffSec + diffMilliSec);
    //echo("%s says: difference of %d ms", name.c_str(), diffms);

    float fractionIncrement = static_cast<float>(diffms/cycleInterval);

    fraction += fractionIncrement;
    if (fraction > 1) {
      if (loop) {
        fraction -= fraction;
      }
      else fraction = 1;
    }
    //echo("%s says: new increment is %.1f", name.c_str(), fraction);

    for (vector<X3dInterpolator*>::iterator it = targets.begin(); it != targets.end(); it++) {
      (*it)->updateValue(fraction);
    }
  }
  previousTime = currTime;
}


/*
 * X3dRoute methods
 */

bool X3dRoute::initRoute(XMLNode* xmlnode)
{
  XMLCSTR nodename = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (!X3d::isEqual(nodename, "Route")) {
    error("this node is not a route");
    return false;
  }

  for (int i=0; i < nattr; i++) {
    attr = xmlnode->getAttributeName(i);

    if (X3d::isEqual(attr, "fromNode")) {
      fromNode = xmlnode->getAttributeValue(i);
    }
    else if (X3d::isEqual(attr, "toNode")) {
      toNode = xmlnode->getAttributeValue(i);
    }
    else if (X3d::isEqual(attr, "fromField")) {
      string field = xmlnode->getAttributeValue(i);

      if (X3d::isEqual(field.c_str(), "FRACTION_CHANGED"))
        fromField = FRACTION_CHANGED;
      else if (X3d::isEqual(field.c_str(), "VALUE_CHANGED"))
        fromField = VALUE_CHANGED;
    }
    else if (X3d::isEqual(attr, "toField")) {
      string field = xmlnode->getAttributeValue(i);

      if      (X3d::isEqual(field.c_str(), "SET_FRACTION")) toField = SET_FRACTION;
      else if (X3d::isEqual(field.c_str(), "TRANSLATION")) toField = TRANSLATION;
      else if (X3d::isEqual(field.c_str(), "ROTATION")) toField = ROTATION;
      else if (X3d::isEqual(field.c_str(), "SCALE")) toField = SCALE;
      else if (X3d::isEqual(field.c_str(), "DIFFUSECOLOR")) toField = DIFFUSECOLOR;
      else if (X3d::isEqual(field.c_str(), "SPECULARCOLOR")) toField = SPECULARCOLOR;
      else if (X3d::isEqual(field.c_str(), "EMISSIVECOLOR")) toField = EMISSIVECOLOR;
      else if (X3d::isEqual(field.c_str(), "AMBIENTINTENSITY")) toField = AMBIENTINTENSITY;
      else if (X3d::isEqual(field.c_str(), "SHININESS")) toField = SHININESS;
      else if (X3d::isEqual(field.c_str(), "TRANSPARENCY")) toField = TRANSPARENCY;
    }
  } //end "for all attribs"

  if (fromNode == "" ||
      toNode == "" ||
      fromField == NOOUTX3DFIELD ||
      toField == NOINX3DFIELD)
    return false;

  //echo("route added");
  //echo("fromNode: %s", fromNode.c_str());
  //echo("fromField: %d", fromField);
  //echo("toNode: %s", toNode.c_str());
  //echo("toField  %d", toField);

  return true;
}
