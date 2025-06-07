//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// animator.cpp
//
// Animator handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "animator.hpp"
#include "netobj.hpp"	// NetObj
#include "payload.hpp"	// Payload


const OClass Animator::oclass(ANIMATOR_TYPE, "Animator", Animator::creator);

// local
static uint16_t oid = 0;


Object * Animator::creator(char *l)
{
  return new Animator(l);
}

/** default member values */
void Animator::defaults()
{
  frame = 0;
  state = INACTIVE;
  ttl = 0;
}

/** parser */
void Animator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (!stringcmp(l, "state")) l = parseUInt16(l, (uint16_t*) &state, "state");
    else if (!stringcmp(l, "anim"))  l = parseUInt16(l, (uint16_t*) &state, "anim");
    else if (!stringcmp(l, "begin")) l = parseUInt8(l, &frame, "begin");
    else if (!stringcmp(l, "ttl"))   l = parseFloat(l, &ttl, "ttl");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Animator::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
}

void Animator::inits()
{
  mobileObject(0);
  createNetObj(PROPS, ++oid);

  if (state == ACTIVE) {
    imposedMovement(MAXFLOAT);
  }
}

/** constructor */
Animator::Animator(char *l)
{
  parser(l);
  behaviors();
  inits();
}

bool Animator::isMoving()
{
  return (state != INACTIVE);
}

void Animator::timing(time_t sec, time_t usec, float *lasting)
{
  *lasting = deltaTime(sec, usec);
  if (*lasting > ttl) {
    move.sec = sec;
    move.usec = usec;
  }
}

void Animator::imposed(float lasting)
{
  if (lasting > ttl) {
    nb_frames = getFrames();	// get frames number from the solid
    frame %= nb_frames;
    frame = (frame + 1) % nb_frames;
    setFrame(frame);
  }
}

void Animator::start(Animator *animator, void *d, time_t s, time_t u)
{
  animator->move.sec = s;
  animator->move.usec = u;
  animator->state = Animator::ACTIVE;
}

void Animator::stop(Animator *animator, void *d, time_t s, time_t u)
{
  animator->state = Animator::INACTIVE;
}

/* object intersects: stop */
bool Animator::intersect(Object *pcur, Object *pold)
{
  pcur->pos = pold->pos;
  return true;
}

void Animator::get_frame(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("d", &(pcur->frame));
  pcur->setFrame(pcur->frame);
  pcur->updateGrid(oldpos);
}

void Animator::put_frame(Animator *po, Payload *pp)
{
  pp->putPayload("d", po->frame);
}

void Animator::get_nb_frames(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("d", &(pcur->nb_frames));
  pcur->updateGrid(oldpos);
}

void Animator::put_nb_frames(Animator *po, Payload *pp)
{
  pp->putPayload("d", po->nb_frames);
}

void Animator::get_ttl(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("f", &(pcur->ttl));
  pcur->updateGrid(oldpos);
}

void Animator::put_ttl(Animator *po, Payload *pp)
{
  pp->putPayload("f", po->ttl);
}

Animator::~Animator()
{
  oid = 0;
  savePersist();
}

void Animator::funcs()
{
  getPropFunc(ANIMATOR_TYPE, PROPFRAME, _Payload get_frame);
  getPropFunc(ANIMATOR_TYPE, PROPNBFRAMES, _Payload get_nb_frames);
  getPropFunc(ANIMATOR_TYPE, PROPTTL, _Payload get_ttl);

  putPropFunc(ANIMATOR_TYPE, PROPFRAME, _Payload put_frame);
  putPropFunc(ANIMATOR_TYPE, PROPNBFRAMES, _Payload put_nb_frames);
  putPropFunc(ANIMATOR_TYPE, PROPTTL, _Payload put_ttl);

  setActionFunc(ANIMATOR_TYPE, 0, _Action start, "Start");
  setActionFunc(ANIMATOR_TYPE, 1, _Action stop, "Stop");
}
