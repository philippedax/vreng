#ifndef MAP_HPP
#define MAP_HPP

class Map {

public:
  Map(const char* const URL);
  virtual ~Map();
  const GLubyte* const getData();
  int getWidth() const;
  int getHeight() const;

private:
  GLubyte* data;
  int width;
  int height;
  bool error;
  char* url;

public:
  static int defaultTexSize;  

};

#endif

