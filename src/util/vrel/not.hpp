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

#ifndef NOT_H
#define NOT_H

#include "logical.hpp"
#include "data.hpp"

/**
 * class not
 */
class Not : public Oplogique
{
 private:
  
  // Operande.
  Data * op_erande;


 public:
  
  // Constructeur de la classe.
  Not(Data *);

  // Methode d'execution du Not.
  Data* get_data ();
  
  virtual ~Not();	///< Destructeur

};

#endif
