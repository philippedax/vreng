#include "mpeg.h"		/* for typedef's */


/* util.c */
void       correct_underflow (void);
int        next_bits (int num, unsigned int mask);
char      *get_ext_data (void);
int        next_start_code (void);
char      *get_extra_bit_info (void);

/* video.c */
void       init_stats (void);
void       PrintAllStats (void);
double     ReadSysClock (void);
VidStream *NewVidStream (int bufLength);
void       DestroyVidStream (VidStream *astream);
PictImage *NewPictImage (unsigned int width, unsigned int height);
void       DestroyPictImage (PictImage *apictimage);
VidStream *mpegVidRsrc (TimeStamp time_stamp, VidStream *vid_stream);
void       ToggleBFlag (void);
void       TogglePFlag (void);

/* parseblock.c */
void       ParseReconBlock (int n);
void       ParseAwayBlock (int n);

/* motionvector.c */
void       ComputeForwVector (int *recon_right_for_ptr, 
			      int *recon_down_for_ptr);
void       ComputeBackVector (int *recon_right_back_ptr,
			      int *recon_down_back_ptr);

/* decoders.c */
void       init_tables (void);
void       decodeDCTDCSizeLum (unsigned int *value);
void       decodeDCTDCSizeChrom (unsigned int *value);
void       decodeDCTCoeffFirst (unsigned int *run, int *level);
void       decodeDCTCoeffNext (unsigned int *run, int *level);

/* gdith.c */
void       InitDither (ImageDesc *Image);

/* 24bit.c */
void InitColorDither (void);
void ColorDitherImage (unsigned char *lum, unsigned char *cr, unsigned char *cb, unsigned char *out, int rows, int cols);

/* jrevdct.c */
void init_pre_idct (void);
void mpeg_j_rev_dct_sparse (DCTBLOCK data, int pos);
void mpeg_j_rev_dct (DCTBLOCK data);
void mpeg_j_rev_dct_sparse (DCTBLOCK data, int pos);
void mpeg_j_rev_dct (DCTBLOCK data);

#if (ENABLE_DITHER)
/* fs2.c */
void InitFS2Dither (void);
void FS2DitherImage (unsigned char *lum, 
		     unsigned char *cr,
		     unsigned char *cb,
		     unsigned char *disp, 
		     int rows, int cols);

/* fs2fast.c */
void InitFS2FastDither (void);
void FS2FastDitherImage (unsigned char *lum, 
			 unsigned char *cr,
			 unsigned char *cb,
			 unsigned char *out,
			 int h, int w);

/* fs4.c */
void InitFS4Dither (void);
void FS4DitherImage (unsigned char *lum,
		     unsigned char *cr,
		     unsigned char *cb,
		     unsigned char *disp,
		     int rows, int cols);

/* hybrid.c */
void InitHybridDither (void);
void HybridDitherImage (unsigned char *lum,
			unsigned char *cr,
			unsigned char *cb,
			unsigned char *out,
			int h, int w);

/* hybriderr.c */
void InitHybridErrorDither (void);
void HybridErrorDitherImage (unsigned char *lum,
			     unsigned char *cr,
			     unsigned char *cb,
			     unsigned char *out,
			     int h, int w);

/* 2x2.c */
void Init2x2Dither (void);
void RandInit (int h, int w);
void PostInit2x2Dither (void);
void Twox2DitherImage (unsigned char *lum,
		       unsigned char *cr,
		       unsigned char *cb,
		       unsigned char *out,
		       int h, int w);

/* gray.c */
void GrayDitherImage (unsigned char *lum,
		      unsigned char *cr,
		      unsigned char *cb,
		      unsigned char *out,
		      int h, int w);

/* mono.c */
void MonoDitherImage (unsigned char *lum,
                      unsigned char *cr,
                      unsigned char *cb,
                      unsigned char *out,
                      int h, int w);
void MonoThresholdImage (unsigned char *lum,
                         unsigned char *cr,
                         unsigned char *cb,
                         unsigned char *out,
                         int h, int w);

/* ordered.c */
void InitOrderedDither (void);
void OrderedDitherImage (unsigned char *lum,
			 unsigned char *cr,
			 unsigned char *cb,
			 unsigned char *out,
			 int h, int w);

/* ordered2.c */
void InitOrdered2Dither (void);
void Ordered2DitherImage (unsigned char *lum,
			  unsigned char *cr,
			  unsigned char *cb,
			  unsigned char *out,
			  int h, int w);

/* mb_ordered.c */
void InitMBOrderedDither (void);
void MBOrderedDitherImage (unsigned char *lum,
			   unsigned char *cr,
			   unsigned char *cb,
			   unsigned char *out,
			   int h, int w);
void MBOrderedDitherDisplayCopy (VidStream *vid_stream,
				 int mb_addr,
				 int motion_forw,
				 int r_right_forw,
				 int r_down_forw,
				 int motion_back,
				 int r_right_back,
				 int r_down_back,
				 unsigned char *past,
				 unsigned char *future);

#endif

/* globals.c */
int    get_more_data (unsigned int *buf_start,
		      int max_length, 
		      int *length_ptr,
		      unsigned int **buf_ptr);
#if (ENABLE_DITHER)
void   DoDitherImage (unsigned char *l, 
		      unsigned char *Cr,
		      unsigned char *Cb,
		      unsigned char *disp,
		      int h, int w);
#endif
void   ExecuteDisplay (VidStream *vid_stream);
