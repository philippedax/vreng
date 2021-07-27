/*
 * Chargement d'image GIF
 * Origine des sources du decompresseur GIF: Internet
 */

#include "vred.hpp"

#define NEXTBYTE (*ptr++)
#define IMAGESEP 0x2c
#define INTERLACEMASK 0x40
#define COLORMAPMASK 0x80


static const char *id = "GIF87a";
static long BitOffset = 0L;     /* Bit Offset of next code */
static int  XC = 0, YC = 0,     /* Output X and Y coords of current pixel */
            Pass = 0;           /* Used by output routine if interlaced pic */
static unsigned char *RRaster;  /* The raster data stream, unblocked */
static unsigned char *img; /* Destination : uncompressed image (huge copy) */
static unsigned char *image;    /* Destination : uncompressed image */
static int OutCount = 0,        /* Decompressor output 'stack count' */ 
           RWidth, RHeight,     /* screen dimensions */
           Width,Height,        /* image dimensions */
           LeftOfs, TopOfs,     /* image offset */
           BitsPerPixel,        /* Bits per pixel, read from GIF header */
           BytesPerScanline,    /* bytes per scanline in output raster */
           ColorMapSize,        /* number of colors */
           Background,          /* background color */
           CodeSize,            /* Code size, read from GIF header */
           InitCodeSize,        /* Starting code size, used during Clear */
           Code;                /* Value returned by ReadCode */
static long MaxCode;            /* limiting value for current code size */
static int  CClearCode,         /* GIF clear code */
            EOFCode,            /* GIF end-of-information code */
            CurCode,OldCode,InCode, /* Decompressor variables */
            FirstFree,          /* First free code, generated per GIF spec */
            FreeCode,           /* Decompressor, next free slot in hash table */
            FinChar,            /* Decompressor variable */
            BitMask;            /* AND mask for data size */
static long ReadMask;           /* Code AND mask for current code size */
static int Interlace, HasColormap;

/* The hash table used by the decompressor */
static unsigned int Prefix[4096];
static unsigned int Suffix[4096];

/* An output array used by the decompressor */
static unsigned int OutCode[1025];

/* prototypes des sous-fonctions */
void AddToPixel(unsigned char i);


/* fonction de decompression et de chargement d'une image GIF */
unsigned char *load_gif(unsigned char* adr, int filesize,
                        unsigned char rgbpalette[3*256],
                        int& largeur,
                        int& hauteur,
                        int Verbose)
{       
  long nb;
  unsigned char  ch, ch1;
  unsigned char  *ptr;
  unsigned char  *ptr1;
  int   i;
  unsigned long RawCode;
  unsigned long ByteOffset;
  BitOffset=0L;
  XC=YC=0;
  Pass=0;
  OutCount=0;
 
#if 0
  FILE* in=fopen(nom,"rb");
  if (in==NULL) {
    printf("file not found !\n");
    return NULL;
  }
  fseek(in,0,SEEK_END);
  filesize = ftell(in);
  fseek(in,0,SEEK_SET);
  printf("filesize=%ld\n",filesize);
  adr=(unsigned char *)malloc(filesize*2);
  if (adr==NULL) {
    if (Verbose) printf("not enough memory\n");
    return NULL;
  }
#endif

  RRaster=ptr=(unsigned char *)adr;

#if 0
  int taille=fread(adr,1,filesize,in);
  if (taille != filesize) {
    printf("Error while loading the file\n");
    free(adr);
    return NULL;
  }
#endif

  if (strncmp((char *)ptr, id, 3)) {
    if (Verbose) printf("not a GIF file (GIF87a)\n");
    free(adr);
    return NULL;
  }
  ptr += 6;

  /* Get variables from the GIF screen descriptor */
  ch = NEXTBYTE;
  RWidth = ch + 0x100 * NEXTBYTE;     /* screen dimensions... not used.  */
  ch = NEXTBYTE;
  RHeight = ch + 0x100 * NEXTBYTE;
  ch = NEXTBYTE;
  HasColormap = ((ch & COLORMAPMASK) ? 1 : 0);
  BitsPerPixel = (ch & 7) + 1;
  ColorMapSize = 1 << BitsPerPixel;
  BitMask = ColorMapSize - 1;
  Background = NEXTBYTE;              /* background color... not used. */

  if (NEXTBYTE) /* supposed to be NULL */ {
    if (Verbose) printf("corrupt GIF file (bad screen descriptor)\n");
    free(adr);
    return NULL;
  }

  /* Read in global colormap. */
  if (HasColormap) {
    for (i = 0; i < ColorMapSize; i++) {
      rgbpalette[3*i]=NEXTBYTE;
      rgbpalette[3*i+1]=NEXTBYTE;
      rgbpalette[3*i+2]=NEXTBYTE;
    }
  }
  else { /* no colormap in GIF file */
    if (Verbose) printf(" warning! no colortable in this file.  Winging it.\n");
  }

  /* Check for image seperator */
  for (i=0;i<100;i++)
    if (NEXTBYTE == IMAGESEP)
      break;
  if (i==100) {
    if (Verbose) printf("corrupt GIF file (no image separator)\n");
    free(adr);
    return NULL;
  }

  /* Now read in values from the image descriptor */
  ch = NEXTBYTE;
  LeftOfs = ch + 0x100 * NEXTBYTE;
  ch = NEXTBYTE;
  TopOfs = ch + 0x100 * NEXTBYTE;
  ch = NEXTBYTE;
  Width = ch + 0x100 * NEXTBYTE;
  ch = NEXTBYTE;
  Height = ch + 0x100 * NEXTBYTE;
  Interlace = ((NEXTBYTE & INTERLACEMASK) ? 1 : 0);

  if ((Verbose)&&(Interlace)) printf("Interlaced image\n");
  largeur=Width;
  hauteur=Height;
 
  /* Note that I ignore the possible existence of a local color map.
   * I'm told there aren't many files around that use them, and the spec
   * says it's defined for future use.  This could lead to an error
   * reading some files. 
   */
 
  /* Start reading the raster data. First we get the intial code size
   * and compute decompressor constant values, based on this code size.
   */
  CodeSize = NEXTBYTE;
  CClearCode = (1 << CodeSize);
  EOFCode = CClearCode + 1;
  FreeCode = FirstFree = CClearCode + 2;
 
  /* The GIF spec has it that the code size is the code size used to
   * compute the above values is the code size given in the file, but the
   * code size used in compression/decompression is the code size given in
   * the file plus one. (thus the ++).
   */
  CodeSize++;
  InitCodeSize = CodeSize;
  MaxCode = (1L << (long)CodeSize);
  ReadMask = MaxCode - 1L;
 
  /* Read the raster data.  Here we just transpose it from the GIF array
   * to the Raster array, turning it from a series of blocks into one long
   * data stream, which makes life much easier for ReadCode().
   */
  ptr1 = RRaster;
  nb=0;
  do {   
    ch = ch1 = NEXTBYTE;
    while (ch--) {
      nb++;
      *ptr1++ = NEXTBYTE;
    }
    if (nb > filesize) {
      if (Verbose) printf("corrupt GIF file (unblock)\n");
      free(adr);
      return NULL;
    }
  }
  while(ch1) ;

  /* We're done with the raw data now... */
  if (Verbose) printf("Decompressing...\n");
  image=(unsigned char *)malloc(((long)Width)*Height);
  if (image==NULL) {
    if (Verbose) printf("not enough memory\n");
    free(adr);
    return NULL;
  }
  else
    img=(unsigned char *)image;
  BytesPerScanline = Width;

  /* Decompress the file, continuing until you see the GIF EOF code.
   * One obvious enhancement is to add checking for corrupt files here.
   */     

  /* ReadCode() */
  ByteOffset =BitOffset>>3L;
  RawCode = (long)RRaster[ByteOffset] + (0x100L * RRaster[ByteOffset + 1L]);
  if (CodeSize >= 8)
    RawCode += (0x10000L * RRaster[ByteOffset + 2L]);
  RawCode >>= (BitOffset & 7L);
  BitOffset +=(long) CodeSize;
  Code = ((int)(RawCode & ReadMask));
 
  while (Code != EOFCode) {
    /* Clear code sets everything back to its initial value, then reads the
     * immediately subsequent code as uncompressed data.
     */     
     if (Code == CClearCode) {
       CodeSize = InitCodeSize;
       MaxCode = (1L << (long)CodeSize);
       ReadMask = MaxCode - 1L;
       FreeCode = FirstFree;
 
       /* ReadCode() */                
       ByteOffset =BitOffset>>3L;
       RawCode = (long)RRaster[ByteOffset] + (0x100L * RRaster[ByteOffset +1L]);
       if (CodeSize >= 8)
         RawCode += (0x10000L * RRaster[ByteOffset + 2L]);
       RawCode >>= (BitOffset & 7L);
       BitOffset +=(long) CodeSize;
       CurCode = OldCode = Code = ((int)(RawCode & ReadMask));
       FinChar = CurCode & BitMask;
       if (Interlace)
         AddToPixel(FinChar);
       else
         *(img++)=FinChar;
     }
     else {
       /* If not a clear code, then must be data: save same as CurCode and InCode */
       CurCode = InCode = Code;

       /* If greater or equal to FreeCode, not in the hash table yet;
        * repeat the last character decoded
        */
       if (CurCode >= FreeCode) {
         CurCode = OldCode;
         OutCode[OutCount++] = FinChar;
       }
 
       /* Unless this code is raw data, pursue the chain pointed to by CurCode
        * through the hash table to its end; each code in the chain puts its
        * associated output code on the output queue.
        */
       while (CurCode > BitMask) {
         if (OutCount > 1024) {
           if (Verbose) printf("Corrupt GIF file (OutCount)!\n");
           free(adr);
           free(image);
           return NULL;
         }
         OutCode[OutCount++] = Suffix[CurCode];
         CurCode = Prefix[CurCode];
       }

       /* The last code in the chain is treated as raw data. */
       FinChar = CurCode & BitMask;
       OutCode[OutCount++] = FinChar;

       /* Now we put the data out to the Output routine.
        * It's been stacked LIFO, so deal with it that way...
        */
       if (Interlace)
         for (i = OutCount - 1; i >= 0; i--)
           AddToPixel(OutCode[i]);
       else
         for (i = OutCount - 1; i >= 0; i--)
           *(img++)=OutCode[i];
       OutCount = 0;

       /* Build the hash table on-the-fly. No table is stored in the file. */
       Prefix[FreeCode] = OldCode;
       Suffix[FreeCode] = FinChar;
       OldCode = InCode;

       /* Point to the next slot in the table.  If we exceed the current
        * MaxCode value, increment the code size unless it's already 12. If it
        * is, do nothing: the next code decompressed better be CLEAR
        */
       FreeCode++;
       if ((long)FreeCode >= MaxCode) {
         if (CodeSize < 12) {
           CodeSize++;
           MaxCode <<= 1L;
           ReadMask = (1L <<(long)CodeSize) - 1L;
         }
       }
     }

     /* ReadCode() */
     ByteOffset =BitOffset>>3L;
     RawCode = (long)RRaster[ByteOffset] + (0x100L * RRaster[ByteOffset + 1L]);
     if (CodeSize >= 8)
       RawCode += (0x10000L * RRaster[ByteOffset + 2L]);
     RawCode >>= (BitOffset & 7L);
     BitOffset +=(long) CodeSize;
     Code = ((int)(RawCode & ReadMask)); 
   }
   if (Verbose) printf("done.\n");
   free(adr);				
   return image;
}

/* Fetch the next code from the raster data stream.  The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the RRaster array as a BIT Offset.  We compute
 * the byte Offset into the raster array by dividing this by 8, pick up
 * three bytes, compute the bit Offset into our 24-bit chunk, shift to
 * bring the desired code to the bottom, then mask it off and return it. 
 */

/* sous-fonction pour decompresser les images GIF */
/* ReadCode(void) has been 'unfolded' into source */

/* sous-fonction pour enregistrer le no du pixel suivant (image interlacee) */
void AddToPixel(unsigned char i)
{
  *(img++)=i;

  /* Update the X-coordinate, and if it overflows, update the Y-coordinate */
  if (++XC == Width) {
    /* If a non-interlaced picture, just increment YC to the next scan line. 
     * If it's interlaced, deal with the interlace as described in the GIF
     * spec.  Put the decoded scan line out to the screen if we haven't gone
     * past the bottom of it
     */
    XC = 0;
    img-=Width;

    switch (Pass) {
    case 0:
      YC += 8;
      img+=8*Width;
      if (YC >= Height) {
        Pass++;
        YC = 4;
        img=(unsigned char *)image;
        img+=YC*Width;
      }
      break;
    case 1:
      YC += 8;
      img+=8*Width;
      if (YC >= Height) {
        Pass++;
        YC = 2;
        img=(unsigned char *)image;
        img+=YC*Width;
      }
      break;
    case 2:
      YC += 4;
      img+=4*Width;
      if (YC >= Height) {
        Pass++;
        YC = 1;
        img=(unsigned char *)image;
        img+=YC*Width;
      }
      break;
    case 3:
      YC += 2;
      img+=2*Width;
      break;
    }
  }
}

