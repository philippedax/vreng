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
#ifndef VNCCLI_HPP
#define VNCCLI_HPP

#include "vreng.hpp"
#include "vncrfb.hpp"

#define RFB_BUF_SIZE	640*480


/**
 * class VNCRGB
 * this is what our framebuffer is made of 24 bits per pixel
 * 8bits for Red, 8 for Green, 8 for Blue
 */
class VNCRGB {
 public:

  VNCRGB();
  ///< this constructor is only used to handle faster 32bits pixels we get from the server
  VNCRGB(const uint32_t &pixel);
  VNCRGB(uint8_t red, uint8_t green, uint8_t blue);

  uint8_t red;
  uint8_t green;
  uint8_t blue;
};


/**
 * class VNCClient
 * object will be used in VReng:
 * - to connect to the VNC server
 * - to handle its messages
 * - to forward pointer and keyboard events to the server
 * framebuffer is public
 */
class VNCClient {

 private:
  char rfbbuffer[RFB_BUF_SIZE];
  ///< this buffer is used to get Server MSGs

  bool viewonly;
  ///< do we want to forward events? (no if true)

  void fillRect(int rx, int ry, int rw, int rh, VNCRGB pixel);
  ///< handling Rectangles we got from server

  uint8_t rescalePixValue(uint32_t Pix, uint8_t Shift, uint16_t Max);
  VNCRGB cardToVNCRGB(uint32_t Pixel);

  bool handleRAW32(int rx, int ry, int rw, int rh);
  bool handleCR(int srcx, int srcy, int rx, int ry, int rw, int rh);
  bool handleRRE32(int rx, int ry, int rw, int rh);
  bool handleCoRRE32(int rx, int ry, int rw, int rh);
  bool handleHextile32(int rx, int ry, int rw, int rh);

 protected:
  VNCRFB rfbproto;
  /**< the VNCRFB object implements all we need to to send and receive
   * messages specified in the RFB protocol
   */

 public:
  VNCClient(char *server, int port, char *passFile);
  ///< constructor

  char *serverCutText;
  bool newCutText;
  ///< for now, public attributes we keep to handle server cut texts

  VNCRGB *framebuffer;
  uint16_t fbWidth;
  uint16_t fbHeight;
  ///< here is our framebuffer

  int getSock();
  ///< we might want to get the socket descriptor to listen to it

  // set of functions to initialize the connection
  enum {BPP8, BPP16, BPP32};

  bool VNCInit();
#if 0 //not used
  bool VNCInit(int flag);
  ///< flag can be BPP8, BPP16 or BPP16 to indicate how many bits per pixel we want
#endif
  ///< VNCInit has to allocate memory for the framebuffer if everything goes fine
  bool VNCClose();
  ///< VNCClose has to free it

  /* Remote Frame Buffer Protocol v3.3 */

  void sendRFBEvent(char **params, unsigned int *num_params) ;
  bool sendIncrementalFramebufferUpdateRequest();
  bool sendFramebufferUpdateRequest(int x, int y, int w, int h, bool incremental);
  ///< messages from client to server

  bool handleRFBMessage();
  ///< handle the messages from the server
};


/**
 * class VNCClientTextured
 * inherits everything from VNCClient
 * the framebuffer allocated is bigger than the real VNC server screen size
 * and has dimensions that are a power of 2
 * needed by OpenGL to map the texture
 */
class VNCClientTextured : public VNCClient {

 public:
  VNCClientTextured(char *server, int port, char *pass);

  uint16_t realScreenWidth;
  uint16_t realScreenHeight;
  ///< here we keep what the real server screen size is

  bool VNCInit();
};

#endif
