AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = COPYING.txt HISTORY.txt README.txt LICENSE.txt

FTGL_HDRS = FTGL.h FTBBox.h FTGLBitmapFont.h FTList.h FTBitmapGlyph.h FTGLExtrdFont.h FTOutlineGlyph.h FTCharToGlyphIndexMap.h FTGLOutlineFont.h FTPixmapGlyph.h FTCharmap.h FTGLPixmapFont.h FTPoint.h FTContour.h FTGLPolygonFont.h FTPolyGlyph.h FTExtrdGlyph.h FTGLTextureFont.h FTSize.h FTFace.h FTGlyph.h FTTextureGlyph.h FTFont.h FTGlyphContainer.h FTVector.h FTLibrary.h FTVectoriser.h

FTGL_SRCS = FTBitmapGlyph.cpp FTGLBitmapFont.cpp FTGlyph.cpp FTPolyGlyph.cpp FTCharmap.cpp FTGLExtrdFont.cpp FTGlyphContainer.cpp FTSize.cpp FTContour.cpp FTGLOutlineFont.cpp FTLibrary.cpp FTTextureGlyph.cpp FTExtrdGlyph.cpp FTGLPixmapFont.cpp FTOutlineGlyph.cpp FTVectoriser.cpp FTFace.cpp FTGLPolygonFont.cpp FTPixmapGlyph.cpp FTFont.cpp FTGLTextureFont.cpp FTPoint.cpp $(FTGL_HDRS)

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = @GL_CFLAGS@ -I$(SRCDIR)/bundle/ftgl

if BUNDLE_FTGL
noinst_LIBRARIES = libftgl.a
libftgl_a_SOURCES = $(FTGL_SRCS)
endif
