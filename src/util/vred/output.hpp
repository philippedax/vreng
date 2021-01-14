#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "vre.hpp"

void printHeader(const Item& i, ostream& out);
void printTailer(const Item& i, ostream& out);
void pos_ang(const Solid& b, ostream& out);
void pos_ang_sphere(const Solid& b, ostream& out);
void box_props(const Box& b, ostream& out);
void spher_props(const Sphere& b, ostream& out);

#endif
