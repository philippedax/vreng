//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// book.cpp
//
// Book handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "book.hpp"
#include "sheet.hpp"	// class Sheet
#include "matvec.hpp"   // V3
#include "browser.hpp"	// start
#include "user.hpp"	// User
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// open, close


const OClass Book::oclass(BOOK_TYPE, "Book", Book::creator);

const float Book::ARIGHT = 0;
const float Book::ALEFT = 3.14;
const float Book::ASPEED = 1.0;
const float Book::LSPEED = 1.0;

// local
static uint16_t oid = 0;


/* Downloads urls: filling of the two arrays */
void Book::reader(void *_book, Http *http)
{
  Book *book = (Book *) _book;
  if (! book) return;

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't open %s", http->url);
    return;
  }

  char line[256];

  // get the number of pages (first line)
  if (! fgets(line, sizeof(line), f)) {
    error("can't get number of sheets");
    return;
  }
  line[strlen(line) - 1] = '\0';
  book->nb = atoi(line) / 2;
  if (book->nb <= 0) {
    error("number of pages is negative");
    return;
  }

  book->tex = new char*[2*book->nb+3]; // +3 for edge
  book->html = new char*[2*book->nb];

  int i;
  char *p = NULL;

  for (i=0 ; i < 2*book->nb ; i++) {
    if (! fgets(line, sizeof(line), f))
      return;  // eof
    line[strlen(line) - 1] = '\0';
    if (! (p = strtok(line, " \t"))) {
      error("book reader: bad line %s", line);
      return;
    }
    char* sheet_tex = strdup(p);
    book->tex[i] = strdup(p);
    // copy url of the html page matching this texture
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
  if (! fgets(line, sizeof(line), f))
    strcpy(line, DEF_URL_PAPER);
  line[strlen(line) - 1] = '\0';
  if (! (p = strtok(line, " \t")))
    strcpy(p, DEF_URL_PAPER);

  book->tex[i] = strdup(p);
  char *tranche = strdup(p);
  i++;
  if ((p = strtok(NULL, " \t")))
    book->tex[i] = strdup(p);
  else
    book->tex[i] = strdup(tranche);
  i++;
  if ((p = strtok(NULL, " \t")))
    book->tex[i] = strdup(p);
  else
    book->tex[i] = strdup(tranche);
  free(tranche);

  cache->close();
  delete cache;
}

/** Creation from a file. */
WO * Book::creator(char *l)
{
  return new Book(l);
}

/** Constructor. */
Book::Book(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/** Parses. */
void Book::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url="))    l = parseUrl(l, url);
  }
  end_while_parse(l);
}

void Book::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(TAKABLE);
}

/** Do specific initializations */
void Book::inits()
{
  initMobileObject(1);
  createNetObj(PROPS, ++oid);

  getDim(size);
  width = size.v[0];
  height = size.v[2];

  right = NULL;
  left = NULL;
  temp = NULL;
  nb = 0;
  current = 0;
  aright = -M_PI_2;
  aleft = M_PI_2;
  thick = 0.001;	// 1mm
  fwd = 0;
  bwd = 0;
  vspeed = LSPEED;
  state = 0;

  if (! *url) return;
  if (Http::httpOpen(url, reader, this, 0) < 0) return;
  if (! nb) return;	// no sheets

  char s[256];
  float depth, depth2;

  // create the heaps (right and left) of sheets

  if (current == 0) {	// book closed on the right side - default case
    state = CLOSED_R;

    // create the right heap
    depth = thick * nb;	// epaisseur du tas droit
    setPos(s, pos.x-depth/2*sin(aright), pos.y+depth/2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    //setCov(s, tex[0], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[0]);
    createSheet(s, RIGHT);

    // create the left heap
    setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
    setDim(s, width, thick, height);
    //setCov(s, tex[0], tex[1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[0]);
    createSheet(s, LEFT);
  }
  else if (current == nb) { // book closed on the left side
    state = CLOSED_L;

    // create the right heap
    setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
    setDim(s, width, thick, height);
    //setCov(s, tex[2*nb-2], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[2*nb-2]);
    createSheet(s, RIGHT);

    // create the left heap
    depth = thick * nb; // epaisseur du tas gauche
    setPos(s, pos.x+depth/2*sin(aleft), pos.y-depth/2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    //setCov(s, tex[0], tex[2*nb-2], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[0]);
    createSheet(s, LEFT);
  }
  else { // book opened
    state = OPENED;

    // create the right heap
    depth = thick * (nb - current); // right heap thick
    depth2 = depth + thick;	// augmented
    setPos(s, pos.x-depth2*sin(aright), pos.y+depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    //setCov(s, tex[2*current], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[2*current]);
    createSheet(s, RIGHT);

    // create the left heap
    depth = thick * current;	// epaisseur du tas gauche
    setPos(s, pos.x+(depth/2)*sin(aleft), pos.y-(depth/2)*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    //setCov(s, tex[0], tex[2*current-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    setCov(s, tex[0]);
    createSheet(s, LEFT);

    // create the temp sheet
    setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
    setDim(s, width, thick, height);
    setCov(s, tex[2*current], tex[2*current+1]);
    createSheet(s, TEMP);
  }
}

/** Creates a sheet */
void Book::createSheet(char *s, uint8_t heap)
{
  doAction(SHEET_TYPE, Sheet::CREATE, this, s, heap, heap);
}

void Book::setPos(char *s, float x, float y, float z, float az, float ax)
{
  sprintf(s, "pos=\"%.3f %.3f %.3f %.3f %.3f\"", x, y, z, az, ax);
}

void Book::setDim(char *s, float x, float y, float z)
{
  sprintf(s, "%s solid dim=\"%.3f %.3f %.3f\"", strdup(s), x, y, z);
}

void Book::setCov(char *s, char *tx)
{
  sprintf(s, "%s tx=\"%s\" />", strdup(s), tx);
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

/** next page */
void Book::nextPage(Book *book, void *d, time_t sec, time_t usec)
{
  if (state == CLOSED_L) return;

  char s[256];
  float depth, depth2;

  if (current == nb-1) {	// cas 1: it remains only one sheet on the right
    // increase the left heap minor edge size
    depth = thick * nb; // new left heap thick
    //depth2 = depth + 2.5*thick; // augmented
    depth2 = depth + thick; // augmented
    setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[0], tex[2*nb-3], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(left);
    createSheet(s, LEFT);
    delPage(temp);	// destruction de la temp sheet

    // rotation de la partie droite
    turnNext(right, d, sec, usec);
    state = CLOSED_L;
  }
  else if (current == 0) {	// cas 2: none sheet one the left, temp is NULL
    // on cree la temp mais on fait tourner la partie gauche (resumee a 1 feuille)
    // create temp sheet first
    setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
    setDim(s, width, thick, height);
    setTex(s, tex[2], tex[3]);
    createSheet(s, TEMP);

    // replace the right heap
    depth = thick * (nb-1); // right heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[2], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]); //  p 2 + couv arrière
    delPage(right);
    createSheet(s, RIGHT);

    // replace the left heap if too big (case of bookClose)
    if (left) {
      left->getDim(size);
    }
    else {
      return;
    }
    if (size.v[1] > 0.7 * thick) {
      setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[0], tex[1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]); // couverture avant + p 1
      delPage(left);
      createSheet(s, LEFT);
    }

    // turn the heap (1 sheet) left
    turnNext(left, d, sec, usec);
    state = OPENED;
  }
  else { // general case
    // replace the left heap
    depth = thick * (current+1); // new left heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[0], tex[2*current-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(left);
    createSheet(s, LEFT);

    // replacement of temp sheet if needed
    if (! temp) {
      return;
    }
    if (temp->state == Sheet::LEFT) {
      setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[2*current], tex[2*current+1]);
      delPage(temp);
      createSheet(s, TEMP);
    }

    // replace the right heap
    depth = thick * (nb-(current+1));//right heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[2*(current+1)], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(right);
    createSheet(s, RIGHT);

    // rotation of temp sheet
    turnNext(temp, d, sec, usec);
  }
}

/** previous page */
void Book::prevPage(Book *book, void *d, time_t sec, time_t usec)
{
  if (state == CLOSED_R) return;

  char s[256];
  float depth, depth2;

  if (current == 1) {	// cas 1: it remains only one sheet on the left
    // increase the right heap minor edge size
    depth = thick * nb; // right heap thick
    //depth2 = depth + 2.5*thick; // augmented
    depth2 = depth + thick; // augmented
    setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[2], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(right);
    createSheet(s, RIGHT);
    delPage(temp);	// destroy temp

    // rotation left sheet
    turnPrev(left, d, sec, usec);
    state = CLOSED_R;
  }
  else if (current == nb) {	// cas 2: none sheet on the right
    // cree la temp mais on fait tourner la partie droite: 1 feuille
    // create the temp sheet
    setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
    setDim(s, width, thick, height);
    setTex(s, tex[2*nb-4], tex[2*nb-3]); // p 2N-4 + p 2N-3
    createSheet(s, TEMP);

    // replace the left heap
    depth = thick * (nb-1); // new left heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[0], tex[2*nb-3], tex[2*nb], tex[2*nb+2], tex[2*nb+1]); // p 0 + p 2N-3
    delPage(left);
    createSheet(s, LEFT);

    // replace the right heap if not too big
    if (right) {
      right->getDim(size);
    }
    else {
      return;
    }
    if (size.v[1] > 0.7 * thick) {
      setPos(s, pos.x, pos.y, pos.z, aright, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[2*nb-2], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
      delPage(right);
      createSheet(s, RIGHT);
    }

    // turn the heap of 1 sheet on the right
    turnPrev(right, d, sec, usec);
    state = OPENED;
  }
  else { // general case
    // replace the right heap
    depth = thick * (nb-current+1); // right heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x - depth2*sin(aright), pos.y + depth2*cos(aright), pos.z, aright, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[2*current], tex[2*nb-1], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(right);
    createSheet(s, RIGHT);

    // replacement of temp sheet if needed
    if (! temp) {
      return;
    }
    if (temp->state == Sheet::RIGHT) {
      setPos(s, pos.x, pos.y, pos.z, aleft, pos.ax);
      setDim(s, width, thick, height);
      setTex(s, tex[2*(current-1)],tex[2*current-1]);
      delPage(temp);
      createSheet(s, TEMP);
    }

    // replace the left heap
    depth = thick  * (current -1); // new left heap thick
    depth2 = depth + thick; // augmented
    setPos(s, pos.x + depth2*sin(aleft), pos.y - depth2*cos(aleft), pos.z, aleft, pos.ax);
    setDim(s, width, depth, height);
    setTex(s, tex[0], tex[2*current-3], tex[2*nb], tex[2*nb+2], tex[2*nb+1]);
    delPage(left);
    createSheet(s, LEFT);

    // rotate temp sheet
    turnPrev(temp, d, sec, usec);
  }
}

/** open a book */
void Book::open(Book *book, void *d, time_t s, time_t u)
{
  if (state == OPENED) return;
  if (state == CLOSED_R) nextPage(book, d, s, u);
  if (state == CLOSED_L) prevPage(book, d, s, u);
}

/** close a book */
void Book::close(Book *book, void *d, time_t s, time_t u)
{
   if ( (state == CLOSED_R) || (state == CLOSED_L) ) return;

   // destruction de la temp
   delPage(temp);
   // we close always the book on its right side
   if (left) {
     pullPrev(left);
   }
   if (right) {
     pushPrev(right, thick * current);
   }
   current = 0;
   state = CLOSED_R;
}

/** turn forward 10 pages in one time */
void Book::forward(void *d, time_t sec, time_t usec)
{
  vspeed = 5 * LSPEED;
  if (temp) temp->aspeed = vspeed * ASPEED;

  nextPage(this, d, sec, usec);

  vspeed = LSPEED;
  if (temp) temp->aspeed = vspeed * ASPEED;
}

/** turn backward 10 pages in one time */
void Book::backward(void *d, time_t sec, time_t usec)
{
  vspeed = 5 * LSPEED;
  if (temp) temp->aspeed = vspeed * ASPEED;

  prevPage(this, d, sec, usec);

  vspeed = LSPEED;
  if (temp) temp->aspeed = vspeed * ASPEED;
}

/** forward: 10 pages en avant */
void Book::forwardPage(Book *book, void *d, time_t sec, time_t usec)
{
  if (state != OPENED) return;

  if (current + 10 > nb) {
    fwd = nb - current - 1;
  }
  else {
    fwd = 9;
  }
  forward(d, sec, usec);
}

/** backward: 10 pages en arriere */
void Book::backwardPage(Book *book, void *d, time_t sec, time_t usec)
{
  if (state != OPENED) return;

  if (current - 10 < 0) {
    bwd = current - 1;
  }
  else {
    bwd = 9;
  }
  backward(d, sec, usec);
}

void Book::turnPrev(Sheet *sheet, void *d, time_t sec, time_t usec)
{
  if (sheet) sheet->turnPrev();
  current--;
}

void Book::turnNext(Sheet *sheet, void *d, time_t sec, time_t usec)
{
  if (sheet) sheet->turnNext();
  current++;
}

void Book::pullPrev(Sheet *sheet)
{
  if (sheet) sheet->pullPrev();
}

void Book::pullNext(Sheet *sheet)
{
  if (sheet) sheet->pullNext();
}

/** pousser un tas droit pendant que le tas gauche tourne */
void Book::pushPrev(Sheet* sheet, float dist)
{
  if (sheet) sheet->pushPrev(dist);
}

/** pousser un tas gauche pendant que le tas droit tourne */
void Book::pushNext(Sheet* sheet, float dist)
{
  if (sheet) sheet->pushNext(dist);
}

void Book::delPage(Sheet* sheet)
{
  if (sheet) {
    sheet->toDelete();
    sheet = NULL;
  }
}

/** approach the book near the avatar */
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
    // move the temp sheet
    if (temp) {
      temp->move.lspeed.v[0] = dx / ttl;
      temp->move.lspeed.v[1] = dy / ttl;
      temp->move.lspeed.v[2] = vz;
      temp->move.aspeed.v[0] = vaz;
      temp->move.aspeed.v[1] = vay;
      temp->move.aspeed.v[2] = vax;
      temp->aright = azend;
      temp->aleft += daz;
      temp->initImposedMovement(ttl);
    }
    if (left)  left->initImposedMovement(ttl);
    if (right) right->initImposedMovement(ttl);
    initImposedMovement(ttl);
  }
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

void Book::nextPage_cb(Book *book, void *d, time_t s, time_t u)
{
  book->nextPage(book, d, s, u);
}

void Book::prevPage_cb(Book *book, void *d, time_t s, time_t u)
{
  book->prevPage(book, d, s, u);
}

void Book::forwardPage_cb(Book *book, void *d, time_t s, time_t u)
{
  book->forwardPage(book, d, s, u);
}

void Book::backwardPage_cb(Book *book, void *d, time_t s, time_t u)
{
  book->backwardPage(book, d, s, u);
}

/** display the right page in a browser */
void Book::lookRight_cb(Book *book, void *d, time_t s, time_t u)
{
  if (book->current < book->nb) Browser::start(book->html[2*book->current]);
}

/** display the left page in a browser */
void Book::lookLeft_cb(Book *book, void *d, time_t s, time_t u)
{
  if (book->current > 0) Browser::start(book->html[2*book->current-1]);
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

  setActionFunc(BOOK_TYPE, OPEN, _Action open_cb, "Open");
  setActionFunc(BOOK_TYPE, CLOSE, _Action close_cb, "Close");
  setActionFunc(BOOK_TYPE, NEXT, _Action nextPage_cb, " > ");
  setActionFunc(BOOK_TYPE, PREV, _Action prevPage_cb, " < ");
  setActionFunc(BOOK_TYPE, FWD, _Action forwardPage_cb, " >> ");
  setActionFunc(BOOK_TYPE, REW, _Action backwardPage_cb, " << ");
  setActionFunc(BOOK_TYPE, PULL, _Action approach_cb, "Approach");
  setActionFunc(BOOK_TYPE, LOOKL, _Action lookLeft_cb, "LookLeft");
  setActionFunc(BOOK_TYPE, LOOKR, _Action lookRight_cb, "LookRight");
}
