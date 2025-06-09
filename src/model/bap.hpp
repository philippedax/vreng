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
#define	HIP_L_FLEX		4	// hip / anche
#define	HIP_R_FLEX		5
#define	HIP_L_ABDU		6
#define	HIP_R_ABDU		7
#define	HIP_L_TORS		8
#define	HIP_R_TORS		9
#define	KNEE_L_FLEX		10	// knee / genou
#define	KNEE_R_FLEX		11
#define	KNEE_L_TORS		12
#define	KNEE_R_TORS		13
#define	ANKLE_L_FLEX		14	// ankle / cheville
#define	ANKLE_R_FLEX		15
#define	ANKLE_L_TORS		16
#define	ANKLE_R_TORS		17
#define	SUBTALAR_L		18	// foot / pied
#define	SUBTALAR_R		19
#define	FOOT_MID_L		20
#define	FOOT_MID_R		21
#define	TOE_L_FLEX		22	// toe / orteil
#define	TOE_R_FLEX		23
#define	CLAV_L_ABDU		24	// clavicule
#define	CLAV_R_ABDU		25
#define	CLAV_L_ROTATE		26
#define	CLAV_R_ROTATE		27
#define	SCAP_L_ABDU		28	// scapular
#define	SCAP_R_ABDU		29
#define	SCAP_L_ROTATE		30
#define	SCAP_R_ROTATE		31
#define	SHOULDER_L_FLEX		32	// shoulder / epaule
#define	SHOULDER_R_FLEX		33
#define	SHOULDER_L_ABDU		34
#define	SHOULDER_R_ABDU		35
#define	SHOULDER_L_TORS		36
#define	SHOULDER_R_TORS		37
#define	ELBOW_L_FLEX		38	// elbow / coude
#define	ELBOW_R_FLEX		39
#define	ELBOW_L_TORS		40
#define	ELBOW_R_TORS		41
#define	WRIST_L_FLEX		42	// wrist / poignet
#define	WRIST_R_FLEX		43
#define	WRIST_L_PIVOT		44
#define	WRIST_R_PIVOT		45
#define	WRIST_L_TORS		46
#define	WRIST_R_TORS		47
#define	SKULLBASE_ROLL		48	// skull / crane
#define	SKULLBASE_TORS		49
#define	SKULLBASE_TILT		50
#define	C1_ROLL			51	// cervical / nuque
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
#define PINKY0_L_FLEX		123	// pinky / oriculaire
#define PINKY0_R_FLEX		124
#define PINKY1_L_FLEX		125
#define PINKY1_R_FLEX		126
#define PINKY1_L_PIVOT		127
#define PINKY1_R_PIVOT		128
#define PINKY1_L_TWIST		129
#define PINKY1_R_TWIST		130
#define PINKY2_L_FLEX		131
#define PINKY2_R_FLEX		132
#define PINKY3_L_FLEX		133
#define PINKY3_R_FLEX		134
#define RING0_L_FLEX		135	// ring / annulaire
#define RING0_R_FLEX		136
#define RING1_L_FLEX		137
#define RING1_R_FLEX		138
#define RING1_L_PIVOT		139
#define RING1_R_PIVOT		140
#define RING1_L_TWIST		141
#define RING1_R_TWIST		142
#define RING2_L_FLEX		143
#define RING2_R_FLEX		144
#define RING3_L_FLEX		145
#define RING3_R_FLEX		146
#define MIDDLE0_L_FLEX		147	// middle / majeur
#define MIDDLE0_R_FLEX		148
#define MIDDLE1_L_FLEX		149
#define MIDDLE1_R_FLEX		150
#define MIDDLE1_L_PIVOT		151
#define MIDDLE1_R_PIVOT		152
#define MIDDLE1_L_TWIST		153
#define MIDDLE1_R_TWIST		154
#define MIDDLE2_L_FLEX		155
#define MIDDLE2_R_FLEX		156
#define MIDDLE3_L_FLEX		157
#define MIDDLE3_R_FLEX		158
#define INDEX0_L_FLEX		159	// index
#define INDEX0_R_FLEX		160
#define INDEX1_L_FLEX		161
#define INDEX1_R_FLEX		162
#define INDEX1_L_PIVOT		163
#define INDEX1_R_PIVOT		164
#define INDEX1_L_TWIST		165
#define INDEX1_R_TWIST		166
#define INDEX2_L_FLEX		167
#define INDEX2_R_FLEX		168
#define INDEX3_L_FLEX		169
#define INDEX3_R_FLEX		170
#define THUMB1_L_FLEX		171	// thumb / pouce
#define THUMB1_R_FLEX		172
#define THUMB1_L_PIVOT		173
#define THUMB1_R_PIVOT		174
#define THUMB1_L_TWIST		175
#define THUMB1_R_TWIST		176
#define THUMB2_L_FLEX		177
#define THUMB2_R_FLEX		178
#define THUMB3_L_FLEX		179
#define THUMB3_R_FLEX		180
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
  bool bapmasks[1 + NUM_BAPS];		///< bap bit masks
  bool fapmasks[1 + NUM_FAPS];		///< fap bit masks
  float bapvalues[1 + NUM_BAPS];	///< baps angles
  float fapvalues[1 + NUM_FAPS];	///< faps angles

  void reset();
  /**< Resets masks and values */

 public:
  uint8_t type;			///< stream type bap-3.1, bap-3.2, fap-2.1, fap-2.0
  uint16_t params;		///< number of params (186 or 68)
  uint16_t frames;		///< number of frames

  Bap();			///< constructor
  ~Bap(){};			///< destructor

  uint8_t getType() const;
  /**< Returns stream type */

  bool isBap() const;
  bool isBap(uint8_t _type) const;
  bool isFap() const;
  bool isFap(uint8_t _type) const;

  void resetMasks();
  /**< Resets bit mask */

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
