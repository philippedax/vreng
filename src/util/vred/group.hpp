#ifndef GROUP_H
#define GROUP_H

#include "solid.hpp"

#define GROUP_CHUNK_SIZE 16


class Group: public Solid {

public:
  Group(const char* _name, const int _renderStyle, const Color& _color);
  virtual ~Group();

  int GetClassId() const  {return GROUP;}
  const char* GetClassName() const {return "Group";}

  friend ostream& operator<<(ostream& out, const Group& g);
  void Render();

  Solid& operator[](const int n) {return (*group[n]);}
  Solid* GetElement(int i) {return group[i];}
  void Add(Solid* const solid);
  void Remove(Solid* const solid);
  void Explode();
  int GetCard() const {return card;}
  void SetRenderStyle(const int _renderStyle);
  void SetColor(const Color& _color);
  Solid** GetGroup() {return group;}

  virtual void UpdateBoundingVolumes(int recalc);
  virtual int Collide(Solid& moving) const;

  void Print() const {
    cout << "group ";
    Solid::Print();
    cout << " card=" << card << " containing:" << endl;
    for (int i = 0; i < card; ++i)
      group[i]->Print();
  }

  void PrintFile(const char *fichier) const;
  int LoadFile(const char *file);
  int LoadFile(FILE *file);

private:
  Solid** group;
  int card;

};

#endif
