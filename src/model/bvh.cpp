/*****************************************************************************

  bioviewer
    
    Copyright (C) 2002  Lucas Walter

    lucasw@u.washington.edu
    http://students.washington.edu/lucasw

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

******************************************************************************/
#include "vreng.hpp"
#include "bvh.hpp"
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open, close
#include "obj.hpp"	// class Obj

using namespace std;


Bvh::Bvh(const char *url)
{
  verbose = false;
  string bvhfile;
  download(url);
  Cache::getCacheName(url, bvhfile);
  init(bvhfile);
}

bvhPart::bvhPart()
{
  objPart = NULL;
}

void Bvh::download(const char *_url)
{
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

char * Bvh::getUrl() const
{
  return (char *) url;
}

void Bvh::reader(void *_bvh, Http *http)
{
  Bvh *bvh = (Bvh *) _bvh;
  if (! bvh) return;

  FILE *f = NULL;
  Cache *cache = new Cache();
  f = cache->open(bvh->getUrl(), http);
  if (f) {
    cache->close();
    delete cache;
  }
}

void Bvh::process(string line)
{  
  if (line == "OFFSET") {
    vertIndex = 0;  
    theMode = OFFSET;
  }
  else if (line == "ROOT") {
    root = new bvhPart;
    root->parent = NULL;
    current = root;
    theMode = ROOT;
  }
  else if (line == "{") {
    ;  
  }
  else if (line == "}") {
    if (current != root) { 
      current = current->parent;
      theMode = NONE;
    }
  }
  else if (line == "JOINT") {
    bvhPart *temp = new bvhPart;
    temp->parent = current;
    current->child.push_back(temp);
    current = temp;
    theMode = JOINT; 
  }
  else if (line == "CHANNELS") {
    theMode = CHANNELS;
  }
  else if (line == "End") {  
    theMode = End;
  }
  else if (line == "Site") {
    bvhPart *temp = new bvhPart;
    temp->parent = current;
    current->child.push_back(temp);
    current = temp;
    theMode = NONE; //Site;
  }
  else if (line == "MOTION") {
    theMode = MOTION;
  }
  else if (line == "Frames:") {
    theMode = Frames;
  }
  else if (line == "Frame") {
    theMode = Frame;
  }
  else if (line == "Time:") {
    theMode = Time;    
  }
  else {
    switch (theMode) {
      case (ROOT):
        root->name = line;
        theMode = NONE;
        break;
      case (JOINT):
        current->name = line;
        
        if (line == "LeftKnee") {
          try { current->objPart = new rigid("/obj/shin_l.obj"); }
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightKnee") {
          try { current->objPart = new rigid("/obj/shin_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "LeftHip") {
          try { current->objPart = new rigid("/obj/thigh_l.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightHip") {
          try { current->objPart = new rigid("/obj/thigh_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "LeftAnkle") {
          try { current->objPart = new rigid("/obj/foot_l.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightAnkle") {
          try { current->objPart = new rigid("/obj/foot_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "Chest") {
          //current->objPart = new objloader("obj/torso.obj",0,subdir);
        }
        else if (line == "Chest2") {
          try { current->objPart = new rigid("/obj/chest.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "LeftShoulder") {
          try { current->objPart = new rigid("/obj/shoulder_l.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightShoulder") {
          try { current->objPart = new rigid("/obj/shoulder_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "LeftElbow") {
          try { current->objPart = new rigid("/obj/forearm_l.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightElbow") {
          try { current->objPart = new rigid("/obj/forearm_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "LeftWrist") {
          try { current->objPart = new rigid("/obj/hand_l.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "RightWrist") {
          try { current->objPart = new rigid("/obj/hand_r.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "Neck") {
          try { current->objPart = new rigid("/obj/neck.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        else if (line == "Head") {
          try { current->objPart = new rigid("/obj/head.obj");}
          catch (fileNotFound) { throw fileNotFound(); return; }
          current->objPart->location.matrix[10]=1.0f;
        }
        theMode = NONE;
        break;
      case (OFFSET):
        current->offset.vertex[vertIndex] = atof(line.c_str());
        vertIndex++;
        if (vertIndex >2) {
          vertIndex = 0;
          theMode = NONE;
        }
        break;
      case (CHANNELS):
        // assume channelsNum == 0 in the .bvh .is impossible
        if (channelsNum == 0) {
          channelsNum = atoi(line.c_str());
        }
        else if (line == "Xposition") {    
          current->channels.push_back(bvhPart::Xpos);  
        }
        else if (line == "Yposition") {          
          current->channels.push_back(bvhPart::Ypos);  
        }
        else if (line == "Zposition") {        
          current->channels.push_back(bvhPart::Zpos);  
        }
        else if (line == "Zrotation") {
          current->channels.push_back(bvhPart::Zrot);        
        }
        else if (line == "Yrotation") {
          current->channels.push_back(bvhPart::Yrot);        
        }
        else if (line == "Xrotation") {
          current->channels.push_back(bvhPart::Xrot);        
        }
        // if there are additional channel types in error, they'll be ignored(?)
        if (current->channels.size() == channelsNum) {
          theMode = NONE;
          channelsNum = 0;
        }
        break;
      case (Frames):
        framesNum = atoi(line.c_str());
        theMode = NONE;
        break;
      case (Frame):
        break;
      case (Time):
        frameTime = atof(line.c_str());
        theMode = MOTIONDATA;
        current = root;
        recurs(root);
        break;
      case (MOTIONDATA):
        data++;
        //echo("%d: %s", data, line.c_str());
        switch (bvhPartsLinear[partIndex]->channels[channelIndex]) {
          case (bvhPart::Xpos):  
            tempMotion.translate(atof(line.c_str()),0,0);  
            channelIndex++;
            break;
          case (bvhPart::Ypos):
            tempMotion.translate(0,atof(line.c_str()),0);
            channelIndex++;
            break;
          case (bvhPart::Zpos):
            tempMotion.translate(0,0,atof(line.c_str()));
            channelIndex++;
            break;
          case (bvhPart::Zrot):
            // it seems like some limbs need a negative, and out limbs don't
            tempMotionZ.RotateZ((float)-DEG2RAD(atof(line.c_str())));
            channelIndex++;
            break;
          case (bvhPart::Yrot):
            //if (partIndex == 3) cout << atof(line.c_str()) << "\n";
            tempMotionY.RotateY((float)-DEG2RAD(atof(line.c_str())));
            //tempMotion.print();
            channelIndex++;
            break;
          case (bvhPart::Xrot):
            //if (partIndex == 3) cout << atof(line.c_str()) << "\n";
            tempMotionX.RotateX((float)-DEG2RAD(atof(line.c_str())));  
            channelIndex++;
            break;
        }
        //echo("channelIndex=%d partIndex=%d", channelIndex, partIndex);
        if (channelIndex >= bvhPartsLinear[partIndex]->channels.size()) {
          // store tempMotion and move to next part
          tempMotion = tempMotion * (tempMotionZ *tempMotionX * tempMotionY );
          bvhPartsLinear[partIndex]->motion.push_back(tempMotion);
          tempMotion.identity();
          tempMotionX.identity();
          tempMotionY.identity();
          tempMotionZ.identity();
          channelIndex = 0;
          partIndex++;
        }  
        if (partIndex >= bvhPartsLinear.size()) {
          // this should be the end of one line of motion data
          //echo("end motion line");
          partIndex = 0;
        }
        break;
      case (NONE):
      case (End):
      case (Site):
        break;
      case (MOTION):
        echo("MOTION");
        break;
    }
  }
}

void Bvh::recurs(bvhPart* some)
{
  //cout << "Bvh::recurs " << some->name << endl;
  matrix16f motion0;
  motion0.identity();
   
  motion0.translate(some->offset.vertex[0], some->offset.vertex[1], some->offset.vertex[2]);
  some->motion.push_back(motion0);
  if (some->child.size() != 0) bvhPartsLinear.push_back(some);
    //cout << some->name << ": "
    //     << some->offset.vertex[0]
    //     << " "
    //     << some->offset.vertex[1]
    //     << " "
    //     << some->offset.vertex[2]
    //     << endl;
  for (uint32_t i=0; i < some->child.size(); i++) {
    //cout << "recurs-child " << some->child[i]->name << " (" << i << ")" << endl;
    recurs(some->child[i]);
  }
}
 
void Bvh::init(string bvhFile)
{
  //echo("Bvh::init");
  data = 0;
  partIndex = 0;
  channelIndex = 0;  

  tempMotion.identity();
  tempMotionX.identity();
  tempMotionY.identity();
  tempMotionZ.identity();
  
  ifstream bvhStream(bvhFile.c_str());
  if (!bvhStream) {
    error("File %s not found", bvhFile.c_str());
    throw fileNotFound(); 
    return;
  }
  //echo("File %s found", bvhFile.c_str());
  istream_iterator<string> bvhIt(bvhStream);
  istream_iterator<string> sentinel;

  vector<string> lines(bvhIt, sentinel);
  
  // for_each(lines.begin(),lines.end(),&bvh::process);
  for (int i=0; i < lines.size(); i++) {
    try { process(lines[i]); }
    catch (fileNotFound) { throw fileNotFound(); return; }
  }
  
  bvhStream.close();
  
  framesNum = bvhPartsLinear[0]->motion.size();
  
  if (verbose) cout << "framesNum = " << framesNum << "\nframetime = " << frameTime << "\n";
  if (verbose) cout << data/(framesNum*3)-1 << " bodyparts\n";
}


rigid::rigid(const char *url)
{
  echo("rigid::rigid url=%s", url);
#if 0 //dax
  string objfile;
  download(url);
  Cache::getCacheName(url, objfile);

  theObj = new objloader(objfile);
#else
  download(url);
  Cache::getFilePath(url);
#endif
  //vertices = theObj->vertices;
  //normals = theObj->normals;
  //mtls = theObj->mtls;
  //translucent  = theObj->translucent;
  //centerOfMass = theObj->centerOfMass;
  //drawSurface = true;
}

void rigid::download(const char *_url)
{
  echo("rigid::download %s", url);
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

const char * rigid::getUrl() const
{
  return (const char *) url;
}

void rigid::reader(void *_rigid, Http *http)
{
  rigid *rigi = (rigid *) _rigid;
  if (! _rigid) return;

  Cache *cache = new Cache();
  FILE *f = NULL;
  f = cache->open(rigi->getUrl(), http);
  if (f) {
    cache->close();
    delete cache;
  }
}

#if 0 //notcalled
void rigid::draw()
{
  echo("rigid::draw");
  glPushMatrix();
  if (drawBB) drawAABB();
  glMultMatrixf(location.matrix);

  glEnable(GL_LIGHTING);
  
  if (drawSurface) makeList();
   glCallList(listNum);
  
  glDisable(GL_LIGHTING);

#ifdef SHOW_TRI_NORMS
  glColor3f(0, 0.6, 0.9);
  glBegin(GL_LINES);
  for (int m=0; m < mtls.size(); m++) {
    for (int i=0; i <mtls[m]->faces.size(); i++) {
      glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[0]-1].vertex);
      glVertex3fv(((vertices[mtls[m]->faces[i].vertIndices[0]-1] + mtls[m]->faces[i].normal)).vertex);
    }
  }
  glEnd();  
#endif

#ifdef SHOW_INT_POINTS
    glBegin (GL_POINTS);
    glColor3f(0.3, 0.6, 0.9);
    for (int i=0; i < theObj->iStack.size(); i++) {    
      glVertex3fv(theObj->iStack[i].vertex);
    }
    glColor3f(0.9, 0.6, 0.3);
    for (int i=0; i < theObj->upperI.size(); i++) {    
      glVertex3fv(theObj->upperI[i].vertex);  
    }
    glColor3f(0.3, 0.9, 0.3);
    for (int i=0; i < theObj->lowerI.size(); i++) {    
      glVertex3fv(theObj->lowerI[i].vertex);  
    }
    glEnd ();
#endif
  //makeList();
  if (drawBB) {
    drawBoundingBox();

    // draw Center of Mass
    glBegin(GL_LINES);
    glColor3f(0.2f,0.3f,1.0f);
    vector3f up(0,0.5,0);
    glVertex3fv((centerOfMass+up).vertex);
    glVertex3fv((centerOfMass-up).vertex);
    vector3f right(.5,0,0);
    glVertex3fv((centerOfMass+right).vertex);
    glVertex3fv((centerOfMass-right).vertex);
    vector3f out(0,0,.5);
    glVertex3fv((centerOfMass+out).vertex);
    glVertex3fv((centerOfMass-out).vertex);
    glEnd();
  }
  glPopMatrix();
  return;
}
#endif //notcalled

#if 0 //notcalled
void rigid::drawDim(vector<light*> lights)
{
  echo("rigid::drawDim");
  glPushMatrix();
  glMultMatrixf(location.matrix);
  glEnable(GL_LIGHTING);

  for (uint32_t m=0; m < mtls.size(); m++) {
      glMaterialfv(GL_FRONT,GL_AMBIENT, mtls[m]->Ka);
      glMaterialfv(GL_FRONT,GL_DIFFUSE, mtls[m]->Kd);
      glMaterialfv(GL_FRONT,GL_SPECULAR,mtls[m]->Ks);
      glMaterialfv(GL_FRONT,GL_EMISSION,mtls[m]->Ke);

      if (translucent) glColor4f(1,1,1, mtls[m]->opacity);

      for (uint32_t i=0; i < mtls[m]->faces.size(); i++) {
        uint32_t j;
        
        //location.scale(6.5,7.5,6.5);
        matrix16f temp = location;
        temp.scale(6.5,7.5,6.5);
        for (j=0; j < lights.size(); j++) {
          lights[j]->drawDim(temp.transform(vertices[mtls[m]->faces[i].vertIndices[0]-1]));
        }
        glPushMatrix();
        glMultMatrixf(location.matrix);
        
        glScalef(6.5,7.5,6.5);
        glBegin(GL_TRIANGLES);
        glNormal3fv(normals[mtls[m]->faces[i].vertIndices[0]-1].vertex);
        glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[0]-1].vertex);
        glNormal3fv(normals[mtls[m]->faces[i].vertIndices[1]-1].vertex);
        glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[1]-1].vertex);
        glNormal3fv(normals[mtls[m]->faces[i].vertIndices[2]-1].vertex);
        glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[2]-1].vertex);

        glEnd();
        glPopMatrix();
      }
    }
  glDisable(GL_LIGHTING);
  glPopMatrix();
}
#endif //notcalled

void rigid::makeList()
{
  echo("rigid::makeList");
  listNum = glGenLists(1);
  glNewList(listNum, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (uint32_t m=0; m < mtls.size(); m++) {
    glMaterialfv(GL_FRONT,GL_AMBIENT, mtls[m]->Ka);
    glMaterialfv(GL_FRONT,GL_DIFFUSE, mtls[m]->Kd);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mtls[m]->Ks);
    glMaterialfv(GL_FRONT,GL_EMISSION,mtls[m]->Ke);

    if (translucent)
      glColor4f(1,1,1,mtls[m]->opacity);
    for (uint32_t i=0; i < mtls[m]->faces.size(); i++) {
      glNormal3fv(normals[mtls[m]->faces[i].vertIndices[0]-1].vertex);
      glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[0]-1].vertex);
      glNormal3fv(normals[mtls[m]->faces[i].vertIndices[1]-1].vertex);
      glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[1]-1].vertex);
      glNormal3fv(normals[mtls[m]->faces[i].vertIndices[2]-1].vertex);
      glVertex3fv(vertices[mtls[m]->faces[i].vertIndices[2]-1].vertex);
    }
  }
  glEnd();
  glEndList();
}

void rigid::getBoundingBox()
{
  //echo("rigid::getBB");
  for (int j=0; j<3;j++) {
    //boundingBox[0].vertex[j] = test[0]->location.vertex[j]; // lower right bound
    //boundingBox[6].vertex[j] = test[0]->location.vertex[j]; // upper right bound
  }
  boundingBox[0] = vertices[1];
  boundingBox[6] = vertices[1];
      
  // find extremities
  for (uint32_t i=1; i < vertices.size(); i++) {
    for (int j=0; j<3;j++) {
    if (boundingBox[0].vertex[j] > vertices[i].vertex[j])
      boundingBox[0].vertex[j] = vertices[i].vertex[j]; // lower left   minimum bound 
    if (boundingBox[6].vertex[j] < vertices[i].vertex[j])
      boundingBox[6].vertex[j] = vertices[i].vertex[j]; // upper right  maximum bound 
    }
  }
  centerBB = (boundingBox[0] + boundingBox[6])/2;
  edgesBB = boundingBox[6] - centerBB;

  // having found maximums, populate rest of corners
  boundingBox[1].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[1].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[1].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[2].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[2].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[2].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[3].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[3].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[3].vertex[2] = boundingBox[0].vertex[2];
  boundingBox[4].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[4].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[4].vertex[2] = boundingBox[0].vertex[2];
  boundingBox[5].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[5].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[5].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[7].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[7].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[7].vertex[2] = boundingBox[0].vertex[2];
}


movable::movable()
{
  init();
}

movable::movable(string name)
{
  echo("movable::movable");
  setName(name);
  init();
}

void movable::init()
{
  echo("movable::init");
  location.identity();
  location.matrix[10] = -1.0f;  

  drawBB    = true;
  BBcollided  = false;
#if 0 //
  physical = false;
  normalize = true;
#endif
  getBoundingBox();
}

movable& movable::operator= (const movable& other)
{
  location = other.location;
  return *this;
}

void movable::setName(string name)
{
  echo("movable::setName");
  location.identity();
  location.matrix[10] = -1.0f; 
  this->name = name;
  for (int i=0; name[i] !=0; i++) {
    this->name[i] = name[i];
  }
  //movableFrame->setText(name);	//
}

void movable::move(int pitch, int turn, int roll, float x, float y, float z)
{
  echo("movable::move");
  if (pitch != 0) location.RotateX(-pitch/ROTATE_SLOWNESS);
  if (turn != 0)  location.RotateY(-turn/ROTATE_SLOWNESS);
  if (roll != 0)  location.RotateZ(roll/ROTATE_SLOWNESS);

  location.translate(x, y, z);
  newLocation = location;
}

void movable::getAABB()
{
  //echo("movable::getAABB");
#if 0 //
  oldCenterAABB = centerAABB;
#endif

  vector3f max = newLocation.transform(boundingBox[0]);
  vector3f min = newLocation.transform(boundingBox[0]);

  vector3f offset(newLocation.matrix[12],newLocation.matrix[13],newLocation.matrix[14]);
  // quick and dirty using the transformed oriented BBox, and hardly volume minimizing
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      if (max.vertex[j] < newLocation.transform(boundingBox[i]).vertex[j])
        max.vertex[j] = newLocation.transform(boundingBox[i]).vertex[j];
      if (min.vertex[j] > newLocation.transform(boundingBox[i]).vertex[j])
        min.vertex[j] = newLocation.transform(boundingBox[i]).vertex[j];
    }
  }
  min = min + offset;
  max = max + offset;
  centerAABB = (min + max)/2;
  edgesAABB = max - centerAABB;
  AABB[0] = min;
  AABB[6] = max;

  // having found maximums, populate rest of corners
  AABB[1].vertex[0] = AABB[0].vertex[0];
  AABB[1].vertex[1] = AABB[0].vertex[1];
  AABB[1].vertex[2] = AABB[6].vertex[2];
  AABB[2].vertex[0] = AABB[0].vertex[0];
  AABB[2].vertex[1] = AABB[6].vertex[1];
  AABB[2].vertex[2] = AABB[6].vertex[2];
  AABB[3].vertex[0] = AABB[0].vertex[0];
  AABB[3].vertex[1] = AABB[6].vertex[1];
  AABB[3].vertex[2] = AABB[0].vertex[2];
  AABB[4].vertex[0] = AABB[6].vertex[0];
  AABB[4].vertex[1] = AABB[0].vertex[1];
  AABB[4].vertex[2] = AABB[0].vertex[2];
  AABB[5].vertex[0] = AABB[6].vertex[0];
  AABB[5].vertex[1] = AABB[0].vertex[1];
  AABB[5].vertex[2] = AABB[6].vertex[2];
  AABB[7].vertex[0] = AABB[6].vertex[0];
  AABB[7].vertex[1] = AABB[6].vertex[1];
  AABB[7].vertex[2] = AABB[0].vertex[2];
}

void movable::drawAABB()
{
  echo("movable::drawAABB");
  getAABB();	//
  glPushMatrix();

  glTranslatef(location.matrix[12],location.matrix[13],location.matrix[14]);
  glDisable(GL_LIGHTING);

  if (BBcollided) glColor3f(0.7, 0.3, 0.4);
  else            glColor3f(0.4, 0.3, 0.7);

  for (int i=0; i<4; i++) {
    glBegin(GL_LINES);
    glVertex3fv(AABB[i].vertex);
    glVertex3fv(AABB[i+4].vertex);
    glEnd();
  }
  glBegin(GL_LINE_LOOP);  
  for (int j=0; j<4;j++) {
    glVertex3fv(AABB[j].vertex);
  }
  glEnd();
  glBegin(GL_LINE_LOOP);  
  for (int k=4; k<8; k++) {
    glVertex3fv(AABB[k].vertex );
  }
  glEnd();

  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void movable::drawBoundingBox()
{
  echo("movable::drawBB");
  glDisable(GL_LIGHTING);

  if (BBcollided) glColor3f(0.9, 0.3, 0.2);
  else            glColor3f(0.2, 0.4, 0.2);
  
  for (int i=0; i<4; i++) {
    glBegin(GL_LINES);
    glVertex3fv(boundingBox[i].vertex);
    glVertex3fv(boundingBox[i+4].vertex);
    glEnd();
  }
  glBegin(GL_LINE_LOOP);  
  for (int j=0; j<4; j++) {
    glVertex3fv(boundingBox[j].vertex );
  }
  glEnd();
  glBegin(GL_LINE_LOOP);  
  for (int k=4; k<8; k++) {
    glVertex3fv(boundingBox[k].vertex );
  }
  glEnd();

  glEnable(GL_LIGHTING);
}


#if 0 //notcalled
void Normal(vector3f* v1, vector3f* v2, vector3f* v3)
{
  //echo("Normal");
  vector3f a(0, 0, 0);
  vector3f b(0, 0, 0);
  vector3f res(0, 0, 0);
  float len;

  a.vertex[0] = v1->vertex[0] - v2->vertex[0];
  a.vertex[1] = v1->vertex[1] - v2->vertex[1];
  a.vertex[2] = v1->vertex[2] - v2->vertex[2];
  b.vertex[0] = v1->vertex[0] - v3->vertex[0];
  b.vertex[1] = v1->vertex[1] - v3->vertex[1];
  b.vertex[2] = v1->vertex[2] - v3->vertex[2];

  res.vertex[0] = (a.vertex[1]*b.vertex[2]) - (b.vertex[1]*a.vertex[2]);
  res.vertex[1] = (b.vertex[0]*a.vertex[2]) - (a.vertex[0]*b.vertex[2]);
  res.vertex[2] = (a.vertex[0]*b.vertex[1]) - (b.vertex[0]*a.vertex[1]);

  // calculate the length of the normal
  len = (float)sqrt(SQR(res.vertex[0]) + SQR(res.vertex[1]) + SQR(res.vertex[2]));
  // normalize and specify the normal
  glNormal3f(res.vertex[0]/len, res.vertex[1]/len, res.vertex[2]/len);
}
#endif //notcalled

// calculate the length of the normal
float vector3f::length()
{
  return (float)sqrt(SQR(vertex[0]) + SQR(vertex[1]) + SQR(vertex[2]));
}

float vector3f::length2()
{
  return (float)(SQR(vertex[0]) + SQR(vertex[1]) + SQR(vertex[2]));
}

void vector3f::reset()
{
  vertex[0] = vertex[1] = vertex[2] = 0;
}

float dotProduct(const vector3f &v1,const vector3f &v2)
{
  return (v1.vertex[0]*v2.vertex[0])+ (v1.vertex[1]*v2.vertex[1])+ (v1.vertex[2]*v2.vertex[2]);
}

vector3f crossProduct(const vector3f &v1, const vector3f &v2)
{
  vector3f res(0, 0, 0);

  res.vertex[0]= (v1.vertex[1]*v2.vertex[2]) - (v1.vertex[2]*v2.vertex[1]);
  res.vertex[1]= (v1.vertex[2]*v2.vertex[0]) - (v1.vertex[0]*v2.vertex[2]);
  res.vertex[2]= (v1.vertex[0]*v2.vertex[1]) - (v1.vertex[1]*v2.vertex[0]);
  return res;
}

vector3f vector3f::abs()
{
  for (int i=0; i<3;i++) {
    if (this->vertex[i] < 0)
      this->vertex[i] = -this->vertex[i]; 
  }  
  return *this;
}

vector3f::vector3f(float x, float y, float z)
{
  vertex[0]=x; vertex[1]=y; vertex[2]=z;
}

void vector3f::set(float x, float y, float z)
{
  vertex[0]=x; vertex[1]=y; vertex[2]=z;
}

vector3f& vector3f::operator=(const vector3f &v1)
{
  vertex[0] =v1.vertex[0]; vertex[1] =v1.vertex[1]; vertex[2] =v1.vertex[2];
  return *this;
}

bool operator== (const vector3f &v1, const vector3f &v2)
{
  return ((v1.vertex[0] == v2.vertex[0]) && 
        (v1.vertex[1] == v2.vertex[1]) &&
        (v1.vertex[2] == v2.vertex[2])); 
}

bool operator<= (const vector3f &v1, const vector3f &v2)
{
  return ((v1.vertex[0] <= v2.vertex[0]) && 
        (v1.vertex[1] <= v2.vertex[1]) &&
        (v1.vertex[2] <= v2.vertex[2])); 
}

bool operator>= (const vector3f &v1, const vector3f &v2)
{
  return ((v1.vertex[0] >= v2.vertex[0]) && 
        (v1.vertex[1] >= v2.vertex[1]) &&
        (v1.vertex[2] >= v2.vertex[2])); 
}

bool operator< (const vector3f &v1, const vector3f &v2)
{
  return ((v1.vertex[0] < v2.vertex[0]) && 
        (v1.vertex[1] < v2.vertex[1]) &&
        (v1.vertex[2] < v2.vertex[2])); 
}

bool operator> (const vector3f &v1, const vector3f &v2)
{
  return ((v1.vertex[0] > v2.vertex[0]) && 
        (v1.vertex[1] > v2.vertex[1]) &&
        (v1.vertex[2] > v2.vertex[2])); 
}

vector3f operator+ (const vector3f &v1, const vector3f &v2)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0] = v1.vertex[0] + v2.vertex[0];
  res.vertex[1] = v1.vertex[1] + v2.vertex[1];
  res.vertex[2] = v1.vertex[2] + v2.vertex[2];
  return res;
}

vector3f operator+ (const vector3f &v1, const float scalar)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0] = v1.vertex[0] + scalar;
  res.vertex[1] = v1.vertex[1] + scalar;
  res.vertex[2] = v1.vertex[2] + scalar;
  return res;
}

vector3f operator- (const vector3f &v1, const vector3f &v2)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0] = v1.vertex[0] - v2.vertex[0];
  res.vertex[1] = v1.vertex[1] - v2.vertex[1];
  res.vertex[2] = v1.vertex[2] - v2.vertex[2];
  return res;
}

vector3f operator- (const vector3f &v1, const float scalar)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0]= v1.vertex[0] - scalar;
  res.vertex[1]= v1.vertex[1] - scalar;
  res.vertex[2]= v1.vertex[2] - scalar;
  return res;
}

vector3f operator* (const vector3f &v1, const float scalar)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0]= v1.vertex[0] * scalar;
  res.vertex[1]= v1.vertex[1] * scalar;
  res.vertex[2]= v1.vertex[2] * scalar;
  return res;
}

vector3f operator* (const vector3f &v1, const vector3f &v2)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0]= v1.vertex[0] * v2.vertex[0];
  res.vertex[1]= v1.vertex[1] * v2.vertex[1];
  res.vertex[2]= v1.vertex[2] * v2.vertex[2];
  return res;
}

vector3f operator/ (const vector3f &v1, const vector3f &v2)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0]= v1.vertex[0] / v2.vertex[0];
  res.vertex[1]= v1.vertex[1] / v2.vertex[1];
  res.vertex[2]= v1.vertex[2] / v2.vertex[2];
  return res;
}

vector3f operator/ (const vector3f &v1, const float scalar)
{
  vector3f res(0.0f, 0.0f, 0.0f);

  res.vertex[0]= v1.vertex[0] / scalar;
  res.vertex[1]= v1.vertex[1] / scalar;
  res.vertex[2]= v1.vertex[2] / scalar;
  return res;
}

///////////////////////////
// OpengGL compatible matrix:
// 
// float[16]
// [0 4 8  12]
// [1 5 9  13]
// [2 6 10 14]
// [3 7 11 15]
// 
// [rightX upX outX  X]
// [rightY upY outY  Y]
// [rightZ upZ outZ  Z]
// [0      0   0     1]
///////////////////////////

void matrix16f::reset()
{
  for (int i=0; i<16; i++) matrix[i] = 0.;
}

void matrix16f::identity()
{
  matrix[0] =1.0f; matrix[4] =0.0f; matrix[8] =0.0f; matrix[12] =0.0f;
  matrix[1] =0.0f; matrix[5] =1.0f; matrix[9] =0.0f; matrix[13] =0.0f;
  matrix[2] =0.0f; matrix[6] =0.0f; matrix[10]=1.0f; matrix[14] =0.0f;
  matrix[3] =0.0f; matrix[7] =0.0f; matrix[11]=0.0f; matrix[15] =1.0f;
}

void matrix16f::translate(float x, float y, float z)
{
  this->matrix[12]=(this->matrix[0]*x)+(this->matrix[4]*y)+(this->matrix[8] *z)+(this->matrix[12]);
  this->matrix[13]=(this->matrix[1]*x)+(this->matrix[5]*y)+(this->matrix[9] *z)+(this->matrix[13]);
  this->matrix[14]=(this->matrix[2]*x)+(this->matrix[6]*y)+(this->matrix[10]*z)+(this->matrix[14]);
  this->matrix[15]=(this->matrix[3]*x)+(this->matrix[7]*y)+(this->matrix[11]*z)+(this->matrix[15]);
}

void matrix16f::scale(float x, float y, float z)
{
  matrix[0] *= x;
  matrix[5] *= y;
  matrix[10] *= z;
}

// current uses only 3x3 orientation portion, not location triple
vector3f matrix16f::transform(vector3f point)
{
  vector3f res;

  res.vertex[0] = (this->matrix[0]*point.vertex[0])+(this->matrix[4]*point.vertex[1])+(this->matrix[8] *point.vertex[2]) +(this->matrix[12]);
  res.vertex[1] = (this->matrix[1]*point.vertex[0])+(this->matrix[5]*point.vertex[1])+(this->matrix[9] *point.vertex[2]) +(this->matrix[13]);
  res.vertex[2] = (this->matrix[2]*point.vertex[0])+(this->matrix[6]*point.vertex[1])+(this->matrix[10]*point.vertex[2]) +(this->matrix[14]);
  return res;
}

matrix16f matrix16f::FlipOrientation()
{
  matrix16f res;

  res.matrix[0] = matrix[0]; res.matrix[4] = matrix[1]; res.matrix[8] = matrix[2]; res.matrix[12] = matrix[12];
  res.matrix[1] = matrix[4]; res.matrix[5] = matrix[5]; res.matrix[9] = matrix[6]; res.matrix[13] = matrix[13];
  res.matrix[2] = matrix[8]; res.matrix[6] = matrix[9]; res.matrix[10]= matrix[10];res.matrix[14] = matrix[14];
  res.matrix[3] = matrix[3]; res.matrix[7] = matrix[7]; res.matrix[11]= matrix[11];res.matrix[15] = matrix[15];
  return res;
}

void matrix16f::normalize()
{
  vector3f right(matrix[0],matrix[1],matrix[2]);
  vector3f up(matrix[4],matrix[5],matrix[6]);
  vector3f out(matrix[8],matrix[9],matrix[10]);

  // make all axes orthogonal
  out   = crossProduct(up,right);
  right = crossProduct(out,up);

  // normalize
  right = right/right.length();
  up    = up/up.length();
  out   = out/out.length();

  matrix[0] = right.vertex[0]; matrix[4] = up.vertex[0]; matrix[8] = out.vertex[0]; 
  matrix[1] = right.vertex[1]; matrix[5] = up.vertex[1]; matrix[9] = out.vertex[1]; 
  matrix[2] = right.vertex[2]; matrix[6] = up.vertex[2]; matrix[10]= out.vertex[2];
}

void matrix16f::print()
{
  cout    
    << matrix[0] << "\t" << matrix[4] << "\t" << matrix[8] << "\t" << matrix[12] << "\n"
    << matrix[1] << "\t" << matrix[5] << "\t" << matrix[9] << "\t" << matrix[13] << "\n" 
    << matrix[2] << "\t" << matrix[6] << "\t" << matrix[10]<< "\t" << matrix[14] << "\n\n";
}

// 1  0      0        0 
// 0  cos(x) -sin(x)  0  
// 0  sin(x)  cos(x)  0  
// 0  0      0        1 
void matrix16f::RotateX(int deg)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[5] = (float)cos(DEG2RAD(deg));  temp.matrix[9]  = (float) sin(DEG2RAD(deg));
  temp.matrix[6] = (float)-sin(DEG2RAD(deg)); temp.matrix[10] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them - doesn't appear to work
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

// rotate about y-axis about center of this, not origin
// cos(x)   0    sin(x)  0 
// 0        1    0       0
//-sin(x)   0    cos(x)  0  
// 0        0    0       1 
void matrix16f::RotateY(int deg)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[0] = (float)cos(DEG2RAD(deg)); temp.matrix[8]  = (float) -sin(DEG2RAD(deg));
  temp.matrix[2] = (float)sin(DEG2RAD(deg)); temp.matrix[10] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

// rotate about x-axis about center of this, not origin
// cos(x) -sin(x)  0  0 
// sin(x)  cos(x)  0  0  
// 0    0    1  0
// 0    0    0  1 
void matrix16f::RotateZ(int deg)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[0] = (float)cos(DEG2RAD(deg)); temp.matrix[4] = (float)sin(DEG2RAD(deg));
  temp.matrix[1] = (float)-sin(DEG2RAD(deg)); temp.matrix[5] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

// 1  0     0    0 
// 0  cos(x) -sin(x)  0  
// 0  sin(x)  cos(x)  0  
// 0  0     0    1 
void matrix16f::RotateX(float rad)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[5] = (float)cos(rad); temp.matrix[9]  = (float)sin(rad);
  temp.matrix[6] = (float)-sin(rad); temp.matrix[10] = (float) cos(rad);

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them - doesn't appear to work
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

// rotate about y-axis about center of this, not origin
// cos(x)  0  sin(x)  0 
// 0    1   0    0
//-sin(x)  0  cos(x)  0  
// 0    0   0    1 
void matrix16f::RotateY(float rad)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[0] = (float) cos(rad); temp.matrix[8]  = (float)-sin(rad);
  temp.matrix[2] = (float) sin(rad); temp.matrix[10] = (float) cos(rad);

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

// rotate about x-axis about center of this, not origin
// cos(x) -sin(x)  0  0 
// sin(x)  cos(x)  0  0  
// 0    0    1  0
// 0    0    0  1 
void matrix16f::RotateZ(float rad)
{
  float tempX = this->matrix[12];
  float tempY = this->matrix[13];
  float tempZ = this->matrix[14];

  this->matrix[12] = 0;
  this->matrix[13] = 0;
  this->matrix[14] = 0;

  matrix16f temp;
  temp.identity();
  temp.matrix[0] = (float)cos(rad);  temp.matrix[4] = (float)sin(rad);
  temp.matrix[1] = (float)-sin(rad); temp.matrix[5] = (float)cos(rad);

  *this = (*this)*temp;

  // restore x,y,z so rotation won't affect them
  this->matrix[12] = tempX;
  this->matrix[13] = tempY;
  this->matrix[14] = tempZ;
} 

void matrix16f::RotateXYZ(float x, float y, float z)
{
  matrix16f temp;
  *this = (*this)*temp;  
}

// Rotate in place- don't change location
void matrix16f::Rotate(matrix16f m2)
{
  matrix16f temp = *this;
  
  // row 0
  temp.matrix[0] = (this->matrix[0]*m2.matrix[0]) + (this->matrix[4]*m2.matrix[1]) +(this->matrix[8]*m2.matrix[2]);
   temp.matrix[4] = (this->matrix[0]*m2.matrix[4]) + (this->matrix[4]*m2.matrix[5]) +(this->matrix[8]*m2.matrix[6]);
   temp.matrix[8] = (this->matrix[0]*m2.matrix[8]) + (this->matrix[4]*m2.matrix[9]) +(this->matrix[8]*m2.matrix[10]);
  // row 1
   temp.matrix[1] = (this->matrix[1]*m2.matrix[0]) +(this->matrix[5]*m2.matrix[1]) +(this->matrix[9]*m2.matrix[2]);
   temp.matrix[5] = (this->matrix[1]*m2.matrix[4]) +(this->matrix[5]*m2.matrix[5]) +(this->matrix[9]*m2.matrix[6]);
   temp.matrix[9] = (this->matrix[1]*m2.matrix[8]) +(this->matrix[5]*m2.matrix[9]) +(this->matrix[9]*m2.matrix[10]);
  // row 2
   temp.matrix[2] = (this->matrix[2]*m2.matrix[0]) +(this->matrix[6]*m2.matrix[1]) +(this->matrix[10]*m2.matrix[2]);
   temp.matrix[6] = (this->matrix[2]*m2.matrix[4]) +(this->matrix[6]*m2.matrix[5]) +(this->matrix[10]*m2.matrix[6]);
   temp.matrix[10]= (this->matrix[2]*m2.matrix[8]) +(this->matrix[6]*m2.matrix[9]) +(this->matrix[10]*m2.matrix[10]);
  // row 3
   temp.matrix[3] = (this->matrix[3]*m2.matrix[0]) +(this->matrix[7]*m2.matrix[1]) +(this->matrix[11]*m2.matrix[2]);
   temp.matrix[7] = (this->matrix[3]*m2.matrix[4]) +(this->matrix[7]*m2.matrix[5]) +(this->matrix[11]*m2.matrix[6]);
   temp.matrix[11]= (this->matrix[3]*m2.matrix[8]) +(this->matrix[7]*m2.matrix[9]) +(this->matrix[11]*m2.matrix[10]);
  
  *this  = temp;
}

void matrix16f::set(vector3f right, vector3f up, vector3f out)
{
  // row 0
  this->matrix[0] = right.vertex[0];
  this->matrix[1] = right.vertex[1];
  this->matrix[2] = right.vertex[2];
  // row 1
  this->matrix[4] = up.vertex[0];
  this->matrix[5] = up.vertex[1];
  this->matrix[6] = up.vertex[2];
  // row 2
  this->matrix[8] = out.vertex[0];
  this->matrix[9] = out.vertex[1];
  this->matrix[10]= out.vertex[2];
}

matrix16f& matrix16f::operator= (const matrix16f &m1)
{
  for (int i = 0; i<16; i++) {
    matrix[i]= m1.matrix[i];
  }
  return *this;
}

matrix16f operator+ (const matrix16f &m1, const matrix16f &m2)
{
  matrix16f res;

  for (int i = 0; i<16; i++) {
    res.matrix[i]= m1.matrix[i]+m2.matrix[i];
  }
  return res;
}

// for adding location vectors to orientation-location matrices
matrix16f operator+ (const matrix16f &m1, const vector3f  &v2)
{
  matrix16f res;
  res = m1;

  res.matrix[12] += v2.vertex[0];
  res.matrix[13] += v2.vertex[1];
  res.matrix[14] += v2.vertex[2];
  return res;
}

matrix16f operator- (const matrix16f &m1, const matrix16f &m2)
{
  matrix16f res;

  for (int i = 0; i<16; i++) {
    res.matrix[i]= m1.matrix[i]-m2.matrix[i];
  }
  return res;
}

matrix16f operator* (const matrix16f &m1, const float scalar)
{
  matrix16f res;

  for (int i = 0; i<16; i++) {
    res.matrix[i]= m1.matrix[i]*scalar;
  }
  return res;
}

matrix16f operator* (const matrix16f &m1, const matrix16f &m2)
{
  matrix16f res;

  // row 0
  res.matrix[0] = (m1.matrix[0]*m2.matrix[0]) + (m1.matrix[4]*m2.matrix[1]) +(m1.matrix[8]*m2.matrix[2]) +(m1.matrix[12]*m2.matrix[3]);
  res.matrix[4] = (m1.matrix[0]*m2.matrix[4]) + (m1.matrix[4]*m2.matrix[5]) +(m1.matrix[8]*m2.matrix[6]) +(m1.matrix[12]*m2.matrix[7]);
  res.matrix[8] = (m1.matrix[0]*m2.matrix[8]) + (m1.matrix[4]*m2.matrix[9]) +(m1.matrix[8]*m2.matrix[10])+(m1.matrix[12]*m2.matrix[11]);
  res.matrix[12]= (m1.matrix[0]*m2.matrix[12])+ (m1.matrix[4]*m2.matrix[13])+(m1.matrix[8]*m2.matrix[14])+(m1.matrix[12]*m2.matrix[15]);
  // row 1
  res.matrix[1] = (m1.matrix[1]*m2.matrix[0]) +(m1.matrix[5]*m2.matrix[1]) +(m1.matrix[9]*m2.matrix[2]) +(m1.matrix[13]*m2.matrix[3]);
  res.matrix[5] = (m1.matrix[1]*m2.matrix[4]) +(m1.matrix[5]*m2.matrix[5]) +(m1.matrix[9]*m2.matrix[6]) +(m1.matrix[13]*m2.matrix[7]);
  res.matrix[9] = (m1.matrix[1]*m2.matrix[8]) +(m1.matrix[5]*m2.matrix[9]) +(m1.matrix[9]*m2.matrix[10])+(m1.matrix[13]*m2.matrix[11]);
  res.matrix[13]= (m1.matrix[1]*m2.matrix[12])+(m1.matrix[5]*m2.matrix[13])+(m1.matrix[9]*m2.matrix[14])+(m1.matrix[13]*m2.matrix[15]);
  // row 2
  res.matrix[2] = (m1.matrix[2]*m2.matrix[0]) +(m1.matrix[6]*m2.matrix[1]) +(m1.matrix[10]*m2.matrix[2]) +(m1.matrix[14]*m2.matrix[3]);
  res.matrix[6] = (m1.matrix[2]*m2.matrix[4]) +(m1.matrix[6]*m2.matrix[5]) +(m1.matrix[10]*m2.matrix[6]) +(m1.matrix[14]*m2.matrix[7]);
  res.matrix[10]= (m1.matrix[2]*m2.matrix[8]) +(m1.matrix[6]*m2.matrix[9]) +(m1.matrix[10]*m2.matrix[10])+(m1.matrix[14]*m2.matrix[11]);
  res.matrix[14]= (m1.matrix[2]*m2.matrix[12])+(m1.matrix[6]*m2.matrix[13])+(m1.matrix[10]*m2.matrix[14])+(m1.matrix[14]*m2.matrix[15]);
  // row 3
  res.matrix[3] = (m1.matrix[3]*m2.matrix[0]) +(m1.matrix[7]*m2.matrix[1]) +(m1.matrix[11]*m2.matrix[2]) +(m1.matrix[15]*m2.matrix[3]);
  res.matrix[7] = (m1.matrix[3]*m2.matrix[4]) +(m1.matrix[7]*m2.matrix[5]) +(m1.matrix[11]*m2.matrix[6]) +(m1.matrix[15]*m2.matrix[7]);
  res.matrix[11]= (m1.matrix[3]*m2.matrix[8]) +(m1.matrix[7]*m2.matrix[9]) +(m1.matrix[11]*m2.matrix[10])+(m1.matrix[15]*m2.matrix[11]);
  res.matrix[15]= (m1.matrix[3]*m2.matrix[12])+(m1.matrix[7]*m2.matrix[13])+(m1.matrix[11]*m2.matrix[14])+(m1.matrix[15]*m2.matrix[15]);

  return res;
}

matrix16f operator/ (const matrix16f &m1, const float scalar)
{  
  matrix16f res;

  for (int i=0; i<16; i++) {
    res.matrix[i]= m1.matrix[i]/scalar;
  }
  return res;
}

bool operator== (const matrix16f &m1, const matrix16f &m2)
{
  bool res = true;

  for (int i=0; i< 16; i++) {
    if (m1.matrix[i] != m2.matrix[i]) res = false;
  }
  return res;
}

// | f0 f2 |
// | f1 f3 |
float det(float f0, float f1, float f2, float f3)
{
  return (f0*f3-f1*f2);
}

// fill matrix with zeros
void matrix9f::reset()
{
  for (int i=0; i<9; i++) matrix[i] = 0.0;
}

matrix9f matrix9f::inverse()
{
  matrix9f res;

  float detThis = this->matrix[0]*det(this->matrix[4],this->matrix[5],this->matrix[7],this->matrix[8]) -
          this->matrix[3]*det(this->matrix[1],this->matrix[2],this->matrix[7],this->matrix[8]) +
          this->matrix[6]*det(this->matrix[1],this->matrix[2],this->matrix[4],this->matrix[5]);
        
  res.matrix[0] =  det(this->matrix[4],this->matrix[5],this->matrix[7],this->matrix[8]); 
  res.matrix[3] = -det(this->matrix[1],this->matrix[2],this->matrix[7],this->matrix[8]); 
  res.matrix[6] =  det(this->matrix[1],this->matrix[2],this->matrix[4],this->matrix[5]); 

  res.matrix[1] = -det(this->matrix[3],this->matrix[5],this->matrix[6],this->matrix[8]); 
  res.matrix[4] =  det(this->matrix[0],this->matrix[2],this->matrix[6],this->matrix[8]); 
  res.matrix[7] = -det(this->matrix[1],this->matrix[2],this->matrix[4],this->matrix[5]);
      
  res.matrix[2] =  det(this->matrix[3],this->matrix[4],this->matrix[6],this->matrix[7]); 
  res.matrix[5] = -det(this->matrix[0],this->matrix[1],this->matrix[6],this->matrix[7]);
  res.matrix[8] =  det(this->matrix[0],this->matrix[1],this->matrix[3],this->matrix[4]);

  res = res.transpose();
  res = res/detThis;
  return res;
}

void matrix9f::identity()
{
  matrix[0] = 1.0f; matrix[3] = 0.0f; matrix[6] = 0.0f; 
  matrix[1] = 0.0f; matrix[4] = 1.0f; matrix[7] = 0.0f; 
  matrix[2] = 0.0f; matrix[5] = 0.0f; matrix[8] = 1.0f; 
}

matrix9f matrix9f::transpose()
{
  matrix9f res;

  res.matrix[0] = matrix[0]; res.matrix[3] = matrix[1]; res.matrix[6] = matrix[2]; 
  res.matrix[1] = matrix[3]; res.matrix[4] = matrix[4]; res.matrix[7] = matrix[5]; 
  res.matrix[2] = matrix[6]; res.matrix[5] = matrix[7]; res.matrix[8] = matrix[8];
  return res;
}

matrix9f Star(vector3f a)
{
  matrix9f res;

  res.matrix[0] =  0;           res.matrix[3] = -a.vertex[2]; res.matrix[6] =  a.vertex[1]; 
  res.matrix[1] =  a.vertex[2]; res.matrix[4] =  0;           res.matrix[7] = -a.vertex[0]; 
  res.matrix[2] = -a.vertex[1]; res.matrix[5] =  a.vertex[0]; res.matrix[8] =  0;
  return res;
}

// rotate about x-axis about center of this, not origin
// 1  0     0    0 
// 0  cos(x) -sin(x)  0  
// 0  sin(x)  cos(x)  0  
// 0  0     0    1 
void matrix9f::RotateX(int deg)
{
  matrix9f temp;
  temp.identity();
  temp.matrix[4] = (float)cos(DEG2RAD(deg));  temp.matrix[7]  = (float) sin(DEG2RAD(deg));
  temp.matrix[5] = (float)-sin(DEG2RAD(deg)); temp.matrix[8] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;
} 

// rotate about y-axis about center of this, not origin
// cos(x)  0  sin(x)  0 
// 0    1   0    0
//-sin(x)  0  cos(x)  0  
// 0    0   0    1 
void matrix9f::RotateY(int deg)
{
  matrix9f temp;
  temp.identity();
  temp.matrix[0] = (float) cos(DEG2RAD(deg)); temp.matrix[6]  = (float) -sin(DEG2RAD(deg));
  temp.matrix[2] = (float) sin(DEG2RAD(deg)); temp.matrix[8] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;
} 

// rotate about x-axis about center of this, not origin
// cos(x) -sin(x)  0  0 
// sin(x)  cos(x)  0  0  
// 0    0    1  0
// 0    0    0  1 
void matrix9f::RotateZ(int deg)
{
  matrix9f temp;
  temp.identity();
  temp.matrix[0] = (float)cos(DEG2RAD(deg));  temp.matrix[3] = (float)sin(DEG2RAD(deg));
  temp.matrix[1] = (float)-sin(DEG2RAD(deg)); temp.matrix[4] = (float) cos(DEG2RAD(deg));

  *this = (*this)*temp;
} 

matrix9f& matrix9f::operator= (const matrix9f &m1)
{
  for (int i = 0; i<9; i++)  
    matrix[i]= m1.matrix[i];
  return *this;
}

matrix9f operator+ (const matrix9f &m1, const matrix9f &m2)
{
  matrix9f res;

  for (int i = 0; i<9; i++)
    res.matrix[i]= m1.matrix[i]+m2.matrix[i];
  return res;
}

matrix16f operator+ (const matrix16f &m1, const matrix9f &m2)
{
  matrix16f res;
  res = m1;

  // row 0
  res.matrix[0] += m2.matrix[0];
  res.matrix[4] += m2.matrix[3];
  res.matrix[8] += m2.matrix[6];
  // row 1
  res.matrix[1] += m2.matrix[1];
  res.matrix[5] += m2.matrix[4];
  res.matrix[9] += m2.matrix[7];
  // row 2
  res.matrix[2] += m2.matrix[2];
  res.matrix[6] += m2.matrix[5];
  res.matrix[10] += m2.matrix[8];

  return res;
}

matrix9f operator- (const matrix9f &m1, const matrix9f &m2)
{
  matrix9f res;

  for (int i = 0; i<9; i++)
    res.matrix[i]= m1.matrix[i]-m2.matrix[i];
  return res;
}

matrix9f operator* (const matrix9f &m1, const float scalar)
{
  matrix9f res;

  for (int i = 0; i<9; i++)
    res.matrix[i]= m1.matrix[i]*scalar;
  return res;
}

vector3f operator* (const matrix9f &m1, const vector3f &v2)
{
  vector3f res;

  res.vertex[0] = (m1.matrix[0]*v2.vertex[0]) + (m1.matrix[3]*v2.vertex[1]) + (m1.matrix[6]*v2.vertex[2]);
  res.vertex[1] = (m1.matrix[1]*v2.vertex[0]) + (m1.matrix[4]*v2.vertex[1]) + (m1.matrix[7]*v2.vertex[2]);
  res.vertex[2] = (m1.matrix[2]*v2.vertex[0]) + (m1.matrix[5]*v2.vertex[1]) + (m1.matrix[8]*v2.vertex[2]);
  return res;
}

matrix9f operator* (const matrix9f &m1, const matrix9f &m2)
{
  matrix9f res;

  // row 0
  res.matrix[0] = (m1.matrix[0]*m2.matrix[0]) + (m1.matrix[3]*m2.matrix[1]) + (m1.matrix[6]*m2.matrix[2]);
  res.matrix[3] = (m1.matrix[0]*m2.matrix[3]) + (m1.matrix[3]*m2.matrix[4]) + (m1.matrix[6]*m2.matrix[5]);
  res.matrix[6] = (m1.matrix[0]*m2.matrix[6]) + (m1.matrix[3]*m2.matrix[7]) + (m1.matrix[6]*m2.matrix[8]);
  // row 1
  res.matrix[1] = (m1.matrix[1]*m2.matrix[0]) + (m1.matrix[4]*m2.matrix[1]) + (m1.matrix[7]*m2.matrix[2]);
  res.matrix[4] = (m1.matrix[1]*m2.matrix[3]) + (m1.matrix[4]*m2.matrix[4]) + (m1.matrix[7]*m2.matrix[5]);
  res.matrix[7] = (m1.matrix[1]*m2.matrix[6]) + (m1.matrix[4]*m2.matrix[7]) + (m1.matrix[7]*m2.matrix[8]);
  // row 2
  res.matrix[2] = (m1.matrix[2]*m2.matrix[0]) + (m1.matrix[5]*m2.matrix[1]) + (m1.matrix[8]*m2.matrix[2]);
  res.matrix[5] = (m1.matrix[2]*m2.matrix[3]) + (m1.matrix[5]*m2.matrix[4]) + (m1.matrix[8]*m2.matrix[5]);
  res.matrix[8] = (m1.matrix[2]*m2.matrix[6]) + (m1.matrix[5]*m2.matrix[7]) + (m1.matrix[8]*m2.matrix[8]);

  return res;
}

matrix9f operator* (const matrix9f &m1, const matrix16f &m2)
{
  matrix9f res;

  // row 0
  res.matrix[0] = (m1.matrix[0]*m2.matrix[0]) + (m1.matrix[3]*m2.matrix[1]) + (m1.matrix[6]*m2.matrix[2]);
  res.matrix[3] = (m1.matrix[0]*m2.matrix[4]) + (m1.matrix[3]*m2.matrix[5]) + (m1.matrix[6]*m2.matrix[6]);
  res.matrix[6] = (m1.matrix[0]*m2.matrix[8]) + (m1.matrix[3]*m2.matrix[9]) + (m1.matrix[6]*m2.matrix[10]);
  // row 1
  res.matrix[1] = (m1.matrix[1]*m2.matrix[0]) + (m1.matrix[4]*m2.matrix[1]) + (m1.matrix[7]*m2.matrix[2]);
  res.matrix[4] = (m1.matrix[1]*m2.matrix[4]) + (m1.matrix[4]*m2.matrix[5]) + (m1.matrix[7]*m2.matrix[6]);
  res.matrix[7] = (m1.matrix[1]*m2.matrix[8]) + (m1.matrix[4]*m2.matrix[9]) + (m1.matrix[7]*m2.matrix[10]);
  // row 2
  res.matrix[2] = (m1.matrix[2]*m2.matrix[0]) + (m1.matrix[5]*m2.matrix[1]) + (m1.matrix[8]*m2.matrix[2]);
  res.matrix[5] = (m1.matrix[2]*m2.matrix[4]) + (m1.matrix[5]*m2.matrix[5]) + (m1.matrix[8]*m2.matrix[6]);
  res.matrix[8] = (m1.matrix[2]*m2.matrix[8]) + (m1.matrix[5]*m2.matrix[9]) + (m1.matrix[8]*m2.matrix[10]);

  return res;
}

matrix9f operator* (const matrix16f &m1, const matrix9f &m2)
{
  matrix9f res;

  // row 0
  res.matrix[0] = (m1.matrix[0]*m2.matrix[0]) + (m1.matrix[4]*m2.matrix[1]) + (m1.matrix[8]*m2.matrix[2]);
  res.matrix[3] = (m1.matrix[0]*m2.matrix[3]) + (m1.matrix[4]*m2.matrix[4]) + (m1.matrix[8]*m2.matrix[5]);
  res.matrix[6] = (m1.matrix[0]*m2.matrix[6]) + (m1.matrix[4]*m2.matrix[7]) + (m1.matrix[8]*m2.matrix[8]);
  // row 1
  res.matrix[1] = (m1.matrix[1]*m2.matrix[0]) + (m1.matrix[5]*m2.matrix[1]) + (m1.matrix[9]*m2.matrix[2]);
  res.matrix[4] = (m1.matrix[1]*m2.matrix[3]) + (m1.matrix[5]*m2.matrix[4]) + (m1.matrix[9]*m2.matrix[5]);
  res.matrix[7] = (m1.matrix[1]*m2.matrix[6]) + (m1.matrix[5]*m2.matrix[7]) + (m1.matrix[9]*m2.matrix[8]);
  // row 2
  res.matrix[2] = (m1.matrix[2]*m2.matrix[0]) + (m1.matrix[6]*m2.matrix[1]) + (m1.matrix[10]*m2.matrix[2]);
  res.matrix[5] = (m1.matrix[2]*m2.matrix[3]) + (m1.matrix[6]*m2.matrix[4]) + (m1.matrix[10]*m2.matrix[5]);
  res.matrix[8] = (m1.matrix[2]*m2.matrix[6]) + (m1.matrix[6]*m2.matrix[7]) + (m1.matrix[10]*m2.matrix[8]);

  return res;
}

matrix9f operator/ (const matrix9f &m1, const float scalar)
{  
  matrix9f res;

  for (int i = 0; i<9; i++) {
    res.matrix[i]= m1.matrix[i]/scalar;
  }
  return res;
}

bool operator== (const matrix9f &m1, const matrix9f &m2)
{
  bool res = true;

  for (int i = 0; i< 9; i++) {
    if (m1.matrix[i] != m2.matrix[i]) res = false;
  }
  return res;
}

#if 0 //notcalled
//////////////////////////////////////////////////////////////////
// This makes a normal based on the surface normals of all triangles adjacent
// to the point, regardless ('irregardless' as Homer would say) of the size of the
// angle that triangle makes with the point
vector3f findNorm(vector3f &matchVertex, int numSurfTriangles, triangleV **surfTriangles)
{
  vector3f norm;

  for (int i = 0; i < numSurfTriangles; i++) {
    for (int j = 0; j < 3; j++) {
      if (surfTriangles[i]->vertice[j] == &matchVertex) {  
        norm = norm + surfTriangles[i]->norm;
      }
    }
  }
  norm = norm/norm.length();
  return norm;
}
#endif //notcalled

#if 0 //notcalled
light::light(camera *viewer, int GL_LIGHTX, float maxFade, float minFade, float scale)
{
  echo("light::light %d sc=%.2f", GL_LIGHTX, scale);
  //verbose = true;
  
  if (verbose) cout << "light " << GL_LIGHTX << " init..." << endl;

  this->viewer = viewer;
  this->GL_LIGHTX = GL_LIGHTX;
  
  /// Distances are compared squared, so just square these now to save time
  this->maxFade = maxFade * maxFade;
  this->minFade = minFade * minFade;
  this->scale = scale;

  GLfloat pos[] = {location.matrix[12],location.matrix[13],location.matrix[14],1.0f};

  Ka[0]=0.3f; Ka[1]=0.3f; Ka[2]=0.3f; Ka[3]=1.0f; 
  Kd[0]=0.5f; Kd[1]=0.5f; Kd[2]=0.5f; Kd[3]=1.0f; 
  Ks[0]=0.3f; Ks[1]=0.3f; Ks[2]=0.3f; Ks[3]=1.0f; 
  
  if ((GL_LIGHTX >= GL_LIGHT0) && (GL_LIGHTX <= GL_LIGHT7)) {
    glEnable(GL_LIGHTX);
    glLightfv(GL_LIGHTX, GL_POSITION,pos);
    glLightfv(GL_LIGHTX, GL_AMBIENT, Ka);  
    glLightfv(GL_LIGHTX, GL_DIFFUSE, Kd);
    glLightfv(GL_LIGHTX, GL_SPECULAR,Ks);
  }
  if (verbose) cout << "finished\n\t";
}

light::~light()
{
  glDisable(GL_LIGHTX);
}

void light::draw()
{
  echo("light::draw");
  glPushMatrix();
  glEnable(GL_LIGHTING);  
  GLfloat pos[] = {location.matrix[12],location.matrix[13],location.matrix[14],1.0f};
  
  if ((GL_LIGHTX >= GL_LIGHT0) && (GL_LIGHTX <= GL_LIGHT7)) {
    glEnable(GL_LIGHTX);
    glLightfv(GL_LIGHTX, GL_POSITION,pos);
    glLightfv(GL_LIGHTX, GL_AMBIENT, Ka);  
    glLightfv(GL_LIGHTX, GL_DIFFUSE, Kd);
    glLightfv(GL_LIGHTX, GL_SPECULAR,Ks);
  }
  glDisable(GL_LIGHTING); 
  glPopMatrix();
}

void light::drawDim(vector3f distant)
{
  echo("light::drawDim");
  glPushMatrix();  

  vector3f lightPos(location.matrix[12],location.matrix[13],location.matrix[14]);
  GLfloat pos[] = {location.matrix[12],location.matrix[13],location.matrix[14],1.0f};
  
  float distanceSquared = (lightPos-distant).length2();
  float attenuate; // = 1.0;

  if (maxFade != 0) {
    if (distanceSquared < minFade) 
      attenuate = scale;
    else if ((distanceSquared < maxFade) && (distanceSquared > minFade))
      attenuate = scale * (maxFade - distanceSquared) / (maxFade - minFade);
    else attenuate = 0;
  } else attenuate = 1;  
      
  GLfloat Kaf[] = {Ka[0] * attenuate, Ka[1] * attenuate, Ka[2] * attenuate, Ka[3],1.0f};
  GLfloat Kdf[] = {Kd[0] * attenuate, Kd[1] * attenuate, Kd[2] * attenuate, Kd[3],1.0f};
  GLfloat Ksf[] = {Ks[0] * attenuate, Ks[1] * attenuate, Ks[2] * attenuate, Ks[3],1.0f};
  
  if ((GL_LIGHTX >= GL_LIGHT0) && (GL_LIGHTX <= GL_LIGHT7)) {
    glEnable(GL_LIGHTX);
    glLightfv(GL_LIGHTX, GL_POSITION, pos);
    glLightfv(GL_LIGHTX, GL_AMBIENT, Kaf);  
    glLightfv(GL_LIGHTX, GL_DIFFUSE, Kdf);
    glLightfv(GL_LIGHTX, GL_SPECULAR,Ksf);
  }
  glPopMatrix();
}

void light::getBoundingBox()
{
  echo("light::getBB");
#if 0
  centerBB = lightobj->centerBB;
  edgesBB = lightobj->edgesBB;
  for (int i=0; i<8;i++) {
    boundingBox[i] = lightobj->boundingBox[i]; 
  }
#endif
}
#endif //notcalled


#if 0 //notcalled
camera::camera()
{
  init();
}

camera::camera(string name, mode lookMode)
{
  echo("camera::camera");
  this->name = name;
  this->lookMode = lookMode;
  //cout << lookMode << "\n";
  init();
}

void camera::init()
{
  echo("camera::init");
  radius = 10.;

  //location.identity();
  //location.matrix[10] = -1.0f;  // this is critical!! remember in OpenGL x cross y = z
  
  other.identity();
  other.matrix[10] = -1.0f;  
  
  // position camera behind and above object
  other.RotateY(180);

  for (int i=0; i<20;i++) {
    delay[i].identity();
    delay[i].matrix[10] = -1.0f;
  }
  angles.vertex[0] = 0.0f;//M_PI/2;
  angles.vertex[1] = 0.0f;//3*M_PI/4;
  angles.vertex[2] = 0.0f;
}

void camera::move(int pitch, int turn, int roll, float x, float y, float z)
{
  echo("camera::move");
  vector3f temp;
  
  switch (lookMode) {
  case FREE:
    if (pitch != 0) location.RotateX(-pitch/ROTATE_SLOWNESS);
    if (turn != 0) location.RotateY(-turn/ROTATE_SLOWNESS);
    if (roll != 0) location.RotateZ(roll/ROTATE_SLOWNESS);
    location.translate(x,y,z);
  break;
  
  case FREEORIENTED:
    angles = angles + vector3f(DEG2RAD(-pitch/ROTATE_SLOWNESS),
                   DEG2RAD(-turn/ROTATE_SLOWNESS), 
                   DEG2RAD( roll/ROTATE_SLOWNESS));
    
    /// vertex[0] is theta, or pitch
    if (angles.vertex[0] >  M_PI/2) angles.vertex[0] =  M_PI/2;
    if (angles.vertex[0] < -M_PI/2) angles.vertex[0] = -M_PI/2;
    /// vertex[1] is phi, or turn   wrap the angles here
    while (angles.vertex[1] > 2*M_PI) angles.vertex[1] -= 2*M_PI;
    while (angles.vertex[1] < 0.0)    angles.vertex[1] += 2*M_PI;
    
    if (angles.vertex[2] >  M_PI/2) angles.vertex[2] =  M_PI/2;
    if (angles.vertex[2] < -M_PI/2) angles.vertex[2] = -M_PI/2;

    temp.set(location.matrix[12],location.matrix[13],location.matrix[14]);
    location.identity();
    location.matrix[10] = -1.0f; 
    location.matrix[12] = temp.vertex[0];
    location.matrix[13] = temp.vertex[1];
    location.matrix[14] = temp.vertex[2];
  
    /// Ordering of rotations is important
    location.RotateY(angles.vertex[1]);
    /// We want to move in the proper direction here
    location.translate(x,y,z);
    /// but not take these rotations into account
    location.RotateX(angles.vertex[0]);
    location.RotateZ(angles.vertex[2]);
  break;
    
  case CENTERED:
    if (pitch != 0) location.RotateX(-2*pitch/ROTATE_SLOWNESS);
    if (turn != 0) location.RotateY(-2*turn/ROTATE_SLOWNESS);
    if (roll != 0) location.RotateZ(2*roll/ROTATE_SLOWNESS);
    location.matrix[12] = centerPoint.vertex[0];
    location.matrix[13] = centerPoint.vertex[1];
    location.matrix[14] = centerPoint.vertex[2];
    radius -= z;
    location.translate(0,0,-radius);
  break;
  }
  newLocation = location;
}

void camera::look()
{
  echo("camera::look");
  gluLookAt( 
        //eye
        location.matrix[12], location.matrix[13], location.matrix[14], 
        // center
        location.matrix[12]+location.matrix[8],
        location.matrix[13]+location.matrix[9],
        location.matrix[14]+location.matrix[10], 
        // up
        location.matrix[4], location.matrix[5], location.matrix[6]);
  return;  
}
#endif

///////////////////////////////
#if 0 //notcalled
material& material::operator= (const material &mat)
{
  name = mat.name;
  faces = mat.faces;
  Se = mat.Se;
  
  for (int i=0; i <4; i++) {  
    Ka[i] = mat.Ka[i];
    Ks[i] = mat.Ks[i];
    Kd[i] = mat.Kd[i];
    Ke[i] = mat.Ke[i];  
  }
  opacity = mat.opacity;
  return *this;
}

triangleInd& triangleInd::operator= (const triangleInd &tri)
{
  for (int i=0; i<3; i++) { vertIndices[i] = tri.vertIndices[i]; }   
  
  normal = tri.normal;
  collision = tri.collision;  
  return *this;  
}
#endif

#if 0 //notcalled replaced by Obj
objloader::objloader(string objFile)
{  
  counter = 0;
  //verbose = true;
  translucent = false;
  drawBB = true;
  
  /// Strip directory info out of objFile
  string::size_type pos = objFile.find_last_of("/");
  this->subdir = objFile.substr(0,pos+1);
  objFile = objFile.substr(pos+1,objFile.size());

  if (verbose) cout << subdir + objFile << " init...";
  try { load(objFile); }
  catch (fileNotFound) { throw fileNotFound(); return;}
  if (verbose) cout << "finished\n";
  
  mass = 1;
  getBoundingBox();
}

objloader::~objloader()
{}

objloader& objloader::operator= (const objloader &obj1)
{
  return *this;
}

bool objloader::load(string objFile)
{
  /*
  // find the last forward slash in the string
  string::size_type lastSlashPosition;
  while (lastSlashPosition = objFile.find("/")) {}
  subdir = objFile.substr(0,lastSlashPosition);
  cout << "subdir = " << subdir << "\n";
  */
  objFile = subdir + objFile;

  ifstream objStream(objFile.c_str());
  if (!objStream) { 
    cout << "file \"" << objFile << "\" not found\n";
    throw fileNotFound(); 
    return false; 
  }
  istream_iterator<string> objIt(objStream);
  istream_iterator<string> sentinel;

  vector<string> lines(objIt, sentinel);
  theMode = NONE;
 
  //  for_each(lines.begin(),lines.end(),&bvh::process);
  uint32_t i;
  for (i=0; i < lines.size(); i++) {
    try { process(lines[i]); }
    catch (fileNotFound) { throw fileNotFound(); return false; }
  }
  
  if (verbose) cout << vertices.size() << " vertices\n";
  if (verbose) cout << normals.size() << " normals (should == vertices)\n";
  if (verbose) cout << mtls.size() << " materials\n";
  
  /// find face normals for collision detection and other uses
  vector3f normal;
  for (i=0; i < mtls.size(); i++) {        
    for (uint32_t j = 0; j < mtls[i]->faces.size(); j++) {
      int ind0 = mtls[i]->faces[j].vertIndices[0]-1;
      int ind1 = mtls[i]->faces[j].vertIndices[1]-1;
      int ind2 = mtls[i]->faces[j].vertIndices[2]-1;

      normal = crossProduct(vertices[ind1] - vertices[ind0], vertices[ind2] - vertices[ind0]);
      normal = normal/(normal.length());
      mtls[i]->faces[j].normal = normal;
    }
  }
  return true;
}
  
void objloader::process(string line)
{  
  cout << "process " << line << "\n";
  int index; 
  string::size_type slashes;
  
  //cout << "Mode = " << theMode << "\n";
  if ((line.size()) && (line.substr(0,1) != "#")) {
    if (line == "mtllib") {
      theMode = MTLLIB;
    }
    else if (line == "v") {
      theMode = VERTEX;
      tempVectorIndex = 0;
    }
    else if (line == "vn") {
      theMode = NORMAL; 
      tempVectorIndex = 0;
    }
    else if (line == "f") {
      theMode = FACE;
      tempVectorIndex = 0;
    }
    else switch (theMode) {
      case (MTLLIB):
#if 1 //dax
        mtlIndex = 0;
#else
        cout << "\n\tparsing .mtl file: " << subdir + line << "...";
        mtlFile = line;
        
        if (!matchMtl(mtlIndex, mtlFile)) {
          try { loadMtl(mtlFile); }
          catch (fileNotFound) { throw fileNotFound(); return; }
            
          matchMtl(mtlIndex, mtlFile);
        }
        cout << ".mtl " << mtlIndex << " done\n";
        theMode = NONE;
#endif
        break;
      case (VERTEX):
        tempVector.vertex[tempVectorIndex] = atof(line.c_str());
        cout << tempVector.vertex[tempVectorIndex] << "\n";
        tempVectorIndex++;
        if (tempVectorIndex >=3) {
          theMode = NONE;
          vertices.push_back(tempVector);  
        }
        break;
      case (NORMAL):
        tempVector.vertex[tempVectorIndex] = atof(line.c_str());
        tempVectorIndex++;
        if (tempVectorIndex >=3) {
          theMode = NONE;
          normals.push_back(tempVector);
        }
        break;
      case (FACE):
        /// face indices are denoted index//index (redundantly) in the .obj format
        slashes = line.find("//");
        index = atoi(line.substr(0,slashes).c_str());
        cout << slashes << " " << index << "\n";
        tempTriangle.vertIndices[tempVectorIndex] = index;
        tempVectorIndex++;
        if (tempVectorIndex >= 3) {
          theMode = NONE;
          mtls[mtlIndex]->faces.push_back(tempTriangle);  
        }
        break;
      case (NONE):
        break;
    }
  }
}

void objloader::processMtl(string line, material *mtl)
{  
  echo("objloader::processMtl");
#if 1 //dax
  if ((line.size()) && (line.substr(0,1) != "#")) {
    if (line == "newmtl") theMtlMode = NEWMTL;
    else if (line == "Ns") theMtlMode = NS;
    else  if (line == "d") theMtlMode = D;
    else  if (line == "illum") theMtlMode = ILLUM;
    else  if (line == "Kd") { theMtlMode = KD; kIndex = 0; }
    else  if (line == "Ka") { theMtlMode = KA; kIndex = 0; }
    else  if (line == "Ks") { theMtlMode = KS; kIndex = 0; }
    else switch (theMtlMode) {
      case KD:
        mtl->Kd[kIndex] = atof(line.c_str());
        kIndex++;
        if (kIndex >= 3) {
          mtl->Kd[3] = 1.0f;
          //cout << mtl->name << " " << mtl->Kd[0] << " " << 
          //     mtl->Kd[1] << " " << mtl->Kd[2] << " " <<
          //     mtl->Kd[3] << "\n";
          theMtlMode = NONEM;
        }
        break;
      case KA:
        mtl->Ka[kIndex] = atof(line.c_str());
        kIndex++;
        if (kIndex >= 3) {
          mtl->Ka[3] = 1.0f;
          theMtlMode = NONEM;
        }
        break;
      case KS:
        mtl->Ks[kIndex] = atof(line.c_str());
        kIndex++;
        if (kIndex >= 3) {
          mtl->Ks[3] = 1.0f;
          theMtlMode = NONEM;
        }
        break;
      case (NONEM):
      case (NEWMTL):
      case (NS):
      case (D):
      case (ILLUM):
        break;
    }
  }
#endif
}

void objloader::loadMtl(string mtlFile)
{  
  echo("objloader::loadMtl");
#if 1 //dax
  ifstream objStream((subdir + mtlFile).c_str());
  if (!objStream) { 
    cout << "file \"" << subdir + mtlFile << "\" not found\n";
    throw fileNotFound(); 
    return;
  }
  istream_iterator<string> objIt(objStream);
  istream_iterator<string> sentinel;
  vector<string> lines(objIt, sentinel);
  theMtlMode = NONEM;
  material* newMtl = new material;
  newMtl->name = mtlFile;
  newMtl->Ke[0] = 0;
  newMtl->Ke[1] = 0;
  newMtl->Ke[2] = 0;
  newMtl->Ke[3] = 1.0f;
  
  for (int i=0; i < lines.size(); i++) {
    processMtl(lines[i], newMtl);
  }
  mtls.push_back(newMtl);
#endif
}

bool objloader::matchMtl(uint32_t &index, string name)
{
  echo("objloader::matchMtl");
#if 1 //dax
  for (int m=0; m < (mtls.size()); m++) {
    if (name == mtls[m]->name) {
      index = m;
      return true;
    }
  }
  cout << (int)mtls.size() <<" mtlssize \n";
#endif
  return false;
}

void objloader::setMass(float newMass)
{
  //echo("objloader::setMass");
  if (step != 0) {
    Ibody = (Ibody*newMass) / mass;
    IbodyInv = Ibody.inverse();
  }
  mass = newMass;
}

void objloader::update(float seconds)
{
  //echo("objloader::update");
  getAABB();
}

void objloader::draw()
{
}

void objloader::getBoundingBox()
{
  echo("objloader::getBB");
  /*  for (int j = 0; j<3;j++) {
    boundingBox[0].vertex[j] = test[0]->location.vertex[j]; // lower right bound
    boundingBox[6].vertex[j] = test[0]->location.vertex[j]; // upper right bound
  } */
  boundingBox[0] = vertices[1];
  boundingBox[6] = vertices[1];

  // find extremities
  for (int i=1; i < vertices.size(); i++) {
    for (int j=0; j<3; j++) {
      if (boundingBox[0].vertex[j] > vertices[i].vertex[j])
        boundingBox[0].vertex[j] = vertices[i].vertex[j]; // lower left   minimum bound 
      if (boundingBox[6].vertex[j] < vertices[i].vertex[j])
        boundingBox[6].vertex[j] = vertices[i].vertex[j]; // upper right  maximum bound 
    }
  }
  centerBB = (boundingBox[0] + boundingBox[6])/2;
  edgesBB = boundingBox[6] - centerBB;

  // having found maximums, populate rest of corners
  boundingBox[1].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[1].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[1].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[2].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[2].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[2].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[3].vertex[0] = boundingBox[0].vertex[0];
  boundingBox[3].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[3].vertex[2] = boundingBox[0].vertex[2];
  boundingBox[4].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[4].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[4].vertex[2] = boundingBox[0].vertex[2];
  boundingBox[5].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[5].vertex[1] = boundingBox[0].vertex[1];
  boundingBox[5].vertex[2] = boundingBox[6].vertex[2];
  boundingBox[7].vertex[0] = boundingBox[6].vertex[0];
  boundingBox[7].vertex[1] = boundingBox[6].vertex[1];
  boundingBox[7].vertex[2] = boundingBox[0].vertex[2];
}
#endif
