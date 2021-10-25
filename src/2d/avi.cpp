//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
// 
// Copyright (C) 1997-2012 Philippe Dax
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
/*
 *  Some utilities for writing and reading AVI files.
 *  These are not intended to serve for a full blown
 *  AVI handling software (this would be much too complex)
 *  The only intention is to write out MJPEG encoded
 *  AVIs with sound and to be able to read them back again.
 *
 *  Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "vreng.hpp"
#include "avi.hpp"
#include "http.hpp"	// Http
#include "cache.hpp"	// openCache
#include "file.hpp"	// openFile


void Avi::defaults()
{
  fp = NULL;
  url = NULL;
  audio_samples = 0;
  frames = 0;
  n_idx = 0;
  list_depth = 0;
  fps = 0;
  width = 0;
  height = 0;
  a_rate = 0;
  a_bits = 0;
  a_chans = 0;
}

Avi::Avi()
{
  defaults();
}

Avi::Avi(const char *_url)
{
  defaults();
  download(_url);
}

Avi::~Avi()
{
  if (url) delete[] url;
  if (fp) File::closeFile(fp);
}

/*******************************************************************
 *    Utilities for reading video and audio from an AVI File       *
 *******************************************************************/
void Avi::download(const char *_url)
{
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, httpReader, this, 0);
}

const char * Avi::getUrl() const
{
  return (const char *) url;
}

void Avi::httpReader(void *_avi, Http *http)
{     
  Avi *avi = (Avi *) _avi;
  if (! avi) return;
        
  avi->fp = Cache::openCache(avi->getUrl(), http);
  //avi->read_header();
}

FILE * Avi::getFile() const
{
  return fp;
}

void Avi::getInfos(uint16_t *_width, uint16_t *_height, float *_fps) const
{
  if (width) *_width = width;
  if (height) *_height = height;
  if (fps) *_fps = (float) fps;
}

/*
 * Return codes:
 *    0 = OK
 *    1 = reached EOF
 *    2 = not a AVI file
 */
int32_t Avi::read_header()
{
  int32_t tag, n, name;
  int32_t lasttag = 0;
  int32_t vids_strh_seen = 0;
  int32_t vids_strf_seen = 0;
  uint32_t args[64];

  /* Read first 12 bytes and check that this is an AVI file */
  if (fread(&tag , 4, 1, fp) != 1) return 1;
  if (fread(&n   , 4, 1, fp) != 1) return 1;
  if (fread(&name, 4, 1, fp) != 1) return 1;
  if (tag != RIFFtag || name != MAKEFOURCC('A','V','I',' ')) {
    error("avi: not a AVI fp");
    return ERR_NO_AVI;
  }

  /* Read all tags until we encounter a 'LIST xxx movi' tag */
  while (1) {
    if (fread(&tag, 4, 1, fp) != 1) return 1;
    if (fread(&n  , 4, 1, fp) != 1) return 1;
#if WORDS_BIGENDIAN
    char tmp;
    SWAPL(&n, tmp);
#endif
    if (n&1) n++; /* Odd values are padded */
    //error("avi: tag=%08x n=%d", tag, n);
    /* Unless a LIST tag hasn't name movi, it is ignored */
    if (tag == LISTtag) {
      if (fread(&name, 4, 1, fp) != 1) return 1;
      //error("avi: name=%08x", name);
      if (name == MAKEFOURCC('m','o','v','i')) {
        error("avi: movi");
        return 0;
      }
      continue;
    }

    /* Read the arguments of this tag, 256 bytes are sufficient, remainder is skipped */
    if (n <= 256) {
      if (fread(args, n, 1, fp) != 1) return 1;
    }
    else {
      if (fread(args, 256, 1, fp) != 1) return 1;
      if (fseek(fp, n-256, SEEK_CUR)) return 1;
    }

    /* Interpret the tag and its args */
    switch (tag) {
      case strhtag:
        //error("avi: strhtag n=%d", n);
        if (args[0] == MAKEFOURCC('v','i','d','s')) {
          if (args[1] != MAKEFOURCC('M','J','P','G') &&
              args[1] != MAKEFOURCC('m','j','p','g') &&
              args[1] != MAKEFOURCC('M','S','V','C') &&
              args[1] != MAKEFOURCC('m','s','v','c')) {
            error("avi: args1 no mjpg %08x", args[1]);
            return ERR_NO_MJPG;
          }
#if WORDS_BIGENDIAN
          SWAPL(&args[5], tmp);
          SWAPL(&args[6], tmp);
#endif
          if (args[5] != 0) fps = (double)args[6] / (double)args[5];
          //error("avi: fps=%.2f", fps);
          vids_strh_seen = 1;
          lasttag = 1; /* vids */
        }
        else if (args[0] == MAKEFOURCC('a','u','d','s')) lasttag = 2; /* auds */
        break;
      case strftag:
        //error("avi: strftag n=%d", n);
        if (lasttag == 1) {
#if WORDS_BIGENDIAN
          SWAPL(&args[1], tmp);
          SWAPL(&args[2], tmp);
#endif
          width  = args[1];
          height = args[2];
          //error("avi: width=%d height=%d", width, height);
          if (args[4] != MAKEFOURCC('M','J','P','G') &&
              args[4] != MAKEFOURCC('m','j','p','g') &&
              args[4] != MAKEFOURCC('C','R','A','M') &&
              args[4] != MAKEFOURCC('c','r','a','m') &&
              args[4] != MAKEFOURCC('M','S','V','C') &&
              args[4] != MAKEFOURCC('m','s','v','c')) {
            error("avi: args4 no mjpg %08x", args[4]);
            return ERR_NO_MJPG;
          }
          vids_strf_seen = 1;
        }
        else if (lasttag == 2) {
#if 0 //dax
          /* Check audio format (must be PCM) */
          if ((args[0]&0xffff) != 1) {
            error("avi: no PCM tag=%08x", tag);
            break;
            //return ERR_NO_PCM;
          }
#endif
          a_chans = args[0]>>16;
          a_rate  = args[1];
          a_bits  = args[3]>>16;
        }
        break;
      default:
        //error("avi: default tag=%08x n=%d", tag, n);
        lasttag = 0;
        break;
    }
  }
  if (!vids_strh_seen || !vids_strf_seen) return ERR_NO_VIDS;
  return 0;
}

/*
 * Return codes:
 *    1 = video data read
 *    2 = audio data read
 *    0 = reached EOF
 *   -1 = video buffer too small
 *   -2 = audio buffer too small
 */
int Avi::read_data(uint8_t *vidbuf, uint32_t max_vid, int32_t *retlen)
{
  uint32_t tag, n;

  while (fp) {
    /* Read tag and length */
    if (fread(&tag, 4, 1, fp) != 1 ||
        fread(&n  , 4, 1, fp) != 1 ) {
      error("avi: read error");
      fp = NULL;
      return 0;
    }
#if WORDS_BIGENDIAN
    char tmp;
    SWAPL(&n, tmp);
#endif
    if (n&1) n++; /* Odd values are padded */

    //error("avi: read tag=%08x n=%d", tag, n);
    switch (tag) {
      case MAKEFOURCC('0','0','d','b'):
      case MAKEFOURCC('0','0','d','c'):
        if (n > max_vid) {
          error("avi: video buffer too small n=%d", n);
          fp = NULL;
          return -1;
        }
        *retlen = n;
        if (fread(vidbuf, n, 1, fp) != 1) {
          error("avi: eof! n=%d", n);
          fp = NULL;
          return 0;
        }
        return 1;	// OK
        break;
      default:
        if (fseek(fp, n, SEEK_CUR)) {
          error("avi: ignore tag=%08x n=%d", tag, n);
          fp = NULL;
          return 0;
        }
        break;
    }
  }
  return 0;
}

int Avi::read_data(uint8_t *vidbuf, uint32_t max_vid, uint8_t *audbuf, uint32_t max_aud, int32_t *retlen)
{
  uint32_t tag;
  uint32_t n;

  while (fp) {
    /* Read tag and length */
    if (fread(&tag, 4, 1, fp) != 1 ||
        fread(&n, 4, 1, fp) != 1 ) {
      //error("avi: read error");
      fp = NULL;
      return 0;
    }
#if WORDS_BIGENDIAN
    char tmp;
    SWAPL(&n, tmp);
#endif
    if (n&1) n++; /* Odd values are padded */

    switch (tag) {
      case MAKEFOURCC('0','0','d','b'):
      case MAKEFOURCC('0','0','d','c'):
        if (n > (uint32_t) max_vid) {
          error("avi: video buffer too small n=%d", n);
          fp = NULL;
          return -1;
        }
        *retlen = n;
        if (fread(vidbuf, n, 1, fp) != 1) {
          error("avi: eof! n=%d", n);
          fp = NULL;
          return 0;
        }
        return 1;
        break;
      case MAKEFOURCC('0','1','w','b'):
        if (n > (uint32_t) max_aud) return -2;
        *retlen = n;
        if (fread(audbuf, n, 1, fp) != 1) return 0;
        return 2;
        break;
      default:
        if (fseek(fp, n, SEEK_CUR)) {
          error("avi: ignore tag=%08x n=%d", tag, n);
          fp = NULL;
          return 0;
        }
        break;
    }
  }
  return 0;
}

/*******************************************************************
 *    Utilities for writing an AVI File                            *
 *******************************************************************/
int32_t Avi::tell(int fd)
{
  return (fseek(fp, fd, SEEK_CUR));
}

void Avi::out4cc(const char *s)
{
  if (fwrite(s, 4, 1, fp) != 1 ) { perror("write"); return; }
}

void Avi::outlong(int32_t n)
{
  if (fwrite(&n, 4, 1, fp) != 1 ) { perror("write"); return; }
}

void Avi::outshrt(int32_t n)
{
  int16_t s = (int16_t)n;

  if (fwrite(&s, 2, 1, fp) != 1 ) { perror("write"); return; }
}

void Avi::start_list(const char * name)
{
  list_depth++;
  if (list_depth > MAX_LIST_DEPTH) {
    error("MAX_LIST_DEPTH exceeded"); return;
  }

  out4cc("LIST");
  outlong(0);        /* Length of list in bytes, will be written by finish_list */
  list_pos[list_depth-1] = ftell(fp);
  out4cc(name);
  strncpy(list_name[list_depth-1], name,4);
}

void Avi::finish_list(const char * name)
{
  int pos;

  if (list_depth < 1) {
    error("finish_list called without a list started"); return;
  }
  if (strncmp(name, list_name[list_depth-1], 4)) {
    error("finish_list does not match open list"); return;
  }

  pos = ftell(fp);
  fseek(fp, list_pos[list_depth-1]-4, SEEK_SET);
  outlong(pos-list_pos[list_depth-1]);
  fseek(fp, pos, SEEK_SET);

  list_depth--;
}

void Avi::open_output_file(const char *filename)
{
  int i;
  char c[DATASTART-12];

  fp = File::openFile(filename, "wb+");//O_RDWR|O_CREAT|O_TRUNC,0600);
  if (fp == NULL) { error("open"); return; }

  /* Set it nonblocking */
  //i = fcntl(fp, F_SETFL, O_NONBLOCK);
  //if( i<0 ) { perror("fcntl on avi output"); return; }

  /* Write out DATASTART - 12 bytes, the header will go here when we are finished with writing */
  for (i=0; i<DATASTART-12; i++) c[i] = 0;
  fwrite(c, DATASTART-12, 1, fp);
 
  fseek(fp, DATASTART-12, SEEK_SET);
  //int x = ftell(fp);
  start_list("movi");
}

void Avi::write_header(int width, int height, int norm, int audio, int stereo, int size, int rate)
{
  int32_t nbytes, pos, njunk, abytes, achans, sampsize;

  /* write index */
  finish_list("movi");
  fseek(fp, 0, SEEK_END);
  out4cc("idx1");
  outlong(n_idx*4);       /* # of bytes to follow */
  nbytes = fwrite(idx, n_idx*4, 1, fp);
  if (nbytes != 1) { perror("write"); return; }

  /* get the file size */
  fseek(fp, 0, SEEK_END);
  nbytes = ftell(fp);
  fseek(fp, 0, SEEK_SET); /* Rewind */

  if (!audio && audio_samples)
    error("AVI: Audio samples present, but no audio specified");

  /* The RIFF header */
  out4cc("RIFF");
  outlong(nbytes - 8);    /* # of bytes to follow */
  out4cc("AVI ");

  /* Start the header list */
  start_list("hdrl");

  /* The main AVI header */
  out4cc("avih");
  outlong(56);            /* # of bytes to follow */
  if (norm == VIDEO_MODE_PAL)
    outlong(40000);       /* Microseconds per frame */
  else
    outlong(33780);       /* Microseconds per frame, NTSC, might need adjustment */
  outlong(10000000);      /* MaxBytesPerSec, I hope this will never be used */
  outlong(0);             /* PaddingGranularity (whatever that might be) */
  outlong(0x10010);       /* Flags: AVIF_HASINDEX | AVIF_WASCAPTUREFILE */
  //outlong(0x00000);
  outlong(frames);    /* TotalFrames */
  outlong(0);             /* InitialFrames */
  if (audio_samples)
    outlong(2);           /* Streams */
  else
    outlong(1);           /* Streams */
  outlong(0);             /* SuggestedBufferSize */
  outlong(width);         /* Width */
  outlong(height);        /* Height */
                          /* MS calls the following 'reserved': */
  outlong(0);             /* TimeScale:  Unit used to measure time */
  outlong(0);             /* DataRate:   Data rate of playback     */
  outlong(0);             /* StartTime:  Starting time of AVI data */
  outlong(0);             /* DataLength: Size of AVI data chunk    */

  /* Start the video stream list */
  start_list("strl");

  /* The video stream header */
  out4cc("strh");
  outlong(64);            /* # of bytes to follow */
  out4cc("vids");
  outlong(0);		  // RGB 4cc
  outlong(0);             /* Flags */
  outlong(0);             /* Reserved, MS says: wPriority, wLanguage */
  outlong(0);             /* InitialFrames */
  if (norm == VIDEO_MODE_PAL) outlong(40000);      /* Scale */
  else outlong(33780);    /* Scale */
  outlong(1000000);       /* Rate: Rate/Scale == samples/second */
  outlong(0);             /* Start */
  outlong(frames);    /* Length */
  outlong(0);             /* SuggestedBufferSize */
  outlong(-1);            /* Quality */
  outlong(0);             /* SampleSize */
  outlong(0);             /* Frame */
  outlong(0);             /* Frame */
  outlong(0);             /* Frame */
  outlong(0);             /* Frame */

  /* The video stream format */
  out4cc("strf");
  outlong(68);            /* # of bytes to follow */
  outlong(68);            /* Size */
  outlong(width);         /* Width */
  outlong(height);        /* Height */
  outshrt(1); outshrt(24);/* Planes, Count */
  //out4cc ("MJPG");        /* Compression */
  outlong(0); // RGB 4cc
  outlong(width*height);  /* SizeImage ??? */
  outlong(0);             /* ??? */
  outlong(0);             /* ??? */
  outlong(0);             /* ??? */
  outlong(0);             /* ??? */
  outlong(44);            /* ??? */
  outlong(24);            /* ??? */
  outlong(0);             /* ??? */
  outlong(2);             /* ??? */
  outlong(8);             /* ??? */
  outlong(2);             /* ??? */
  outlong(1);             /* ??? */

  /* Finish stream list */
  finish_list("strl");
  if (audio_samples) {
    abytes = (size==16) ? 2 : 1;
    achans = stereo ? 2 : 1;
    sampsize = abytes*achans;

    /* Start the audio stream list ---------------------------------- */
    start_list("strl");

    /* The audio stream header */
    out4cc("strh");
    outlong(64);            /* # of bytes to follow */
    out4cc("auds");
    out4cc("\0\0\0\0");
    outlong(0);             /* Flags */
    outlong(0);             /* Reserved, MS says: wPriority, wLanguage */
    outlong(0);             /* InitialFrames */
    outlong(sampsize);      /* Scale */
    outlong(sampsize*rate); /* Rate: Rate/Scale == samples/second */
    outlong(0);             /* Start */
    outlong(audio_samples/sampsize);   /* Length */
    outlong(0);             /* SuggestedBufferSize */
    outlong(-1);            /* Quality */
    outlong(sampsize);      /* SampleSize */
    outlong(0);             /* Frame */
    outlong(0);             /* Frame */
    outlong(0);             /* Frame */
    outlong(0);             /* Frame */

    /* The audio stream format */
    out4cc("strf");
    outlong(16);            /* # of bytes to follow */
    outshrt(WAVE_FORMAT_PCM);  /* Size */
    outshrt(achans);        /* Number of channels */
    outlong(rate);          /* SamplesPerSec */
    outlong(sampsize*rate); /* AvgBytesPerSec */
    outshrt(sampsize);      /* BlockAlign */
    outshrt(abytes*8);      /* BitsPerSample */

    /* Finish stream list */
    finish_list("strl");
  }

  /* Finish header list */
  finish_list("hdrl");

  /* Calculate the needed amount of junk bytes, output junk */
  pos = ftell(fp);
  njunk = DATASTART - pos - 8 - 12;
  out4cc("JUNK");
  outlong(njunk);
  File::closeFile(fp);
  fp = NULL;
}

void Avi::add_frame(const char *jpeg_data, int length)
{
  fseek(fp, 0, SEEK_END);
  if (n_idx >= MAXIDX) {error("IDX"); return;}
  idx[n_idx  ] = MAKEFOURCC('0','0','d','b');
  idx[n_idx+1] = 0x10; /* RJ: No idea what that means */
  idx[n_idx+2] = ftell(fp);
  idx[n_idx+3] = length;
  n_idx += 4;
  out4cc("00db");
  outlong(length);

  int nw = fwrite(jpeg_data, length, 1, fp);
  if (nw != 1) { perror("write"); return; }
  frames++;
}

void Avi::add_audio(const char *audio_data, int length)
{
  if (n_idx >= MAXIDX) {error("IDX"); return;}
  idx[n_idx  ] = MAKEFOURCC('0','1','w','b');
  idx[n_idx+1] = 0x00; /* RJ: No idea what that means */
  idx[n_idx+2] = ftell(fp);
  idx[n_idx+3] = length;
  n_idx += 4;
  out4cc("01wb");
  outlong(length);

  int nw = fwrite(audio_data, length, 1, fp);
  if (nw != 1) { perror("write"); return; }
  audio_samples += length;
}
