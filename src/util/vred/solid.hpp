#ifndef SOLID_H
#define SOLID_H

#include "matrix.hpp"
#include "item.hpp"
#include "color.hpp"
#include "bbox.hpp"
#include "bsphere.hpp"
#include "app.hpp"
#include "tex.hpp"

enum ItemId {SPHERE, BOX, CAMERA, GROUP, WALL, GATE, EARTH, WEB, BOARD, STEP, HOST, DOC};
enum RenderStyle {WIRED, COLORED, TEXTURED};

class Group;

class  Solid : public Item {

public:
  Solid(const char* _name,
        const Vect& _center = Vect::null,
        const Vect& _orientation = Vect::null,
        const Vect& _size = Vect::unit,
        const int _renderStyle = WIRED,
        const Color& _color = Color::white,
        const Tex& _tex = Tex(),
        const App& _app = App());
  virtual ~Solid() {}

  friend ostream& operator<<(ostream& out, const Solid& solid);

  // those accessors deal with *absolute* coordinates 
  virtual Vect GetCenter() const;
  virtual void SetCenter(const Vect& _center);
  virtual Vect GetOrientation() const;
  virtual void SetOrientation(const Vect& _orientation);
  virtual Vect GetSize() const;
  virtual void SetSize(const Vect& _size);
  virtual void UpdateBoundingVolumes(int recalc) {}
  virtual void SetTexture(const Tex& _tex) { tex = _tex; }
  const Tex& GetTexture() const { return tex; }
  virtual void SetApp(const App& _app) { app = _app; }
  const App& GetApp() const { return app; }
  const Bbox& GetBoundingBox() const {return myBoundingBox;}
  const Bsphere& GetBoundingSphere() const {return myBoundingSphere;}
  int GetRenderStyle() const {return renderStyle;}
  virtual void SetRenderStyle(const int _renderStyle);
  const Color& GetColor() const {return color;}
  virtual void SetColor(const Color& _color) {color = _color;}
  Solid& GetFather() {return *father;}
  void SetFather(Solid* const father) {this->father = father;}
  Group& GetRoot();

  //virtual void Add(Solid* const solid) {}
  //virtual void Remove(Solid* const solid) {}
  //virtual int GetCard() const {return 1;}
  //virtual Solid& operator[](const int n) {return *this;}

  virtual int Collide(Solid& moving) const;
  void Transform(const Vect& translation, const Vect& rotation, const Vect& scaling);

  const Matrix& GetModelView() const {return modelView;}
  Matrix GetAbsoluteModelView() const;
  void SetAbsoluteModelView() {modelView = GetAbsoluteModelView();}
  void ChangeRef(const Matrix&);

  virtual void Render() = 0;

  virtual void Print() const { cout << this << " father=" << father << endl; }

public:
  Bbox myBoundingBox;
  Bsphere myBoundingSphere;

protected:
  Vect center, orientation, size;
  Matrix modelView;
  int renderStyle;
  Color color;
  Solid* father;
  Tex tex;
  App app;

};

#endif
