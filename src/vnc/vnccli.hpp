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
// vnccli.hpp
//---------------------------------------------------------------------------
#ifndef VNCCLI_HPP
#define VNCCLI_HPP

#include "vreng.hpp"
#include "vncrfb.hpp"

#define RFB_BUF_SIZE	3*1280*960	// dax orig: 640*480


/**
 * VNCrgb class
 *  this is what our framebuffer is made of 24 bits per pixel
 *  8bits for Red, 8 for Green, 8 for Blue
 */
class VNCrgb {
 public:
  VNCrgb(); 	///< is only used to handle faster 32bits pixels we get from the server
  VNCrgb(const uint32_t &pixel);
  VNCrgb(uint8_t red, uint8_t green, uint8_t blue);

  uint8_t red;
  uint8_t green;
  uint8_t blue;
};


/**
 * VNCCli class
 *  object will be used in VReng:
 *  - to connect to the VNC server
 *  - to handle its messages
 *  - to forward pointer and keyboard events to the server
 *  framebuffer is public
 */
class VNCCli {
 private:
  char rfbbuffer[RFB_BUF_SIZE];		///< this buffer is used to get Server MSGs
  void fillRect(int rx, int ry, int rw, int rh, VNCrgb pixel); ///< handling Rectangles
  uint8_t rescalePixValue(uint32_t Pix, uint8_t Shift, uint16_t Max);
  VNCrgb cardToVNCrgb(uint32_t pixel);

  bool handleRAW32(int rx, int ry, int rw, int rh);
  //notused bool handleCR(int srcx, int srcy, int rx, int ry, int rw, int rh);
  //notused bool handleRRE32(int rx, int ry, int rw, int rh);
  //notused bool handleCoRRE32(int rx, int ry, int rw, int rh);
  //notused bool handleHextile32(int rx, int ry, int rw, int rh);

 protected:
  VNCRFB rfbproto;		///< implements all we need to send and receive RFB messages

 public:
  VNCCli(char *server, int port, char *passFile);	///< constructor

  VNCrgb *framebuffer;		///< here is our framebuffer
  uint16_t fbWidth;		///< framebuffer width
  uint16_t fbHeight;		///< framebuffer height
  char *serverCutText;		///< to handle server cut texts

  int getSock();		///< to get the socket descriptor to listen to it
  bool closeVNC();		///< closeVNC has to free it
  bool handleRFBMessage();	///< handle the messages from the server

  /* Remote Frame Buffer Protocol v3.3 */
  void sendRFBEvent(char **params, uint32_t *num_params);
  bool sendIncrementalFramebufferUpdateRequest();
  bool sendFramebufferUpdateRequest(int x, int y, int w, int h, bool incremental);
};


/**
 * VNCCliTextured class
 *  inherits everything from VNCCli
 *  the framebuffer allocated is bigger than the real VNC server screen size
 *  and has dimensions that are a power of 2
 *  needed by OpenGL to map the texture
 */
class VNCCliTextured : public VNCCli {
 public:
  VNCCliTextured(char *server, int port, char *pass);

  uint16_t realScreenWidth;	///< real screen width
  uint16_t realScreenHeight;	///< real screen height

  bool initVNC();		///< to allocate memory for the framebuffer
};

#endif
