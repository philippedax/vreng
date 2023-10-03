//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "wobject.hpp"
#include "matvec.hpp"   // V3 M4
#include "aoi.hpp"	// AOI_TYPE
#include "walls.hpp"	// whenIntersect
#include "user.hpp"	// USER_TYPE
#include "world.hpp"	// current
#include "olist.hpp"	// OList


/* intersection cases */
enum {
  NO_INTER,
  IN2,
  IN1,
  INTERSECT
};

/** Saves last position and Bounding Box of an object */
void WO::copyPosAndBB(Pos &newpos)
{
  newpos = pos;
}

/** Saves last position, Bounding Box and type of an object */
void WO::copyPositionAndBB(WO *o)
{
  if (o) {
    o->type = type;
    o->mode = mode;
    o->num = num;
    o->behavior = behavior;
    o->pos = pos;
  }
}

/* Returns a constant describing intersection status of both Bounding Boxes */
int WO::interAABB(WO *o1, WO *o2)
{
  return interAABB(o1->pos.bbc, o1->pos.bbs, o2->pos.bbc, o2->pos.bbs);
}

/* Returns a constant describing intersection status of both Bounding Boxes */
int WO::interAABB(V3 center1, V3 size1, V3 center2, V3 size2)
{
  float dcx = ABSF(center2.v[0] - center1.v[0]);
  float dcy = ABSF(center2.v[1] - center1.v[1]);
  float dcz = ABSF(center2.v[2] - center1.v[2]);
  float sx1 = ABSF(size1.v[0]);
  float sx2 = ABSF(size2.v[0]);
  float sy1 = ABSF(size1.v[1]);
  float sy2 = ABSF(size2.v[1]);
  float sz1 = ABSF(size1.v[2]);
  float sz2 = ABSF(size2.v[2]);

  if ( (dcz >= sz1+sz2 - 1e-3) || (dcx >= sx1+sx2 - 1e-3) || (dcy >= sy1+sy2 - 1e-3) ) {
    return NO_INTER;	// doesn't intersect == 0
  }
  if (dcz < sz1+sz2 + 1e-3) {
    return INTERSECT;	// intersects == 3
  }
  if ( (dcx+sx1 <= sx2) && (dcy+sy1 <= sy2) && (dcz+sz1 <= sz2) ) {
    return IN2;		// obj1 inside obj2 == 1
  }
  if ( (dcx+sx2 <= sx1) && (dcy+sy2 <= sy1) && (dcz+sz2 <= sz1) ) {
    return IN1;		// obj2 inside obj1 == 2
  }
  if ( (dcy < sy1+sy2) || (dcx < sx1+sx2) ) {
    return INTERSECT;	// == 3
  }
  return NO_INTER;	// doesn't intersect == 0
}

int WO::interAABBVertical(V3 center1, V3 size1, V3 center2, V3 size2)
{
  float dcz = ABSF(center2.v[2] - center1.v[2]);
  float sz1 = ABSF(size1.v[2]);
  float sz2 = ABSF(size2.v[2]);

  if ((dcz >= sz1+sz2 - 1e-3)) return NO_INTER;	// doesn't intersect
  if ((dcz+sz1 <= sz2)) return IN2;		// obj1 inside obj2
  if ((dcz+sz2 <= sz1)) return IN1;		// obj2 inside obj1
  return INTERSECT;
}

int WO::interAABBHorizontal(V3 center1, V3 size1, V3 center2, V3 size2)
{
  float dcx = ABSF(center2.v[0] - center1.v[0]);
  float dcy = ABSF(center2.v[1] - center1.v[1]);
  float sx1 = ABSF(size1.v[0]);
  float sx2 = ABSF(size2.v[0]);
  float sy1 = ABSF(size1.v[1]);
  float sy2 = ABSF(size2.v[1]);

  if ((dcx >= sx1+sx2) || (dcy >= sy1+sy2)) return NO_INTER;	// doesn't intersect
  if ((dcx+sx1 <= sx2) && (dcy+sy1 <= sy2)) return IN2;		// obj1 inside obj2
  if ((dcx+sx2 <= sx1) && (dcy+sy2 <= sy1)) return IN1;		// obj2 inside obj1
  return INTERSECT;
}

/* Checks intersection with Walls */
void WO::ingoingWalls(WO *pold)
{
  V3 normal;

  if (Walls::whenIntersect(pos.bbc, pos.bbs, normal)) {
    float nx = normal.v[0];
    float ny = normal.v[1];
    float cx = pos.x;
    float cy = pos.y;
    float ox = pold->pos.x;
    float oy = pold->pos.y;

    if ((ox != cx) || (oy != cy)) {
      float nn = nx*nx + ny*ny;
      if (nn == 0)  return;
      //trace(DBG_WO, "(ox=%.2f oy=%.2f) (cx=%.2f cy=%.2f)", ox, oy, cx, cy);
      pos.y = ((nx*ny) * (ox-cx) + ny*ny*oy + nx*nx*cy) / nn;
      pos.x = ((nx*ny) * (oy-cy) + nx*nx*ox + ny*ny*cx) / nn;

      // user position has changed => need to update BBs
      updatePositionAndGrid(pold);
    }
    pold->copyPositionAndBB(this);
  }
}

/*
 * Checks for ingoing intersection between current object and neighbor objects.
 * There is collision if current object intersects a neighbor object
 * and if its old position didn't intersect.
 */
bool WO::ingoingNeighbor(WO *pold, WO *neighbor)
{
  if ((interAABB(pos.bbc,           pos.bbs,
                 neighbor->pos.bbc, neighbor->pos.bbs) != NO_INTER) &&
      (interAABB(pold->pos.bbc,     pold->pos.bbs,
                 neighbor->pos.bbc, neighbor->pos.bbs) == NO_INTER)) {

    // check wether the neighbor is oblique
    int az = (int) RAD2DEG(neighbor->pos.az) % 90;
    int ax = (int) RAD2DEG(neighbor->pos.ax) % 90;
    if (((az >= 2) && (az <= 88)) ||
        ((ax >= 2) && (ax <= 88))) {
      //trace(DBG_FORCE, "%s is oblique az=%d ax=%d", neighbor->getInstance(), az, ax);
      // TODO: raffinate the collision
      return true;
    }
    return true;
  }
  return false;
}

/* Checks for outgoing intersection (lift, step, guide, attractor,...) */
bool WO::outgoingNeighbor(WO *pold, WO *neighbor)
{
  if ((interAABB(pos.bbc,           pos.bbs,
                 neighbor->pos.bbc, neighbor->pos.bbs) == NO_INTER) &&
      (interAABB(pold->pos.bbc,     pold->pos.bbs,
                 neighbor->pos.bbc, neighbor->pos.bbs) != NO_INTER)) {
    return true;
  }
  return false;
}

/*
 * General function to handle intersections
 *
 * Warning: WO *pold is an incomplete copy of *this
 */
void WO::generalIntersect(WO *pold, OList *vicinity)
{
  /* check walls first (maybe expensive) */
  ingoingWalls(pold);

  /*
   * check neighbors
   */
  int scans = 0;
  int rescans = 0;
  // held the first object
  WO *wofirst = (vicinity && vicinity->pobject) ? vicinity->pobject : NULL;

  // Scans neighbors for collision discovery
  for (OList *vl = vicinity; vl ; scans++) {
    if (! vl->pobject) {
      vl = vl->next;
      continue;
    }  // discard non existant object

    WO *neighbor = vl->pobject;

    // Hack-3! Assertion on valid neighbor
    if (! neighbor) {
      vl = vl->next;
      continue;
    }

    // Hack-1! Skip scanning if neighbor has already been seen
    if ((neighbor == wofirst) && (scans > 1)) {
      //echo("first=%s scans=%d", wofirst->getInstance(), scans);
      vl = vl->next;
      continue;
    }
    if (neighbor->behavior & COLLIDE_NEVER) {
      vl = vl->next;
      continue;
    }

    if (ingoingNeighbor(pold, neighbor)) {
      // current object intersects and its old instance didn't intersect
      switch (neighbor->type) {
      case AOI_TYPE:
        if (this == (WO *) localuser) {
          if (currentAoi != neighbor) {
            Aoi *aoi = (Aoi *) neighbor;
            aoi->aoiEnter();	// avatars: change mcast address
          }
        }
        else  // other mobile objects: problem of property transfert
          ;
        break;  // avoids a warning

      default:
        if (! neighbor->whenIntersect(this, pold)) { // call the object itself
          vl = vl->next;
          continue;
        }

        // assertion
        if (! isValid()) {
          vl = vl->next;
          continue;
        }
        if (! neighbor->num) {
          vl = vl->next;
          continue;
        }
        switch (neighbor->collideBehavior()) {
        case COLLIDE_ONCE: case COLLIDE_GHOST:
          vl = vl->next;
          continue;
        default:
          if (isBehavior(COLLIDE_GHOST) || isBehavior(COLLIDE_ONCE)) {
            vl = vl->next;
            continue;
          }
          else {
            if (rescans++ > 99) {
              echo("collide loop between %s & %s", getInstance(), neighbor->getInstance());
              scans = rescans = 0;
              vl = vl->next;
              continue;
            }
            scans = 0;
            vl = vicinity;	// not next COLLIDE_EVER ?
          }
        }
      }
    }
    else if (outgoingNeighbor(pold, neighbor)) { // current object leaves intersection
      neighbor->whenIntersectOut(this, pold);
    }
    if (vl) {
      vl = vl->next;
    }
  } //end neighbors
}

uint32_t WO::collideBehavior() const
{
  return (behavior & (COLLIDE_ONCE | COLLIDE_NEVER | COLLIDE_GHOST));
}


/** Returns normal vectors 'normal' of still object */
void WO::computeNormal(WO *mobil, V3 *normal)
{
  float fxmin = pos.bbc.v[0] - pos.bbs.v[0];
  float fxmax = pos.bbc.v[0] + pos.bbs.v[0];
  float fymin = pos.bbc.v[1] - pos.bbs.v[1];
  float fymax = pos.bbc.v[1] + pos.bbs.v[1];

  normal->v[2] = 1;
  if (fxmax - fxmin < fymax - fymin) {
    normal->v[0] = 1;
    normal->v[1] = 0;
  } else {
    normal->v[0] = 0;
    normal->v[1] = 1;
  }
  /* test which normal we take, computes dotprod */
  float dp = (mobil->pos.bbc.v[0] - pos.bbc.v[0]) * normal->v[0]
           + (mobil->pos.bbc.v[1] - pos.bbc.v[1]) * normal->v[1]
           + (mobil->pos.bbc.v[2] - pos.bbc.v[2]) * normal->v[2];
  if (dp < 0) {
    normal->v[0] = -normal->v[0];
    normal->v[1] = -normal->v[1];
    normal->v[2] = -normal->v[2];
  }
}

void WO::computeNormal(Pos &mobil, Pos &still, V3 *normal)
{
  normal->v[2] = 1;
  if (still.bbs.v[0] < still.bbs.v[1]) {
    normal->v[0] = 1;
    normal->v[1] = 0;
  } else {
    normal->v[0] = 0;
    normal->v[1] = 1;
  }

  /* test which normal we take, computes dotprod */
  float dp = (mobil.bbc.v[0] - still.bbc.v[0]) * normal->v[0]
           + (mobil.bbc.v[1] - still.bbc.v[1]) * normal->v[1]
           + (mobil.bbc.v[2] - still.bbc.v[2]) * normal->v[2];
  if (dp < 0) {
    normal->v[0] = -normal->v[0];
    normal->v[1] = -normal->v[1];
    normal->v[2] = -normal->v[2];
  }
}

/* Projete le mouvement de l'objet mobile parallelement a l'objet fixe */
int WO::projectPositionOnObstacle(Pos &mobil, Pos &mobilold, Pos &obstacle)
{
  V3 normal;
  float dx = mobil.x - mobilold.x;
  float dy = mobil.y - mobilold.y;
  float dz = 0; //pdcol mobil.z - mobilold.z;

  if ((ABSF(dx) + ABSF(dy) + ABSF(dz)) == 0)  return 0;	// nothing to do

  computeNormal(mobil, obstacle, &normal);
  float nx = normal.v[0];
  float ny = normal.v[1];
  float nz = normal.v[2];
  float dp = dx*nx + dy*ny + dz*nz;	// dotprod

  if (dx) mobil.x = mobil.x - dp*nx;
  if (dy) mobil.y = mobil.y - dp*ny;
  if (dz) mobil.z = mobil.z - dp*nz;

  if (dp > 0) return 1;
  if (dp < 0) return -1;
  return 0;
}

/** Project position on the obstacle */
bool WO::projectPosition(WO *pcur, WO *pold)
{
  if (projectPositionOnObstacle(pcur->pos, pold->pos, pos)) {
    pcur->updatePositionAndGrid(pold);
    return true;
  }
  pold->copyPosAndBB(pcur->pos);
  return false;
}

/** Bounce position, we do not change the position, we only change the deltas */
void WO::bounceTrajectory(WO *pold, V3 *normal)
{
  pold->copyPositionAndBB(this);

  float nx = normal->v[0];
  float ny = normal->v[1];
  float nz = normal->v[2];
  float vx = move.lspeed.v[0];
  float vy = move.lspeed.v[1];
  float vz = move.lspeed.v[2];
  float nn = sqrt(vx*vx + vy*vy + vz*vz);
  if (nn == 0)  return;

  float vxn = vx/nn;
  float vyn = vy/nn;
  float vzn = vz/nn;
  float dp = vxn*nx + vyn*ny + vzn*nz;
  if (dp >= 0)  return;

  move.lspeed.v[0] = (vxn - 2*dp*nx) * nn;
  move.lspeed.v[1] = (vyn - 2*dp*ny) * nn;
  move.lspeed.v[2] = (vzn - 2*dp*nz) * nn;
}

//
// GRID vicinity
//

static uint8_t dim[3];
static V3 dist;

// sets locals
void World::localGrid()
{
  for (int i=0; i<3 ; i++) {
    dim[i] = dimgrid[i];
  }
  dist = setV3(bbslice.v[0], bbslice.v[1], bbslice.v[2]);
}

/**
 * Evals and fills the igrid array
 * array of position indices in the grid
 */
static void indiceGrid(const float bb[3], int igrid[3])
{
  igrid[0] = (int) ((bb[0] + dist.v[0] * (dim[0]/2)) / dist.v[0]);
  igrid[1] = (int) ((bb[1] + dist.v[1] * (dim[1]/2)) / dist.v[1]);
  igrid[2] = (int) ((bb[2] + dist.v[2] * (dim[2]/2)) / dist.v[2]);
  igrid[0] = MIN(dim[0]-1, MAX(0, igrid[0]));
  igrid[1] = MIN(dim[1]-1, MAX(0, igrid[1]));
  igrid[2] = MIN(dim[2]-1, MAX(0, igrid[2]));
}

/** Adds an object into the grid */
void WO::insertIntoGrid()
{
  float bbmin[3], bbmax[3];

  for (int i=0; i<3; i++) {
    bbmin[i] = pos.bbc.v[i] - pos.bbs.v[i]; // pos min
    bbmax[i] = pos.bbc.v[i] + pos.bbs.v[i]; // pos max
  }

  int imin[3], imax[3];
  indiceGrid(bbmin, imin);
  indiceGrid(bbmax, imax);

  for (int x=imin[0]; x <= imax[0]; x++) {
    for (int y=imin[1]; y <= imax[1]; y++) {
      for (int z=imin[2]; z <= imax[2]; z++) {
	World::gridArray[x][y][z] = addOListOnce(World::gridArray[x][y][z]);
      }
    }
  }
}

/** Deletes an object from the vicinity grid.
 *  This estimation doesn't work 100%.
 *  Doing the above reduces crash rate when sending a bunch of darts.
 */
void WO::delFromGrid()
{
  float bbmin[3], bbmax[3];

  for (int i=0; i<3; i++) {
    bbmin[i] = pos.bbc.v[i] - pos.bbs.v[i];
    bbmax[i] = pos.bbc.v[i] + pos.bbs.v[i];
  }

  int imin[3], imax[3];
  indiceGrid(bbmin, imin);
  indiceGrid(bbmax, imax);

  for (int x=imin[0]; x <= imax[0]; x++) {
    for (int y=imin[1]; y <= imax[1]; y++) {
      for (int z=imin[2]; z <= imax[2]; z++) {
        if (World::gridArray[x][y][z]) {
	  World::gridArray[x][y][z] = delOList(World::gridArray[x][y][z]);
        }
      }
    }
  }
}

/** Updates an object in the grid */
void WO::updateGrid(const float *bbminnew, const float *bbmaxnew, const float *bbminold, const float *bbmaxold)
{
  int iminnew[3], imaxnew[3], iminold[3], imaxold[3];

  indiceGrid(bbminnew, iminnew);
  indiceGrid(bbmaxnew, imaxnew);
  indiceGrid(bbminold, iminold);
  indiceGrid(bbmaxold, imaxold);

  bool change = false;
  for (int i=0; i<3; i++) {
    if ( (iminnew[i] != iminold[i]) || (imaxnew[i] != imaxold[i]) )
      change = true;
  }

  if (change) {
    delFromGrid();
    for (int x=iminnew[0]; x <= imaxnew[0]; x++) {
      for (int y=iminnew[1]; y <= imaxnew[1]; y++) {
	for (int z=iminnew[2]; z <= imaxnew[2]; z++) {
	  World::gridArray[x][y][z] = addOListOnce(World::gridArray[x][y][z]);
        }
      }
    }
  }
}

void WO::updateGrid(const WO *obj)
{
  float bbminnew[3], bbmaxnew[3], bbminold[3], bbmaxold[3];

  for (int i=0; i<3; i++) {
    bbminnew[i] = pos.bbc.v[i] - pos.bbs.v[i];
    bbmaxnew[i] = pos.bbc.v[i] + pos.bbs.v[i];
    bbminold[i] = obj->pos.bbc.v[i] - obj->pos.bbs.v[i];
    bbmaxold[i] = obj->pos.bbc.v[i] + obj->pos.bbs.v[i];
  }
  updateGrid(bbminnew, bbmaxnew, bbminold, bbmaxold);
}

void WO::updateGrid(const Pos& oldpos)
{
  float bbminnew[3], bbmaxnew[3], bbminold[3], bbmaxold[3];

  for (int i=0; i<3; i++) {
    bbminnew[i] = pos.bbc.v[i] - pos.bbs.v[i];
    bbmaxnew[i] = pos.bbc.v[i] + pos.bbs.v[i];
    bbminold[i] = oldpos.bbc.v[i] - oldpos.bbs.v[i];
    bbmaxold[i] = oldpos.bbc.v[i] + oldpos.bbs.v[i];
  }
  updateGrid(bbminnew, bbmaxnew, bbminold, bbmaxold);
}

/**
 * retourne la liste des pointeurs sur les objets touchants
 * la case des grilles intersectees par la BB englobante des 2 objets
 */
OList * WO::getVicinity(const WO *obj)
{
  float bbmin[3], bbmax[3];

  for (int i=0; i<3; i++) {
    bbmin[i] = MIN(pos.bbc.v[i] - pos.bbs.v[i],
               obj->pos.bbc.v[i] - obj->pos.bbs.v[i]);
    bbmax[i] = MAX(pos.bbc.v[i] + pos.bbs.v[i],
               obj->pos.bbc.v[i] + obj->pos.bbs.v[i]);
  }

  int imin[3], imax[3];
  indiceGrid(bbmin, imin);
  indiceGrid(bbmax, imax);

  OList *vl = NULL;
  for (int x=imin[0]; x <= imax[0]; x++) {
    for (int y=imin[1]; y <= imax[1]; y++) {
      for (int z=imin[2]; z <= imax[2]; z++) {
        vl = addListToList(World::gridArray[x][y][z], vl);
      }
    }
  }

  if (vl) {
    vl->clearIspointed();
  }
  return vl;
}