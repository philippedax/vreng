#include    "FTFace.h"
#include    "FTFont.h"
#include    "FTGlyphContainer.h"
#include    "FTBBox.h"


FTFont::FTFont( const char* fontFilePath)
:   face( fontFilePath),
    useDisplayLists(true),
    glyphList(0)
{
    err = face.Error();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::FTFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   face( pBufferBytes, bufferSizeInBytes),
    glyphList(0)
{
    err = face.Error();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::~FTFont()
{
    delete glyphList;
}


bool FTFont::Attach( const char* fontFilePath)
{
    if( face.Attach( fontFilePath))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error();
        return false;
    }
}


bool FTFont::Attach( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
{
    if( face.Attach( pBufferBytes, bufferSizeInBytes))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error();
        return false;
    }
}


bool FTFont::FaceSize( const unsigned int size, const unsigned int res )
{
    charSize = face.Size( size, res);
    err = face.Error();
    
    if( err != 0)
    {
        return false;
    }
    
    if( glyphList != NULL)
    {
        delete glyphList;
    }
    
    glyphList = new FTGlyphContainer( &face);
    return true;
}


unsigned int FTFont::FaceSize() const
{
    return charSize.CharSize();
}


bool FTFont::CharMap( FT_Encoding encoding)
{
    bool result = glyphList->CharMap( encoding);
    err = glyphList->Error();
    return result;
}


unsigned int FTFont::CharMapCount()
{
    return face.CharMapCount();
}


FT_Encoding* FTFont::CharMapList()
{
    return face.CharMapList();
}


void FTFont::UseDisplayList( bool useList)
{
    useDisplayLists = useList;
}

float FTFont::Ascender() const
{
    return charSize.Ascender();
}


float FTFont::Descender() const
{
    return charSize.Descender();
}

float FTFont::LineHeight() const
{
    return charSize.Height();
}

void FTFont::BBox( const char* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const unsigned char* c = (unsigned char*)string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }
                
        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}


void FTFont::BBox( const wchar_t* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const wchar_t* c = string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }
        
        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}

// ELC: 02apr07: returns the width of 'len' chars if len is >= 0 ; 
// ELC: returns the width of the entire string if len < 0
//
float FTFont::Advance( const wchar_t* string, int len)
{
    const wchar_t* c = string;
    float width = 0.0f;

    if (len < 0) { 
      for( ; *c; ++c) {
        if(CheckGlyph( *c)) {
          width += glyphList->Advance( *c, *(c + 1));
        }
      }
    }
    else {
      for(int k = 0; k < len && *c; ++c, ++k) {
        if(CheckGlyph( *c)) {
          width += glyphList->Advance( *c, *(c + 1));
        }
      }
    }
    return width;
}

// ELC: 02apr07: returns the width of 'len' chars if len is >= 0 ; 
// ELC: returns the width of the entire string if len < 0
//
float FTFont::Advance( const char* string, int len)
{
    const unsigned char* c = (unsigned char*)string;
    float width = 0.0f;

    if (len < 0) { 
      for( ; *c; ++c) {
        if(CheckGlyph( *c)) {
          width += glyphList->Advance( *c, *(c + 1));
        }
      }
    }
    else {
      for(int k = 0; k < len && *c; ++c, ++k) {
        if(CheckGlyph( *c)) {
          width += glyphList->Advance( *c, *(c + 1));
        }
      }
    } 
    return width;
}

// =============================================================================
/* ELC:
* - renders 'len' chars if len >= 0 ; renders until the end of the string if len < 0
* - we cannot use glTranslate() because glClipPlane was called before Render()
*   => absolute (x,y) coordinates are given as an argument to Render()
*/
void FTFont::Render( const char* string, int len, float x, float y)
{
  const unsigned char* c = (unsigned char*)string;
  //EX: pen.X(0); pen.Y(0);
  pen.X(x); pen.Y(y);   // pen contains the absolute x,y position

  if (len < 0) {   // render until the end of the string 
    for( ; *c; ++c) {
      if(CheckGlyph( *c)) {
        //EX: pen = glyphList->Render( *c, *(c + 1), pen);
        pen += glyphList->Render( *c, *(c + 1), pen);  // add advance to the absolute pos
      }
    }
  }
  else {  // renders 'len' chars
    for(int k = 0; k < len && *c; ++c, ++k) {
      if(CheckGlyph( *c)) {
        //EX: pen = glyphList->Render( *c, *(c + 1), pen);
        pen += glyphList->Render( *c, *(c + 1), pen);
      }
    }
  }
}

// see notes above.
void FTFont::Render( const wchar_t* string, int len, float x, float y)
{
  const wchar_t* c = string;
  //EX: pen.X(0); pen.Y(0);
  pen.X(x); pen.Y(y);
  
  if (len < 0) { 
    for( ; *c; ++c) {
      if(CheckGlyph( *c)) {
        // pen = glyphList->Render( *c, *(c + 1), pen);
        pen += glyphList->Render( *c, *(c + 1), pen);
      }
    }
  }
  else {
    for(int k = 0; k < len && *c; ++c, ++k) {
      if(CheckGlyph( *c)) {
        //pen = glyphList->Render( *c, *(c + 1), pen);
        pen += glyphList->Render( *c, *(c + 1), pen);
      }
    }          
  }    
}

// =============================================================================

bool FTFont::CheckGlyph( const unsigned int characterCode)
{
    if( NULL == glyphList->Glyph( characterCode))
    {
        unsigned int glyphIndex = glyphList->FontIndex( characterCode);
        FTGlyph* tempGlyph = MakeGlyph( glyphIndex);
        if( NULL == tempGlyph)
        {
            if( 0 == err)
            {
                err = 0x13;
            }
            
            return false;
        }
        glyphList->Add( tempGlyph, characterCode);
    }
    
    return true;
}

