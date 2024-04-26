//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// vncrfb.cpp
//
// rfbproto.cpp - functions to deal with client side of RFB protocol.
//
// Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vncrfb.hpp"


int endianTest = 1;
char *serverCutText = NULL;
bool newCutText = false;

/* note that the CoRRE encoding uses this buffer and assumes it is big enough
   to hold 255 * 255 * 32 bits -> 260100 bytes.  640*480 = 307200 bytes */
/* also hextile assumes it is big enough to hold 16 * 16 * 32 bits */


/** Constructor */
VNCRFB::VNCRFB(char *_servername, int _port, char* _pswdFile)
 : vncsock(_servername, _port)
{
  shareDesktop = true;
  buffered = 0;
  strcpy(passwdFile, _pswdFile);
#if 0 //not used
  Encodings = NULL; // not used
#endif //not used
}

/** connectRFB server */
bool VNCRFB::connectRFB()
{
  int sock = vncsock.connectRFB();
  if (sock < 0) {
    error("connectRFB: unable to connect");
    return false;
  }
  return vncsock.setBlocking();
}

/** Inits RFB */
bool VNCRFB::initRFB()
{
  rfbProtocolVersionMsg pv;
  rfbClientInitMsg ci;
  int major, minor;
  char *reason, *passwd = NULL;
  int authScheme, reasonLen, authResult;
  uint8_t challenge[CHALLENGESIZE];

  if (! vncsock.readRFB(pv, sz_rfbProtocolVersionMsg)) {
    return false;
  }

  pv[sz_rfbProtocolVersionMsg] = 0;

  if (sscanf(pv, rfbProtocolVersionFormat, &major, &minor) != 2) {
    error("initRFB: not a valid RFB version");
    return false;
  }
  //echo("initRFB: VNC server supports protocol version %d.%d (viewer %d.%d)", major, minor, rfbProtocolMajorVersion, rfbProtocolMinorVersion);

  major = rfbProtocolMajorVersion;
  minor = rfbProtocolMinorVersion;
  sprintf(pv, rfbProtocolVersionFormat, major, minor);

  if (! vncsock.writeExact(pv, sz_rfbProtocolVersionMsg)) {
    return false;
  }
  if (! vncsock.readRFB(reinterpret_cast<char *>(&authScheme), 4)) {
    return false;
  }

  authScheme = (uint32_t) swap32(authScheme);
  switch (authScheme) {

  case rfbConnFailed:
    if (! vncsock.readRFB(reinterpret_cast<char *>(&reasonLen), 4)) {
      return false;
    }
    reasonLen = swap32(reasonLen);
    reason = new char[reasonLen];

    if (! vncsock.readRFB(reason, reasonLen)) {
      return false;
    }
    error("RFB connection failed: %s", reason);
    delete[] reason;
    return false;

  case rfbNoAuth:
    break;

  case rfbVncAuth:
    if (! vncsock.readRFB(reinterpret_cast<char *>(challenge), CHALLENGESIZE)) {
      return false;
    }

    // look for a password stored in passwdFile
    passwd = vncDecryptPasswdFromFile(passwdFile);
    if (! passwd || (! strlen(passwd))) {
      error("initRFB: cannot read valid password from file \"%s\"", passwdFile);
      return false;
    }
    if (strlen(passwd) > 8)
      passwd[8] = '\0';

    vncEncryptBytes(challenge, passwd);

    // lose the password from memory
    for (int i = strlen(passwd); i >= 0; i--) {
      passwd[i] = '\0';
    }

    if (! vncsock.writeExact(reinterpret_cast<char *>(challenge), CHALLENGESIZE)) {
      return false;
    }
    if (! vncsock.readRFB(reinterpret_cast<char *>(&authResult), 4)) {
      return false;
    }

    authResult = swap32(authResult);
    switch (authResult) {
    case rfbVncAuthOK:
      //echo("initRFB: VNC authentication succeeded");
      break;
    case rfbVncAuthFailed:
      error("initRFB: VNC authentication failed");
      return false;
    case rfbVncAuthTooMany:
      error("RFB authentication failed - too many tries");
      return false;
    default:
      error("unknown VNC authentication result: %d", authResult);
      return false;
    }
    break;

  default:
    error("unknown authentication scheme from VNC server: %d", authScheme);
    return false;
  }

  ci.shared = (shareDesktop ? 1 : 0);

  if (! vncsock.writeExact(reinterpret_cast<char *>(&ci), sz_rfbClientInitMsg)) {
    return false;
  }
  if (! vncsock.readRFB(reinterpret_cast<char *>(&si), sz_rfbServerInitMsg)) {
    return false;
  }

  si.framebufferWidth = swap16(si.framebufferWidth);
  si.framebufferHeight = swap16(si.framebufferHeight);
  si.format.redMax = swap16(si.format.redMax);
  si.format.greenMax = swap16(si.format.greenMax);
  si.format.blueMax = swap16(si.format.blueMax);
  si.nameLength = swap32(si.nameLength);

  desktopName = new char[si.nameLength + 1];
  if (! vncsock.readRFB(desktopName, si.nameLength)) {
    delete[] desktopName;
    desktopName = NULL;
    return false;
  }
  desktopName[si.nameLength] = 0;
  //echo("Desktop: %s", desktopName);
  delete[] desktopName;
  desktopName = NULL;

  //echo("initRFB: connected to VNC server, using protocol version %d.%d", rfbProtocolMajorVersion, rfbProtocolMinorVersion);
  printPixelFormat(&si.format);
  return true;
}

bool VNCRFB::setFormatAndEncodings()
{
  rfbSetPixelFormatMsg spf;
  char buf[sz_rfbSetEncodingsMsg + MAX_ENCODINGS * 4];

  rfbSetEncodingsMsg *se = (rfbSetEncodingsMsg *)buf;
  uint32_t *encs = (uint32_t *)(&buf[sz_rfbSetEncodingsMsg]);
  int len = 0;

  spf.type = rfbSetPixelFormat;
  spf.format = pixFormat;
  spf.format.redMax = swap16(spf.format.redMax);
  spf.format.greenMax = swap16(spf.format.greenMax);
  spf.format.blueMax = swap16(spf.format.blueMax);

  if (! vncsock.writeExact(reinterpret_cast<char *>(&spf), sz_rfbSetPixelFormatMsg)) {
    return false;
  }

  se->type = rfbSetEncodings;
  se->nEncodings = 0;

  if ( vncsock.sameMachine()) {
    //echo("same machine: preferring raw encoding");
    encs[se->nEncodings++] = swap32(rfbEncodingRaw);
  }
  encs[se->nEncodings++] = swap32(rfbEncodingCopyRect);
  encs[se->nEncodings++] = swap32(rfbEncodingHextile);
  encs[se->nEncodings++] = swap32(rfbEncodingCoRRE);
  encs[se->nEncodings++] = swap32(rfbEncodingRRE);

  len = sz_rfbSetEncodingsMsg + se->nEncodings * 4;
  se->nEncodings = swap16(se->nEncodings);

  if (! vncsock.writeExact(buf, len)) {
    return false;
  }
  return true;
}

bool VNCRFB::sendIncrementalFramebufferUpdateRequest()
{
  return sendFramebufferUpdateRequest(0, 0, si.framebufferWidth, si.framebufferHeight, true);
}

bool VNCRFB::sendFramebufferUpdateRequest(int x, int y, int w, int h, bool incremental)
{
  rfbFramebufferUpdateRequestMsg fur;

  fur.type = rfbFramebufferUpdateRequest;
  fur.incremental = incremental ? 1 : 0;
  fur.x = swap16(x);
  fur.y = swap16(y);
  fur.w = swap16(w);
  fur.h = swap16(h);

  if (! vncsock.writeExact(reinterpret_cast<char *>(&fur), sz_rfbFramebufferUpdateRequestMsg)) {
    return false;
  }
  return true;
}

bool VNCRFB::sendPointerEvent(int x, int y, int buttonMask)
{
  rfbPointerEventMsg pe;

  pe.type = rfbPointerEvent;
  pe.buttonMask = buttonMask;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  pe.x = swap16(x);
  pe.y = swap16(y);

  return vncsock.writeExact(reinterpret_cast<char *>(&pe), sz_rfbPointerEventMsg);
}

bool VNCRFB::sendKeyEvent(uint32_t key, bool down)
{
  rfbKeyEventMsg ke;

  ke.type = rfbKeyEvent;
  ke.down = down ? 1 : 0;
  ke.key = swap32(key);

  //echo("sendKey: %02x %d", ke.key, sz_rfbKeyEventMsg);
  return vncsock.writeExact(reinterpret_cast<char *>(&ke), sz_rfbKeyEventMsg);
}

bool VNCRFB::sendClientCutText(char *str, int len)
{
  rfbClientCutTextMsg cct;

  if (serverCutText) delete[] serverCutText;
  serverCutText = NULL;

  cct.type = rfbClientCutText;
  cct.length = swap32(len);

  return (vncsock.writeExact(reinterpret_cast<char *>(&cct), sz_rfbClientCutTextMsg) &&
	  vncsock.writeExact(str, len));
}

/**
 * initialise pixFormat qui donne le format de pixel que l'on veut que
 * le serveur nous envoie.
 * initialise pour demander le RGB
 * sur 3*8 bits = 24 bits
 * A APPELER DANS LE CONSTRUCTEUR (mto)
 */
void VNCRFB::setVisual()
{
  pixFormat.bitsPerPixel = 32;
  pixFormat.depth = 24;
  pixFormat.trueColour = 1;
  pixFormat.bigEndian = 1;
  pixFormat.redMax = 255;
  pixFormat.greenMax = 255;
  pixFormat.blueMax = 255;
  pixFormat.redShift = 0;
  pixFormat.greenShift = 8;
  pixFormat.blueShift = 16;
}

void VNCRFB::printPixelFormat(rfbPixelFormat *format)
{
  if (format->bitsPerPixel == 1) {
    //echo(" Single bit per pixel");
    //echo(" %s significant bit in each byte is leftmost on the screen", (format->bigEndian ? "Most" : "Least"));
  }
  else {
    //echo(" %d bits per pixel", format->bitsPerPixel);
    if (format->bitsPerPixel != 8) {
      //echo(" %s significant byte first in each pixel", (format->bigEndian ? "Most" : "Least"));
    }
    if (format->trueColour) {
      //echo(" True color: max red %d green %d blue %d, shift red %d green %d blue %d", format->redMax, format->greenMax, format->blueMax, format->redShift, format->greenShift, format->blueShift);
    }
    else {
      //echo(" Color map (not true color)");
    }
  }
}

int VNCRFB::getSock()
{
  return vncsock.getSock();
}
