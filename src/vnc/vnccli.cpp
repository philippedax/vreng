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
#include "vnccli.hpp"
#include <X11/Xlib.h>	// XStringToKeysym


/*
 * Constructors
 */
VNCrgb::VNCrgb()
{
  red = 0;
  green = 0;
  blue = 0;
}

VNCrgb::VNCrgb(const uint32_t &pixel)
{
  red = (uint8_t) (pixel);
  green = (uint8_t) (pixel >> 8);
  blue = (uint8_t) (pixel >> 16);
}

VNCrgb::VNCrgb(uint8_t r, uint8_t g, uint8_t b)
{
  red = r;
  green = g;
  blue = b;
}

VNCCli::VNCCli(char *server, int port, char *pswdfile) : rfbproto(server,port,pswdfile)
{
  fbWidth = 0;
  fbHeight = 0;
  framebuffer = NULL;
  serverCutText = NULL;
  newCutText = false;
}

VNCCliTextured::VNCCliTextured(char *server, int port, char *pswdfile) : VNCCli(server,port,pswdfile)
{
  realScreenWidth = 0;
  realScreenHeight = 0;
}

bool VNCCli::initVNC()
{
  if (rfbproto.connectRFB() && rfbproto.initRFB()) {

    rfbproto.setVisual();

    if (! rfbproto.setFormatAndEncodings()) {
      error("initVNC: unable to set default PixelFormat");
      closeVNC();
      return false;
    }
    fbWidth = rfbproto.si.framebufferWidth;
    fbHeight = rfbproto.si.framebufferHeight;
    //error("initVNC: fbWidth=%d fbHeight=%d", fbWidth,fbHeight);
    framebuffer = new VNCrgb[fbWidth * fbHeight];
    if (! framebuffer) {
      error("initVNC: unable to allocate memory for framebuffer");
      closeVNC();
      return false;
    }
    return true;
  }
  else {
    error("initVNC: connection or initialization impossible");
    closeVNC();
  }
  return false;
}

bool VNCCliTextured::initVNC()
{
  if (rfbproto.connectRFB() && rfbproto.initRFB()) {

    rfbproto.setVisual();

    if (!rfbproto.setFormatAndEncodings()) {
      error("initVNC: unable to set default PixelFormat");
      closeVNC();
      return false;
    }
    realScreenWidth = rfbproto.si.framebufferWidth;
    realScreenHeight = rfbproto.si.framebufferHeight;
    trace(DBG_VNC, "initVNC: realscreen w=%d h=%d", realScreenWidth, realScreenHeight);

    uint16_t powerof2;
    for (powerof2 = 1; powerof2 < realScreenWidth; powerof2 *= 2) ;
    fbWidth = powerof2;
    for (powerof2 = 1; powerof2 < realScreenHeight; powerof2 *= 2) ;
    fbHeight = powerof2;
    trace(DBG_VNC, "initVNC: fbw=%d fbh=%d", fbWidth, fbHeight);

    framebuffer = new VNCrgb[fbWidth * fbHeight];

    if (! framebuffer) {
      error("initVNC: unable to allocate memory for framebuffer");
      closeVNC();
      return false;
    }
    return true;
  }
  else {
    error("initVNC: connection or initialization impossible");
    closeVNC();
  }
  return false;
}

bool VNCCli::closeVNC()
{
  close(getSock());
  if (framebuffer) delete[] framebuffer;
  framebuffer = NULL;
  return true;
}

/*
 * sendRFBEvent is an action which sends an RFB event.  It can be used in two
 * ways.  Without any parameters it simply sends an RFB event corresponding to
 * the X event which caused it to be called.  With parameters, it generates a
 * "fake" RFB event based on those parameters.  The first parameter is the
 * event type, either "ptr", "keydown", "keyup" or "key" (down&up).  For a
 * "key" event the second parameter is simply a keysym string as understood by
 * XStringToKeysym().  For a "ptr" event, the following three parameters are
 * just X, Y and the button mask (0 for all up, 1 for button1 down, 2 for
 * button2 down, 3 for both, etc).
 */
void VNCCli::sendRFBEvent(char **params, unsigned int *num_params)
{
  KeySym ks;
  int buttonMask, x, y;

  if (! framebuffer) return;

  if (strncasecmp(params[0], "key", 3) == 0) {
    if (*num_params != 2) {
      error("invalid params: sendRFBEvent(key|keydown|keyup, <keysym>)");
      return;
    }
    ks = XStringToKeysym(params[1]);
    //error("ks = %02x",ks);
    //if (ks == 0) ks = '\r';
    if (ks == NoSymbol) {
      error("Invalid keysym '%s' passed to sendRFBEvent", params[1]);
      return;
    }
    if (strcasecmp(params[0], "keydown") == 0) {
      //error("keydown ks=%2x p1=%2x", ks, params[1][0]);
      rfbproto.sendKeyEvent(ks, 1);
    }
    else if (strcasecmp(params[0], "keyup") == 0) {
      //error("keyup");
      rfbproto.sendKeyEvent(ks, 0);
    }
    else if (strcasecmp(params[0], "key") == 0) {
      //error("key");
      rfbproto.sendKeyEvent(ks, 1);
      rfbproto.sendKeyEvent(ks, 0);
    }
    else {
      error("invalid event '%s' passed to sendRFBEvent", params[0]);
      return;
    }
  }
  else if (strcasecmp(params[0], "ptr") == 0) {
    if (*num_params == 4) {
      x = atoi(params[1]);
      y = atoi(params[2]);
      buttonMask = atoi(params[3]);
      rfbproto.sendPointerEvent(x, y, buttonMask);
    }
    else {
      error("invalid params: sendRFBEvent(ptr, x, y, buttonMask)");
      return;
    }
  }
  else {
    error("invalid event '%s' passed to sendRFBEvent", params[0]);
  }
}

uint8_t VNCCli::rescalePixValue(uint32_t pix, uint8_t shift, uint16_t max)
{
  return (uint8_t) (((pix >> shift)&max) * (256 / (max+1))) ;
}

// can be used with a uint8_t, uint16_t or uint32_t pix
VNCrgb VNCCli::cardToVNCrgb(uint32_t pix)
{
  rfbPixelFormat pf = rfbproto.pixFormat;

  return VNCrgb(rescalePixValue(pix, pf.redShift, pf.redMax), rescalePixValue(pix, pf.greenShift, pf.greenMax), rescalePixValue(pix, pf.blueShift, pf.blueMax));
}

// All the methods we need to handle a given rect message
// and update the framebuffer

bool VNCCli::handleRAW32(int rx, int ry, int rw, int rh)
{
  uint32_t *rfb = (uint32_t *) rfbbuffer;
  VNCrgb *fb = (framebuffer + ry * fbWidth + rx);
  //error("handleRAW: %dx%d", rw,rh);

  for (int h = 0; h < rh; h++) {
    for (int w = 0; w < rw; w++) {
      *fb++ = cardToVNCrgb(swap32(*rfb));	//BUG! segfault on rfbbuffer if not enough large
      rfb++;
    }
    fb += fbWidth - rw;
  }
  return true;
}

bool VNCCli::handleCR(int srcx, int srcy, int rx, int ry, int rw, int rh)
{
  VNCrgb *src;
  VNCrgb *dst;
  VNCrgb *buftmp = new VNCrgb[rh*rw];

  trace(DBG_VNC, "handleCR: rx=%d ry=%d rw=%d rh=%d", rx, ry, rw, rh);
  src = framebuffer + (srcy * fbWidth + srcx);
  dst = buftmp;
  for (int h = 0; h < rh; h++) {
    memcpy(dst, src, rw*sizeof(VNCrgb));
    src += fbWidth;
    dst += rw;
  }
  src = buftmp;
  dst = framebuffer + (ry * fbWidth + rx);
  for (int h = 0; h < rh; h++) {
    memcpy(dst, src, rw*sizeof(VNCrgb));
    src += rw;
    dst += fbWidth;
  }
  delete[] buftmp;
  return true;
}

void VNCCli::fillRect(int rx, int ry, int rw, int rh, VNCrgb pixel)
{
  VNCrgb *dst = framebuffer + (ry * fbWidth + rx);

  error("fillRect: %dx%d", rw,rh);
  for (int h = 0; h < rh; h++) {
    for (int w = 0; w < rw; w++) {
      *dst++ = pixel;
    }
    dst += fbWidth - rw;
  }
}

bool VNCCli::handleRRE32(int rx, int ry, int rw, int rh)
{
  rfbRREHeader hdr;
  uint32_t pix;
  rfbRectangle subrect;
  VNCrgb pixel;

  trace(DBG_VNC, "handleRRE32: rx=%d ry=%d rw=%d rh=%d", rx, ry, rw, rh);
  if (! rfbproto.vncsock.readRFB((char *)&hdr, sz_rfbRREHeader))
    return false;
  hdr.nSubrects = swap32(hdr.nSubrects);

  if (! rfbproto.vncsock.readRFB((char *)&pix, sizeof(pix)))
    return false;
  pixel = cardToVNCrgb(swap32(pix));
  fillRect(rx, ry, rw, rh, pixel);

  for (unsigned int i=0; i < hdr.nSubrects; i++) {
    if (! rfbproto.vncsock.readRFB((char *)&pix, sizeof(pix)))
      return false;
    if (! rfbproto.vncsock.readRFB((char*)&subrect, sz_rfbRectangle))
      return false;

    subrect.x = swap16(subrect.x);
    subrect.y = swap16(subrect.y);
    subrect.w = swap16(subrect.w);
    subrect.h = swap16(subrect.h);
    pixel = cardToVNCrgb(swap32(pix));
    fillRect(subrect.x, subrect.y, subrect.w, subrect.h, pixel);
  }
  return true;
}

bool VNCCli::handleCoRRE32(int rx, int ry, int rw, int rh)
{
  rfbRREHeader hdr;
  uint32_t pix;
  uint8_t *ptr;
  int x, y, ww, hh;
  VNCrgb pixel;

  trace(DBG_VNC, "handleCoRRE32: rx=%d ry=%d rw=%d rh=%d", rx, ry, rw, rh);
  if (! rfbproto.vncsock.readRFB((char *)&hdr, sz_rfbRREHeader))
    return false;
  hdr.nSubrects = swap32(hdr.nSubrects);

  if (! rfbproto.vncsock.readRFB((char *)&pix, sizeof(pix)))
    return false;

  pixel = cardToVNCrgb(swap32(pix));
  fillRect(rx, ry, rw, rh, pixel);

  if (! rfbproto.vncsock.readRFB(rfbbuffer, hdr.nSubrects * 8))
    return false;
  ptr = (uint8_t *) rfbbuffer;

  for (unsigned int i=0; i < hdr.nSubrects; i++) {
    pix = *(uint32_t *)ptr;
    ptr += 4;
    x = *ptr++;
    y = *ptr++;
    ww = *ptr++;
    hh = *ptr++;

    pixel = cardToVNCrgb(swap32(pix));
    fillRect(rx+x, ry+y, ww, hh, pixel);
  }
  return true;
}

#define GET_PIXEL32(pix, ptr) (((uint8_t*)&(pix))[0] = *(ptr)++, \
			       ((uint8_t*)&(pix))[1] = *(ptr)++, \
			       ((uint8_t*)&(pix))[2] = *(ptr)++, \
			       ((uint8_t*)&(pix))[3] = *(ptr)++)

bool VNCCli::handleHextile32(int rx, int ry, int rw, int rh)
{
  uint32_t bg, fg;
  int i;
  uint8_t *ptr;
  int x, y, w, h;
  int sx, sy, sw, sh;
  uint8_t subencoding;
  uint8_t nSubrects;
  VNCrgb pixel;

  trace(DBG_VNC, "handleHextile32: rx=%d ry=%d rw=%d rh=%d",rx,ry,rw,rh);
  for (y = ry; y < ry+rh; y += 16) {
    for (x = rx; x < rx+rw; x += 16) {
      w = h = 16;
      if (rx+rw - x < 16)
	w = rx+rw - x;
      if (ry+rh - y < 16)
	h = ry+rh - y;

      if (! rfbproto.vncsock.readRFB((char *)&subencoding, 1))
	return false;
      if (subencoding & rfbHextileRaw) {
	if (! rfbproto.vncsock.readRFB(rfbbuffer, w * h * 4))
	  return false;
	handleRAW32(x, y, w, h);
	continue;
      }

      if (subencoding & rfbHextileBackgroundSpecified) {
	if (! rfbproto.vncsock.readRFB((char *)&bg, sizeof(bg)))
	  return false;
      }

      pixel = VNCrgb(swap32(bg));
      fillRect(x, y, w, h, pixel);

      if (subencoding & rfbHextileForegroundSpecified) {
	if (! rfbproto.vncsock.readRFB((char *)&fg, sizeof(fg)))
	  return false;
      }
      if (! (subencoding & rfbHextileAnySubrects))
	continue;
      if (! rfbproto.vncsock.readRFB((char *)&nSubrects, 1))
	return false;

      ptr = (uint8_t *) rfbbuffer;

      if (subencoding & rfbHextileSubrectsColoured) {
	if (! rfbproto.vncsock.readRFB(rfbbuffer, nSubrects * 6))
	  return false;

	for (i=0; i < nSubrects; i++) {
	  GET_PIXEL32(fg, ptr);
	  sx = rfbHextileExtractX(*ptr);
	  sy = rfbHextileExtractY(*ptr);
	  ptr++;
	  sw = rfbHextileExtractW(*ptr);
	  sh = rfbHextileExtractH(*ptr);
	  ptr++;
	  pixel = VNCrgb(swap32(fg));
	  fillRect(x+sx, y+sy, sw, sh, pixel);
	}
      }
      else {
	if (! rfbproto.vncsock.readRFB(rfbbuffer, nSubrects * 2))
	  return false;

	for (i=0; i < nSubrects; i++) {
	  sx = rfbHextileExtractX(*ptr);
	  sy = rfbHextileExtractY(*ptr);
	  ptr++;
	  sw = rfbHextileExtractW(*ptr);
	  sh = rfbHextileExtractH(*ptr);
	  ptr++;
	  pixel = VNCrgb(swap32(fg));
	  fillRect(x+sx, y+sy, sw, sh, pixel);
	}
      }
    }
  }
  return true;
}

bool VNCCli::handleRFBMessage()
{
  rfbToClientMsg msg;

  if (! framebuffer)
    return false;
  if (! rfbproto.vncsock.readRFB((char *) &msg, 1))
    return false;

  switch (msg.type) {

  case rfbSetColourMapEntries:
  {
    error("handleRFBMessage: rfbSetColourMapEntries not supported yet");
    uint16_t rgb[3];

    if (! rfbproto.vncsock.readRFB(((char *)&msg) + 1, sz_rfbSetColourMapEntriesMsg - 1))
      return false;
    msg.scme.firstColour = swap16(msg.scme.firstColour);
    msg.scme.nColours = swap16(msg.scme.nColours);

    for (int i=0; i < msg.scme.nColours; i++) {
      if (! rfbproto.vncsock.readRFB((char *)rgb, 6))
	return false;
    }
    break;
  }

  case rfbFramebufferUpdate:
  {
    trace(DBG_VNC, "handleRFBMessage: rfbFramebufferUpdate");
    rfbFramebufferUpdateRectHeader rect;
    int linestoread;
    int bytesPerLine;
    int i;

    if (! rfbproto.vncsock.readRFB(((char *)&msg.fu) + 1, sz_rfbFramebufferUpdateMsg - 1))
      return false;
    msg.fu.nRects = swap16(msg.fu.nRects);

    for (i=0; i < msg.fu.nRects; i++) {
      if (! rfbproto.vncsock.readRFB((char *)&rect, sz_rfbFramebufferUpdateRectHeader))
	return false;

      rect.r.x = swap16(rect.r.x);
      rect.r.y = swap16(rect.r.y);
      rect.r.w = swap16(rect.r.w);
      rect.r.h = swap16(rect.r.h);
      rect.encoding = swap32(rect.encoding);

      if ((rect.r.x + rect.r.w > rfbproto.si.framebufferWidth) ||
	  (rect.r.y + rect.r.h > rfbproto.si.framebufferHeight)) {
	  error("Rect too large: %dx%d at (%d, %d)", rect.r.w, rect.r.h, rect.r.x, rect.r.y);
	  return false;
      }
      if ((rect.r.h * rect.r.w) == 0) {
	error("Zero rfbproto.size rect - ignoring");
	continue;
      }

      switch (rect.encoding) {
      case rfbEncodingRaw:
	bytesPerLine = rect.r.w * rfbproto.pixFormat.bitsPerPixel / 8;
	linestoread = sizeof(rfbbuffer) / bytesPerLine;
        trace(DBG_VNC, "rfbEncodingRaw: bytesPerLine=%d linestoread=%d", bytesPerLine, linestoread);

	while (rect.r.h > 0) {
	  if (linestoread > rect.r.h) {
	    linestoread = rect.r.h;
          }
	  if (! rfbproto.vncsock.readRFB(rfbbuffer, bytesPerLine * linestoread)) {
	    return false;
          }
          trace(DBG_VNC, "rfbEncodingRaw: %d,%d w=%d h=%d", rect.r.x, rect.r.y, rect.r.w, rect.r.h);
          if (! handleRAW32(rect.r.x, rect.r.y, rect.r.w, rect.r.h)) {
	    return false;
          }
	  rect.r.h -= linestoread;
	  rect.r.y += linestoread;
          //rfbproto.vncsock.PrintInHex(rfbbuffer, 64);
	}
	break;
      case rfbEncodingCopyRect:
      {
	rfbCopyRect cr;
	if (! rfbproto.vncsock.readRFB((char *) &cr, sz_rfbCopyRect))
	  return false;
	cr.srcX = swap16(cr.srcX);
	cr.srcY = swap16(cr.srcY);
	if (! handleCR(cr.srcX, cr.srcY, rect.r.x, rect.r.y, rect.r.w, rect.r.h))
	  return false;
	break;
      }
      case rfbEncodingRRE:
	if (! handleRRE32(rect.r.x, rect.r.y, rect.r.w, rect.r.h))
	  return false;
	break;
      case rfbEncodingCoRRE:
	if (! handleCoRRE32(rect.r.x, rect.r.y, rect.r.w, rect.r.h))
	  return false;
	break;
      case rfbEncodingHextile:
        if (! handleHextile32(rect.r.x, rect.r.y, rect.r.w, rect.r.h))
	  return false;
	break;
      default:
	error("Unknown rect encoding %d", (int) rect.encoding);
	return false;
      }
    }
    if (! sendIncrementalFramebufferUpdateRequest())
      return false;
    break;
  }

  case rfbBell:
    //XBell(dpy,100);
    break;

  case rfbCutText:
    if (! rfbproto.vncsock.readRFB(((char *) &msg) + 1, sz_rfbCutTextMsg - 1))
      return false;
    msg.sct.length = swap32(msg.sct.length);

    if (serverCutText) delete[] serverCutText;
    serverCutText = NULL;
    serverCutText = new char[msg.sct.length+1];

    if (! rfbproto.vncsock.readRFB(serverCutText, msg.sct.length))
      return false;

    serverCutText[msg.sct.length] = 0;
    newCutText = true;
    break;

  default:
    error("Unknown message type %d from VNC server", msg.type);
    return false;
  }
  return true;
}

int VNCCli::getSock()
{
  return rfbproto.getSock();
}

bool VNCCli::sendIncrementalFramebufferUpdateRequest()
{
  return rfbproto.sendIncrementalFramebufferUpdateRequest();
}

bool VNCCli::sendFramebufferUpdateRequest(int x, int y, int w, int h, bool incremental)
{
  return rfbproto.sendFramebufferUpdateRequest(x, y, w, h, incremental);
}
