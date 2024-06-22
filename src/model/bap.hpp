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
// bap.hpp
//---------------------------------------------------------------------------
#ifndef BAP_HPP
#define BAP_HPP

#include "fap.hpp"	// NUM_FAPS


#define NUM_BAPS	186	// number of baps (Mpeg4 BAP-V3.1 specif)

// MPEG4 BAP types
#define HEAD_BAP_V31    "3.1"
#define HEAD_BAP_V32    "3.2"
#define TYPE_BAP_V31    31      // 3.1
#define TYPE_BAP_V32    32      // 3.2

#define TR_DIV		300	// can't explain why
#define BAPV32_DIV	555     // 1e5 / 180
#define BAPV31_DIV	1745    // PI*1e5 / 180

// MPEG4 V31 Bap parameters (#186)
#define	PELVIC_TILT		1	// pelvic
#define	PELVIC_TORS		2
#define	PELVIC_ROLL		3
#define	L_HIP_FLEX		4	// hip / anche
#define	R_HIP_FLEX		5
#define	L_HIP_ABDU		6
#define	R_HIP_ABDU		7
#define	L_HIP_TORS		8
#define	R_HIP_TORS		9
#define	L_KNEE_FLEX		10	// knee / genou
#define	R_KNEE_FLEX		11
#define	L_KNEE_TORS		12
#define	R_KNEE_TORS		13
#define	L_ANKLE_FLEX		14	// ankle / cheville
#define	R_ANKLE_FLEX		15
#define	L_ANKLE_TORS		16
#define	R_ANKLE_TORS		17
#define	L_SUBTALAR		18	// foot / pied
#define	R_SUBTALAR		19
#define	L_MID_FOOT		20
#define	R_MID_FOOT		21
#define	L_TOE_FLEX		22	// toe / orteil
#define	R_TOE_FLEX		23
#define	L_CLAV_ABDU		24	// clavicule
#define	R_CLAV_ABDU		25
#define	L_CLAV_ROTATE		26
#define	R_CLAV_ROTATE		27
#define	L_SCAP_ABDU		28	// scapular
#define	R_SCAP_ABDU		29
#define	L_SCAP_ROTATE		30
#define	R_SCAP_ROTATE		31
#define	L_SHOULDER_FLEX		32	// shoulder / epaule
#define	R_SHOULDER_FLEX		33
#define	L_SHOULDER_ABDU		34
#define	R_SHOULDER_ABDU		35
#define	L_SHOULDER_TORS		36
#define	R_SHOULDER_TORS		37
#define	L_ELBOW_FLEX		38	// elbow / coude
#define	R_ELBOW_FLEX		39
#define	L_ELBOW_TORS		40
#define	R_ELBOW_TORS		41
#define	L_WRIST_FLEX		42	// wrist / poignet
#define	R_WRIST_FLEX		43
#define	L_WRIST_PIVOT		44
#define	R_WRIST_PIVOT		45
#define	L_WRIST_TORS		46
#define	R_WRIST_TORS		47
#define	SKULLBASE_ROLL		48	// skull / crane
#define	SKULLBASE_TORS		49
#define	SKULLBASE_TILT		50
#define	C1_ROLL			51	// cervical
#define	C1_TORS			52
#define	C1_TILT			53
#define	C2_ROLL			54
#define	C2_TORS			55
#define	C2_TILT			56
#define	C3_ROLL			57
#define	C3_TORS			58
#define	C3_TILT			59
#define	C4_ROLL			60
#define	C4_TORS			61
#define	C4_TILT			62
#define	C5_ROLL			63
#define	C5_TORS			64
#define	C5_TILT			65
#define	C6_ROLL			66
#define	C6_TORS			67
#define	C6_TILT			68
#define	C7_ROLL			69
#define	C7_TORS			70
#define	C7_TILT			71
#define	T1_ROLL			72	// thoracics / dorsales
#define	T1_TORS			73
#define	T1_TILT			74
#define	T2_ROLL			75
#define	T2_TORS			76
#define	T2_TILT			77
#define	T3_ROLL			78
#define	T3_TORS			79
#define	T3_TILT			80
#define	T4_ROLL			81
#define	T4_TORS			82
#define	T4_TILT			83
#define	T5_ROLL			84
#define	T5_TORS			85
#define	T5_TILT			86
#define	T6_ROLL			87
#define	T6_TORS			88
#define	T6_TILT			89
#define	T7_ROLL			90
#define	T7_TORS			91
#define	T7_TILT			92
#define	T8_ROLL			93
#define	T8_TORS			94
#define	T8_TILT			95
#define	T9_ROLL			96
#define	T9_TORS			97
#define	T9_TILT			98
#define	T10_ROLL		99
#define	T10_TORS		100
#define	T10_TILT		101
#define	T11_ROLL		102
#define	T11_TORS		103
#define	T11_TILT		104
#define	T12_ROLL		105
#define	T12_TORS		106
#define	T12_TILT		107
#define	L1_ROLL			108	// lombaires
#define	L1_TORS			109
#define	L1_TILT			110
#define	L2_ROLL			111
#define	L2_TORS			112
#define	L2_TILT			113
#define	L3_ROLL			114
#define	L3_TORS			115
#define	L3_TILT			116
#define	L4_ROLL			117
#define	L4_TORS			118
#define	L4_TILT			119
#define	L5_ROLL			120
#define	L5_TORS			121
#define	L5_TILT			122
#define L_PINKY0_FLEX		123	// pinky / oriculaire
#define R_PINKY0_FLEX		124
#define L_PINKY1_FLEX		125
#define R_PINKY1_FLEX		126
#define L_PINKY1_PIVOT		127
#define R_PINKY1_PIVOT		128
#define L_PINKY1_TWIST		129
#define R_PINKY1_TWIST		130
#define L_PINKY2_FLEX		131
#define R_PINKY2_FLEX		132
#define L_PINKY3_FLEX		133
#define R_PINKY3_FLEX		134
#define L_RING0_FLEX		135	// ring / annulaire
#define R_RING0_FLEX		136
#define L_RING1_FLEX		137
#define R_RING1_FLEX		138
#define L_RING1_PIVOT		139
#define R_RING1_PIVOT		140
#define L_RING1_TWIST		141
#define R_RING1_TWIST		142
#define L_RING2_FLEX		143
#define R_RING2_FLEX		144
#define L_RING3_FLEX		145
#define R_RING3_FLEX		146
#define L_MIDDLE0_FLEX		147	// middle / majeur
#define R_MIDDLE0_FLEX		148
#define L_MIDDLE1_FLEX		149
#define R_MIDDLE1_FLEX		150
#define L_MIDDLE1_PIVOT		151
#define R_MIDDLE1_PIVOT		152
#define L_MIDDLE1_TWIST		153
#define R_MIDDLE1_TWIST		154
#define L_MIDDLE2_FLEX		155
#define R_MIDDLE2_FLEX		156
#define L_MIDDLE3_FLEX		157
#define R_MIDDLE3_FLEX		158
#define L_INDEX0_FLEX		159	// index
#define R_INDEX0_FLEX		160
#define L_INDEX1_FLEX		161
#define R_INDEX1_FLEX		162
#define L_INDEX1_PIVOT		163
#define R_INDEX1_PIVOT		164
#define L_INDEX1_TWIST		165
#define R_INDEX1_TWIST		166
#define L_INDEX2_FLEX		167
#define R_INDEX2_FLEX		168
#define L_INDEX3_FLEX		169
#define R_INDEX3_FLEX		170
#define L_THUMB1_FLEX		171	// thumb / pouce
#define R_THUMB1_FLEX		172
#define L_THUMB1_PIVOT		173
#define R_THUMB1_PIVOT		174
#define L_THUMB1_TWIST		175
#define R_THUMB1_TWIST		176
#define L_THUMB2_FLEX		177
#define R_THUMB2_FLEX		178
#define L_THUMB3_FLEX		179
#define R_THUMB3_FLEX		180
#define	TR_VERTICAL		181	// body translations
#define	TR_LATERAL		182
#define	TR_FRONTAL		183
#define	RT_BODY_TURN		184	// body rotations
#define	RT_BODY_ROLL		185
#define	RT_BODY_TILT		186


/**
 * Bap class
 *
 * Body Animator Parameter
 */
class Bap {
 private:
  bool masks[1 + NUM_BAPS];		///< bap/fap bit mask
  float bapvalues[1 + NUM_BAPS];	///< baps angles
  float fapvalues[1 + NUM_FAPS];	///< baps angles

  void reset();
  /**< Resets masks and values */

  void resetMask(int n);
  /**< Resets bit mask */

 public:
  uint8_t type;			///< stream type bap-3.1, bap-3.2, fap-2.1, fap-2.0
  uint16_t params;		///< number of params (186 or 68)
  uint16_t frames;		///< number of frames

  Bap();			///< constructor
  virtual ~Bap(){};		///< destructor

  uint8_t getType() const;
  /**< Returns stream type */

  bool isBap() const;
  bool isBap(uint8_t _type) const;
  bool isFap() const;
  bool isFap(uint8_t _type) const;

  uint8_t parse(char *line);
  /**< Parses and reads header and data of a frame */

  bool isBapMask(int param) const;
  bool isFapMask(int param) const;
  /**< Checks bit indexed by param */

  void setMask(int param, bool val);

  float get(int param) const;
  /**< Gets a Bap angle indexed by param */

  void set(int param, float val);
  /**< Sets bap value */

  uint16_t getParams();
  /**< Gets params number */
};

#endif
