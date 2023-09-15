/* ----------------------------- MNI Header -----------------------------------
@NAME       : globals.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Declarations form global variables used internally by the
              Berkeley MPEG decoding engine (except for those specific
	      to image dithering, which are declared in dither.h).
@METHOD     : 
@GLOBALS    : ditherType
              input
              [shmemFlag]
              totNumFrames
@CALLS      : 
@CREATED    : Greg Ward, 94/6/16
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdio.h>
#include "mpeg.h"

#if (!ENABLE_DITHER)
# define DoDitherImage(l,Cr,Cb,disp,h,w) ColorDitherImage(l,Cr,Cb,disp,h,w)
#endif

#ifdef DEBUG
# define mpeg_debug(printf_args) printf (printf_args)
#else
# define mpeg_debug(printf_args)
#endif

extern DitherEnum ditherType;
extern FILE *input;

extern Boolean EOF_flag;
extern unsigned int curBits;
extern int bitOffset, bufLength;
extern unsigned int *bitBuffer;


#endif
