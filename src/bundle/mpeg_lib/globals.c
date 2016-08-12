/*
 * Copyright (c) 1994 by Gregory P. Ward.
 * All rights reserved.
 * 
 * This file is part of the MNI front end to the Berkeley MPEG decoder.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE
 * UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER
 * IS ON AN "AS IS" BASIS, AND THE AUTHOR HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.  
 */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : globals.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Definitions of global variables and functions used 
              internally by the MPEG video decoder.  The functions 
              defined in this file are those that are called by the 
              Berkeley decoding engine, but aren't really integral
              to decoding MPEG's, namely functions to read data from
              the stream and to "do something" with each frame after 
              it is decoded.

              Global variables defined here, and declared in globals.h
              (i.e. those used by the decoding engine itself)
                 ditherType
                 input

	      Functions defined here:
	         get_more_data ()
		 DoDitherImage ()
		 ExecuteDisplay ()
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Greg Ward, 94/6/16.
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <config.h>
#include <stdio.h>
#if HAVE_NETINET_IN_H
# include <sys/types.h>   /* to make netinet/in.h happy */
# include <netinet/in.h>  /* just for htonl() (no networking here!) */
#endif
#include <memory.h>
#include "video.h"
#include "proto.h"
#include "globals.h"
#include "my_dmalloc.h"


/* Universal global variables -- those needed by the decoding engine: */

DitherEnum  ditherType = FULL_COLOR_DITHER;
FILE       *input;		/* file pointer to incoming data. */

/* Global variables used only in this source file: */

Boolean EOF_flag = FALSE; /* have we reached end of input stream? */

/* Declarations for global variables shared between this file and 
 * wrapper.c (all are defined in wrapper.c)
 */

extern Boolean   FrameDone;
extern char     *CurrentImage;
extern ImageDesc ImageInfo;
extern int       totNumFrames;

#if !HAVE_NETINET_IN_H
# ifdef WORDS_BIGENDIAN
#  define htonl(l) l
# else
long htonl (long l)
{
   register unsigned char *p=(unsigned char *)&l;
   p[0]^=p[3];p[3]^=p[0];p[0]^=p[3];
   p[1]^=p[2];p[2]^=p[1];p[1]^=p[2];
   return l;
}
# endif
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_more_data
@INPUT      : buf_start
              max_length
              length_ptr
              buf_ptr
@OUTPUT     : 
@RETURNS    : Returns 1 if data read, 0 if EOF, -1 if error.
@DESCRIPTION: Called by correct_underflow in bit parsing utilities to
              read in more data.  Updates input buffer and buffer length.

              Note that this function is called in a rather sneaky,
              implicit manner -- the decoding routines call macros
              in util.h, and those macros call get_more_data.
@METHOD     : 
@GLOBALS    : input - file pointer to the open MPEG stream
@CALLS      : 
@CREATED    : (taken from the original Berkeley code)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int 
get_more_data(buf_start, max_length, length_ptr, buf_ptr)
     unsigned int *buf_start;
     int max_length;
     int *length_ptr;
     unsigned int **buf_ptr;
{
  
  int length, num_read, i, request;
  unsigned char *buffer, *mark;
  unsigned int *lmark;

  if (EOF_flag) return 0;

  length = *length_ptr;
  buffer = (unsigned char *) *buf_ptr;

  if (length > 0) {
    memcpy((unsigned char *) buf_start, buffer, (length*4));
    mark = ((unsigned char *) (buf_start + length));
  }
  else {
    mark = (unsigned char *) buf_start;
    length = 0;
  }

  request = (max_length-length)*4;
  
  num_read = fread( mark, 1, request, input);

  /* Paulo Villegas - 26/1/1993: Correction for 4-byte alignment */
  {
    int num_read_rounded;
    unsigned char *index;
 
    num_read_rounded = 4*(num_read/4);
 
    /* this can happen only if num_read<request; i.e. end of file reached */
    if( num_read_rounded < num_read )
      { 
 	num_read_rounded = 4*( num_read/4+1 );
 	/* fill in with zeros */
 	for( index=mark+num_read; index<mark+num_read_rounded; *(index++)=0 );
 	/* advance to the next 4-byte boundary */
 	num_read = num_read_rounded;
      }
  }
  
  if   (num_read < 0) {
    return -1;
  }
  else if (num_read == 0) {
    *buf_ptr = buf_start;
    
    /* Make 32 bits after end equal to 0 and 32
       bits after that equal to seq end code
       in order to prevent messy data from infinite
       recursion.
    */

    *(buf_start + length) = 0x0;
    *(buf_start + length+1) = SEQ_END_CODE;

    EOF_flag = 1;
    return 0;
  }

  lmark = (unsigned int *) mark;

  num_read = num_read/4;

  for (i=0; i<num_read; i++) {
    *lmark = htonl(*lmark);
    lmark++;
  }

  *buf_ptr = buf_start;
  *length_ptr = length + num_read;
 
  return 1;
}    /* get_more_data () */



#if (ENABLE_DITHER)
/* ----------------------------- MNI Header -----------------------------------
@NAME       : DoDitherImage
@INPUT      : l, Cr, Cb - pointers to the luminance, Cr, and Cb planes
              disp - ?
              h, w - height and width of image (?)
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Called when image needs to be dithered. Selects correct
              dither routine based on info in ditherType.
@METHOD     : 
@GLOBALS    : ditherType
@CALLS      : One of the following, depending on the value of ditherType:
                 HybridDitherImage       (hybrid.c)
		 HybridErrorDitherImage  (hybriderr.c)
		 FS2FastDitherImage      (fs2fast.c)
		 FS2DitherImage          (fs2.c)
		 FS4DitherImage          (fs4.c)
		 Twox2DitherImage        (2x2.c)
		 ColorDitherImage        (24bit.c)
		 GrayDitherImage         (gray.c)
		 OrderedDitherImage      (ordered.c)
		 MonoDitherImage         (mono.c)
		 MonoThresholdImage      (mono.c)
		 Ordered2DitherImage     (ordered2.c)
		 MBOrderedDitherImage    (mb_ordered.c)
@CREATED    : (taken from the original Berkeley code)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
DoDitherImage(l, Cr, Cb, disp, h, w) 
unsigned char *l, *Cr, *Cb, *disp;
int h,w;
{

  switch(ditherType) {
  case HYBRID_DITHER:
    HybridDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case HYBRID2_DITHER:
    HybridErrorDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS2FAST_DITHER:
    FS2FastDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS2_DITHER:
    FS2DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS4_DITHER:
    FS4DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case Twox2_DITHER:
    Twox2DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FULL_COLOR_DITHER:
    ColorDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case GRAY_DITHER:
    GrayDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case NO_DITHER:
    break;

  case ORDERED_DITHER:
    OrderedDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case MONO_DITHER:
    MonoDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case MONO_THRESHOLD:
    MonoThresholdImage(l, Cr, Cb, disp, h, w);
    break;

  case ORDERED2_DITHER:
    Ordered2DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case MBORDERED_DITHER:
    MBOrderedDitherImage(l, Cr, Cb, disp, h, w);


    break;
  }
}   /* DoDitherImage () */

#endif



#ifdef DEBUG
long ImageCheckSum (long Size, char *Data)
{
   long	*LongData = (long *) Data;
   long	 Sum = 0;
   int	 i, LongSize;

   LongSize = Size / sizeof(long);

   for (i = 0; i < LongSize; i++)
      Sum += LongData[i];
  
   return (Sum);      
}
#endif


#ifdef DEBUG
void PrintVidStreamStatus (VidStream *stream)
{
   int	i;

   printf ("  bit offset in stream    = %8d\n", stream->bit_offset);
   printf ("  remaining buffer length = %8d\n", stream->buf_length);
   printf ("  buffer contents:");
   for (i = 0; i < 5; i++) printf (" %08X", stream->buf_start[i]);
   putchar ('\n');

   printf ("  ring buffer of images:");
   for (i = 0; i < RING_BUF_SIZE; i++) printf (" %08X", stream->ring[i]);
   putchar ('\n');

   printf ("     past image = %08X\n", stream->past);
   printf ("   future image = %08X\n", stream->future);
   printf ("  current image = %08X\n", stream->current);
   
}
#else
void PrintVidStreamStatus (VidStream *stream)
{
}
#endif



/* ----------------------------- MNI Header -----------------------------------
@NAME       : ExecuteDisplay
@INPUT      : vid_stream - the currently-being-decoded video stream.
              The frame with which we are concerned is points to by
	      vid_stream->current->display.
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION:Called by the MPEG decoder after each frame has been
             decoded.  In the original (Berkeley) implementation,
             ExecuteDisplay called Xlib routines (XCreateImage,
             XPutImage) to display the just-decoded image immediately,
             but now all it does is update the frame counter, set the
             global variable CurrentImage (shared between wrapper.c
             and this file) to point to the image data, and set 
	     FrameDone to TRUE so that GetMPEGFrame() will know that
	     we have received another finished frame.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Greg Ward, 94/6/16 (almost nothing remains of the 
              ExecuteDisplay() in the original Berkeley source,
	      so I'm claiming creator status.)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void ExecuteDisplay(VidStream *vid_stream)
{
   if (ditherType == NO_DITHER) return;

#ifdef DEBUG
   printf ("ExecuteDisplay:\n");
   PrintVidStreamStatus (vid_stream);
   printf ("  frame %2d: image checksum = %08X\n\n", totNumFrames,
	    ImageCheckSum (vid_stream->h_size*vid_stream->v_size*4, 
			   vid_stream->current->display));
#endif
   
   totNumFrames++;
   
   /* 
    * vid_stream->current->display points to the actual image data;
    * the actual format is described by the ImageDesc structure (which
    * was based on the XImage structure created by ExecuteDisplay() in
    * the original Berkeley code), and can be displayed however 
    * you like.
    *
    * Right now, we just copy the image data to a newly-allocated
    * area, which is pointed to by ImageData[totNumFrames].  Later we
    * will display these saved images as fast as possible.  
    */
   
   CurrentImage = (char *) vid_stream->current->display;
   FrameDone = TRUE;
}  /* ExecuteDisplay () */
