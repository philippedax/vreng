#ifndef VREL_HPP
#define VREL_HPP

#include <iostream>
#include "sysdep.hpp"

extern FILE * yyout;			///< vre file pointer
extern int yylineno;			///< line number in the current file
extern int  comptfichier;
extern FILE *tabyyin[];
extern int  tablinenumber[];
extern char vrelfile[];			///< current input file
extern char tabvrelfile[20][100];

using namespace std;

#endif
