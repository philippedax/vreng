//---------------------------------------------------------------------------
// Vreng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
// aiinit.hpp
//---------------------------------------------------------------------------
#ifndef AIINIT_HPP
#define AIINIT_HPP

#ifdef  __cplusplus
extern "C" {
#endif

#if HAVE_OCAML

#if HAVE_CAML_CALLBACK_H
#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include <caml/alloc.h>	//useful to compile with g++
#endif


/* fonctions de transformation et d'appels */
value recherche_Objet(value mot);
value recherche_Type(value mot);
value recherche_Typegen(value mot,value action);
value recherche_Func(value mot, value act);

/* fonction de deplacement */
value deplacement_to_Objet(value px, value py, value pz, value orientation,value dcptType);
value deplacement_to_Proximite(value mot,value pos);

/* fonction de question de point de vue */
void viewed_objects(value mot);

/* fonctions pour envoyer des informations a l'ecran */
void msg_info(value mot);
void msg_debug(value mot);

/* fonction recevant la requete et */
int read_request(const char *req);

#endif //HAVE_OCAML

/* fonction d'initialisation de l'environnement de Ocaml */
void initOcaml();

#ifdef  __cplusplus
}
#endif

#endif
