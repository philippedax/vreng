/* ==================================================== ======== ======= *
 *
 *  ufontmetrics.hpp
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE : 
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE 
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU 
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef _ufontmetrics_hpp_
#define	_ufontmetrics_hpp_ 1
namespace ubit {

class UFontMetrics {
public:
  UFontMetrics();

  UFontMetrics(const UFont&, UDisp*);

  UFontMetrics(const UFontDesc&, UDisp*);
  ///< Note: the UFontDesc argument must NOT be destroyed while this UFontMetrics is used.

  UFontMetrics(UUpdateContext&);

  ~UFontMetrics();
    
  void set(const UFont&, UDisp* = null);
  void set(const UFontDesc&, UDisp* = null);

  float getAscent() const;  
  float getDescent() const;
  float getHeight() const;

  float getWidth(char) const;  
  float getWidth(const UStr&) const;
  float getWidth(const UStr&, int from_char, int len = -1) const;
  float getWidth(const char* str, int str_len = -1) const;

  int getCharPos(const char* str, int str_len, float x) const;
  ///< converts x pos to char pos.

  float getXPos(const char* str, int str_len, int char_pos) const;
  ///< converts char pos to x pos.
  
  bool getSubTextSize(const char* s, int s_len, UDimension&, float available_width, 
                      int& s_sublen, int& change_line) const;
  /**< returns font metrics of a substring that verifies certain conditions.
    * Notes:
    * - the substring starts from the beginning of 'str' and ends at 
    *   the 1st \n  or 1st " " that is before 'available_width'.
    * - it is equal to 'str' otherwise.
    * - returns w and h that this substring occupies and its length 'sublen'
    * - change_line = 1 si line must be changed because of size
    *   and 2 if a \n was found
    */
  
  static bool 
    getClippedText(const UHardFont*, float clip_x, float clip_width,
                   const char* str, int str_len, 
                   float x0, float y0, int& charpos_begin, int& charpos_end,
                   float& xpos_begin, float& xpos_end);
  /**< returns the substring that is enclosed in the current clip.
    * utilise par drawString en mode OpenGL pour eviter de dessiner
    * ce qui sort de la zone de clipping
    */

protected:
  UDisp* disp;
  const UFontDesc* fd;
  bool own_fd;
};

}
#endif
