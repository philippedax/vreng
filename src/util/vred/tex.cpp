#include "vred.hpp"
#include "safe.hpp"
#include "tex.hpp"


Tex& Tex::operator=(const Tex& tb)
{
  SetTex_xp(tb.tex_xp);
  SetTex_xn(tb.tex_xn);
  SetTex_yp(tb.tex_yp);
  SetTex_yn(tb.tex_yn);
  SetTex_zp(tb.tex_zp);
  SetTex_zn(tb.tex_zn);

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
  
  SetTex_xp(_xp);
  SetTex_xn(_xn);
  SetTex_yp(_yp);
  SetTex_yn(_yn);
  SetTex_zp(_zp);
  SetTex_zn(_zn);
}

Tex::Tex(const Tex& _tb) 
{
  tex_xp = tex_xn = tex_yp = tex_yn = tex_zp = tex_zn = NULL;
  map_xp = map_yp = map_zp = map_xn = map_yn = map_zn = NULL;
  
  SetTex_xp(_tb.tex_xp);
  SetTex_xn(_tb.tex_xn);
  SetTex_yp(_tb.tex_yp);
  SetTex_yn(_tb.tex_yn);
  SetTex_zp(_tb.tex_zp);
  SetTex_zn(_tb.tex_zn);
}

void Tex::SetTex_xp(const char* url) 
{
  Safe::safe_strdup(&tex_xp, url);
  if (map_xp) delete map_xp;
  if (! url) map_xp = 0;
  else map_xp = new Map(tex_xp);
}

void Tex::SetTex_yp(const char* url) 
{
  Safe::safe_strdup(&tex_yp, url);
  if (map_yp) delete map_yp;
  if (! url) map_yp = 0;
  else map_yp = new Map(tex_yp);
}

void Tex::SetTex_zp(const char* url) 
{
  Safe::safe_strdup(&tex_zp, url);
  if (map_zp) delete map_zp;
  if (! url) map_zp = 0;
  else map_zp = new Map(tex_zp);
}

void Tex::SetTex_xn(const char* url) 
{
  Safe::safe_strdup(&tex_xn, url);
  if (map_xn) delete map_xn;
  if (! url) map_xn = 0;
  else map_xn = new Map(tex_xn);
}

void Tex::SetTex_yn(const char* url) 
{
  Safe::safe_strdup(&tex_yn, url);
  if (map_yn) delete map_yn;
  if (! url) map_yn = 0;
  else map_yn = new Map(tex_yn);
}

void Tex::SetTex_zn(const char* url) 
{
  Safe::safe_strdup(&tex_zn, url);
  if (map_zn) delete map_zn;
  if (! url) map_zn = 0;
  else map_zn = new Map(tex_zn);
}

void Tex::Print() const
{
#ifdef VERBOSE
  printf("[%s %s %s %s %s %s]", tex_xp, tex_xn, tex_yp, tex_yn, tex_zp, tex_zn);
#endif
}

const GLvoid* const Tex::GetMap_xp() const
{
  if (! map_xp) return 0;
  else return map_xp->GetData();
}

const GLvoid* const Tex::GetMap_xn() const
{
  if (! map_xn) return 0;
  else return map_xn->GetData();
}

const GLvoid* const Tex::GetMap_yp() const
{
  if (! map_yp) return 0;
  else return map_yp->GetData();
}

const GLvoid* const Tex::GetMap_yn() const
{
  if (! map_yn) return 0;
  else return map_yn->GetData();
}

const GLvoid* const Tex::GetMap_zp() const
{
  if (! map_zp) return 0;
  else return map_zp->GetData();
}

const GLvoid* const Tex::GetMap_zn() const
{
  if (! map_zn) return 0;
  else return map_zn->GetData();
}
