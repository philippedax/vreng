#include "vred.hpp"
#include "safe.hpp"
#include "tex.hpp"


Tex& Tex::operator=(const Tex& tb)
{
  setTex_xp(tb.tex_xp);
  setTex_xn(tb.tex_xn);
  setTex_yp(tb.tex_yp);
  setTex_yn(tb.tex_yn);
  setTex_zp(tb.tex_zp);
  setTex_zn(tb.tex_zn);

  return *this;
}

Tex::~Tex()
{
  if (tex_xp) free(tex_xp);
  if (map_xp) delete map_xp;
  if (tex_xn) free(tex_xn);
  if (map_xn) delete map_xn;
  if (tex_yp) free(tex_yp);
  if (map_yp) delete map_yp;
  if (tex_yn) free(tex_yn);
  if (map_yn) delete map_yn;
  if (tex_zp) free(tex_zp);
  if (map_zp) delete map_zp;
  if (tex_zn) free(tex_zn);
  if (map_zn) delete map_zn;
}

Tex::Tex(const char* _xp, const char* _xn,const char* _yp, const char* _yn,const char* _zp, const char* _zn) 
{
  tex_xp = tex_xn = tex_yp = tex_yn = tex_zp = tex_zn = NULL;
  map_xp = map_yp = map_zp = map_xn = map_yn = map_zn = NULL;
  
  setTex_xp(_xp);
  setTex_xn(_xn);
  setTex_yp(_yp);
  setTex_yn(_yn);
  setTex_zp(_zp);
  setTex_zn(_zn);
}

Tex::Tex(const Tex& _tb) 
{
  tex_xp = tex_xn = tex_yp = tex_yn = tex_zp = tex_zn = NULL;
  map_xp = map_yp = map_zp = map_xn = map_yn = map_zn = NULL;
  
  setTex_xp(_tb.tex_xp);
  setTex_xn(_tb.tex_xn);
  setTex_yp(_tb.tex_yp);
  setTex_yn(_tb.tex_yn);
  setTex_zp(_tb.tex_zp);
  setTex_zn(_tb.tex_zn);
}

void Tex::setTex_xp(const char* url) 
{
  Safe::safe_strdup(&tex_xp, url);
  if (map_xp) delete map_xp;
  if (! url) map_xp = 0;
  else map_xp = new Map(tex_xp);
}

void Tex::setTex_yp(const char* url) 
{
  Safe::safe_strdup(&tex_yp, url);
  if (map_yp) delete map_yp;
  if (! url) map_yp = 0;
  else map_yp = new Map(tex_yp);
}

void Tex::setTex_zp(const char* url) 
{
  Safe::safe_strdup(&tex_zp, url);
  if (map_zp) delete map_zp;
  if (! url) map_zp = 0;
  else map_zp = new Map(tex_zp);
}

void Tex::setTex_xn(const char* url) 
{
  Safe::safe_strdup(&tex_xn, url);
  if (map_xn) delete map_xn;
  if (! url) map_xn = 0;
  else map_xn = new Map(tex_xn);
}

void Tex::setTex_yn(const char* url) 
{
  Safe::safe_strdup(&tex_yn, url);
  if (map_yn) delete map_yn;
  if (! url) map_yn = 0;
  else map_yn = new Map(tex_yn);
}

void Tex::setTex_zn(const char* url) 
{
  Safe::safe_strdup(&tex_zn, url);
  if (map_zn) delete map_zn;
  if (! url) map_zn = 0;
  else map_zn = new Map(tex_zn);
}

void Tex::print() const
{
#ifdef VERBOSE
  printf("[%s %s %s %s %s %s]", tex_xp, tex_xn, tex_yp, tex_yn, tex_zp, tex_zn);
#endif
}

const GLvoid* const Tex::getMap_xp() const
{
  if (! map_xp) return 0;
  else return map_xp->getData();
}

const GLvoid* const Tex::getMap_xn() const
{
  if (! map_xn) return 0;
  else return map_xn->getData();
}

const GLvoid* const Tex::getMap_yp() const
{
  if (! map_yp) return 0;
  else return map_yp->getData();
}

const GLvoid* const Tex::getMap_yn() const
{
  if (! map_yn) return 0;
  else return map_yn->getData();
}

const GLvoid* const Tex::getMap_zp() const
{
  if (! map_zp) return 0;
  else return map_zp->getData();
}

const GLvoid* const Tex::getMap_zn() const
{
  if (! map_zn) return 0;
  else return map_zn->getData();
}
