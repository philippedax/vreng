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
#include "texture.hpp"	// getFromCache
#include "cache.hpp"	// setCacheName
#include "file.hpp"	// openFile

using namespace std;


void X3d::defaults(const char *_url)
{
  animationOn = true;
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);

  selected = false;
}

X3d::X3d(const char *_url) : rootShape(0)
{
  defaults(_url);

  Http::httpOpen(url, httpReader, this, 0);
}

X3d::~X3d()
{
  if (url) delete[] url;
}

const char* X3d::getUrl() const
{
  return (const char*) url;
}

void X3d::httpReader(void *_x3d, class Http *http)
{
  X3d* x3d = (X3d *) _x3d;
  if (! x3d) { x3d = NULL; return; }

  FILE *f;
  char filename[PATH_LEN];
  Cache::setCacheName(x3d->getUrl(), filename);

  //error("X3d filename=%s", filename);
  if ((f = File::openFile(filename, "r")) == NULL) {
    if ((f = File::openFile(filename, "w")) == NULL) {
      error("x3dReader: can't create %s", filename);
      return;
    }

    char buf[BUFSIZ];
    int len;
    while ((len = http->httpRead(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, len, f);
    }
    File::closeFile(f);
  }
  x3d->loadFromFile(filename);
}

bool X3d::loadFromFile(char *filename)
{
  // this opens and parses the XML file:
  XMLNode xMainNode = XMLNode::openFileHelper(filename);

  browseX3dTree(&xMainNode, &rootShape);

  //error("setup routes for animations");
  setupInterpolators();

  //error("initializing at time 0 animed fields");
  for (int i=0; i < interpolators.size(); i++) {
    interpolators[i].updateValue(0);
  }

  //affichage de l'arbre xml - useless now
  //dax XMLSTR result = xMainNode.createXMLString(1);
  //dax printf("xml tree: %s", result);

  return true;
}

void X3d::browseX3dTree(XMLNode* xmlnode, X3dShape* shape)
{
  if (!xmlnode || !shape) return;

  XMLCSTR nodeName = xmlnode->getName();

  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (isEqual(nodeName, "Shape")) {  //new shape
    //error("on ajoute un fils shape niveau %d", shape->level + 1);
    X3dShape* son = new X3dShape(shape->level + 1);

    shape->childrenShapes.push_back(son);
    shape = son; // we change of node

    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "DEF")) {
        //error("we have an object DEF name: %s",xmlnode->getAttributeValue(i));
	shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodeName, "Transform")) {
    if (xmlnode->nChildNode() != 0) {
      //error("on ajoute un fils transform niveau %d", shape->level + 1);

      X3dShape* son = new X3dShape(shape->level + 1);

      shape->childrenShapes.push_back(son);
      shape = son; // we change of node
    }

    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "translation")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->translation,3))
          shape->translationOn = true;
        else error("Translation non correcte !");
      }
      else if (isEqual(attr, "rotation")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->rotation,4))
          shape->rotationOn = true;
        else error("Rotation non correcte !");
      }
      else if (isEqual(attr, "scale")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->scale,3))
          shape->scaleOn = true;
        else error("Scale non correcte !");
      }
      else if (isEqual(attr, "DEF")) {
        //error("we have an object DEF name: %s",xmlnode->getAttributeValue(i));
        shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodeName, "Material")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "emissiveColor")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->emissive,3))
          shape->emissiveOn = true;
      }
      else if (isEqual(attr, "diffuseColor")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->diffuse,3))
          shape->diffuseOn = true;
      }
      else if (isEqual(attr, "specularColor")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),shape->specular,3))
          shape->specularOn = true;
      }
      else if (isEqual(attr, "ambientIntensity")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),&shape->ambientIntensity,1))
          shape->ambientIntensityOn = true;
      }
      else if (isEqual(attr, "shininess")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),&shape->shininess,1))
          shape->shininessOn = true;
      }
      else if (isEqual(attr, "transparency")) {
        if (VectorTools::parseFloats(xmlnode->getAttributeValue(i),&shape->transparency,1))
          shape->transparencyOn = true;
      }
      else if (isEqual(attr, "DEF")) {
        //error("We have an object DEF name: %s", xmlnode->getAttributeValue(i));
        shape->name = xmlnode->getAttributeValue(i);
      }
    }
  }
  else if (isEqual(nodeName, "ImageTexture")) {
    // on modifie la texture
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "url"))
        shape->texture = Texture::getFromCache(xmlnode->getAttributeValue(i));
    }
  }
  else if (isEqual(nodeName, "Coordinate")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "point"))
        VectorTools::parseCertifiedVectors(xmlnode->getAttributeValue(i),&shape->meshInfos.Coordinate,3);
    }
  }
  else if (isEqual(nodeName, "TextureCoordinate")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "point"))
        VectorTools::parseCertifiedVectors(xmlnode->getAttributeValue(i),&shape->meshInfos.TextureCoordinate,2);
    }
  }
  else if (isEqual(nodeName, "Color")) {
    for (int i=0; i < nattr; i++) {
      attr = xmlnode->getAttributeName(i);
      if (isEqual(attr, "color"))
        VectorTools::parseCertifiedVectors(xmlnode->getAttributeValue(i),&shape->meshInfos.Color,3);
    }
  }
  else if (isEqual(nodeName, "ColorInterpolator") ||
           isEqual(nodeName, "PositionInterpolator") ||
           isEqual(nodeName, "ScaleInterpolator") ||
           isEqual(nodeName, "OrientationInterpolator") ||
           isEqual(nodeName, "ScalarInterpolator")) {
    Interpolator temp;

    if (temp.initArrays(xmlnode)) {
      interpolators.push_back(temp);
    }
    else error("interpolator rejected !");
  }
  else if (isEqual(nodeName, "TimeSensor")) {
    TimeSensor temp;

    if (temp.initSensor(xmlnode)) timeSensors.push_back(temp);
    else error("timesensor rejected !");
  }
  else if (isEqual(nodeName, "ROUTE")) {
    Route temp;

    if (temp.initRoute(xmlnode)) routes.push_back(temp);
    else error("route rejected !");
  }
  else {        // is it a PRIMITIVE ???
    X3dShapes vrengPrimitiveIndex = X3DNONE;

    if (X3DNONE != (vrengPrimitiveIndex = isKnownPrimitive(xmlnode->getName()))) {
      GLuint newPrimitive = drawPrimitive(vrengPrimitiveIndex);
      shape->meshes.push_back(newPrimitive);
    }
  }

  // we take care of the children nodes
  int nchild = xmlnode->nChildNode();
  for (int i=0; i < nchild; i++) {
    XMLNode mynode = xmlnode->getChildNode(i);
    browseX3dTree(&mynode, shape);
  }
  if (isEqual(nodeName, "IndexedFaceSet")) {
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
        VectorTools::parseVectors(temp,&shape->meshInfos.coordIndex);
      }
      else if (isEqual(attr,"texCoordIndex")) {
        string temp(xmlnode->getAttributeValue(i));
        uint32_t offset = temp.find("-1");
        while (offset != (uint32_t) string::npos) {
          temp.replace(offset, 2, ", ");
          offset = temp.find("-1");
        }
        VectorTools::parseVectors(temp,&shape->meshInfos.texCoordIndex);
      }
      else if (isEqual(attr,"colorIndex")) {
        string temp(xmlnode->getAttributeValue(i));
        uint32_t offset = temp.find("-1");
        while (offset != (uint32_t) string::npos) {
          temp.replace(offset, 2, ", ");
          offset = temp.find("-1");
        }
        VectorTools::parseVectors(temp, &shape->meshInfos.colorIndex);
      }
    }
    // we draw the complex mesh with the stored data
    GLuint newPrimitive = drawMesh(&shape->meshInfos);
    shape->meshes.push_back(newPrimitive);

    // we reset everything for the next indexedFaceSet
    shape->meshInfos.coordIndex.clear();
    shape->meshInfos.texCoordIndex.clear();
    shape->meshInfos.colorIndex.clear();
    shape->meshInfos.Coordinate.clear();
    shape->meshInfos.TextureCoordinate.clear();
    shape->meshInfos.Color.clear();
  }
}

void X3d::setupInterpolators()
{
  for (vector<Route>::iterator route = routes.begin(); route != routes.end(); route++) {
    X3DOUTField fromfield = route->fromField;
    X3DINField tofield = route->toField;

    // timer to interpolator route
    if (fromfield == FRACTION_CHANGED && tofield == SET_FRACTION) {
      TimeSensor* sensor = findItem<TimeSensor>(&timeSensors, route->fromNode);
      Interpolator* interpolator = findItem<Interpolator>(&interpolators, route->toNode);

      if (sensor && interpolator)
        sensor->initTarget(interpolator);
      else error("route sensor->interpolator buggee: de %p a %p", sensor, interpolator);
    }

    else if (fromfield == VALUE_CHANGED && tofield >= TRANSLATION && tofield <= TRANSPARENCY) {
      // interpolator to shape field
      Interpolator* interpolator = findItem<Interpolator>(&interpolators, route->fromNode);
      X3dShape* targetShape = findShape(&rootShape, route->toNode);

      if (interpolator && targetShape) {
        interpolator->initTarget(targetShape, tofield);
      }
      else error("route interpolator->x3dshape buggee: de %p a %p", interpolator, targetShape);
    }
    else
      error("bugged route with improper fields from %s to %s", route->fromNode.c_str(), route->toNode.c_str());
  } //end loop on routes

  //error("initialisation des liens d'animation done");
  routes.clear();
}

X3dShape* X3d::findShape(X3dShape* root, string name)
{
  if (root->getName() == name) return root;

  X3dShape* temp = NULL;
  //recursivity
  for (int i=0; i < root->childrenShapes.size(); i++) {
    temp = findShape(root->childrenShapes[i], name);
    if (temp) return temp;
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

GLuint X3d::drawPrimitive(X3dShapes id)
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
    GLfloat x = .5, y = .5, z = 1; //For the box drawing;

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
#if 0 //dax debug
  // we display what we have in the temporary structure
  error("New mesh in creation:");
  printf("coordindex: ");
  VectorTools::displayVector(& meshInfos->coordIndex);
  printf("\n");

  printf("texcoordindex: ");
  VectorTools::displayVector(& meshInfos->texCoordIndex);
  printf("\n");

  printf("colordindex: ");
  VectorTools::displayVector(& meshInfos->colorIndex);
  printf("\n");

  printf("coordinate: ");
  VectorTools::displayVector(& meshInfos->Coordinate);
  printf("\n");

  printf("texturecoordinate: ");
  VectorTools::displayVector(& meshInfos->TextureCoordinate);
  printf("\n");

  printf("color: ");
  VectorTools::displayVector(& meshInfos->Color);
  printf("\n");
#endif

  GLuint dList = glGenLists(1);;
  glNewList(dList, GL_COMPILE);

  // we browse the vectors containing a polygon each
  for (int polygonNum=0; polygonNum < meshInfos->coordIndex.size(); polygonNum++) {
    glBegin(GL_POLYGON);	// create a polygon

    // one color per polygon
    if (meshInfos->colorPerVertex == false) {

      // we really have at least a single list of colors
      if (0 < meshInfos->colorIndex.size()) {

        // if a color exists at that index
        if (polygonNum < meshInfos->colorIndex.front().size()) {

          // this is the color index we retrieve from the index list
          uint32_t tempRealColor = (uint32_t)meshInfos->colorIndex.front()[polygonNum];
          if (tempRealColor>=(uint32_t)meshInfos->Color.size())
            error("index de couleur hors des limites !");
          else {
            uint32_t realColor = (uint32_t) tempRealColor;

            // if the color is well formed
            if (meshInfos->Color[realColor].size() == 3) {
              glColor3f(
                meshInfos->Color[realColor][0],
                meshInfos->Color[realColor][1],
                meshInfos->Color[realColor][2]);
            }
          }
        }
      }
    }

    // we browse the vertexes of one polygon
    for (int vertexNum=0; vertexNum < meshInfos->coordIndex[polygonNum].size(); vertexNum++) {

      // this is the vertex index we retrieve from the index list
      uint32_t tempRealVertex = (uint32_t)meshInfos->coordIndex[polygonNum][vertexNum];
      uint32_t realVertex = (uint32_t) tempRealVertex;

      //dax if (meshInfos->Coordinate.size() == 0) break;	//dax

      // if a vertex exists at that index
      if (realVertex < meshInfos->Coordinate.size()) {

        // if the vertex is well formed
        if (meshInfos->Coordinate[realVertex].size() == 3) {

          // one color per polygon
          if (meshInfos->colorPerVertex == true) {

            // we really have a list of colors for that polygon
            if (polygonNum < meshInfos->colorIndex.size()) {

              // if a color index exists for that vertex in the polygon
              if (vertexNum < meshInfos->colorIndex[polygonNum].size()) {

                // this is the color index we retrieve from the index list
                uint32_t tempRealColor = (uint32_t)meshInfos->colorIndex[polygonNum][vertexNum];
                if (tempRealColor>=(uint32_t)meshInfos->Color.size())
                  error("index de couleur hors des limites !");
                else {
                  uint32_t realColor = (uint32_t) tempRealColor;

                  // if the color is well formed
                  if (meshInfos->Color[realColor].size() == 3) {
                    glColor3f(
                      meshInfos->Color[realVertex][0],
                      meshInfos->Color[realVertex][1],
                      meshInfos->Color[realVertex][2]);
                  }
                }
              }
            }
          }
          // texture coords to apply ?
          if (polygonNum < meshInfos->texCoordIndex.size() &&
              vertexNum < meshInfos->texCoordIndex[polygonNum].size()) {
            uint32_t tempRealTex = (uint32_t)meshInfos->texCoordIndex[polygonNum][vertexNum];
            uint32_t realTex = (uint32_t) tempRealTex;

            // if a texturecoord exists at that index
            if (realTex < meshInfos->TextureCoordinate.size()) {
              // if the texcoord is well formed
              if (meshInfos->TextureCoordinate[realTex].size() == 2)
                glTexCoord2f(meshInfos->TextureCoordinate[realTex][0], meshInfos->TextureCoordinate[realTex][1]);
            }
          }
          // we put a vertex, after all that
          glVertex3f(
            meshInfos->Coordinate[realVertex][0],
            meshInfos->Coordinate[realVertex][1],
            meshInfos->Coordinate[realVertex][2]);
        }
        else error("coordonnees du sommet incorrectes");
      }
      else {
        error("index de sommet hors-limites %d > %d", realVertex,meshInfos->Coordinate.size());
      }
    }
    glEnd();
  }
  glEndList();
  return dList;
}

void X3d::displayShape(X3dShape* myShape) //NOT RECURSIVE !!
{
  //error("On display la shape: %d au niveau %d",(int) myShape, myShape->level);

  float reset[4] = {0,0,0,1};
  glMaterialfv(GL_FRONT, GL_AMBIENT, reset);

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
    //error("on affiche la dlist %d", *i);
    glCallList(*i);
    if (selected) {  //selected mesh
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
  float shin = 1;
  glColor4f(0,0,0,1); //must track the default ambient material wanted else problems with gl_color_material
  float reset1[4] = {0,0,0,1};
  float reset2[4] = {1,1,1,1};
  glMaterialfv(GL_FRONT, GL_AMBIENT, reset1);
  glMaterialfv(GL_FRONT, GL_SPECULAR, reset2);
  glMaterialfv(GL_FRONT, GL_SHININESS, &shin);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, reset1);
  glMaterialfv(GL_FRONT, GL_EMISSION, reset1);

  // we update the timers for animation
  for (vector<TimeSensor>::iterator t = timeSensors.begin(); t != timeSensors.end(); t++) {
    t->updateFraction(animationOn);
  }

  // list to make an iterative treatment of the tree
  vector<X3dShape*> nextShapes;
  nextShapes.push_back(&rootShape);

  X3dShape* currentShape;
  int previousLevel = -1;
  int npop=0, npush=0;

  while (! nextShapes.empty()) {
    // we take the next node of the tree
    currentShape = nextShapes.back();
    nextShapes.pop_back();

    // we change the matrix stack according to the new change of level in the X3D tree
    // and if we switch to a child, we don't touch the current (parent) matrix
    if (currentShape->level<=previousLevel) {
      glPopMatrix(); // we remove the matrix of the current shape
      //dax1 glPopAttrib();
      npop++;
      if (currentShape->level<previousLevel) {
        glPopMatrix(); // we aso remove the matrix of the previous shape because we went down
        //dax1 glPopAttrib();
        npop++;
      }
    }
    previousLevel = currentShape->level;

    glPushMatrix();
    npush++;

    if (currentShape->texture) {  //texture is there in priority
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture(GL_TEXTURE_2D, currentShape->texture);
    }
    else if (!currentShape->diffuseOn) {  // we'll use glColors instead of diffuse material
      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }
    //else
    //error("materiaux actives *****");

    displayShape(currentShape); // call the display of the shape

    // Reset
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    for (vector<X3dShape*>::iterator j = currentShape->childrenShapes.begin(); j!=currentShape->childrenShapes.end(); j++)
      nextShapes.push_back(*j);
  }

  // we remove the remaining matrixes after having browsed the complete tree
  for (int k = -1; k < previousLevel; k++) {
    glPopMatrix();
    npop++;
  }
}

/* returns the X3DOBJECT index of the primitive */
X3dShapes X3d::isKnownPrimitive(XMLCSTR vredata)
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
  selected = true;
}

void X3d::resetFlashy()
{
  selected = false;
}

void X3d::resetAnimations()
{
  for (vector<TimeSensor>::iterator t = timeSensors.begin(); t != timeSensors.end(); t++) {
    t->resetFraction();
  }
}


/*
 * VectorTools methods
 */

bool VectorTools::parseFloats(const string str, float* outputs, uint32_t number)
{
  vector<vector<float> > tempOutput;

  bool res = parseCertifiedVectors(str, &tempOutput, number, 1);

  if (res) {
    vector<float> myVector = tempOutput.front();
    for (int i=0; i<number; i++) {
      //outputs[i]=myVector.at(i);
      outputs[i] = myVector[i];
    }
    return true;
  }
  return false;
}

/* returns true if succeeded, false else */
bool VectorTools::parseFloats(const string str, vector<float>* output)
{
  vector<vector<float> > tempOutput;

  bool res = parseCertifiedVectors(str, &tempOutput, 0, 1);

  if (res) {
    vector<float> myVector = tempOutput.front();
    vector<float>::iterator i = myVector.begin();
    vector<float>::iterator j = myVector.end();
    output->assign(i, j);
    return true;
  }
  return false;
}

bool VectorTools::parseCertifiedVectors(const string str, vector<vector<float> >* outputs, uint32_t vectorLength, uint32_t numVector)
{
  bool res = parseVectors(str, outputs);
  //error("parsevectors return %d", res);

  if (numVector > 0 && outputs->size() != numVector) {
    error("Trop de vecteurs ont ete parses");
    return false;
  }

  if (vectorLength > 0) {
    for (vector<vector<float> >::iterator i=outputs->begin(); i!=outputs->end();i++) {
      if (i->size() != vectorLength) {
        error("des vecteurs trop longs ont ete parses");
        return false;
      }
    }
  }
  return res;
}

bool VectorTools::parseVectors(const string str, vector<vector<float> > *outputs)
{
  int limit = str.size();
  int in = 0;
  int out = 0;
  float newFloat;

  char* temp = new char[limit+1]; //to store chars before conversion to float

  vector<float> tempOutput;

  while (in < limit) {

    while (in < limit &&
           (str[in] == '\n' ||
            str[in] == '\r' ||
            str[in] == ' ' ||
            str[in] == ',')) {
      in++;  // we skip initial spaces and semicolons

      if (str[in] == ',') {  //end of a vector
        if (!tempOutput.empty()) {
          outputs->push_back(tempOutput);
          tempOutput.clear();
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
        error("non digit character '%c' detected ! digit parsing aborted !",str[in]);
        return false;
      }

      temp[out] = str[in];
      in++;
      out++;
    }

    temp[out] = '\0';
    out = 0;  // we begin again forming a float

    if (strlen(temp) != 0) {  // we have something to convert
      sscanf(temp, "%f", &newFloat); // 0.0 if conversion error
      tempOutput.push_back(newFloat);
    }
    if (str[in] == ',' || in >= limit) {  // end of a vector
      if (!tempOutput.empty()) {
        outputs->push_back(tempOutput);
        tempOutput.clear();
      }
    }
  }

  delete[] temp;
  return true;
}

void VectorTools::displayVector(vector<vector<float> >* outputs)
{
  for (vector<vector<float> >::iterator i = outputs->begin(); i != outputs->end(); i++) {
    for (vector<float>::iterator j = i->begin(); j != i->end(); j++)
      printf("%.3f ", *j);
  }
}

void VectorTools::displayVector(vector<float>* outputs)
{
  for (vector<float>::iterator i = outputs->begin(); i != outputs->end(); i++)
    printf("%.3f ", *i);
}


/*
 * Interpolator methods
 */

bool Interpolator::initArrays(XMLNode* xmlnode)
{
  XMLCSTR nodeName = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  int vectorSize = 0;

  if (X3d::isEqual(nodeName, "PositionInterpolator")) {
    type = POSITIONINTERPOLATOR;
    vectorSize = 3;
  }
  else if (X3d::isEqual(nodeName, "OrientationInterpolator")) {
    type = ROTATIONINTERPOLATOR;
    vectorSize = 4;
  }
  else if (X3d::isEqual(nodeName, "ScaleInterpolator")) {
    type = SCALEINTERPOLATOR;
    vectorSize = 3;
  }
  else if (X3d::isEqual(nodeName, "ColorInterpolator")) {
    type = COLORINTERPOLATOR;
    vectorSize = 3;
  }
  else if (X3d::isEqual(nodeName, "ScalarInterpolator")) {
    type = SCALARINTERPOLATOR;
    vectorSize = 1;
  }
  else {
    error("%s is not an interpolator", nodeName);
    return false;
  }

  for (int i=0; i < nattr; i++) {
    attr = xmlnode->getAttributeName(i);

    if (X3d::isEqual(attr, "DEF"))
      name = xmlnode->getAttributeValue(i);
    else if (X3d::isEqual(attr, "key"))
      VectorTools::parseFloats(xmlnode->getAttributeValue(i), &keys);
    else if (X3d::isEqual(attr, "keyValue"))
      VectorTools::parseCertifiedVectors(xmlnode->getAttributeValue(i), &keyValues, vectorSize);
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
    VectorTools::displayVector(&keys);
    error("interpolator bounding keys must be 0 and 1, not %.2f and %.2f, in %s", keys.front(), keys.back(),name.c_str());
    return false;
  }

  for (int i=0; i < keys.size()-1; i++) {
    if (keys[i] >= keys[i+1]) {
      error("interpolator keys must be inreasing in %s", name.c_str());
      return false;
    }
  }

  //error("interpolator added: %s of type %d", name.c_str(), type);
  //printf("Valeurs Key: ");
  //VectorTools::displayVector(&keys);
  //printf("\n");
  //printf("Valeurs KeyValues: ");
  //VectorTools::displayVector(&keyValues);
  //printf("\n");

  return true;
}

bool Interpolator::initTarget(X3dShape* target, X3DINField field)
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
    //error("route ajoutee entre interpolator -%s- et X3dShape -%s- pour champ %d", name.c_str(), target->name.c_str(), field);
    return true;
  }
  else {
    error("Interpolator init error: unproper field type %d for interpolator type %d",field,type);
    return false;
  }
}

/* updates the target values in the shapes according to the time raction we receive */
void Interpolator::updateValue(float newFraction)
{
  int index = -1;	// left index of the interpolation key
  float percentage = 0; // % of interpolation between keyValues index and index+1

  if (targets.size() == 0) {
    error("No targets for the updated Interpolator %s", name.c_str());
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
        //error("Vector: value %.2d between %.2f and %.2f", i,keyValues[index][i],keyValues[index+1][i]);
        for (int j=0; j < targets.size(); j++)
          targets[j][i] = t[i];
      }
      //error("Vector interpolation: %.2f %.2f %.2f", t[0],t[1],t[2]);
    }
    break;

  case SCALARINTERPOLATOR:
      {
      float tempScalar;

      tempScalar = (1-percentage)*keyValues[index][0] + percentage*keyValues[index+1][0];
      //error("Scalar: Value between %.2f and %.2f", keyValues[index][0],keyValues[index+1][0]);
      for (int j=0; j < targets.size(); j++) {
        *targets[j] = tempScalar;
      }
      //error("Scalar interpolation: %.2f", tempScalar);
      }
      break;

  case ROTATIONINTERPOLATOR:
      {
      float tempVect[4];

      for (int i=0; i<3; i++) {
      tempVect[i] = (1-percentage)*keyValues[index][i] + percentage*keyValues[index+1][i];
        //error("Vector: Value %d between %.2f and %.2f", i,keyValues[index][i],keyValues[index+1][i]);
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

      //error("ROTATION: fraction=%.2f, pourcentage=%.2f, entre %.2f et %.2f",
      //newFraction,percentage,keyValues[index][3],targetAngle);
      //error("Vector 3 interpolation: %.2f %.2f %.2f",t[0],t[1],t[2]);

#if 0 //QUATERNION
      // QUATERNION INTERPOLATION HERE INSTEAD ????
      /*float tempRot[4];

      float q;
      for (int i=0; i<3; i++) {
        for (int j=0; j < targets.size(); j++) {
          targets[j][i] = tempRot[i];
        }
      }
      //error("Scalar interpolation : %.2f", tempScalar);*/
#endif //QUATERNION
    }
    break;

  default:
    error("unproper interpolator type in %s", name.c_str());
    break;
  }
}


/*
 * TimeSensor methods
 */

bool TimeSensor::initSensor(XMLNode* xmlnode)
{
  XMLCSTR nodeName = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (! X3d::isEqual(nodeName, "TimeSensor")) {
    error("this node is not a timeSensor");
    return false;
  }

  for (int i=0; i < nattr; i++) {
    attr = xmlnode->getAttributeName(i);

    if (X3d::isEqual(attr, "DEF")) {
      name = strdup(xmlnode->getAttributeValue(i));
    }
    else if (X3d::isEqual(attr, "cycleInterval")) {
      VectorTools::parseFloats(xmlnode->getAttributeValue(i), &cycleIntervalMs, 1);
      cycleIntervalMs *= 1000;
    }
    else if (X3d::isEqual(attr, "loop")) {
      if (X3d::isEqual(xmlnode->getAttributeValue(i),"false"))
        loop = false;
    }
  }

  if (name == "") {
    error("no name for the sensor");
    return false;
  }
  if (cycleIntervalMs <= 0) {
    error("cycleInterval <= 0 in sensor");
    return false;
  }

  //error("TimeSensor ajoute: %s", name.c_str());
  //error("cycleIntervalMs: %.2f", cycleIntervalMs);
  //error("loop: %d", loop);

  return true;
}

void TimeSensor::initTarget(Interpolator* interpolator)
{
  targets.push_back(interpolator);
  //error("route ajoutee entre initSensor -%s- et interpolator -%s-", name.c_str(), interpolator->getName().c_str());
}

void TimeSensor::resetFraction()
{
  fraction = 0;

  for (vector<Interpolator*>::iterator it = targets.begin(); it != targets.end(); it++) {
    (*it)->updateValue(fraction);
  }
}

void TimeSensor::updateFraction(bool animationOn)
{
  if (fraction < 0) {
    error("Fraction is negative in timer");
    fraction = 0;
  }

  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  if (animationOn) {
    int32_t diffSec = currentTime.tv_sec - previousTime.tv_sec;
    int32_t diffMilliSec = (currentTime.tv_usec - previousTime.tv_usec)/1000;
    int diffms = (int) (1000*diffSec + diffMilliSec);
    //error("%s says: difference of %d ms",name.c_str(), diffms);

    float fractionIncrement = (float)diffms/cycleIntervalMs;

    fraction += fractionIncrement;
    if (fraction > 1) {
      if (loop) {
        fraction = fraction - (int)fraction;
      }
      else fraction = 1;
    }
    //error("%s says: new increment is %.2f",name.c_str(), fraction);

    for (vector<Interpolator*>::iterator it = targets.begin(); it != targets.end(); it++) {
      (*it)->updateValue(fraction);
    }
  }
  previousTime = currentTime;
}


/*
 * Route methods
 */

bool Route::initRoute(XMLNode* xmlnode)
{
  XMLCSTR nodeName = xmlnode->getName();
  int nattr = xmlnode->nAttribute();
  const char* attr = NULL;

  if (!X3d::isEqual(nodeName, "Route")) {
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
      X3DOUTField tempField = NOOUTX3DFIELD;

      if      (X3d::isEqual(field.c_str(), "FRACTION_CHANGED")) tempField = FRACTION_CHANGED;
      else if (X3d::isEqual(field.c_str(), "VALUE_CHANGED")) tempField = VALUE_CHANGED;
      fromField = tempField;
    }
    else if (X3d::isEqual(attr, "toField")) {
      string field = xmlnode->getAttributeValue(i);
      X3DINField tempField = NOINX3DFIELD;

      if      (X3d::isEqual(field.c_str(), "SET_FRACTION")) tempField = SET_FRACTION;
      else if (X3d::isEqual(field.c_str(), "TRANSLATION")) tempField = TRANSLATION;
      else if (X3d::isEqual(field.c_str(), "ROTATION")) tempField = ROTATION;
      else if (X3d::isEqual(field.c_str(), "SCALE")) tempField = SCALE;
      else if (X3d::isEqual(field.c_str(), "DIFFUSECOLOR")) tempField = DIFFUSECOLOR;
      else if (X3d::isEqual(field.c_str(), "SPECULARCOLOR")) tempField = SPECULARCOLOR;
      else if (X3d::isEqual(field.c_str(), "EMISSIVECOLOR")) tempField = EMISSIVECOLOR;
      else if (X3d::isEqual(field.c_str(), "AMBIENTINTENSITY")) tempField = AMBIENTINTENSITY;
      else if (X3d::isEqual(field.c_str(), "SHININESS")) tempField = SHININESS;
      else if (X3d::isEqual(field.c_str(), "TRANSPARENCY")) tempField = TRANSPARENCY;
      toField = tempField;
    }
  } //end "for all attribs"

  if (fromNode == "" ||
      toNode == "" ||
      fromField == NOOUTX3DFIELD ||
      toField == NOINX3DFIELD)
    return false;

  //error("structure route temporaire ajoutee");
  //error("fromNode: %s", fromNode.c_str());
  //error("fromField: %d", fromField);
  //error("toNode: %s", toNode.c_str());
  //error("toField  %d", toField);

  return true;
}
