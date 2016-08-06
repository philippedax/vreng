//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
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
/* based on examplecode for IJG's JPEG-library found on
 * http://courses.cs.deu.edu.tr/cse566/newpage2.htm
 * Author Fredrik Orderud, 2005
 */
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// openCache
#include "file.hpp"	// openFile
#include "texture.hpp"	// Texture


#if defined(HAVE_JPEG) && defined(HAVE_JPEGLIB_H)
extern "C" {  // stupid JPEG library
#undef HAVE_STDLIB_H	// stupid jpeglib.h
#undef HAVE_STDDEF_H	// stupid jpeglib.h
#include <jpeglib.h>
}
#include <setjmp.h>


static void readJpegHeader(FILE* infile, struct jpeg_decompress_struct& cinfo)
{
  // Step 1: allocate and initialize JPEG decompression object
  jpeg_create_decompress(&cinfo);

  // Step 2: specify data source (eg, a file)
  jpeg_stdio_src(&cinfo, infile);

  // Step 3: read file parameters with jpeg_read_header()
  jpeg_read_header(&cinfo, TRUE);
}

static void readJpegData(Img *img, struct jpeg_decompress_struct& cinfo)
{
  JSAMPARRAY buffer;	// Output row buffer
  int row_stride;	// physical row width in output buffer

  // Step 4: set parameters for decompression

  // Step 5: Start decompressor
  jpeg_start_decompress(&cinfo);

  row_stride = cinfo.output_width * cinfo.output_components;

  // Make a one-row-high sample array that will go away when done with image
  buffer = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  // Step 6: while (scan lines remain to be read)
  trace(DBG_IMG, "readJpegData: width=%d height=%d channel=%d",
                 cinfo.output_width, cinfo.output_height, cinfo.output_components);
  int k = 0;
  int y = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    JSAMPROW prow = buffer[0];

    if (cinfo.output_components == 3) {      // RGB
      for (unsigned int x = 0; x < cinfo.output_width; x++) {
        img->pixmap[k +0] = prow[0];
        img->pixmap[k +1] = prow[1];
        img->pixmap[k +2] = prow[2];
        prow += 3;
        k += 3;
      }
    }
    else if (cinfo.output_components == 1) {  // GREYSCALE
      for (unsigned int x = 0; x < cinfo.output_width; x++) {
        img->pixmap[k] = *prow;
        prow++;
        k++;
      }
    }
    y++;
  }

  // Step 7: Finish decompression
  jpeg_finish_decompress(&cinfo);

  // Step 8: Release JPEG decompression object
  jpeg_destroy_decompress(&cinfo);
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	// "public" fields
  jmp_buf setjmp_buffer;	// for return to caller
};

static void my_error_exit(j_common_ptr cinfo)
{
  my_error_mgr* myerr = (my_error_mgr*) cinfo->err;
  // (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}
#endif //!HAVE_JPEG


Img * Img::loadJPG(void *tex, ImageReader read_func)
{
#if HAVE_JPEG
  Texture *_tex = (Texture *) tex;
  FILE *fp;
  if ((fp = Cache::openCache(_tex->url, _tex->http)) == NULL) return NULL;

  struct jpeg_decompress_struct cinfo;

  /* error handling */
  struct my_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    error("loadJPG: invalidData");
    jpeg_destroy_decompress(&cinfo);
    File::closeFile(fp);
    return NULL;
  }

  /* we read the header */
  readJpegHeader(fp, cinfo);

  /* we read the data */
  if (cinfo.image_width == 0 || cinfo.image_height == 0
      // we can only decode RGB (num=3) or greayscale (num=1)
      || (cinfo.num_components != 3 && cinfo.num_components != 1)) {
    error("loadJPG: num_components=%d", cinfo.num_components);
    jpeg_destroy_decompress(&cinfo);
    File::closeFile(fp);
    return NULL;
  }

  Img *img = new Img(cinfo.image_width, cinfo.image_height, cinfo.num_components);
  if (!img) {
    error("loadJPG: can't new image");
    jpeg_destroy_decompress(&cinfo);
    File::closeFile(fp);
    return NULL;
  }

  readJpegData(img, cinfo);
  File::closeFile(fp);

  return img;
#else
  return NULL;
#endif //!HAVE_JPEG
}

void Img::saveJPG(const char *filename, GLint width, GLint height, GLint quality, const GLubyte *buffer)
{
#if HAVE_JPEG
  JSAMPLE *image_buffer = (JSAMPLE *) buffer;

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *outfile;		/* target file */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  if ((outfile = File::openFile(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename); return;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */
  cinfo.image_width = width;	/* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;	/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  row_stride = width * 3; /* JSAMPLEs per row in image_buffer */
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */
  jpeg_finish_compress(&cinfo);
  File::closeFile(outfile);

  /* Step 7: release JPEG compression object */
  jpeg_destroy_compress(&cinfo);
#endif //!HAVE_JPEG
}
