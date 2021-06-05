#ifndef SAFE_HPP
#define SAFE_HPP

class Safe {

 public:
  static char* safe_strdup( char **dest, const char *src );
  static char* strcpy( char *dest, const char *src );

};

#endif
