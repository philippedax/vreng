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
/* GiMd2Viewer - Quake2 model viewer
 * Copyright (C) 1998  Lionel ULMER <bbrox@mygale.org>
 *
 * Based on code Copyright (C) 1997 Trey Harrison <trey@crack.com>
 */
#include "vreng.hpp"
#include "md2.hpp"
#include "wobject.hpp"	// Pos
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open, close
#include "file.hpp"	// read_*
#include "draw.hpp"	// vertex3f


Md2::Md2(const char *_url)
{
  numframes = 0;
  skinwidth = 0;
  skinheight = 0;
  texinfo = NULL;
  glcmds = NULL;
  frames = NULL;
  loaded = false;
  desiredScale = 1;
  dlists = NULL;
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, httpReader, this, 0);
}

Md2::~Md2()
{
#if 0 //FIXME segfault
  for (int f=0; f < numframes; f++)
    delete[] frames[f].vert_table;
  delete[] frames;
  delete[] glcmds;
  delete[] texinfo;
  //FIXME! if (dlists > 0) glDeleteLists(dlists, 1);
  if (dlists > 0 && dlists < 1000) delete[] dlists;
#endif
  if (url) delete[] url;
}

/** Md2 model http-reader */
void Md2::httpReader(void *_md2, Http *http)
{
  Md2 *md2 = static_cast<Md2 *>(_md2);
  if (! md2) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't read %s", http->url);
    delete cache;
    return;
  }
  File *file = new(File);
  md2->readFile(file, f);	// from cache
  cache->close();
  delete cache;
}

/** Md2 model filereader */
bool Md2::readFile(class File *file, FILE *f)
{
  /* Read the header first */
  tHeader md2_hdr;
  int32_t o = 0;	// offset in file

  // read in little endian format (read_long_le)
  file->read_buf(f, md2_hdr.ident, 4); o+=4;
  md2_hdr.version = file->read_long_le(f); o+=4;
  md2_hdr.skinwidth = file->read_long_le(f); o+=4;
  md2_hdr.skinheight = file->read_long_le(f); o+=4;
  md2_hdr.framesize = file->read_long_le(f); o+=4;
  md2_hdr.num_skins = file->read_long_le(f); o+=4;
  md2_hdr.num_xyz = file->read_long_le(f); o+=4;
  md2_hdr.num_st = file->read_long_le(f); o+=4;
  md2_hdr.num_tris = file->read_long_le(f); o+=4;
  md2_hdr.num_glcmds = file->read_long_le(f); o+=4;
  md2_hdr.num_frames = file->read_long_le(f); o+=4;
  md2_hdr.ofs_skins = file->read_long_le(f); o+=4;
  md2_hdr.ofs_st = file->read_long_le(f); o+=4;
  md2_hdr.ofs_tris = file->read_long_le(f); o+=4;
  md2_hdr.ofs_frames = file->read_long_le(f); o+=4;
  md2_hdr.ofs_glcmds = file->read_long_le(f); o+=4;
  md2_hdr.ofs_end = file->read_long_le(f); o+=4;

  /* check if this is really a .md2 file */
  if (strncmp(md2_hdr.ident, "IDP2", 4)) return false;
  /* we do not need all the info from the header, just some of it */
  numframes = md2_hdr.num_frames;
  skinwidth = md2_hdr.skinwidth;
  skinheight = md2_hdr.skinheight;

  /* check which info is first */
  if (md2_hdr.ofs_frames > md2_hdr.ofs_glcmds) {
    file->skip(f, md2_hdr.ofs_glcmds - o); o = md2_hdr.ofs_glcmds;
    o += getGLCmds(&md2_hdr, file, f);
    file->skip(f, md2_hdr.ofs_frames - o); o = md2_hdr.ofs_frames;
    o += getFrames(&md2_hdr, file, f);
  }
  else {
    file->skip(f, md2_hdr.ofs_frames - o); o = md2_hdr.ofs_frames;
    o += getFrames(&md2_hdr, file, f);
    file->skip(f, md2_hdr.ofs_glcmds - o); o = md2_hdr.ofs_glcmds;
    o += getGLCmds(&md2_hdr, file, f);
  }
  loaded = true;
  //echo("md2 loaded");
  return loaded;
}

int32_t Md2::getFrames(tHeader *md2_hdr, class File *file, FILE *f)
{
  /* converts the FrameInfos to Frames */
  frames = new tFrame[numframes];

  // allocate display list
  dlists = new GLint[numframes];

  int32_t o = 0;
  for (int fr=0; fr < numframes; fr++) {
    tVec3 scale, origin;

    scale.x = file->read_float_le(f); o +=4;
    scale.y = file->read_float_le(f); o +=4;
    scale.z = file->read_float_le(f); o +=4;
    origin.x = file->read_float_le(f); o +=4;
    origin.y = file->read_float_le(f); o +=4;
    origin.z = file->read_float_le(f); o +=4;
    file->read_buf(f, frames[fr].name, 16); o +=16;

    frames[fr].vert_table = new tVertex[md2_hdr->num_xyz];

    /* loads the vertices */
    for (int i=0; i < md2_hdr->num_xyz; i++) {
      tTrivertex cur_vert;
      tVertex *p = (frames[fr].vert_table) + i;

      cur_vert.x = file->read_char(f); o++;
      cur_vert.y = file->read_char(f); o++;
      cur_vert.z = file->read_char(f); o++;
      cur_vert.normal = file->read_char(f); o++;
      p->x = ((cur_vert.x * scale.x) + origin.x);
      p->y = ((cur_vert.y * scale.y) + origin.y);
      p->z = ((cur_vert.z * scale.z) + origin.z);
    }
  }
  return o;
}

/**
 * we keep only the commands and the index in the glcommands
 * we 'pre-parse' the texture coordinates
 * do not ask me how I found this formula :-))
 */
int32_t Md2::getGLCmds(tHeader *md2_hdr, class File *file, FILE *f)
{
  int32_t num_vertices = ((md2_hdr->num_tris + 2 * md2_hdr->num_glcmds - 2) / 7);
  //echo("getGLCmds: num_vertices=%d", num_vertices);

  texinfo = new tTexInfo[num_vertices];
  glcmds = new int32_t[md2_hdr->num_glcmds - 2 * num_vertices];
  //echo("glcmds size = %d", sizeof(int32_t) * (md2_hdr->num_glcmds - 2 * num_vertices));

  /* now transform the GL commands */
  int32_t *glcmds_copy = glcmds;
  tTexInfo *_texinfo = texinfo;

  int32_t glcmd, o;

  for (o = 0, glcmd = 0; (glcmd = file->read_long_le(f)) != 0; ) {
    int32_t nb_verts;

    o += 4;

    /* determine the command to draw the triangles */
    if (glcmd > 0)	// triangle strip
      nb_verts = glcmd;
    else		// triangle fan
      nb_verts = -glcmd;
    *(glcmds_copy++) = glcmd;	// copy the command

    /* gets the texture information */
    for (int i=0; i < nb_verts; i++) {
      _texinfo->s = file->read_float_le(f); o+=4;
      _texinfo->t = file->read_float_le(f); o+=4;
      _texinfo++;

      /* we keep the vertex index */
      glcmd = file->read_long_le(f); o+=4;
      *(glcmds_copy++) = glcmd;	// copy the vertex index
    }
  }
  /* do not forget to copy the zero :-) */
  *(glcmds_copy++) = glcmd;
  return o+4;
}

void Md2::updBbox(float x, float y, float z)
{
  if (x < bbox.xmin) bbox.xmin = x;
  else if ( x > bbox.xmax ) bbox.xmax = x;
  if (y < bbox.ymin) bbox.ymin = y;
  else if (y > bbox.ymax) bbox.ymax = y;
  if (z < bbox.zmin) bbox.zmin = z;
  else if (z > bbox.zmax) bbox.zmax = z;
}

float Md2::getScale()
{
  float radius = static_cast<float>(fabs(bbox.xmin));

  if (fabs(bbox.xmax) > radius) radius = bbox.xmax;
  if (fabs(bbox.ymin) > radius) radius = bbox.ymin;
  if (fabs(bbox.ymax) > radius) radius = bbox.ymax;
  if (fabs(bbox.zmin) > radius) radius = bbox.zmin;
  if (fabs(bbox.zmax) > radius) radius = bbox.zmax;
  return sqrt(radius);
}

void Md2::setScale(float scale)
{
  desiredScale = scale;
}

void Md2::draw(int32_t frame, int32_t nextframe, float ratio, float scale)
{
  /* keeps the frame value valid */
  if (numframes == 0) {
    error("drawMd2: numframes null");
    return;
  }

  frame %= numframes;
  nextframe %= numframes;
  //echo("draw: frame=%d", frame);

  /* get the frames information */
  tFrame *frame1 = frames + frame;
  tFrame *frame2 = frames + nextframe;

  /* do the gl commands */
  int32_t *glcmd = glcmds;
  int32_t nb_vert = 0;

  while (*glcmd != 0) {
    int32_t num_verts;

    /* determine the command to draw the triangles */
    if (*glcmd > 0) {	// triangle strip
      num_verts = *glcmd;
      glBegin(GL_TRIANGLE_STRIP);
    }
    else { 		// triangle fan
      num_verts = -(*glcmd);
      glBegin(GL_TRIANGLE_FAN);
    }
    glcmd++;

    for (int i=0; i < num_verts; i++) {
      tVec3 p;  // Interpolated point
      int32_t vert_index;

      /* Grab the vertex index */
      vert_index = *glcmd;
      glcmd++;

      /* Interpolate */
      p.x = frame1->vert_table[vert_index].x +
	(frame2->vert_table[vert_index].x -
	 frame1->vert_table[vert_index].x) * ratio;
      p.y = frame1->vert_table[vert_index].y +
	(frame2->vert_table[vert_index].y -
	 frame1->vert_table[vert_index].y) * ratio;
      p.z = frame1->vert_table[vert_index].z +
	(frame2->vert_table[vert_index].z -
	 frame1->vert_table[vert_index].z) * ratio;

      glTexCoord2f(texinfo[nb_vert].s, texinfo[nb_vert].t);

      /* apply scale */
      Draw::vertex3f(p.x * scale, p.y * scale, p.z * scale);
      updBbox(p.x * scale, p.y * scale, p.z * scale);
      nb_vert++;
    }
    glEnd();
  }
}

int32_t Md2::displaylist(int frame, int texture)
{
  return displaylist(frame, frame, 0, desiredScale, texture);
}

int32_t Md2::displaylist(int frame, int nextframe, float inter, float scale, int texture)
{
  if (! loaded) return -1;

  dlists[frame] = glGenLists(1);

  glNewList(dlists[frame], GL_COMPILE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_TEXTURE_2D);
  if (texture > 0) glBindTexture(GL_TEXTURE_2D, texture);
  draw(frame, nextframe, inter, scale);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  glEndList();
  return dlists[frame];
}

void Md2::render(Pos &pos)
{
  render(pos, 1);	// first frame
}

void Md2::render(Pos &pos, uint16_t frame)
{
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z + pos.bbs.v[2]);
  glRotatef(RAD2DEG(pos.ax), 1,0,0);
  glRotatef(RAD2DEG(pos.ay), 0,1,0);
  glRotatef(RAD2DEG(pos.az), 0,0,1);
  glScalef(desiredScale, desiredScale, desiredScale);

  glCallList(dlists[frame]);

  glPopMatrix();
}
