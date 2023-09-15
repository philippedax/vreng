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
#include <memory.h>
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "mpeg.h"
#include "dither.h"
#include "video.h"
#include "proto.h"
#include "globals.h"		/* for global variable ditherType */
#include "my_dmalloc.h"

#define NUM_COLORS 256		/* number of entries in colormap */
				/* for gray-scale dithering */

/* Range values for lum, cr, cb. */
int LUM_RANGE = 8;
int CR_RANGE = 4;
int CB_RANGE = 4;

/* Array that remaps color numbers to actual pixel values used by X server. */

unsigned char pixel[256];

/* Arrays holding quantized value ranged for lum, cr, and cb. */

int *lum_values;
int *cr_values;
int *cb_values;


/*
 *--------------------------------------------------------------
 *
 * InitColor --
 *
 *	Initialized lum, cr, and cb quantized range value arrays.
 *
 * Results: 
 *      None.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

static void
InitColor(void)
{
  int i;

  for (i=0; i<LUM_RANGE; i++) {
    lum_values[i] = ((i * 256) / (LUM_RANGE)) + (256/(LUM_RANGE*2));
  }

  for (i=0; i<CR_RANGE; i++) {
    cr_values[i] = ((i * 256) / (CR_RANGE)) + (256/(CR_RANGE*2));
  }

  for (i=0; i<CB_RANGE; i++) {
    cb_values[i] = ((i * 256) / (CB_RANGE)) + (256/(CB_RANGE*2));
  }

}


/*
 *--------------------------------------------------------------
 *
 * ConvertColor --
 *
 *	Given a l, cr, cb tuple, converts it to r,g,b.
 *
 * Results:
 *	r,g,b values returned in pointers passed as parameters.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

static void
ConvertColor (unsigned char l,
              unsigned char cr,
              unsigned char cb,
              unsigned char *r,
              unsigned char *g,
              unsigned char *b)
{
  double fl, fcr, fcb, fr, fg, fb;

  fl = (double) l;
  fcr =  ((double) cr) - 128.0;
  fcb =  ((double) cb) - 128.0;


  fr = fl + (1.40200 * fcb);
  fg = fl - (0.71414 * fcb) - (0.34414 * fcr);
  fb = fl + (1.77200 * fcr);

  if (fr < 0.0) fr = 0.0;
  else if (fr > 255.0) fr = 255.0;

  if (fg < 0.0) fg = 0.0;
  else if (fg > 255.0) fg = 255.0;

  if (fb < 0.0) fb = 0.0;
  else if (fb > 255.0) fb = 255.0;

  *r = (unsigned char) fr;
  *g = (unsigned char) fg;
  *b = (unsigned char) fb;

}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : InitColormap
@INPUT      : (none)
@OUTPUT     : *NumColors - number of entries in the newly-created colormap
              *Map - an array of colourmap entries; each one contains a 
                     red, green, and blue byte-values (0 .. 255).  
		     *Map[i] gives the colour to display a pixel value i.
@RETURNS    : (none)
@DESCRIPTION: Creates a colour map used for most dithering methods 
              (everything except full-colour, gray, and monochrome).
	      The colour map itself is pretty self-explanatory -- a 
	      pixel with value i is to be displayed using the red, green
	      and blue values in *Map[i] after InitColormap() is done.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 95/3/4, Greg Ward: based on InitColorDisplay(), from gdith.c
                                 in the original Berkeley player
@MODIFIED   : 
---------------------------------------------------------------------------- */
#if (ENABLE_DITHER)
static void
InitColormap (int *NumColors, ColormapEntry **Map)
{
   int i, lum_num, cr_num, cb_num;

   *NumColors =  LUM_RANGE*CB_RANGE*CR_RANGE;
   if (ditherType == NO_DITHER) return;
   *Map = (ColormapEntry *) malloc (*NumColors * sizeof (ColormapEntry));

   for (i = 0; i < *NumColors; i++) 
   {
      lum_num = (i / (CR_RANGE*CB_RANGE))%LUM_RANGE;
      cr_num = (i / CB_RANGE)%CR_RANGE;
      cb_num = i % CB_RANGE;
      
      ConvertColor(lum_values[lum_num], cr_values[cr_num], cb_values[cb_num], 
                   (unsigned char*)(&((*Map)[i]).red),
                   (unsigned char*)(&((*Map)[i]).green),
                   (unsigned char*)(&((*Map)[i]).blue));
      pixel[i] = i;
   }
}
#endif


/* ----------------------------- MNI Header -----------------------------------
@NAME       : InitGrayColormap
@INPUT      : (none)
@OUTPUT     : *NumColors - number of entries in the newly-created colormap
              *Map - an array of colourmap entries
@RETURNS    : (none)
@DESCRIPTION: Creates a colour map used for gray-scale dithering, i.e.
              the red/green/blue values are the same for any given
	      pixel value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 95/3/4, Greg Ward: based on InitGrayDisplay(), from gdith.c
                                 in the original Berkeley player
@MODIFIED   : 
---------------------------------------------------------------------------- */
#if (ENABLE_DITHER)
static void
InitGrayColormap (int *NumColors, ColormapEntry **Map)
{
   int  i;
   
   *NumColors =  NUM_COLORS;
   if (ditherType == NO_DITHER) return;
   *Map = (ColormapEntry *) malloc (*NumColors * sizeof (ColormapEntry));

   for (i = 0; i < *NumColors; i++) 
   {
      (*Map)[i].red = (*Map)[i].green = (*Map)[i].blue = i;
      pixel[i] = i;
   }
}
#endif


/* ----------------------------- MNI Header -----------------------------------
@NAME       : InitDither
@INPUT      : Image - pointer to the image descriptor for the current MPEG
@OUTPUT     : Image->ColormapSize, Image->Colormap - the colour map for
              this movie, as initialized by either InitColormap or
	      InitGrayColormap (unless the current dithering scheme
	      is full colour, in which case there is no colour map)
@RETURNS    : (none)
@DESCRIPTION: Does all initialization particular to the type of dithering
              being used.  Basically, sets up the internal data structures
	      needed by the dithering code, and then sets up a colour map
	      needed to display the pixels output by the ditherers.
@METHOD     : 
@GLOBALS    : 
@CALLS      : InitColor     (for most dithering methods)
              InitColormap  (for most dithering methods)
	      InitGrayColormap (for gray-scale dithering)
              Init(..)Dither  (.. = the current dithering method)
@CREATED    : 95/3/3, Greg Ward: taken mostly from main() in the original 
                                 Berkeley player
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
InitDither (ImageDesc *Image)
{
   switch (ditherType)
   {
#if (ENABLE_DITHER)
      case HYBRID_DITHER:
	 InitColor ();
	 InitHybridDither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case HYBRID2_DITHER:
	 InitColor ();
	 InitHybridErrorDither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case FS4_DITHER:
	 InitColor ();
	 InitFS4Dither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case FS2_DITHER:
	 InitColor ();
	 InitFS2Dither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case FS2FAST_DITHER:
	 InitColor ();
	 InitFS2FastDither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case Twox2_DITHER:
	 InitColor ();
	 Init2x2Dither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 PostInit2x2Dither ();
	 break;

      case GRAY_DITHER:
	 InitGrayColormap (&Image->ColormapSize, &Image->Colormap);
	 break;
#endif
      case FULL_COLOR_DITHER:
	 InitColorDither ();
	 Image->ColormapSize = -1;
	 Image->Colormap = NULL;
	 break;

#if (ENABLE_DITHER)
      case NO_DITHER:
	 break;

      case ORDERED_DITHER:
	 InitColor ();
	 InitOrderedDither ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 break;

      case MONO_DITHER:
      case MONO_THRESHOLD:
	 break;

      case ORDERED2_DITHER:
	 InitColor ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 InitOrdered2Dither ();
	 break;

      case MBORDERED_DITHER:
	 InitColor ();
	 InitColormap (&Image->ColormapSize, &Image->Colormap);
	 InitMBOrderedDither ();
	 break;
#endif
   }
}   

