/*
 * Copyright (c) 1994 by Gregory P. Ward.
 * All rights reserved.
 * 
 * This file is part of the MNI front end of the Berkeley MPEG decoder.
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
@NAME       : wrapper.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Functions and variables used in the interface between
              user applications and the Berkely MPEG decoder.  This
              file essentially comprises the MNI front end to the
	      Berkeley decoder; it has access to the decoder's global
	      variables (via globals.h), and also has a few of its own;
              however, applications should NOT have access to any of 
              these.  The functions in this file are the only ones to
              which users of the MPEG library have access; all other
              functions in the library are private.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <config.h>
#include <memory.h>
#include <stdlib.h>
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdio.h>
#include <errno.h>
#include "video.h"
#include "proto.h"
#include "util.h"
#include "dither.h"
#include "globals.h"
#include "my_dmalloc.h"

#define BUF_LENGTH 80000


/*
 * Global variables shared between this file and globals.c 
 * (but nowhere else!) 
 */
Boolean	    FrameDone;
char       *CurrentImage;
ImageDesc   ImageInfo;
int         totNumFrames = 0;

/* Global variables used in this file only (but passed to mpegVidRsrc()): */

static VidStream *theStream;


/* Prototypes for functions local to this file: */

void GetMPEGInfo (VidStream *vid_stream, ImageDesc *Info);



/* ----------------------------- MNI Header -----------------------------------
@NAME       : OpenMPEG
@INPUT      : MPEGfile - pointer to a stream opened for reading, positioned
                         at the beginning of an MPEG stream
	      ImgInfo  - pointer to an ImageDesc structure which will have
	                 information such as frame height, width, depth
			 and size (total bytes per frame) put in it.
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Creates and initializes the variables needed to start 
              reading/decoding an MPEG stream.  

              This function is part of the MNI front end to the Berkeley
	      MPEG decoder, adapted from the original Berkeley code.
@METHOD     : 
@GLOBALS    : theStream
              ditherType
	      input
	      LUM_RANGE, CR_RANGE, CB_RANGE
	      lum_values, cr_values, cb_values
@CALLS      : GetMPEGInfo()
              init_tables()
              InitDither()
@CREATED    : 94/6/16, Greg Ward (adapted from main() in the original
              Berkeley source)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Boolean OpenMPEG (FILE *MPEGfile, ImageDesc *ImgInfo)
{
   /* 
    * First reinitialize these globals in case we're opening a second
    * (or more) file - thanks to Loring Holding <art054@cs.brown.edu>
    * for the patch.
    */

   curBits = bitOffset = bufLength = 0;
   EOF_flag = FALSE;


   /* 
    * Create the video stream and read the first chunk to get movie
    * stats -- width and height in particular.
    */

   theStream = NewVidStream(BUF_LENGTH);
   if (theStream == NULL)
   {
      fprintf (stderr, "Error creating video stream\n");
      return (FALSE);
   }
   
   input = MPEGfile;
   if (mpegVidRsrc(0, theStream) == NULL)
   {
      fprintf (stderr, "Error reading video stream, or stream empty\n");
      return (FALSE);
   }

   GetMPEGInfo (theStream, ImgInfo);

   /* Allocate/initialize tables used for dithering (?) */

   lum_values = (int *) malloc(LUM_RANGE*sizeof(int));
   cr_values = (int *) malloc(CR_RANGE*sizeof(int));
   cb_values = (int *) malloc(CB_RANGE*sizeof(int));

   init_tables();		/* initialize decoding stuff */

   InitDither (ImgInfo);	/* initializes dithering structures and */
				/* colormap (i.e. this is where we do */
				/* all dither-specific stuff) */
   return (TRUE);
}     /* OpenMPEG () */


/* ----------------------------- MNI Header -----------------------------------
@NAME       : CloseMPEG
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : (void)
@DESCRIPTION: Frees up some of the memory allocated by OpenMPEG() (some
              is not freed because the Berkeley code doesn't take into 
	      account the fact that somebody might want to, say, free
	      up the memory it allocates... someday, I'll probably have
	      to hack into it to fix that, but not today thanks.)
@METHOD     : 
@GLOBALS    : theStream
              lum_values
	      cr_values
	      cb_values
@CALLS      : 
@CREATED    : 94/6/27, Greg Ward
@MODIFIED   : 
@COMMENTS   : calling CloseMPEG() on a stream that has not reached its
              natural end results in memory leaks.
---------------------------------------------------------------------------- */
void CloseMPEG (void)
{
   /* 
    * It turns out that in order to avoid memory leaks, we have to call
    * DestroyVidStream() here -- but only for the case of closing an MPEG
    * file that has not come to its natural end.  When an MPEG *does* come
    * to its natural end, mpegVidRsrc() takes care of calling
    * DestroyVidStream() (without doing the other necessary cleanup).
    * Thus, if we unconditionally call DestroyVidStream() here, then we'll
    * be twice-freeing some memory, which is even worse than having a leak.
    * So, in the absence of a way to detect whether a stream has already
    * been cleaned up, I'll leave it leaking.  Yecchhhh.  --GPW 1998/07/13
    */

   free (lum_values);
   free (cr_values);
   free (cb_values);
}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : RewindMPEG
@INPUT      : MPEGfile - the input stream where the MPEG's coming from
              Image    - image descriptor (just passed to OpenMPEG ())
@OUTPUT     : (none)
@RETURNS    : (void)
@DESCRIPTION: Resets things so that the caller can start reading the MPEG
              stream from the start again.  Note that the caller does NOT
	      need to call OpenMPEG() again -- after a call to RewindMPEG(),
	      the next call to GetMPEGFrame() will return the first frame
	      of the MPEG.
@METHOD     : 
@GLOBALS    : EOF_flag, curBits, bitOffset, bufLength, bitBuffer, totNumFrames
@CALLS      : 
@CREATED    : 94/7/20, Greg Ward
@MODIFIED   : 
@COMMENTS   : The global variables declared in this function should
              not normally be used by the front end to the MPEG
              decoder.  However, I've chosen to bend the rules for
              just this one function.  N.B.: EOF_flag comes from
              globals.c; curBits, bitOffset, bufLength, and bitBuffer
              from util.c; and totNumFrames is defined in this file.
---------------------------------------------------------------------------- */
Boolean RewindMPEG (FILE *MPEGfile, ImageDesc *Image)
{
   CloseMPEG ();
   rewind (MPEGfile);
   bitBuffer = NULL;
   totNumFrames = 0;

   return (OpenMPEG (MPEGfile, Image));
}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : GetMPEGInfo
@INPUT      : vid_stream - a video stream that is open and has had at 
                           least one call to mpegVidRsrc() performed on it
              Info       - a pointer to an ImageDesc struct in the caller's
	                   space (i.e., the argument to OpenMPEG()) where
			   the image information will be copied
@OUTPUT     : 
@RETURNS    : (void)
@DESCRIPTION: From the video stream, determines the width, height, pixel
              size and depth (in bits) and total image size (in bytes)
	      for an MPEG stream.  Sets the global variable ImageInfo
	      (part of the interface between wrapper.c and globals.c),
	      and then copies that struct to the user application's 
	      space via the Info pointer.
@METHOD     : 
@GLOBALS    : ImageInfo
@CALLS      : 
@CREATED    : 94/6/17, Greg Ward: based on code from ExecuteDisplay() in the
              original Berkeley source
@MODIFIED   : 
---------------------------------------------------------------------------- */
void GetMPEGInfo (VidStream *vid_stream, ImageDesc *Info)
{
   switch (ditherType)
   {
      case Twox2_DITHER:
      {
	 ImageInfo.Height = vid_stream->mb_height * 32;
	 ImageInfo.Width = vid_stream->mb_width * 32; 
	 ImageInfo.Depth = 8;
	 ImageInfo.PixelSize = 8;
	 ImageInfo.BitmapPad = 8;
	 break;
      } 
      case FULL_COLOR_DITHER:
      {
	 ImageInfo.Height = vid_stream->mb_height * 16;
	 ImageInfo.Width = vid_stream->mb_width * 16; 
	 ImageInfo.Depth = 24;
	 ImageInfo.PixelSize = 32;
	 ImageInfo.BitmapPad = 32;
	 break;
      } 
      case MONO_DITHER:
      case MONO_THRESHOLD:
      {
	 ImageInfo.Height = vid_stream->mb_height * 16;
	 ImageInfo.Width = vid_stream->mb_width * 16; 
	 ImageInfo.Depth = 1;
	 ImageInfo.PixelSize = 1;
	 ImageInfo.BitmapPad = 8;
	 break;
      } 
      default:			/* including GRAY_DITHER and ORDERED_DITHER */
      {
	 ImageInfo.Height = vid_stream->mb_height * 16;
	 ImageInfo.Width = vid_stream->mb_width * 16; 
	 ImageInfo.Depth = 8;
	 ImageInfo.PixelSize = 8;
	 ImageInfo.BitmapPad = 8;
	 break;
      }
   }     /* switch on ditherType */

   ImageInfo.Size = (ImageInfo.Height*ImageInfo.Width*ImageInfo.PixelSize) / 8;
   ImageInfo.PictureRate = vid_stream->picture_rate;
   ImageInfo.BitRate = vid_stream->bit_rate;

   memcpy (Info, &ImageInfo, sizeof (ImageDesc));

}     /* GetMPEGInfo () */



/* ----------------------------- MNI Header -----------------------------------
@NAME       : SetMPEGOption
@INPUT      : Option - which option to set
              Value  - what to set it to
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Set an MPEG option.  The options are all assigned intelligent
              defaults when they are created (as global variables), so 
	      calling SetMPEGOption is optional (as you might expect 
	      from the name).  Whether SetMPEGOption() is called before 
	      or after OpenMPEG() is important, but it depends on which
	      option you're setting.  In particular, the dithering type
	      and luminance/chromaticity ranges must be set before
	      OpenMPEG(); but (unless your code is more clever than it
	      needs to be), the colourmap indeces probably won't be set
	      until after OpenMPEG().  RTFM for explanations of what the
	      individual options do.

	      The currently available options are:
                 MPEG_DITHER
		 MPEG_LUM_RANGE
		 MPEG_CR_RANGE
		 MPEG_CB_RANGE
		 MPEG_CMAP_INDEX

@METHOD     : 
@GLOBALS    : Depending on the value of Option, sets one of the MPEG
              decoders global variables:
	         ditherType
		 LUM_RANGE
		 CR_RANGE
		 CB_RANGE
@CALLS      : 
@CREATED    : 94/6/17, Greg Ward.
@MODIFIED   : 95/3/18, GW: added MPEG_CMAP_INDEX option.
---------------------------------------------------------------------------- */
void SetMPEGOption (MPEGOptionEnum Option, int Value)
{
   switch (Option)
   {
      case MPEG_DITHER:    ditherType = (DitherEnum) Value; break;
      case MPEG_LUM_RANGE: LUM_RANGE = Value; break;
      case MPEG_CR_RANGE:  CR_RANGE = Value; break;
      case MPEG_CB_RANGE:  CB_RANGE = Value; break;
      case MPEG_QUIET:  break;
      case MPEG_CMAP_INDEX:
      {
#if (ENABLE_DITHER)
	 int            i;
	 unsigned char *cmap_index;

	 cmap_index = Value;
	 for (i = 0; i < ImageInfo.ColormapSize; i++)
	 {
	    pixel[i] = cmap_index[i];
	 }
#endif
	 break;
      }
   }
}     /* SetMPEGOption () */



/* ----------------------------- MNI Header -----------------------------------
@NAME       : GetMPEGFrame
@INPUT      : 
@OUTPUT     : Frame - the image data, converted to RGB space, is
              copied to the area pointed to by Frame.  There must be
              enough room for the entire image; the ImageDesc
              structure returned by OpenMPEG() will tell you just how
              much memory this is.  The format of the data depends on
              the dithering type; for full colour dithering, there are
              four bytes per pixel: red, green, blue, and unused.
              (Perfect for passing to lrectwrite() or XPutImage().)
@RETURNS    : TRUE if there are still frames left to decode
              FALSE if we have just decoded the last frame
@DESCRIPTION: Part of the MNI front end to the Berkeley MPEG decoder.  
              Essentially reads, decodes, and converts to RGB space the
	      next frame in the MPEG stream opened with OpenMPEG().
@METHOD     : 
@GLOBALS    : theStream
              CurrentImage
@CALLS      : mpegVidRsrc ()
@CREATED    : 94/6/16, Greg Ward
@MODIFIED   : 
---------------------------------------------------------------------------- */
Boolean GetMPEGFrame (char *Frame)
{
   Boolean   MovieDone = FALSE;

   FrameDone = FALSE;
   mpeg_debug (("GetMPEGFrame: starting or continuing movie\n"));

   while (!MovieDone && !FrameDone) /* FrameDone is set by ExecuteDisplay() */
   {
      MovieDone = (mpegVidRsrc (0, theStream) == NULL);
   }

   mpeg_debug (("\nGetMPEGFrame: just received a finished frame: "
                "copying from %08X to %08X\n", CurrentImage, Frame));

   if (!MovieDone)
      memcpy (Frame, CurrentImage, ImageInfo.Size);
   return (!MovieDone);

}     /* GetMPEGFrame () */
