#include "vred.hpp"
#include "item.hpp"

Item::Item(const char * _name)
{
  name = (char *) malloc(strlen(_name) + 1);
  strcpy(name, _name);
}
