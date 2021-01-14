#ifndef MAP_HPP
#define MAP_HPP

class Map {

public:
  Map(const char* const URL);
  virtual ~Map();
  const GLubyte* const getData();
  int getLength() const;
  int getHeight() const;

private:
  GLubyte* data;
  int length;
  int height;
  bool error;
  char* url;

public:
  static int defaultTexSize;  

};

#endif

