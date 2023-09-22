/******************************************************************************
 *                               VREL COMPONENTS                              *
 *                                                                            *
 *                           Copyright (C) 2000                               *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin      *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *****************************************************************************/

#ifndef MUL_HPP
#define MUL_HPP

#include "operation.hpp"
#include "data.hpp"

/**
 * multiplication of variables
 */
class Multiplication : public Operation
{
 private:

  Data * op_gauche,* op_droite;		///< Operandes gauche et droite

 public:
  
  Multiplication(Data *, Data *);	///< Constructeur de la classe
  Data* get_data();			///< Methode d'execution de la division

  virtual ~Multiplication();		///< Destructeur
};

#endif
