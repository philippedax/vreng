#ifndef ITEM_H
#define ITEM_H


class Item {

public:
  Item(const char* _name);
  virtual ~Item() {if (name != NULL) {free(name);}}

  char* Name() {return name;}

  virtual int GetClassId() const = 0;
  virtual const char* GetClassName() const = 0;

private:
  char* name;

};

#endif
