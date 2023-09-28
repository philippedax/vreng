/****************************************************************************
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 ****************************************************************************/

#ifndef VREL_HPP
#define VREL_HPP

#include <iostream>
#include "includes.hpp"		///< system includes

extern FILE * yyout;		///< vre file pointer
extern int yylineno;		///< line number in the current file
extern int  comptfichier;
extern FILE *tabyyin[];
extern int  tablinenumber[];
extern char vrelfile[];		///< current input file
extern char tabvrelfile[20][100];

using namespace std;

#endif
