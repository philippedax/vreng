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

#ifndef DATA_HPP
#define DATA_HPP

/**
 * metaclass data
 */
class Data
{
 public:

  char * nom;

  bool booleen;

  // Destructeur.
  virtual ~Data() { };

  // Methodes get/set.
  virtual Data * get_data() = 0;
  virtual float get_float() = 0;
  virtual void set_float (float) = 0; 
  virtual void set_float (int) = 0;

  // Methodes de nombres.
  virtual Data* plus  (Data *) = 0;
  virtual Data* moins (Data *) = 0;
  virtual Data* mult  (Data *) = 0;
  virtual Data* div   (Data *) = 0;
  virtual Data* mod   (Data *) = 0;
  virtual Data* abs    () = 0;
  virtual Data* oppose () = 0;
  virtual Data* EgalEgal (Data *) = 0;
  virtual Data* Sup      (Data *) = 0;
  virtual Data* Inf      (Data *) = 0;
  virtual Data* Supegal  (Data *) = 0;
  virtual Data* Infegal  (Data *) = 0;
  virtual Data* Different(Data *) = 0;

  // Methodes de test.
  virtual Data* And (Data *) = 0;
  virtual Data* Or  (Data *) = 0;
  virtual Data* Not () = 0;
};

#endif
