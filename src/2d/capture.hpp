//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
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
/*
 *  capture.hpp : captures for the VREng GUI
 *  VREng Project
 *  Author: Eric Lecolinet - ENST Paris
 *
 *  WWW: http://www.enst.fr/~elc  http://vreng.enst.fr/
 */
#ifndef CAPTURE_HPP
#define CAPTURE_HPP


class Capture {
 public:
  static const unsigned int MAX_CAPTURE_COUNT = 1000;

  Capture();
  
  void writeWINImage();
  /**< Capture and save one image in JPG */
  
  void writeJPGImage();
  /**< Capture and save one image in JPG */
  
  void writePNGImage();
  /**< Capture and save one image in PNG */
  
  void startVideo();
  /**< Start capture video */

  void stopVideo();
  /**< Stop capture video */
  
  void writeVideoFrame();
  /**< Make capture video */

  bool isCapturingVideo() const {return is_capturing;}

 private:
  bool is_capturing;		      ///< is capturing video.
  unsigned int capture_no;
  
  void captureXwd(const char *ext);
  /**< Capture and save one image by xwd */

  void captureGl2JPG(const char *ext);
  /**< Capture and save one image in JPEG */

  void captureGl2PNG(const char *ext);
  /**< Capture and save one image in PNG */
  
#if WANT_GL2PS
  void captureImagePS();
  /**< Capture and save one image in PS */
  
  void captureImageEPS();
  /**< Capture and save one image in EPS */
  
  void captureImagePDF();
  /**< Capture and save one image in PDF */
  
  void captureImageSVG();
  /**< Capture and save one image in SVG */
  
  void captureGl2PS(const char *ext);
  /**< Capture and save one image by gl2ps */
#endif  
};


#endif
