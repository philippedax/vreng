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
#include "hairs.hpp"
#include "timer.hpp"    // fTime, delta
#include "http.hpp"	// Http
#include "cache.hpp"	// open, close
#include "file.hpp"	// closeFile
#include "lwo.hpp"	// Lwo

static Hairs *hairs = NULL;


const OClass Hairs::oclass(HAIRS_TYPE, "Hairs", Hairs::creator);

void Hairs::funcs() {}

/* creation from a file */
WObject * Hairs::creator(char *l)
{
  return new Hairs(l);
}

void Hairs::defaults()
{
  scale = 1;
}

void Hairs::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (!stringcmp(l, "url=")) l = parse()->parseUrl(l, names.url);
    else if (!stringcmp(l, "scale=")) l = parse()->parseFloat(l, &scale, "scale");
  }
  end_while_parse(l);
}

Hairs::Hairs(char *l)
{
  hairs = this;
  affobj = true;
  objchanged = true; // rebuild list
  angle = 150.;

  hair = new Hair();
  parser(l);

  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(UNSELECTABLE);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);

  if (loader(names.url, scale)) {	// load model
    dlist = glGenLists(1);
  }
  else {
    error("hairs: %s not well loaded", names.url);
  }
}

bool Hairs::loader(const char *url, float scale)
{
#if 0 //dax use Lwo
  obj = new Lwo(url);
#else
  obj = new Object();
  obj->scal = scale;
#endif
  Http::httpOpen(url, httpReader, obj, 0);	// get model

  Surface *s_hair = obj->findSurface("hair");
  if (s_hair == NULL) {
    error("no surface hair");
    return false;
  }
  hair->init(obj, s_hair, scale);
  Surface *s_skin = obj->findSurface("hair");
  if (s_skin != NULL) *(Material*)s_hair = *(Material*)s_skin;
  if (s_skin) return true;
  return false;
}

void Hair::init(Object *_o, Surface *_s, float scale)
{
  o = _o;
  s = _s;
  m = *s;
  m.flag &= ~Material::Lighting;
  for (int i=0; i<NBHAIR ; i++) {
    int f = rand() % s->nbf;
    TriFace *fc = s->fc + f;
    float a = rnd(1.);
    float b = rnd(1.);
    if (a+b > 1) {
      a = 1 - a;
      b = 1 - b;
    }
    Vector3 posh = fc->v[0]->p + a*(fc->v[1]->p - fc->v[0]->p) + b*(fc->v[2]->p - fc->v[0]->p);
/*
    posh.x *= scale;
    posh.y *= scale;
    posh.z *= scale;
*/
    Vector3 dir = fc->v[0]->n + a*(fc->v[1]->n - fc->v[0]->n) + b*(fc->v[2]->n - fc->v[0]->n);
    //O dir.Scale(HAIRLENGTH / dir.len());
    dir.scale(0.1);
    //echo("init: f=%d a,b %.2f,%.2f sc=%.3f posh=%.2f %.2f %.2f",f,a,b,HAIRLENGTH/dir.len(),posh.x,posh.y,posh.z);
    ln[i].init(posh, dir);
  }

  Color4f col = m.diffuse;
  for (int i=0; i<HAIRNCOLOR ; i++)
    c[i] = col*((i+HAIRNCOLOR/2)*1./HAIRNCOLOR);
}

void Line::init(Vector3 p0, Vector3 dp)
{
  n = dp;
  n.normalize();
  p0 -= dp;
  for (int i=0; i<HAIRSIZE ; i++) {
    pt[i].p = p0;
    p0 += dp;
    dp.x += rnd(2.*1)-1;
    dp.y += rnd(2.*1)-1;
    dp.z += rnd(2.*1)-1;
    //D error("dp: %d %.2f %.2f %.2f", i,dp.x,dp.y,dp.z);
#if 0 //DAX
    dp.x *= 0.0028;
    dp.y *= 0.0028;
    dp.z *= 0.0028;
#endif
  }
}

void Node::put(int pdata)
{
  glVertex3f(p.x, p.y, p.z);
  //echo("p: %d %.2f %.2f %.2f", pdata, p.x,p.y,p.z);
}

void Line::draw(int pdata)
{
  Node *p = pt + 1;

#if 0 //DAX
  glBegin(GL_LINE_STRIP);
  for (int n = HAIRSIZE-1; n; n--) {
    p->put(pdata);
    p++;
  }
  glEnd();
#else
  int n = HAIRSIZE-1;
  glInterleavedArrays(GL_V3F, sizeof(Node), p);
  glDrawArrays(GL_LINE_STRIP, 0, n);
#endif
}

void Hair::draw()
{
  int pdata = m.put();
  Line *l = ln;

  for (int n=0; n < NBHAIR ; n++)  {
#if 1 //TEST
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c[((n-1)*HAIRNCOLOR)/NBHAIR].t);
#else
    glColor4fv(c[((n-1)*HAIRNCOLOR)/NBHAIR].t);
#endif
    l->draw(pdata);	// draw line
    l++;
  }
}

void Hair::animate(float dt, Vector3 depl)
{
  Vector3 v, v2;
  float l, f1, f2;
  Line *pl = ln;
  int i = NBHAIR;

  do {
    int p = HAIRSIZE - 2;
    Node *pp = pl->pt + 2;
    do {
      pp->p += depl;
      v = pp[-1].p - pp[0].p;
      l = v.length2();
      if (l > MAXL) {
        v *= (1 - MAXL2 / (float)sqrt(l));
        pp->p += v;
        pp->v *= 1 - FROT;
      }
      else {
        f1 = STIFFNESS * (1 - (HAIRLENGTH * HAIRLENGTH) / l);
        v2 = pp[-2].p - pp[0].p;
        l = v2.length2();
        f2 = 0.4 * STIFF2 * (1 - (LENGTH2 * LENGTH2) / l);
        pp->v = pp->v * (1 - FROT) + v*f1 + v2*f2; //+GForce;
      }
      pp->v.z += HAIRGRAVITY;
      ++pp;
    } while (--p);

    p = HAIRSIZE - 2;
    pp = pl->pt + 2;
    do {
      pp->p += pp->v;
      ++pp;
    } while (--p);
    pl++;
  } while (--i);
}

void Hairs::render()
{
  static uint32_t nf = 0;

  glPushMatrix();

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glCullFace(GL_BACK); // reject fliped faces
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  GLfloat light2[4] = {0,0,1,0};
  glEnable(GL_NORMALIZE);
  //DAX glTranslatef(0, 0, -OBJSIZE/2);
  //glPushMatrix();
  glLightfv(GL_LIGHT2, GL_POSITION, light2); // light position
  glEnable(GL_LIGHT2);
  //glPopMatrix();
  glRotatef(90, 1, 0, 0);
  glPushMatrix();
   glRotatef(angle, 0, 0, 1);
   //echo("%.2f %.2f %.2f", pos.x+posh.x, pos.y+posh.y, pos.z+posh.z);
   glTranslatef(pos.x+posh.x, pos.y+posh.y, pos.z+posh.z);
   if (affobj) {
     if (objchanged) {	// rebuild list
       glNewList(dlist, GL_COMPILE_AND_EXECUTE);
       obj->draw();
       glEndList();
       objchanged = false;
     }
     else glCallList(dlist);
   }
   hair->draw();
   glDisable(GL_LIGHT2);
  glPopMatrix();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPopMatrix();
  nf++;
}

void Hairs::changePermanent(float dt)
{
  static bool first = true;

  if (first) {
    first = false;
    pos0.x *= scale; pos0.y *= scale; pos0.z *= scale;
    pos1.x *= scale; pos1.y *= scale; pos1.z *= scale;
  }
  Vector3 p2 = pos0 + (pos1-pos0) * dt;

  pos0 = pos1;
  pos1.x += rnd(2*RANDX)-RANDX; pos1.y += rnd(2*RANDY)-RANDY; pos1.z += rnd(2*RANDZ)-RANDZ;

  hair->animate(dt, posh - p2); // animate the hairs
  posh = p2;

  angle += dt*20;
}

void Hairs::quit()
{
  if (dlist) { glDeleteLists(dlist, 1); dlist = 0; }
}

//-------------------------------------------------------------------
//	File:		Object.cpp
//	Created:	01/12/99 0:24:AM
//	Author:		Allard Jeremie
//	Comments:	Create and manage Objects.
//-------------------------------------------------------------------
Object::Object()
{
  nbf = nbp = nbs = 0;
  fc = NULL;
  pt = NULL;
  sf = NULL;
}

Object::~Object()
{
  nbf = 0; nbp = 0;
  if (fc) { delete[] fc; fc = NULL; }
  if (pt) { delete[] pt; pt = NULL; }
  if (pt) { delete[] pt; pt = NULL; }
}

void HVertex::put(int pdata)
{
  if (pdata & PUT_NORMAL) glNormal3f(n.x,n.y,n.z);
  if (pdata & PUT_COORD)  glVertex3f(p.x,p.y,p.z);
}

bool getSTRING(FILE *fp, char *s, int *l)
{ //lecture d'une chaine de caracteres
  for (;;) {
    fread(s,2,1,fp);
    if (l != NULL) *l -= 2;
    if (s[0] == 0 || s[1] == 0) break;
    s += 2;
  }
  return true;
}

void Hairs::httpReader(void *_lwo, Http *http)
{
  Object *lwo = (Object *) _lwo;

  if (! lwo) return;

  int32_t len = 0, l = 0;
  int16_t w = 0, nw = 0;
  int i = 0, j = 0, sp = 0;
  float f = 0;
  union {
    uint32_t id;
    char sid[5];
  };
  sid[4] = 0;
  Surface *ss = NULL;
  bool littleendian;
  char tmp; //SWAPL
  char str[BUFSIZ];

  Cache *cache = new Cache();
  FILE *fp = cache->open(hairs->urlName(), http);

  /* check for headers */
  littleendian = File::littleEndian();
  trace(DBG_MOD,"%s", littleendian ? "LittleEndian" : "BigEndian");
  fread(&id, 4, 1, fp);
  if (stringcmp(sid, "FORM")) { error("not a ID_FORM"); return; }
  fread(&len, 4, 1, fp);
  SWAPL(&len, tmp);
  fread(&id, 4, 1, fp); len -= 4;
  if (stringcmp(sid, "LWOB")) { error("not a LWOB file"); return; }

  while (len > 0) {
    fread(&id, 4, 1, fp);
    //echo("%d %s", len, sid);
    len -= 4;
    fread(&l, 4, 1, fp); len -= 4;
    SWAPL(&l, tmp);
    if (l > len) { trace(DBG_MOD, "bad size: end after EOF (id=%s),l=%d len=%d",sid,l,len); /*return;*/ }
    //echo("chunk: %x(%s) size=%d",id,sid,l);
    len -= l;

    if (! stringcmp(sid, "SRFS")) { // SRFS
      if (lwo->nbs) { echo("SRFS"); return; }
      lwo->nbs++;
      sp = ftell(fp);
      while (l > 0) {
        getSTRING(fp, str, &l);
        trace(DBG_MOD, "str=%s", str);
        //if (l<0) { error("bad SRFS size"); return; }
      }
      fseek(fp,sp,0);
      if (lwo->nbs) {
        lwo->sf = new Surface[lwo->nbs];
        for (j=0; j<lwo->nbs; ++j) {
          getSTRING(fp, lwo->sf[j].name, NULL);
          //echo("Surface: %s", lwo->sf[j].name);
        }
      }
    }
    else if (! stringcmp(sid, "SURF")) { // SURF
      //if (! lwo->nbs) { error("no SURF"); return; }
      getSTRING(fp, str, &l);
      //echo("SURF=%s", str);
      if (l<0) {
        error("bad SURF size");
        return;
      }
      for (j=0; j<lwo->nbs ; ++j) {
        if (! strcasecmp(str, lwo->sf[j].name))
          break;
      }
      if (j == lwo->nbs) {
        error("unknown SURF:%s", str);
        return;
      }
      ss = &lwo->sf[j];
      ss->flag = 0;
      int ll;
      Color4f colr(.5,.5,.5);
      float lumi = 0;
      float diff = 1.;
      float spec = 0;
      int glos = 64;
      float refl = 0;
      float trans = 0;
      uint8_t byte;

      while (l > 0) {
        fread(&id,4,1,fp); l -= 4;
        fread(&ll,4,1,fp); l -= 4;
        SWAPL(&ll, tmp);
        if (ll > l) { error("bad chunk: after EOF (id=%s),ll=%d l=%d%x",sid,ll,l); /*return;*/ }
        l -= ll;
        //echo("sid=%s l=%d ll=%s", sid, l, ll);
        if (! stringcmp(sid, "FLAG")) { //FLAG
          if (ll != 2) { error("invalid FLAG size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); ll -= sizeof(w);
          if (w&4)   ss->flag |= Material::Smooth; //&=~Material::Flat;
          if (w&256) ss->flag |= Material::Double;
          if (w&512) ss->flag |= Material::Blend|Material::Additive;
        }
        else if (! stringcmp(sid, "COLR")) { //COLR
          if (ll != 4) { error("invalid COLR size"); return; }
          fread(&byte,1,1,fp);
          colr.r = byte*(1./255.);
          fread(&byte,1,1,fp);
          colr.g = byte*(1./255.);
          fread(&byte,1,1,fp);
          colr.b = byte*(1./255.);
          fread(&byte,1,1,fp);
          l -= 3;
        }
        else if (! stringcmp(sid, "LUMI")) { //LUMI
          if (ll != 2) { error("invalid LUMI size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          lumi = w*(1./255.);
        }
        else if (! stringcmp(sid, "DIFF")) { //DIFF
          if (ll != 2) { error("invalid DIFF size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          diff = w*(1./255.);
        }
        else if (! stringcmp(sid, "SPEC")) { //SPEC
          if (ll != 2) { error("invalid SPEC size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          diff = w*(1./255.);
          spec = w*(1./255.);
        }
        else if (! stringcmp(sid, "GLOS")) { //GLOS
          if (ll != 2) { error("invalid GLOS size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          glos = w;
        }
        else if (! stringcmp(sid, "REFL")) { //REFL
          if (ll != 2) { error("invalid REFL size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          refl = w*(1./255.);
        }
        else if (! stringcmp(sid, "TRAN")) { //TRAN
          if (ll != 2) { error("invalid TRAN size"); return; }
          fread(&w,2,1,fp); SWAPS(&w, tmp); l -= sizeof(w);
          trans = 0.5f; //w*(1./255.);
        }
        else {
          error("unkown SURF:%s",sid);
          fseek(fp, ll, 1);
        }
      }
      colr.a = 1. - trans;
      if (trans) ss->flag |= Material::Blend;
      ss->ambient.r = colr.r*lumi;
      ss->ambient.g = colr.g*lumi;
      ss->ambient.b = colr.b*lumi;
      ss->ambient.a = colr.a;
      ss->diffuse.r = colr.r*diff;
      ss->diffuse.g = colr.g*diff;
      ss->diffuse.b = colr.b*diff;
      ss->diffuse.a = colr.a;
      ss->specular.r = spec;
      ss->specular.g = spec;
      ss->specular.b = spec;
      ss->specular.a = colr.a;
      ss->emission.r = colr.r*refl;
      ss->emission.g = colr.g*refl;
      ss->emission.b = colr.b*refl;
      ss->emission.a = colr.a;
      ss->shininess = glos;
      if (diff || spec || refl) ss->flag |= Material::Lighting;
      else ss->flag &= ~Material::Smooth;
    }
    else if (! stringcmp(sid, "PNTS")) { // PNTS
      if (lwo->nbp) { echo("PNTS"); return; }
      if (l % (3*sizeof(float))) { error("bad PNTS size"); return; }
      lwo->nbp = l/(3*sizeof(float));
      if (lwo->nbp) {
        lwo->pt = new HVertex[lwo->nbp];
        for (j=0; j<lwo->nbp ; j++) {
          fread(&f,4,1,fp);
          SWAPL(&f, tmp);
          lwo->pt[j].p.x = f * lwo->scal;
          fread(&f,4,1,fp);
          SWAPL(&f, tmp);
          lwo->pt[j].p.y = f * lwo->scal;
          fread(&f,4,1,fp);
          SWAPL(&f, tmp);
          lwo->pt[j].p.z = f * lwo->scal;
        }
      }
    }
    else if (! stringcmp(sid, "POLS")) { // POLS
      if (lwo->nbf) { echo("POLS"); return; }
      sp = ftell(fp);
      j = 0; lwo->nbf = 0;
      while (j<l) {
        fread(&w,2,1,fp);
        SWAPS(&w, tmp);
        j += 2;
        fseek(fp, 2*w+2, 1);
        j += 2*w+2;
        if (w>=3)
          lwo->nbf += w-2;
      }
      if (j>l) { error("bad POLS %d!=%d",l,j); return; }
      //echo("nbf=%d",lwo->nbf);
      fseek(fp, sp, 0);
      if (lwo->nbf) {
        lwo->fc = new TriFace[lwo->nbf];
        for (j=0; j<lwo->nbf ; j++) {
          fread(&nw,2,1,fp);
          SWAPS(&nw, tmp);
          l -= 4+nw*2;
          if (nw <= 2) fseek(fp, 2*nw+2, 1);
          else {
            fread(&w,2,1,fp);
            SWAPS(&w, tmp);
            if (w >= lwo->nbp) { error("point inexistant:%x (face %d)",w,j); return; }
            lwo->fc[j].v[0] = &(lwo->pt[w]);
            fread(&w,2,1,fp);
            SWAPS(&w, tmp);
            if (w >= lwo->nbp) { error("point inexistant:%x (face %d)",w,j); return; }
            lwo->fc[j].v[1] = &(lwo->pt[w]);
            fread(&w,2,1,fp);
            SWAPS(&w, tmp);
            if (w >= lwo->nbp) { error("point inexistant:%x (face %d)",w,j); return; }
            lwo->fc[j].v[2] = &(lwo->pt[w]);
            i = j;
            while (nw > 3) {
              ++j; --nw;
              lwo->fc[j].v[0] = lwo->fc[i].v[0];
              lwo->fc[j].v[1] = lwo->fc[j-1].v[2];
              fread(&w,2,1,fp);
              SWAPS(&w, tmp);
              if (w >= lwo->nbp) { error("point inexistant:%x (face %d)",w,j); return; }
              lwo->fc[j].v[2] = &(lwo->pt[w]);
            }
            fread(&w,2,1,fp);	// lecture de la surface
            SWAPS(&w, tmp);
            if (w==0 || w>lwo->nbs) { error("surface inexistante:%x",w); return; }
            lwo->sf[w-1].nbf += 1+j-i;
            while (i<=j) lwo->fc[i++].m = &lwo->sf[w-1]; //psurf[w-1];
          }
        }
        if (j != lwo->nbf) { error("bad poly number:%d instead of %d",j,lwo->nbf); return; }
        if (lwo->nbs) {
          TriFace *fb = lwo->fc;
          lwo->fc = new TriFace[lwo->nbf];
          i = 0;
          for (j=0; j<lwo->nbs ; ++j) {
            lwo->sf[j].fc = lwo->fc+i;
            i += lwo->sf[j].nbf;
            lwo->sf[j].nbf = 0;
          }
          if (i != lwo->nbf) { error("faces material num err: %d!=%d",i,lwo->nbf); return; }

          Surface *s;
          for (j=0; j < lwo->nbf ;++j) {
            s = (Surface*) fb[j].m;
            s->fc[s->nbf++] = fb[j];
          }
          if (fb) delete[] fb;
        }
      }
      if (l) {
        fseek(fp, l, 1);
        l = 0;
      }
    }
    else //cout << "unknown "<<(char*)&id<<" chunk\n";
      fseek(fp, l, 1);
      //echo("end chunk ------------ sid=%s len=%d l=%d", sid, len, l);
  } //end while
  trace(DBG_MOD, "end obj file: nbs=%d nbf=%d nbp=%d pt=%d len=%d", lwo->nbs, lwo->nbf, lwo->nbp, lwo->pt, len);

  if (lwo->nbf) {
    HVertex *pv =  lwo->pt;
    for (j=0; j < lwo->nbp ; ++j,++pv)
      pv->n.reset();
    TriFace *pf = lwo->fc;
    for (j=0; j < lwo->nbf ; ++j,++pf) {
      pf->normal();
      pf->v[0]->n += pf->n;
      pf->v[1]->n += pf->n;
      pf->v[2]->n += pf->n;
    }
    pv = lwo->pt;
    for (j=0; j < lwo->nbp ; ++j,++pv)
      pv->n.normalize();
  }
  cache->close();
  delete cache;
  return;
}

void TriFace::put(int pdata)
{
  v[0]->put(pdata);
  v[1]->put(pdata);
  v[2]->put(pdata);
}

void TriFace::draw(int pdata)
{
  glBegin(GL_TRIANGLES);
   put(pdata);
  glEnd();
}

void Object::draw()
{
  if (nbs) for (int j=0; j<nbs ; ++j) sf[j].draw(pt);
}

int Material::put()
{
  int pdata = PUT_COORD;

  if (flag & Smooth) glEnable(GL_SMOOTH);
  else               glDisable(GL_SMOOTH);
  if (flag & Double) glDisable(GL_CULL_FACE);
  else               glEnable(GL_CULL_FACE);
  if (flag & Blend) {
    if (flag & Additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    else                 glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }
  else glDisable(GL_BLEND);
  if (flag & Lighting) {
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient.t);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse.t);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular.t);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission.t);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,(flag & Double));
    glEnable(GL_LIGHTING);
    pdata |= PUT_NORMAL;
  }
  else {
    glDisable(GL_LIGHTING);
    ambient.put();
  }
  return pdata;
}

void Color4f::put()
{
  glColor4f(r,g,b,a);
}

void Surface::draw(HVertex *p0)
{
  int pdata = put(); // configure material
  bool array = false;

  if (p0 != NULL) {
    switch (pdata) {
    case PUT_COORD:
      glInterleavedArrays(GL_V3F, sizeof(HVertex), ((HVertex *)p0)+offsetof(HVertex,p));
      array = true;
      break;
    case PUT_COORD|PUT_NORMAL:
      glInterleavedArrays(GL_N3F_V3F, sizeof(HVertex), ((uint8_t*)p0)+offsetof(HVertex,n));
      array = true;
      break;
    }
    int i;
    TriFace *f;
    glBegin(GL_TRIANGLES);
    f = fc; i = nbf;
    if (array) {
      while (i--) {
        glArrayElement(f->v[0]-p0);
        glArrayElement(f->v[1]-p0);
        glArrayElement(f->v[2]-p0);
        ++f;
      }
    }
    else {
      while (i--) {
        f->put(pdata);
        ++f;
      }
    }
    glEnd();
  }
}

void TriFace::normal()
{
  Vector3 a = v[1]->p - v[0]->p;
  Vector3 b = v[2]->p - v[0]->p;
  n.crossProduct(a, b);
  n.normalize();
}

Surface * Object::findSurface(const char *name)
{
  Surface *ps = sf;
  for (int s=0; s<nbs ; s++, ps++)
    if (! strcasecmp(ps->name, name)) return ps;
  return NULL;
}
