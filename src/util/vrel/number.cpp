/****************************************************************************
 *                                                                          *
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *                                                                          *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
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
 *                                                                          *
 ****************************************************************************/

/******* classe des nombres ***********/

#include "vrel.hpp"

#include "data.hpp"
#include "number.hpp"
#include "test.hpp"
#include "true.hpp"
#include "false.hpp"

/*-------------------------- Operations algebriques ------------------------*/

// Addition.
Nombre* Nombre::plus(Data* autre_nombre)
{
  float resultat;
  Nombre *Nresultat;
  
  resultat = valeur + autre_nombre->get_float();
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Soustraction.
Nombre* Nombre::moins(Data* autre_nombre)
{
  float resultat;
  Nombre *Nresultat;
  
  resultat = valeur - autre_nombre->get_float();
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Multiplication.
Nombre* Nombre::mult(Data* autre_nombre)
{
  float resultat;
  Nombre *Nresultat;
  
  resultat = valeur * autre_nombre->get_float();
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Division.
Nombre* Nombre::div(Data* autre_nombre)
{
  float resultat;
  Nombre *Nresultat;
  
  resultat = valeur / autre_nombre->get_float();
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Modulo.
Nombre* Nombre::mod(Data* autre_nombre)
{
  float resultat;
  Nombre *Nresultat;
  
  resultat = int(valeur) % int(autre_nombre->get_float());
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Valeur absolue.
Nombre* Nombre::abs()
{
  float resultat;
  Nombre *Nresultat;
  
  if (valeur < 0)
    resultat = - valeur;
  else
    resultat = valeur;
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

// Oppose.
Nombre* Nombre::oppose()
{
  float resultat;
  Nombre *Nresultat;

  resultat = - valeur;
  Nresultat = new Nombre(resultat);
  
  return Nresultat;
}

/*------------------------------- Comparaisons ------------------------------*/

// Test d'egalite.
Test* Nombre::EgalEgal(Data *autre_nombre)
{
    if (autre_nombre->get_float() == valeur) 
      return new Vrai();
    else 
      return new Faux();
}

// Test de supériorité stricte.
Test* Nombre::Sup(Data *autre_nombre)
{
  if (autre_nombre->get_float() < valeur) 
    return new Vrai();
  else 
    return new Faux();
}

// Test d'inferiorite stricte.
Test* Nombre::Inf(Data *autre_nombre)
{
  if (autre_nombre->get_float() > valeur) 
    return new Vrai();
  else 
    return new Faux();
}

// Test de supériorité.
Test* Nombre::Supegal(Data *autre_nombre)
{
  if (autre_nombre->get_float() <= valeur) 
    return new Vrai();
  else 
    return new Faux();
}

// Test d'inferiorite.
Test* Nombre::Infegal(Data *autre_nombre)
{
  if (autre_nombre->get_float() >= valeur) 
    return new Vrai();
  else 
    return new Faux();
}

// Test de difference.
Test* Nombre::Different(Data *autre_nombre)
{
  if (autre_nombre->get_float() != valeur) 
    return new Vrai();
  else 
    return new Faux();
}
