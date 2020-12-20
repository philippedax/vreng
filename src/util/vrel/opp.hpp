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

#ifndef OPPOSITION_HPP
#define OPPOSITION_HPP

#include "operation.hpp"
#include "data.hpp"

/**
 * opposee d'une data
 */
class Opposition : public Operation
{
 private:

  // Operande
  Data * op_erande;

 public:

  Opposition(Data *);	///< Constructeur de la classe

  Data* get_data();	///< Methode d'execution de la valeur opposee

  virtual ~Opposition();	///< Destructeur

};

#endif
