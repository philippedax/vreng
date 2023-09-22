#ifndef GROUP_HPP
#define GROUP_HPP

#include "solid.hpp"

#define GROUP_CHUNK_SIZE 16


class Group: public Solid {

public:
  Group(const char* _name, const int _renderStyle, const Color& _color);
  virtual ~Group();

  int getClassId() const  { return GRP; }
  const char* getClassName() const { return "Group"; }

  friend ostream& operator<<(ostream& out, const Group& g);
  void render();

  Solid& operator[](const int n) { return (*group[n]); }
  Solid* getElement(int i) { return group[i]; }
  void add(Solid* const solid);
  void remove(Solid* const solid);
  void explode();
  int getCard() const {return card;}
  void setStyle(const int _renderStyle);
  void setColor(const Color& _color);
  Solid** getGroup() {return group;}

  virtual void updateBB(int recalc);
  virtual int collide(Solid& moving) const;

  void print() const {
    cout << "group ";
    Solid::print();
    cout << " card=" << card << " containing:" << endl;
    for (int i = 0; i < card; ++i)
      group[i]->print();
  }

  void printFile(const char *fichier) const;
  int loadFile(const char *file);
  int loadFile(FILE *file);

private:
  Solid** group;
  int card;

};

#endif
