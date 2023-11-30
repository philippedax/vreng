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
#include "book.hpp"
#include "matvec.hpp"   // V3 M4
#include "sheet.hpp"	// class Sheet
#include "move.hpp"	// gotoFront
#include "browser.hpp"	// start
#include "user.hpp"	// User
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open, close


const OClass Book::oclass(BOOK_TYPE, "Book", Book::creator);

const float Book::ASPEED = 1.0;
const float Book::ARIGHT = 0;
const float Book::ALEFT = 3.14;
const float Book::LSPEED = 1.0;

// local
static uint16_t oid = 0;


/* Downloads urls: filling of the two arrays */
void Book::reader(void *_book, Http *http)
{
  Book *book = (Book *) _book;
  if (! book) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(book->getUrl(), http);
  if (! f) {
    error("can't open %s", book->getUrl());
    return;
  }

  char line[BUFSIZ];

  // get the number of pages (first line)
  if (! fgets(line, sizeof(line), f)) {
    error("can't get number of pages");
    return;
  }
  line[strlen(line) - 1] = '\0';
  book->nbs = atoi(line) / 2;
  if (book->nbs <= 0) {
    error("number of pages is negative");
    return;
  }

  book->tex = new char*[2*book->nbs+3]; // +3 for edge
  book->html = new char*[2*book->nbs];

  int i;
  char *p = NULL;

  for (i=0 ; i < 2*book->nbs ; i++) {
    if (! fgets(line, sizeof(line), f)) return;  // eof
    line[strlen(line) - 1] = '\0';
    if ((p = strtok(line, " \t")) == NULL) {
      error("reader: bad line %s", line);
      return;
    }
    // copy url of the html page matching this texture
    char* sheet_tex = strdup(p);
    book->tex[i] = strdup(p);
    if ((p = strtok(NULL, " \t"))) {
      char* sheet_html = strdup(p);
      book->html[i] = strdup(p);
      free(sheet_html);
    }
    else {
      char* sheet_html = strdup(sheet_tex);
      book->html[i] = strdup(sheet_tex);
      free(sheet_html);
    }
    free(sheet_tex);
  }

  // it remains 3 urls: textures for edge
  if (! fgets(line, sizeof(line), f)) strcpy(line, DEF_URL_PAPER);
  line[strlen(line) - 1] = '\0';
  if ((p = strtok(line, " \t")) == NULL) strcpy(p, DEF_URL_PAPER);

  book->tex[i] = strdup(p);
  char *tranche = strdup(p);
  i++;
  if ((p = strtok(NULL, " \t"))) book->tex[i] = strdup(p);
  else                           book->tex[i] = strdup(tranche);
  i++;
  if ((p = strtok(NULL, " \t"))) book->tex[i] = strdup(p);
  else                           book->tex[i] = strdup(tranche);
  free(tranche);

  cache->close();
  delete cache;
}

const char * Book::getUrl() const
{
  return (const char *) url;
}

void Book::setPos(char *s, float x, float y, float z, float az, float ax)
{
  sprintf(s, "pos=\"%.3f %.3f %.3f %.3f %.3f\"", x, y, z, az, ax);
}

void Book::setDim(char *s, float x, float y, float z)
{
  sprintf(s, "%s solid dim=\"%.3f %.3f %.3f\"", strdup(s), x, y, z);
}

void Book::setCov(char *s, char *yn, char *yp, char *xp, char *xn, char *zp)
{
  sprintf(s, "%s yn=\"%s\" yp=\"%s\" xp=\"%s\" xn=\"%s\" zp=\"%s\" />", strdup(s), yn, yp, xp, xn, zp);
}

void Book::setCov(char *s, char *yn, char *yp)
{
  sprintf(s, "%s yn=\"%s\" yp=\"%s\" />", strdup(s), yn, yp);
}

void Book::setTex(char *s, char *yn, char *xn, char *yp, char *xp, char *zp)
{
  sprintf(s, "%s yn=\"%s\" xn=\"%s\" yp=\"%s\" xp=\"%s\" zp=\"%s\" />", strdup(s), yn, xn, yp, xp, zp);
}

void Book::setTex(char *s, char *yn, char *xn)
{
  sprintf(s, "%s yn=\"%s\" xn=\"%s\" />", strdup(s), yn, xn);
}

void Book::createSheet(char *s, uint8_t t, uint8_t side)
{
  doAction(SHEET_TYPE, Sheet::CREATE, this, s, t, side);
}

/* creation from a file */
WO * Book::creator(char *l)
{
  return new Book(l);
}

void Book::defaults()
{
  nbs = 0;
  num = 0;
  right = NULL;
  left = NULL;
  inter = NULL;
  aright = -M_PI_2;
  aleft = M_PI_2;
  fwd = 0;
  bwd = 0;
  thick = 0.001;	// 1mm
  vspeed = LSPEED;
  state = 0;
  near = false;
}

void Book::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (!stringcmp(l, "url="))           l = parseUrl(l, url);
    else if (!stringcmp(l, "aright"))         l = parseFloat(l, &aright, "aright");
    else if (!stringcmp(l, "aleft"))          l = parseFloat(l, &aleft, "aleft");
    else if (!stringcmp(l, "nbsheets"))       l = parseUInt8(l, &nbs, "nbsheets");
    else if (!stringcmp(l, "sheetthickness")) l = parseFloat(l, &thick, "sheetthickness");
    else if (!stringcmp(l, "currentsheet"))   l = parseUInt8(l, &num, "currentsheet");
  }
  end_while_parse(l);
}

void Book::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(TAKABLE);
}

void Book::inits()
{
  initMobileObject(1);
  createNetObj(PROPS, ++oid);

  getDim(size);
  width = size.v[0];
  height = size.v[2];

  if (! *url) return;
  if (Http::httpOpen(url, reader, this, 0) < 0) return;
  if (! nbs) return;	// no sheets

  // create the heaps (right and left) of sheets

  char s[BUFSIZ];
  if (num == 0) { // book closed on the right side
    state = CLOSED_R;
    // create the right heap
    float depth = thick * nbs; // epaisseur du tas droit
    setPos(s, pos.x-depth/2*sin(aright), pos.y+depth/2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setCov(s, tex[0], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
    createSheet(s, Sheet::RIGHT, RIGHT);
    // create the left heap (thick: 1 sheet)
    setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
    setDim(s, width, thick, height);
    setCov(s, tex[0], tex[1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
    createSheet(s, Sheet::RIGHT, LEFT);
  }
  else { // book closes on the left side
    if (num == nbs) {
      state = CLOSED_L;
      // create the right heap (thick: 1 sheet)
      setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
      setDim(s, width, thick, height);
      setCov(s, tex[2*nbs-2],tex[2*nbs-1],tex[2*nbs],tex[2*nbs+2],tex[2*nbs+1]);
      createSheet(s, Sheet::LEFT, RIGHT);
      // create the left heap
      float depth = thick * nbs; // epaisseur du tas gauche
      setPos(s, pos.x+depth/2*sin(aleft), pos.y-depth/2*cos(aleft), pos.z, aleft, pos.ax);
      setDim(s, width, depth, height);
      setCov(s, tex[0], tex[2*nbs-2], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      createSheet(s, Sheet::LEFT, LEFT);
    }
    else { // book opened
      state = OPENED;
      // create the right heap
      float depth = thick * (nbs - num); // right heap thick
      float depth2 = depth + thick; // augmented
      setPos(s, pos.x-depth2*sin(aright), pos.y+depth2*cos(aright), pos.z, aright, pos.ax);
      setDim(s, width, depth, height);
      setCov(s, tex[2*num],tex[2*nbs-1],tex[2*nbs],tex[2*nbs+2],tex[2*nbs+1]);
      createSheet(s, Sheet::RIGHT, RIGHT);
      // create the left heap
      depth = thick * num; // demi-epaisseur du tas gauche
      setPos(s,pos.x+(depth/2)*sin(aleft), pos.y-(depth/2)*cos(aleft), pos.z, aleft, pos.ax);
      setDim(s, width, depth, height);
      setCov(s, tex[0], tex[2*num-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      createSheet(s, Sheet::LEFT, LEFT);
      // create the inter sheet
      setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
      setDim(s, width, thick, height);
      setCov(s, tex[2*num], tex[2*num+1]);
      createSheet(s, Sheet::RIGHT, VOLATILE);
    }
  }
}

Book::Book(char *l)
{
  parser(l);
  behaviors();
  inits();
}

void Book::changePosition(float dt)
{
  pos.x  += dt * move.lspeed.v[0];
  pos.y  += dt * move.lspeed.v[1];
  pos.z  += dt * move.lspeed.v[2];
  pos.az += dt * move.aspeed.v[0];
  pos.ay += dt * move.aspeed.v[1];
  pos.ax += dt * move.aspeed.v[2];
}

void Book::updateTime(time_t sec, time_t usec, float *dt)
{
  updateLasting(sec, usec, dt);
}

bool Book::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Book::whenIntersect(WO *pcur, WO *pold)
{
  if (state == OPENED) projectPosition(pcur, pold);
  return true;
}

void Book::quit()
{
  if (tex) delete[] tex;
  if (html) delete[] html;
  oid = 0;
}

/* next page */
void Book::nextSheet(Book *book, void *d, time_t sec, time_t usec)
{
  if (state == CLOSED_L) return;
  char s[BUFSIZ];

  if (num == nbs-1) {	// cas 1: it remains only one sheet on the right
    // increase the left heap minor edge size
    float depth = thick * nbs; // new left heap thick
    float depth2 = depth + 2.5*thick; // augmented
    setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[0], tex[2*nbs-3], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
    cancelSheet(left);
    createSheet(s, Sheet::LEFT, LEFT);
    cancelSheet(inter);	// destruction de la inter sheet
    inter = NULL;
    // rotation de la partie droite
    turnNext(right, d, sec, usec);
    state = CLOSED_L;
  }
  else {
    if (num == 0) {	// cas 2: none sheet one the left, inter is NULL
      // on cree la inter mais on fait tourner la partie gauche (resumee a 1 feuille)
      // create inter sheet first
      setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[2], tex[3]);
      createSheet(s, Sheet::LEFT, VOLATILE);
      // replace the right heap
      float depth = thick * (nbs-1); // right heap thick
      float depth2 = depth + thick; // augmented
      setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[2], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]); //  p 2 + couverture arrière
      cancelSheet(right);
      createSheet(s, Sheet::RIGHT, RIGHT);
      // replace the left heap if too big (case of bookClose)
      if (left) {
        left->getDim(size);
      }
      else {
        echo("no left sheet");
        return;
      }
      if (size.v[1] > 0.7 * thick) {
	setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
	setDim(s, width, thick, height);
	setTex(s, tex[0], tex[1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]); // couverture avant + p 1
	cancelSheet(left);
        createSheet(s, Sheet::RIGHT, LEFT);
      }
      // turn the heap (1 sheet) left
      turnNext(left, d, sec, usec);
      state = OPENED;
    }
    else { // general case
      // replace the left heap
      float depth = thick * (num+1); // new left heap thick
      float depth2 = depth + thick; // augmented
      setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[0], tex[2*num-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      cancelSheet(left);
      createSheet(s, Sheet::LEFT, LEFT);
      // replacement of inter sheet if needed
      if (! inter) {
        echo("no inter sheet");
        return;
      }
      if (inter->state == Sheet::LEFT) {
	setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
	setDim(s, width, thick, height);
	setTex(s, tex[2*num], tex[2*num+1]);
	cancelSheet(inter);
        createSheet(s, Sheet::RIGHT, VOLATILE);
      }
      // replace the right heap
      depth = thick * (nbs-(num+1));//right heap thick
      depth2 = depth + thick; // augmented
      setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[2*(num+1)], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      cancelSheet(right);
      createSheet(s, Sheet::RIGHT, RIGHT);
      // rotation of inter sheet
      turnNext(inter, d, sec, usec);
    }
  }
}

/* previous page */
void Book::prevSheet(Book *book, void *d, time_t sec, time_t usec)
{
  if (state == CLOSED_R) return;
  char s[BUFSIZ];

  if (num == 1) {	// cas 1: it remains only one sheet on the left
    // increase the right heap minor edge size
    float depth = thick * nbs; // right heap thick
    float depth2 = depth + 2.5*thick; // augmented
    setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[2], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
    cancelSheet(right);
    createSheet(s, Sheet::RIGHT, RIGHT);
    cancelSheet(inter);	// destroy inter sheet
    inter = NULL;
    // rotation left sheet
    turnPrev(left, d, sec, usec);
    state = CLOSED_R;
  }
  else {
    if (num == nbs) {	// cas 2: none sheet on the right
      // cree la inter mais on fait tourner la partie droite: 1 feuille
      // create the inter sheet
      setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[2*nbs-4], tex[2*nbs-3]); // p 2N-4 + p 2N-3
      createSheet(s, Sheet::RIGHT, VOLATILE);
      // replace the left heap
      float depth = thick * (nbs-1); // new left heap thick
      float depth2 = depth + thick; // augmented
      setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[0], tex[2*nbs-3], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]); // p 0 + p 2N-3
      cancelSheet(left);
      createSheet(s, Sheet::LEFT, LEFT);
      // replace the right heap if not too big
      if (right) {
        right->getDim(size);
      }
      else {
        echo("no right sheet");
        return;
      }
      if (size.v[1] > 0.7 * thick) {
	setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
	setDim(s, width, thick, height);
	setTex(s, tex[2*nbs-2], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
	cancelSheet(right);
        createSheet(s, Sheet::LEFT, RIGHT);
      }
      // turn the heap of 1 sheet on the right
      turnPrev(right, d, sec, usec);
      state = OPENED;
    }
    else { // general case
      // replace the right heap
      float depth = thick * (nbs-num+1); // right heap thick
      float depth2 = depth + thick; // augmented
      setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[2*num], tex[2*nbs-1], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      cancelSheet(right);
      createSheet(s, Sheet::RIGHT, RIGHT);
      // replacement of inter sheet if needed
      if (! inter) {
        echo("no inter sheet");
        return;
      }
      if (inter->state == Sheet::RIGHT) {
	setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
	setDim(s, width, thick, height);
	setTex(s, tex[2*(num-1)],tex[2*num-1]);
	cancelSheet(inter);
        createSheet(s, Sheet::LEFT, VOLATILE);
      }
      // replace the left heap
      depth = thick  * (num -1); // new left heap thick
      depth2 = depth + thick; // augmented
      setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
      setDim(s, width, depth, height);
      setTex(s, tex[0], tex[2*num-3], tex[2*nbs], tex[2*nbs+2], tex[2*nbs+1]);
      cancelSheet(left);
      createSheet(s, Sheet::LEFT, LEFT);
      // rotate inter sheet
      turnPrev(inter, d, sec, usec);
    }
  }
}

/* open a book */
void Book::open(Book *book, void *d, time_t s, time_t u)
{
  if (state == OPENED) return;

  vspeed = LSPEED;
  if (state == CLOSED_R) nextSheet(book, d, s, u);
  if (state == CLOSED_L) prevSheet(book, d, s, u);
}

/* close a book */
void Book::close(Book *book, void *d, time_t s, time_t u)
{
   if ( (state == CLOSED_R) || (state == CLOSED_L) ) return;

   // destruction de la inter
   cancelSheet(inter);
   inter = NULL;
   // we close always the book on its right side
   if (left) {
     pullPrev(left);
   }
   if (right) {
     pushPrev(right, thick * num);
   }
   num = 0;
   state = CLOSED_R;
}

/* turn forward 10 pages in one time */
void Book::forward(void *d, time_t sec, time_t usec)
{
  vspeed = 5 * LSPEED;
  if (inter) inter->aspeed = vspeed * ASPEED;

  nextSheet(this, d, sec, usec);

  vspeed = LSPEED;
  if (inter) inter->aspeed = vspeed * ASPEED;
}

/* turn backward 10 pages in one time */
void Book::backward(void *d, time_t sec, time_t usec)
{
  vspeed = 5 * LSPEED;
  if (inter) inter->aspeed = vspeed * ASPEED;

  prevSheet(this, d, sec, usec);

  vspeed = LSPEED;
  if (inter) inter->aspeed = vspeed * ASPEED;
}

/* forward: 10 pages en avant */
void Book::forwardSheet(Book *book, void *d, time_t sec, time_t usec)
{
  if (state != OPENED) return;

  if (num + 10 > nbs) {
    fwd = nbs - num - 1;
    forward(d, sec, usec);
  }
  else {
    fwd = 9;
    forward(d, sec, usec);
  }
}

/* backward: 10 pages en arriere */
void Book::backwardSheet(Book *book, void *d, time_t sec, time_t usec)
{
  if (state != OPENED) return;

  if (num - 10 < 0) {
    bwd = num - 1;
    backward(d, sec, usec);
  }
  else {
    bwd = 9;
    backward(d, sec, usec);
  }
}

void Book::turnPrev(Sheet *sheet, void *d, time_t sec, time_t usec)
{
  if (sheet) sheet->turnPrev();
  num--;
}

void Book::turnNext(Sheet *sheet, void *d, time_t sec, time_t usec)
{
  if (sheet) sheet->turnNext();
  num++;
}

void Book::pullPrev(Sheet *sheet)
{
  if (sheet) sheet->pullPrev();
}

void Book::pullNext(Sheet *sheet)
{
  if (sheet) sheet->pullNext();
}

/* pousser un tas droit pendant que le tas gauche tourne */
void Book::pushPrev(Sheet* sheet, float dist)
{
  if (sheet) sheet->pushPrev(dist);
}

/* pousser un tas gauche pendant que le tas droit tourne */
void Book::pushNext(Sheet* sheet, float dist)
{
  if (sheet) sheet->pushNext(dist);
}

void Book::cancelSheet(Sheet* sheet)
{
  if (sheet) {
    sheet->toDelete();
  }
}

/* approach the book near the avatar */
void Book::approach()
{
  float dx = .7 * (localuser->pos.x - pos.x);	// not too near
  float dy = .7 * (localuser->pos.y - pos.y);	// not too near
  float dz = localuser->pos.z + localuser->height/2 -.10 - pos.z;
  float aright0 = aright;
  float aleft0 = aleft;
  float azend = localuser->pos.az - M_PI_2; // note: azend = "aright_fin"
  float axend = localuser->pos.ax;
  float ayend = localuser->pos.ay;
  float daz = deltaAngle(azend, aright0);
  float dax = deltaAngle(axend, pos.ax);
  float day = deltaAngle(ayend, pos.ay);

  float ttl = ABSF(dx / (LSPEED * 2));
  if (ttl == 0) ttl = ABSF(dy / (LSPEED * 2));

  if (ttl > 0.001) {
    float vz = dz / ttl;
    float vaz = daz / ttl;
    float vax = dax / ttl;
    float vay = day / ttl;

    // move the book
    move.lspeed.v[0] = dx / ttl;
    move.lspeed.v[1] = dy / ttl;
    move.lspeed.v[2] = vz;
    move.aspeed.v[0] = vaz;
    move.aspeed.v[1] = vay;
    move.aspeed.v[2] = vax;
    aright = azend;
    aleft += daz;

    // move the right sheet
    if (right) {
      right->getDim(size);
      float dxright = dx + size.v[1] * (sin(aright0) - sin(azend));
      float dyright = dy + size.v[1] * (cos(azend) - cos(aright0));
      right->move.lspeed.v[0] = dxright / ttl;
      right->move.lspeed.v[1] = dyright / ttl;
      right->move.lspeed.v[2] = vz;
      right->move.aspeed.v[0] = vaz;
      right->move.aspeed.v[1] = vay;
      right->move.aspeed.v[2] = vax;
      right->aright = azend;
      right->aleft += daz;
    }
    // move the left sheet
    if (left) {
      left->getDim(size);
      float dxleft = dx + size.v[1] * (sin(aleft0+daz) - sin(aleft0));
      float dyleft = dy + size.v[1] * (cos(aleft0) - cos(aleft0+daz));
      left->move.lspeed.v[0] = dxleft / ttl;
      left->move.lspeed.v[1] = dyleft / ttl;
      left->move.lspeed.v[2] = vz;
      left->move.aspeed.v[0] = vaz;
      left->move.aspeed.v[1] = vay;
      left->move.aspeed.v[2] = vax;
      left->aright = azend;
      left->aleft += daz;
    }
    // move the inter sheet
    if (inter) {
      inter->move.lspeed.v[0] = dx / ttl;
      inter->move.lspeed.v[1] = dy / ttl;
      inter->move.lspeed.v[2] = vz;
      inter->move.aspeed.v[0] = vaz;
      inter->move.aspeed.v[1] = vay;
      inter->move.aspeed.v[2] = vax;
      inter->aright = azend;
      inter->aleft += daz;
      inter->initImposedMovement(ttl);
    }
    if (left)  left->initImposedMovement(ttl);
    if (right) right->initImposedMovement(ttl);
    initImposedMovement(ttl);
  }
  near = true;
}

void Book::open_cb(Book *book, void *d, time_t s, time_t u)
{
  book->open(book, d, s, u);
}

void Book::close_cb(Book *book, void *d, time_t s, time_t u)
{
  book->close(book, d, s, u);
}

void Book::approach_cb(Book *book, void *d, time_t s, time_t u)
{
  book->approach();
}

void Book::nextSheet_cb(Book *book, void *d, time_t s, time_t u)
{
  book->nextSheet(book, d, s, u);
}

void Book::prevSheet_cb(Book *book, void *d, time_t s, time_t u)
{
  book->prevSheet(book, d, s, u);
}

void Book::forwardSheet_cb(Book *book, void *d, time_t s, time_t u)
{
  book->forwardSheet(book, d, s, u);
}

void Book::backwardSheet_cb(Book *book, void *d, time_t s, time_t u)
{
  book->backwardSheet(book, d, s, u);
}

/* display the right page in a browser */
void Book::lookRight_cb(Book *book, void *d, time_t s, time_t u)
{
  if (book->num < book->nbs) Browser::start(book->html[2*book->num]);
}

/* display the left page in a browser */
void Book::lookLeft_cb(Book *book, void *d, time_t s, time_t u)
{
  if (book->num > 0) Browser::start(book->html[2*book->num-1]);
}

void Book::funcs()
{
  getPropFunc(BOOK_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(BOOK_TYPE, PROPZ, _Payload get_z);
  getPropFunc(BOOK_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(BOOK_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(BOOK_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(BOOK_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(BOOK_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(BOOK_TYPE, PROPZ, _Payload put_z);
  putPropFunc(BOOK_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(BOOK_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(BOOK_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(BOOK_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(BOOK_TYPE, PULL, _Action approach_cb, "Approach");
  setActionFunc(BOOK_TYPE, OPEN, _Action open_cb, "Open");
  setActionFunc(BOOK_TYPE, CLOSE, _Action close_cb, "Close");
  setActionFunc(BOOK_TYPE, NEXT, _Action nextSheet_cb, ">");
  setActionFunc(BOOK_TYPE, PREV, _Action prevSheet_cb, "<");
  setActionFunc(BOOK_TYPE, FWD, _Action forwardSheet_cb, ">>");
  setActionFunc(BOOK_TYPE, REW, _Action backwardSheet_cb, "<<");
  setActionFunc(BOOK_TYPE, LOOKL, _Action lookLeft_cb, "LookLeft");
  setActionFunc(BOOK_TYPE, LOOKR, _Action lookRight_cb, "LookRight");
  setActionFunc(BOOK_TYPE, MOVE, _Action moveObject, "Move");
}
