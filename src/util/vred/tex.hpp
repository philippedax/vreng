#ifndef TEX_HPP
#define TEX_HPP

#include "vred.hpp"
#include "map.hpp"

class Tex;
class Tex {
public:

  Tex(const char* _xp = NULL, const char* _xn = NULL, 
      const char* _yp = NULL, const char* _yn = NULL,
      const char* _zp = NULL, const char* _zn = NULL);
  Tex(const Tex& _tb);
  virtual ~Tex();
  Tex& operator=(const Tex& v);

  void setTex_xp (const char* url);
  void setTex_yp (const char* url);
  void setTex_zp (const char* url); 
  void setTex_xn (const char* url); 
  void setTex_yn (const char* url);
  void setTex_zn (const char* url);
  
  const char*	const getTex_xp() const {return tex_xp;}
  const char*	const getTex_xn() const {return tex_xn;}
  const char*	const getTex_yp() const {return tex_yp;}
  const char*	const getTex_yn() const {return tex_yn;}
  const char*	const getTex_zp() const {return tex_zp;}
  const char*	const getTex_zn() const {return tex_zn;}

  const GLvoid*	const getMap_xp() const;
  const GLvoid*	const getMap_xn() const;
  const GLvoid*	const getMap_yp() const;
  const GLvoid*	const getMap_yn() const;
  const GLvoid*	const getMap_zp() const;
  const GLvoid*	const getMap_zn() const;

  void print() const;

private:
  char *tex_xp,*tex_xn,*tex_yp,*tex_yn,*tex_zp,*tex_zn;
  Map  *map_xp,*map_xn,*map_yp,*map_yn,*map_zp,*map_zn;

};

#endif
