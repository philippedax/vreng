#ifndef SOLID_HPP
#define SOLID_HPP

#include "matrix.hpp"
#include "item.hpp"
#include "color.hpp"
#include "bbox.hpp"
#include "bsphere.hpp"
#include "app.hpp"
#include "tex.hpp"

enum ItemId { SPHERE, BOX, CAMERA, GRP, WALL, GATE, EARTH, WEB, BOARD, STEP, HOST, DOC, MIRAGE, THING };
enum Style {WIRED, COLORED, TEXTURED};

class Group;

class  Solid : public Item {

public:
  Solid(const char* _name,
        const Vect& _center = Vect::null,
        const Vect& _orient= Vect::null,
        const Vect& _size = Vect::unit,
        const int _renderStyle = WIRED,
        const Color& _color = Color::white,
        const Tex& _tex = Tex(),
        const App& _app = App());
  virtual ~Solid() {}

  friend ostream& operator<<(ostream& out, const Solid& solid);

  // those accessors deal with *absolute* coordinates 
  virtual Vect getCenter() const;
  virtual void setCenter(const Vect& _center);
  virtual Vect getOrient() const;
  virtual void setOrient(const Vect& _orient);
  virtual Vect getSize() const;
  virtual void setSize(const Vect& _size);
  virtual void updateBB(int recalc) {}
  virtual void setTexture(const Tex& _tex) { tex = _tex; }
  const Tex& getTexture() const { return tex; }
  virtual void setApp(const App& _app) { app = _app; }
  const App& getApp() const { return app; }
  const Bbox& getBoundingBox() const { return myBoundingBox; }
  const Bsphere& getBoundingSphere() const { return myBoundingSphere; }
  int getStyle() const { return renderStyle; }
  virtual void setStyle(const int _renderStyle);
  const Color& getColor() const { return color; }
  virtual void setColor(const Color& _color) { color = _color; }
  Solid& getFather() { return *father; }
  void setFather(Solid* const father) { this->father = father; }
  Group& getRoot();

  //virtual void Add(Solid* const solid) {}
  //virtual void Remove(Solid* const solid) {}
  //virtual int getCard() const { return 1; }
  //virtual Solid& operator[](const int n) { return *this; }

  virtual int collide(Solid& moving) const;
  void Transform(const Vect& translation, const Vect& rotation, const Vect& scaling);

  const Matrix& getModelView() const { return modelView; }
  Matrix getAbsoluteModelView() const;
  void setAbsoluteModelView() { modelView = getAbsoluteModelView(); }
  void changeRef(const Matrix&);

  virtual void render() = 0;

  virtual void print() const { cout << this << " father=" << father << endl; }

public:
  Bbox myBoundingBox;
  Bsphere myBoundingSphere;

protected:
  Vect center, orient, size;
  Matrix modelView;
  int renderStyle;
  Color color;
  Solid* father;
  Tex tex;
  App app;

};

#endif
