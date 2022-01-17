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
#ifndef BAP_HPP
#define BAP_HPP

#include "fap.hpp"	// NUM_FAPS = 68


#define NUM_BAPS_V31	186	// number of baps (Mpeg4 BAP-V3.1 specif)
#define NUM_BAPS_V32	296	// number of baps (Mpeg4 BAP-V3.2 specif)

// MPEG4 BAP types
#define HEAD_BAP_V31    "3.1"
#define HEAD_BAP_V32    "3.2"
#define TYPE_BAP_V31    31      // 3.1
#define TYPE_BAP_V32    32      // 3.2

#define TR_DIV		300.	// I can explain why
#define BAPV32_DIV	555     // 180/1e5
#define BAPV31_DIV	1745    // 180/PI*1e5

// MPEG4 Bap parameters
#define	PELVIC_TILT		1
#define	PELVIC_TORSION		2
#define	PELVIC_ROLL		3
#define	L_HIP_FLEXION		4
#define	R_HIP_FLEXION		5
#define	L_HIP_ABDUCT		6
#define	R_HIP_ABDUCT		7
#define	L_HIP_TWIST		8
#define	R_HIP_TWIST		9
#define	L_KNEE_FLEXION		10
#define	R_KNEE_FLEXION		11
#define	L_KNEE_TWIST		12
#define	R_KNEE_TWIST		13
#define	L_ANKLE_FLEXION		14
#define	R_ANKLE_FLEXION		15
#define	L_ANKLE_TWIST		16
#define	R_ANKLE_TWIST		17
#define	L_SUBTALAR		18
#define	R_SUBTALAR		19
#define	L_MID_FOOT		20
#define	R_MID_FOOT		21
#define	L_TOE_FLEXION		22
#define	R_TOE_FLEXION		23
#define	L_CLAV_ABDUCT		24
#define	R_CLAV_ABDUCT		25
#define	L_CLAV_ROTATE		26
#define	R_CLAV_ROTATE		27
#define	L_SCAP_ABDUCT		28
#define	R_SCAP_ABDUCT		29
#define	L_SCAP_ROTATE		30
#define	R_SCAP_ROTATE		31
#define	L_SHOULDER_FLEXION	32
#define	R_SHOULDER_FLEXION	33
#define	L_SHOULDER_ABDUCT	34
#define	R_SHOULDER_ABDUCT	35
#define	L_SHOULDER_TWIST	36
#define	R_SHOULDER_TWIST	37
#define	L_ELBOW_FLEXION		38
#define	R_ELBOW_FLEXION		39
#define	L_ELBOW_TWIST		40
#define	R_ELBOW_TWIST		41
#define	L_WRIST_FLEXION		42
#define	R_WRIST_FLEXION		43
#define	L_WRIST_PIVOT		44
#define	R_WRIST_PIVOT		45
#define	L_WRIST_TWIST		46
#define	R_WRIST_TWIST		47
#define	C1_ROLL			48
#define	C1_TORSION		49
#define	C1_TILT			50
#define	C2_ROLL			51
#define	C2_TORSION		52
#define	C2_TILT			53
#define	C3_ROLL			54
#define	C3_TORSION		55
#define	C3_TILT			56
#define	C4_ROLL			57
#define	C4_TORSION		58
#define	C4_TILT			59
#define	T1_ROLL			69
#define	T1_TORSION		70
#define	T1_TILT			71
#define	T5_ROLL			81
#define	T5_TORSION		82
#define	T5_TILT			83
#define	L1_ROLL			105
#define	L1_TORSION		106
#define	L1_TILT			107
#define L_PINKY_FLEXION1	120
#define R_PINKY_FLEXION1	121
#define L_PINKY_PIVOTABDUCT	122
#define R_PINKY_PIVOTABDUCT	123
#define L_PINKY_TWISTING	124
#define R_PINKY_TWISTING	125
#define L_PINKY_FLEXION2	126
#define R_PINKY_FLEXION2	127
#define L_PINKY_FLEXION3	128
#define R_PINKY_FLEXION3	129
#define L_RING_FLEXION1		130
#define R_RING_FLEXION1		131
#define L_RING_PIVOTABDUCT	132
#define R_RING_PIVOTABDUCT	133
#define L_RING_TWISTING		134
#define R_RING_TWISTING		135
#define L_RING_FLEXION2		136
#define R_RING_FLEXION2		137
#define L_RING_FLEXION3		138
#define R_RING_FLEXION3		139
#define L_MIDDLE_FLEXION1	140
#define R_MIDDLE_FLEXION1	141
#define L_MIDDLE_PIVOTABDUCT	142
#define R_MIDDLE_PIVOTABDUCT	143
#define L_MIDDLE_TWISTING	144
#define R_MIDDLE_TWISTING	145
#define L_MIDDLE_FLEXION2	146
#define R_MIDDLE_FLEXION2	147
#define L_MIDDLE_FLEXION3	148
#define R_MIDDLE_FLEXION3	149
#define L_INDEX_FLEXION1	150
#define R_INDEX_FLEXION1	151
#define L_INDEX_PIVOTABDUCT	152
#define R_INDEX_PIVOTABDUCT	153
#define L_INDEX_TWISTING	154
#define R_INDEX_TWISTING	155
#define L_INDEX_FLEXION2	156
#define R_INDEX_FLEXION2	157
#define L_INDEX_FLEXION3	158
#define R_INDEX_FLEXION3	159
#define L_THUMB_FLEXION1	160
#define R_THUMB_FLEXION1	161
#define L_THUMB_PIVOTABDUCT	162
#define R_THUMB_PIVOTABDUCT	163
#define L_THUMB_TWISTING	164
#define R_THUMB_TWISTING	165
#define L_THUMB_FLEXION2	166
#define R_THUMB_FLEXION2	167
#define L_THUMB_FLEXION3	168
#define R_THUMB_FLEXION3	169
#define	TR_VERTICAL		170
#define	TR_LATERAL		171
#define	TR_FRONTAL		172
#define	RT_BODY_TURN		173
#define	RT_BODY_ROLL		174
#define	RT_BODY_TILT		175

/* externs */
extern const char pause_bap[];
extern const char hi_bap[];
extern const char bye_bap[];
extern const char ask_bap[];
extern const char show_bap[];
extern const char sit_bap[];
extern const char clap_bap[];
extern const char nak_bap[];
extern const char test_bap[];
extern const char reset_bap[];
extern const char eyes_fap[];
extern const char joy_fap[];
extern const char sad_fap[];
extern const char surp_fap[];


/**
 * Bap class
 * Body Animator Parameter
 */
class Bap {

private:
  int num_baps;		///< number of bap
  uint8_t baptype;	///< stream type bap-3.1, bap-3.2, fap

public:
  Bap();		///< constructor
  virtual ~Bap(){}	///< destructor

  virtual uint8_t getType() const;
  /**< Returns stream type */

  virtual uint8_t parse(char *bapline);
  /**< Parses and reads header and data of a frame */

  virtual bool is(uint8_t param) const;
  /**< Checks bit indexed by param */

  virtual GLfloat get(uint8_t param) const;
  /**< Gets a Bap angle indexed by param */

  virtual GLfloat getFap(uint8_t param) const;
  /**< Gets a Fap angle indexed by param */

  virtual void setMask(uint8_t index, uint8_t val);
  virtual void setBap(uint8_t index, int val);
  virtual void setFap(uint8_t index, uint8_t val);
  /**< Sets mask bap fap */

  virtual void jpRX(int param, uint8_t model);
  /**< Rotates around X axis at this Joint Point */

  virtual void jpRY(int param, uint8_t model);
  /**< Rotates around Y axis at this Joint Point */

  virtual void jpRZ(int param, uint8_t model);
  /**< Rotates around Z axis at this Joint Point */

public:
  int bit[NUM_BAPS_V32 + 1];    ///< bap/fap bit mask
  int ba[NUM_BAPS_V32 + 1];     ///< baps angles
  int balast[NUM_BAPS_V32 + 1]; ///< last baps angles
  int fa[NUM_FAPS + 1];         ///< faps angles

  virtual void resetMask(uint8_t num);
  /**< Resets bit mask */

  virtual bool equalLast(uint8_t param);
  /**< Checks whether same bap value */

  virtual void copyLast(uint8_t param);
  /**< Saves last bap */
};

#endif
