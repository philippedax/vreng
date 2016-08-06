/* 
 * This version of 24bit.c has been heavily modified by
 * Magnus Heldestat <Magnus.Heldestad@cs.umu.se> to optimize
 * for speed.  Thanks, Magnus!
 */

/*
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <config.h>
#include "video.h"
#include "dither.h"
#include "proto.h"
#include <time.h>
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "my_dmalloc.h"

/*
	Optimization defines

	SAVE_L		Save L between eacn pixel
	SAVE_CR		Save CR and CB netween each y iter
	SAVE_ALL	SAVE_L + SAVE_CR + Save last L for the first L
	POOR_Q1		Fast, but yields lower quality 
	POOR_Q2		SAVE_ALL + Fast, but yields lower quality 
	PRE_CALC	Precalculates the YCrCb to RGB values (in InitColorDither)
	FLIP		Flips the image in the correct way for lrectwrite (gl)
*/

#define SAVE_ALL   /* SAVE_L and SAVE_CR automatically turned on */
#define PRE_CALC
#undef FLIP
#undef POOR_Q1
#undef POOR_Q2

/* Benchmarking for the above defines for 20 * LiftOff.mpg (in secs)
   Hardware: Indy R4400 (150 MHz) with 64 meg memory running Irix 5.2
   Compiler options: -kpicopt -O2 -mips2
  
   Name            User     Sys    Real   Func only
   ====            ====     ===    ====   =========
   unopt version: 108.941  1.055  1:50.98   50.59
   ALL UNDEFINED: 106.254  1.098  1:50.12   38.99
   SAVE_CR:       92.287   1.118  1:37.13   34.29
   only PRE_CALC: 91.871   1.021  1:33.72   33.54
   SAVE_L:        86.844   1.038  1:28.68   28.80
   SAVE_ALL:      85.385   0.866  1:26.96   27.25
   POOR_Q1:       76.498   0.905  1:18.04   18.41
   POOR_Q2:       76.645   0.914  1:18.37   18.37

   NOTE 1: the unopt version is the same as in mpeg_lib-1.1
   NOTE 2: PRE_CALC is defined in all except ALL UNDEFINED
   NOTE 3: LiftOff is a Alias demo movie (see www.alias.com)

   As can be seen, the ColorDitherImage runs almost twice as fast with
   SAVE_ALL and PRE_CALC defined  compared with the unopt version.
 */


/*
 * We'll define the "ConvertColor" macro here to do fixed point arithmetic
 * that'll convert from YCrCb to RGB using:
 *	R = L + 1.40200*Cr;
 *	G = L - 0.34414*Cb - 0.71414*Cr
 *	B = L + 1.77200*Cb;
 *
 * We'll use fixed point by adding two extra bits after the decimal.
 */

#define BITS	     8
#define ONE          ((int) 1)
#define CONST_SCALE  (ONE << BITS)
#define ALIGN(x,s) (((x & 0xff) == x) ? (x << s) : ((x < 0) ? 0 : (255 << s)))
#define RR(x)      (ALIGN(x, 0))
#define GG(x)      (ALIGN(x, (BITS)))
#define BB(x)      (ALIGN(x, (BITS*2)))

/* Macro to convert a float to a fixed */
#define FIX(x)  	((int) ((x)*CONST_SCALE + 0.5))
#define FIX2(x,v)	(FIX(x) * (v - 128))

/* Turn on SAVE_ALL if POOR_Q2 */
#ifdef POOR_Q2
#ifndef SAVE_ALL
#define SAVE_ALL
#endif
#endif

/* SAVE_ALL implies SAVE_L and SAVE_CR */
#ifdef SAVE_ALL
#ifndef SAVE_L
#define SAVE_L
#endif
#ifndef SAVE_CR
#define SAVE_CR
#endif
#endif

/* If POOR_Q?, simplify the ALIGN define */
#ifdef POOR_Q1
#undef ALIGN
#define ALIGN(x,s)  (x << s)
#endif
#ifdef POOR_Q2
#undef ALIGN
#define ALIGN(x,s)  (x << s)
#endif

#ifdef PRE_CALC
static int *Cb_r_tab, *Cr_g_tab, *Cb_g_tab, *Cr_b_tab;
#endif

/*
 *--------------------------------------------------------------
 *
 * InitColorDither --
 *
 *	To get rid of the multiply and other conversions in color
 *	dither, we use a lookup table.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The lookup tables are initialized.
 *
 *--------------------------------------------------------------
 */

void
InitColorDither()
{
#ifdef PRE_CALC
    int CR, CB, i;

    /* XXX leak alert! these are never free()'d... --GPW 1998/06/30 */
    Cr_b_tab = (int *)malloc(256*sizeof(int));
    Cr_g_tab = (int *)malloc(256*sizeof(int));
    Cb_g_tab = (int *)malloc(256*sizeof(int));
    Cb_r_tab = (int *)malloc(256*sizeof(int));

    for (i=0; i<256; i++) {
	CB = CR = i;

	CB -= 128; CR -= 128;

	Cb_r_tab[i] = FIX(1.40200) * CB;
	Cr_g_tab[i] = -FIX(0.34414) * CR;
	Cb_g_tab[i] = -FIX(0.71414) * CB;   
	Cr_b_tab[i] = FIX(1.77200) * CR;

	Cb_r_tab[i] >>= BITS;
	Cr_g_tab[i] >>= BITS;
	Cb_g_tab[i] >>= BITS;
	Cr_b_tab[i] >>= BITS;
    }
#endif
}



/* Space saving define */
#define RR_GG_BB\
	R = L + cb_r;\
	G = L + cr_cb;\
	B = L + cr_b;\
	r = RR(R);\
	g = GG(G);\
	b = BB(B);

/*
 *--------------------------------------------------------------
 *
 * ColorDitherImage --
 *
 *	Converts image into 24 bit color.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
ColorDitherImage(lum, cr, cb, out, rows, cols)
  unsigned char *lum;
  unsigned char *cr;
  unsigned char *cb;
  unsigned char *out;
  int cols, rows;

{
    int L, CR, CB;
    unsigned int *row1, *row2;
    unsigned char *lum2;
    int x, y;
    unsigned int r, b, g;
    int cb_r=0, cr_b=0, cr_cb=0;
#ifndef PRE_CALC
    int t1, t2;
#endif
#ifdef SAVE_L
    int rgb = 0;
    int LL = 0;
#endif
#ifdef SAVE_CR
    int ccr=-1, ccb=-1;
#endif

#ifdef FLIP
    row1 = (unsigned int *)out + cols * (rows - 1);
    row2 = row1 - cols;
#else
    row1 = (unsigned int *)out;
    row2 = row1 + cols;
#endif
    lum2 = lum + cols;

    for (y=0; y<rows; y+=2) {
	for (x=0; x<cols; x+=2) {
	    int R, G, B;

	    CR = *cr++;
	    CB = *cb++;
#ifdef SAVE_CR
	    if(ccr != CR || ccb != CB) {
#ifdef PRE_CALC
		cb_r = Cb_r_tab[CB];                            
		cr_b = Cr_b_tab[CR];
		cr_cb = Cr_g_tab[CR] + Cb_g_tab[CB];
#else
		cb_r = FIX2(1.40200, CB);
		cr_b = FIX2(1.77200, CR);
		t1 = -FIX2(0.34414, CR);
		t2 = -FIX2(0.71414, CB);
		cb_r >>= BITS;
		cr_b >>= BITS;
		t1 >>= BITS;
		t2 >>= BITS;
		cr_cb = t1 + t2;
#endif
		ccr = CR;
		ccb = CB;
#ifdef SAVE_ALL
		LL = -1;
#endif
	    }
#else
#ifdef PRE_CALC
	    cb_r = Cb_r_tab[CB];
	    cr_b = Cr_b_tab[CR];
	    cr_cb = Cr_g_tab[CR] + Cb_g_tab[CB];
#else
	    cb_r = FIX2(1.40200, CB);
	    cr_b = FIX2(1.77200, CR);
	    t1 = -FIX2(0.34414, CR);
	    t2 = -FIX2(0.71414, CB);
	    cb_r >>= BITS;
	    cr_b >>= BITS;
	    t1 >>= BITS;
	    t2 >>= BITS;
	    cr_cb = t1 + t2;
#endif   /* PRE_CALC */
#endif   /* SAVE_CR  */

#ifdef SAVE_L
#ifdef SAVE_ALL
            L = *lum++;
            if(L != LL) {
                RR_GG_BB
                rgb = r | g | b;
                LL = L;
            }
            *row1++ = rgb;
#else
	    LL = L = *lum++;
	    RR_GG_BB
	    rgb = *row1++ = r | g | b;
#endif
#else
	    L = *lum++;
	    RR_GG_BB
	    *row1++ = r | g | b;
#endif



	    L = *lum++;
#ifdef SAVE_L
	    if(L != LL) {
		RR_GG_BB
		rgb = r | g | b;
		LL = L;
	    } 
	    *row1++ = rgb;
#else
	    RR_GG_BB
            *row1++ = r | g | b;
#endif



	    L = *lum2++;
#ifdef SAVE_L
	    if(L != LL) {
		RR_GG_BB
		rgb = r | g | b;
		LL = L;
	    } 
	    *row2++ = rgb;
#else
	    RR_GG_BB
            *row2++ = r | g | b;
#endif



	    L = *lum2++;
#ifdef SAVE_L
	    if(L != LL) {
		RR_GG_BB
		rgb = r | g | b;
#ifdef SAVE_ALL
		LL = L;
#endif
	    } 
	    *row2++ = rgb;
#else
	    RR_GG_BB
            *row2++ = r | g | b;
#endif



	}
	lum += cols;
	lum2 += cols;
#ifdef FLIP
	row1 -= 3 * cols;
	row2 -= 3 * cols;
#else
        row1 += cols;
        row2 += cols;
#endif
    }
}

