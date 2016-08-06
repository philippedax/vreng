#ifndef MAP_H
#define MAP_H

class Map {

public:
  Map(const char* const URL);
  virtual ~Map();
  const GLubyte* const GetData();
  int GetLength() const;
  int GetHeight() const;

private:
  GLubyte* data;
  int length;
  int height;
  bool error;
  char* URL;

public:
  static int defaultTexSize;  

};

#endif

