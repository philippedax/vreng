//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "animator.hpp"
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload


const OClass Animator::oclass(ANIMATOR_TYPE, "Animator", Animator::creator);

// local
static uint16_t oid = 0;


WObject * Animator::creator(char *l)
{
  return new Animator(l);
}

/* default member values */
void Animator::defaults()
{
  frame = 0;
  state = INACTIVE;
  ttl = 0;
}

/* parser */
void Animator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "state")) l = parse()->parseUInt16(l, (uint16_t*) &state, "state");
    else if (!stringcmp(l, "anim"))  l = parse()->parseUInt16(l, (uint16_t*) &state, "anim");
    else if (!stringcmp(l, "begin")) l = parse()->parseUInt8(l, &frame, "begin");
    else if (!stringcmp(l, "ttl"))   l = parse()->parseFloat(l, &ttl, "ttl");
  }
  end_while_parse(l);
}

/* constructor */
Animator::Animator(char *l)
{
  parser(l);

  enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_NEVER);
  setRenderPrior(RENDER_NORMAL);
  initializeMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);

  if (state == ACTIVE) {
    setRenderPrior(RENDER_HIGH);
    enableImposedMovement();
  }
}

bool Animator::isMoving()
{
  return (state != INACTIVE);
}

void Animator::updateTime(time_t sec, time_t usec, float *lasting)
{
  *lasting = diffTime(sec, usec);
  if (*lasting > ttl) {
    move.sec = sec;
    move.usec = usec;
  }
}

void Animator::changePosition(float lasting)
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
  animator->setRenderPrior(RENDER_NORMAL);
}

/* object intersects: stop */
bool Animator::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Animator::get_frame(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("d", &(pcur->frame));
  pcur->setFrame(pcur->frame);
  pcur->updatePositionAndGrid(oldpos);
}

void Animator::put_frame(Animator *po, Payload *pp)
{
  pp->putPayload("d", po->frame);
}

void Animator::get_nb_frames(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("d", &(pcur->nb_frames));
  pcur->updatePositionAndGrid(oldpos);
}

void Animator::put_nb_frames(Animator *po, Payload *pp)
{
  pp->putPayload("d", po->nb_frames);
}

void Animator::get_ttl(Animator *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("f", &(pcur->ttl));
  pcur->updatePositionAndGrid(oldpos);
}

void Animator::put_ttl(Animator *po, Payload *pp)
{
  pp->putPayload("f", po->ttl);
}

void Animator::quit()
{
  oid = 0;
  flushMySqlPosition();
}

void Animator::funcs()
{
  getPropertyFunc(ANIMATOR_TYPE, PROPFRAME, WO_PAYLOAD get_frame);
  getPropertyFunc(ANIMATOR_TYPE, PROPNBFRAMES, WO_PAYLOAD get_nb_frames);
  getPropertyFunc(ANIMATOR_TYPE, PROPTTL, WO_PAYLOAD get_ttl);

  putPropertyFunc(ANIMATOR_TYPE, PROPFRAME, WO_PAYLOAD put_frame);
  putPropertyFunc(ANIMATOR_TYPE, PROPNBFRAMES, WO_PAYLOAD put_nb_frames);
  putPropertyFunc(ANIMATOR_TYPE, PROPTTL, WO_PAYLOAD put_ttl);

  setActionFunc(ANIMATOR_TYPE, 0, WO_ACTION start, "Start");
  setActionFunc(ANIMATOR_TYPE, 1, WO_ACTION stop, "Stop");
  setActionFunc(ANIMATOR_TYPE, 2, WO_ACTION moveObject, "Move");
}
