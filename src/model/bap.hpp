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
#define	PELVIC_TORSION		2
#define	PELVIC_ROLL		3
#define	L_HIP_FLEXION		4	// hip
#define	R_HIP_FLEXION		5
#define	L_HIP_ABDUCT		6
#define	R_HIP_ABDUCT		7
#define	L_HIP_TORSION		8
#define	R_HIP_TORSION		9
#define	L_KNEE_FLEXION		10	// knee
#define	R_KNEE_FLEXION		11
#define	L_KNEE_TORSION		12
#define	R_KNEE_TORSION		13
#define	L_ANKLE_FLEXION		14	// ankle
#define	R_ANKLE_FLEXION		15
#define	L_ANKLE_TORSION		16
#define	R_ANKLE_TORSION		17
#define	L_SUBTALAR		18	// foot
#define	R_SUBTALAR		19
#define	L_MID_FOOT		20
#define	R_MID_FOOT		21
#define	L_TOE_FLEXION		22	// toe
#define	R_TOE_FLEXION		23
#define	L_CLAV_ABDUCT		24	// clavicule
#define	R_CLAV_ABDUCT		25
#define	L_CLAV_ROTATE		26
#define	R_CLAV_ROTATE		27
#define	L_SCAP_ABDUCT		28	// scapular
#define	R_SCAP_ABDUCT		29
#define	L_SCAP_ROTATE		30
#define	R_SCAP_ROTATE		31
#define	L_SHOULDER_FLEXION	32	// shoulder
#define	R_SHOULDER_FLEXION	33
#define	L_SHOULDER_ABDUCT	34
#define	R_SHOULDER_ABDUCT	35
#define	L_SHOULDER_TORSION	36
#define	R_SHOULDER_TORSION	37
#define	L_ELBOW_FLEXION		38	// elbow
#define	R_ELBOW_FLEXION		39
#define	L_ELBOW_TORSION		40
#define	R_ELBOW_TORSION		41
#define	L_WRIST_FLEXION		42	// wrist
#define	R_WRIST_FLEXION		43
#define	L_WRIST_PIVOT		44
#define	R_WRIST_PIVOT		45
#define	L_WRIST_TORSION		46
#define	R_WRIST_TORSION		47
#define	SKULLBASE_ROLL		48	// skull
#define	SKULLBASE_TORSION	49
#define	SKULLBASE_TILT		50
#define	C1_ROLL			51	// cervicales
#define	C1_TORSION		52
#define	C1_TILT			53
#define	C2_ROLL			54
#define	C2_TORSION		55
#define	C2_TILT			56
#define	C3_ROLL			57
#define	C3_TORSION		58
#define	C3_TILT			59
#define	C4_ROLL			60
#define	C4_TORSION		61
#define	C4_TILT			62
#define	C5_ROLL			63
#define	C5_TORSION		64
#define	C5_TILT			65
#define	C6_ROLL			66
#define	C6_TORSION		67
#define	C6_TILT			68
#define	C7_ROLL			69
#define	C7_TORSION		70
#define	C7_TILT			71
#define	T1_ROLL			72	// torsales
#define	T1_TORSION		73
#define	T1_TILT			74
#define	T2_ROLL			75
#define	T2_TORSION		76
#define	T2_TILT			77
#define	T3_ROLL			78
#define	T3_TORSION		79
#define	T3_TILT			80
#define	T4_ROLL			81
#define	T4_TORSION		82
#define	T4_TILT			83
#define	T5_ROLL			84
#define	T5_TORSION		85
#define	T5_TILT			86
#define	T6_ROLL			87
#define	T6_TORSION		88
#define	T6_TILT			89
#define	T7_ROLL			90
#define	T7_TORSION		91
#define	T7_TILT			92
#define	T8_ROLL			93
#define	T8_TORSION		94
#define	T8_TILT			95
#define	T9_ROLL			96
#define	T9_TORSION		97
#define	T9_TILT			98
#define	T10_ROLL		99
#define	T10_TORSION		100
#define	T10_TILT		101
#define	T11_ROLL		102
#define	T11_TORSION		103
#define	T11_TILT		104
#define	T12_ROLL		105
#define	T12_TORSION		106
#define	T12_TILT		107
#define	L1_ROLL			108	// lombaires
#define	L1_TORSION		109
#define	L1_TILT			110
#define	L2_ROLL			111
#define	L2_TORSION		112
#define	L2_TILT			113
#define	L3_ROLL			114
#define	L3_TORSION		115
#define	L3_TILT			116
#define	L4_ROLL			117
#define	L4_TORSION		118
#define	L4_TILT			119
#define	L5_ROLL			120
#define	L5_TORSION		121
#define	L5_TILT			122
#define L_PINKY0_FLEXION	123	// pinky
#define R_PINKY0_FLEXION	124
#define L_PINKY1_FLEXION	125
#define R_PINKY1_FLEXION	126
#define L_PINKY1_PIVOT		127
#define R_PINKY1_PIVOT		128
#define L_PINKY1_TWIST		129
#define R_PINKY1_TWIST		130
#define L_PINKY2_FLEXION	131
#define R_PINKY2_FLEXION	132
#define L_PINKY3_FLEXION	133
#define R_PINKY3_FLEXION	134
#define L_RING0_FLEXION		135	// ring
#define R_RING0_FLEXION		136
#define L_RING1_FLEXION		137
#define R_RING1_FLEXION		138
#define L_RING1_PIVOT		139
#define R_RING1_PIVOT		140
#define L_RING1_TWIST		141
#define R_RING1_TWIST		142
#define L_RING2_FLEXION		143
#define R_RING2_FLEXION		144
#define L_RING3_FLEXION		145
#define R_RING3_FLEXION		146
#define L_MIDDLE0_FLEXION	147	// middle
#define R_MIDDLE0_FLEXION	148
#define L_MIDDLE1_FLEXION	149
#define R_MIDDLE1_FLEXION	150
#define L_MIDDLE1_PIVOT		151
#define R_MIDDLE1_PIVOT		152
#define L_MIDDLE1_TWIST		153
#define R_MIDDLE1_TWIST		154
#define L_MIDDLE2_FLEXION	155
#define R_MIDDLE2_FLEXION	156
#define L_MIDDLE3_FLEXION	157
#define R_MIDDLE3_FLEXION	158
#define L_INDEX0_FLEXION	159	// index
#define R_INDEX0_FLEXION	160
#define L_INDEX1_FLEXION	161
#define R_INDEX1_FLEXION	162
#define L_INDEX1_PIVOT		163
#define R_INDEX1_PIVOT		164
#define L_INDEX1_TWIST		165
#define R_INDEX1_TWIST		166
#define L_INDEX2_FLEXION	167
#define R_INDEX2_FLEXION	168
#define L_INDEX3_FLEXION	169
#define R_INDEX3_FLEXION	170
#define L_THUMB1_FLEXION	171	// thumb
#define R_THUMB1_FLEXION	172
#define L_THUMB1_PIVOT		173
#define R_THUMB1_PIVOT		174
#define L_THUMB1_TWIST		175
#define R_THUMB1_TWIST		176
#define L_THUMB2_FLEXION	177
#define R_THUMB2_FLEXION	178
#define L_THUMB3_FLEXION	179
#define R_THUMB3_FLEXION	180
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
  uint8_t masks[1 + NUM_BAPS];	///< bap/fap bit mask
  float values[1 + NUM_BAPS];	///< baps angles

  void resetMask(int n);
  /**< Resets bit mask */

 public:
  uint8_t type;			///< stream type bap-3.1, bap-3.2, fap
  uint16_t params;		///< number of params
  uint16_t frames;		///< number of frames

  Bap();			///< constructor
  virtual ~Bap(){};		///< destructor

  uint8_t getType() const;
  /**< Returns stream type */

  uint8_t parse(char *line);
  /**< Parses and reads header and data of a frame */

  bool isMask(int param) const;
  /**< Checks bit indexed by param */

  void setMask(int param, uint8_t val);

  float get(int param) const;
  /**< Gets a Bap angle indexed by param */

  void set(int param, float val);
  /**< Sets bap value */

  uint16_t getParams();
  /**< Gets params number */
};

#endif
