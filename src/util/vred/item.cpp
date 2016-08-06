#include "vred.hpp"
#include "item.hpp"

Item::Item(char * _name) {
  name = (char *) malloc(strlen(_name) + 1);
  (void*) strcpy(name, _name);
}
