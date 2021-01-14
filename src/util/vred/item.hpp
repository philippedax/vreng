#ifndef ITEM_HPP
#define ITEM_HPP


class Item {

public:
  Item(const char* _name);
  virtual ~Item() { if (name != NULL) { free(name); } }

  char* getName() { return name; }

  virtual int getClassId() const = 0;
  virtual const char* getClassName() const = 0;

private:
  char* name;

};

#endif
