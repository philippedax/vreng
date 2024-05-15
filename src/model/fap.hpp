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
// fap.hpp
//
// Facial Animation Parameters
//---------------------------------------------------------------------------
#ifndef FAP_HPP
#define FAP_HPP


#define NUM_FAPS	68	// number of faps (Mpeg4 FAP-V2.0 specif)

// MPEG4 FAP types
#define HEAD_FAP_V20    "2.0"
#define HEAD_FAP_V21    "2.1"
#define TYPE_FAP_V20	20	// 2.0
#define TYPE_FAP_V21	21	// 2.1

#define FAPV20_DIV	20	// I can explain why
#define FAPV21_DIV	1	// I can explain why

// FAP names
#define VISEME			1
#define EXPRESSION		2
#define OPEN_JAW		3
#define LOWER_T_MIDLIP		4
#define RAISE_B_MIDLIP		5
#define STRETCH_L_CORNERLIP	6
#define STRETCH_R_CORNERLIP	7
#define LOWER_T_LIP_LM		8
#define LOWER_T_LIP_RM		9
#define RAISE_B_LIP_LM		10
#define RAISE_B_LIP_RM		11
#define RAISE_L_CORNERLIP	12
#define RAISE_R_CORNERLIP	13
#define THRUST_JAW		14
#define SHIFT_JAW		15
#define PUSH_B_LIP		16
#define PUSH_T_LIP		17
#define DEPRESS_CHIN		18
#define CLOSE_T_L_EYELID	19
#define CLOSE_T_R_EYELID	20
#define CLOSE_B_L_EYELID	21
#define CLOSE_B_R_EYELID	22
#define YAW_L_EYEBALL		23
#define YAW_R_EYEBALL		24
#define PITCH_L_EYEBALL		25
#define PITCH_R_EYEBALL		26
#define THRUST_L_EYEBALL	27
#define THRUST_R_EYEBALL	28
#define DILATE_L_PUPIL		29
#define DILATE_R_PUPIL		30
#define RAISE_L_I_EYEBROW	31
#define RAISE_R_I_EYEBROW	32
#define RAISE_L_M_EYEBROW	33
#define RAISE_R_M_EYEBROW	34
#define RAISE_L_O_EYEBROW	35
#define RAISE_R_O_EYEBROW	36
#define SQUEEZE_L_EYEBROW	37
#define SQUEEZE_R_EYEBROW	38
#define PUFF_L_CHEEK		39
#define PUFF_R_CHEEK		40
#define LIFT_L_CHEEK		41
#define LIFT_R_CHEEK		42
#define SHIFT_TONGUE_TIP	43
#define RAISE_TONGUE_TIP	44
#define THRUST_TONGUE_TIP	45
#define RAISE_TONGUE		46
#define TONGUE_ROLL		47
#define HEAD_PITCH		48
#define HEAD_YAW		49
#define HEAD_ROLL		50
#define LOWER_T_MIDLIP_O	51
#define RAISE_B_MIDLIP_O	52
#define STRETCH_L_CORNERLIP_O	53
#define STRETCH_R_CORNERLIP_O	54
#define LOWER_T_LIP_LM_O	55
#define LOWER_T_LIP_RM_O	56
#define RAISE_B_LIP_LM_O	57
#define RAISE_B_LIP_RM_O	58
#define RAISE_L_CORNERLIP_O	59
#define RAISE_R_CORNERLIP_O	60
#define STRETCH_L_NOSE		61
#define STRETCH_R_NOSE		62
#define RAISE_NOSE		63
#define BEND_NOSE		64
#define RAISE_L_EAR		65
#define RAISE_R_EAR		66
#define PULL_L_EAR		67
#define PULL_R_EAR		68


#endif
