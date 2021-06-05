/* ---------------------------------------------------------------------
 
   Purpose
   -------
   During the parsing of tree 2 in the process of the translation of
   the language 'vrl' into configuration files we create objects of
   classes that are implemented in this file. Due to polymorphism one
   has only to call the 'Write' method of each object in the end and
   the called object is writing its parameters to the configuration 
   file.
 
   Authors
   -------
   Vincent GUILLOPE   VG
   Holger KRAUTH      HK

   History
   -------
   when       who   what
   10-Dec-99  VG    Creation.
   13-Dec-99  HK    Continuing implementation. Beginning at 'Gate'.
   15-Dec-99  VG    Corrections

   ---------------------------------------------------------------------
*/

#include "global.hpp"
#include "vre.hpp"

extern FILE *yyout;

char *rognerGuillemets(char *);

#define DEB


//************************* CLASSES D'AFFICHAGE ************************

//************************** TOKENLIST *********************************
TokenListe::TokenListe()
{
  // Liste des types VRENG qu'on peut reconnaitre avec 'vrl'

  const int N_ARRAY = 13;

  array = (Token **)malloc(N_ARRAY * sizeof(Token *));

  // For closer information about the VRENG types please refer to
  // the documentation that should be available from M. Bellot.
  // HK
  array[0] = new Wall;
  array[1] = new Gate;
  array[2] = new Web;
  array[3] = new Board;
  array[4] = new Host;
  array[5] = new Doc;
  array[6] = new Cauldron;
  array[7] = new Step;
  array[8] = new Aoi;
  array[9] = new Door;
  array[10] = new Button;
  array[11] = new Walls;
  array[12] = new Mirage;

  length = N_ARRAY;
}

Token *TokenListe::readTable(char *nom)
{
  Token *myTok = (Token *)NULL;

  for (int i = 0 ; i < length ; i++){
    if ((myTok = array[i]->readTable(nom)) != NULL)
      break;
  }
  return myTok;
}


/** WALL */
Token * Wall::readTable(char *name)
{
  if (strcasecmp(name, "wall") != 0)
    return NULL;
  return new Wall();
}

void Wall::write2CFG(Table *data)
{
#ifdef DEB
  // Recuperer les elements de la structure
  int k = 0;
  char *nomm;
  while ((data->getEntree(k)) != NULL) {
    nomm = data->getEntreeNom(k);
    printf("------>  %s\n", nomm);
    k++; 
  }
#endif

  // en-tete
  fprintf(yyout, "<wall");

  // arguments obligatoires
  const int N_ATTROBL = 7;
  char *x, *y, *z, *a, *lx, *ly, *lz;

  x = data->getEntreeValeur(0);
  y = data->getEntreeValeur(1);
  z = data->getEntreeValeur(2);
  a = data->getEntreeValeur(3);
  lx = data->getEntreeValeur(4);
  ly = data->getEntreeValeur(5);
  lz = data->getEntreeValeur(6);
  
  fprintf(yyout, " pos=\"%s %s %s %s\">", x, y, z, a);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // arguments facultatifs
  const int N_ATTRFALC = 5;

  AttributFacultatif **attrFalc;
  attrFalc = (AttributFacultatif **)malloc(N_ATTRFALC * sizeof(AttributFacultatif *));

  attrFalc[0] = new BoxTextures();
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  int i = N_ATTROBL;
  char *nom;
  while ((data->getEntree(i)) != NULL) {
    nom = data->getEntreeNom(i);

    for (int j = 0 ; j < N_ATTRFALC ; j++){
      if (attrFalc[j]->read(nom, data->getEntreeValeur(i)) != 0)
	break;
      }
    i++; 
  }
  
  for (int j = 0 ; j < N_ATTRFALC ; j++)
      attrFalc[j]->print(yyout);
  
  fprintf(yyout, " /></wall>\n");
  free(attrFalc);
}

/** GATE */
Token * Gate::readTable (char *name)
{
  if (strcasecmp(name, "gate") != 0)  // strcmp return 0 on match.
    return NULL;
  return new Gate;
}

void Gate::write2CFG (Table *data)
{
  const int cNrofOblGateAttr = 9;  // MUST attributes
  const int cNrofOptGateAttr = 5;  // CAN  attributes
  // MUST
  char * xPos, * yPos, * zPos, * angle;// get attr by char *
  char * lx,   * ly,   * lz;
  char * v2where, * ipMulti;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptGateAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  // others
  int j=0;

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<gate");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6);
  v2where  = rognerGuillemets(data->getEntreeValeur(7));  
  // URL of file of new world to where the gate 'beams' you
  ipMulti = rognerGuillemets(data->getEntreeValeur(8));
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle);
  fprintf(yyout, " world=\"%s\">", v2where);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblGateAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (j = 0 ; j < cNrofOptGateAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 
  // print all  
  for (j = 0 ; j < cNrofOptGateAttr ; j++) {
      attrFalc[j]->print(yyout);
  } 
  fprintf(yyout, " />"); 

  // terminate line
  fprintf(yyout, "</gate>\n"); 
  free(attrFalc);
}

/** WEB */
Token * Web::readTable(char *name)
{
  if (strcasecmp(name, "web") != 0)
    return NULL;
  return new Web;
}

void Web::write2CFG(Table *data)
{
  const int cNrofOblAttr = 8;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  char *url2load;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<web");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6);
  url2load= rognerGuillemets(data->getEntreeValeur(7));  
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle); 
  fprintf(yyout, " url=\"%s\">", url2load);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  fprintf(yyout, " />");   

  // terminate line
  fprintf(yyout, "</web>\n");   
  free(attrFalc);
}

/** BOARD */
Token *Board::readTable(char *name)
{
  if (strcasecmp(name, "board") != 0)
    return NULL;
  return new Board;
}

void Board::write2CFG(Table *data)
{
  const int cNrofOblAttr = 7;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<board");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6);
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\">", xPos, yPos, zPos, angle); 
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  
  // terminate line
  fprintf(yyout, " /></board>\n");   
  free(attrFalc);  
}

/** HOST */
Token *Host::readTable(char *name)
{
  if (strcasecmp(name, "host") != 0)
    return NULL;
  return new Host;
}

void Host::write2CFG(Table *data)
{
  const int cNrofOblAttr = 8;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  char *telnet2open;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<host");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6);
  telnet2open = rognerGuillemets(data->getEntreeValeur(7));
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle); 
  fprintf(yyout, " host=\"%s\">", telnet2open);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  fprintf(yyout, " />");     
  
  // terminate line
  fprintf(yyout, "</host>\n");     
  free(attrFalc);  
}

/** DOC */
Token *Doc::readTable(char *name)
{
  if (strcasecmp(name, "doc") != 0)
    return NULL;
  return new Doc;
}

void Doc::write2CFG(Table *data)
{
  const int cNrofOblAttr = 8;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  char *doc2open;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<doc");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6); 
  doc2open = rognerGuillemets(data->getEntreeValeur(7));
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle); 
  fprintf(yyout, " url=\"%s\">", doc2open);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  fprintf(yyout, " />");     
 
  // terminate line
  fprintf(yyout, "</doc>\n");     
  free(attrFalc);  
}

/** CAULDRON */
Token *Cauldron::readTable(char *name)
{
  if (strcasecmp(name, "cauldron") != 0)
    return NULL;
  return new Cauldron;
}

void Cauldron::write2CFG(Table *data)
{
  const int cNrofOblAttr = 5;  // MUST attributes
  const int cNrofOptAttr = 4;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *radius, *radius2;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new Apparence((char *)"diffuse"); // initialize table
  attrFalc[1] = new Apparence((char *)"ambient");
  attrFalc[2] = new Apparence((char *)"specular");
  attrFalc[3] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<cauldron");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = (char*) malloc (2*sizeof(char));
  *angle  = '0';
  *(angle+1) = '\0';
              // terminate with 0 cause it is read as a string later 
  radius  = data->getEntreeValeur(3);
  radius2 = data->getEntreeValeur(4);
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\">", xPos, yPos, zPos, angle); 
  fprintf(yyout, " <solid shape=\"torus\" r=\"%s\" radius2=\"%s\"", radius, radius2);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  
  // terminate line
  fprintf(yyout, " /></cauldron>\n");    
  free(attrFalc);     
}

/** STEP */
Token *Step::readTable(char *name)
{
  if (strcasecmp(name, "step") != 0)
    return NULL;
  return new Step;
}

void Step::write2CFG(Table *data)
{
  const int cNrofOblAttr = 7;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<step");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6); 
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\">", xPos, yPos, zPos, angle); 
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  
  // terminate line
  fprintf(yyout, " /></step>\n");      
  free(attrFalc);   
}

/** AOI */
Token *Aoi::readTable(char *name)
{
  if (strcasecmp(name, "aoi") != 0)
    return NULL;
  return new Aoi;
}

void Aoi::write2CFG(Table *data)
{
  const int cNrofOblAttr = 8;  // MUST attributes
  const int cNrofOptAttr = 4;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *lx,   *ly,   *lz;
  char *ipMulti;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<aoi");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  angle   = data->getEntreeValeur(3);
  lx      = data->getEntreeValeur(4);
  ly      = data->getEntreeValeur(5);
  lz      = data->getEntreeValeur(6); 
  ipMulti = rognerGuillemets(data->getEntreeValeur(7)); 
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle); 
  fprintf(yyout, " channel=\"%s\">", ipMulti);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  fprintf(yyout, " />");    
  
  // terminate line
  fprintf(yyout, "</aoi>\n");    
  free(attrFalc);
}

/** DOOR */
Token *Door::readTable(char *name)
{
  if (strcasecmp(name, "door") != 0)
    return NULL;
  return new Door;
}

void Door::write2CFG(Table *data)
{
  const int cNrofOblAttr = 12;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *doorName;
  char *xPos, *yPos, *zPos, *angle;// get attr by char *
  char *angOpen, *angClos, *speed, *xOffset;
  char *lx,   *ly,   *lz;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<door");

  // Treatment of MUST parameters
  doorName= rognerGuillemets(data->getEntreeValeur(0));
  xPos    = data->getEntreeValeur(1);
  yPos    = data->getEntreeValeur(2);
  zPos    = data->getEntreeValeur(3);
  angle   = data->getEntreeValeur(4);
  angOpen = data->getEntreeValeur(5);
  angClos = data->getEntreeValeur(6);
  speed   = data->getEntreeValeur(7); 
  xOffset = data->getEntreeValeur(8);
  lx      = data->getEntreeValeur(9);
  ly      = data->getEntreeValeur(10);
  lz      = data->getEntreeValeur(11); 

  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " name=\"%s\"", doorName); 
  fprintf(yyout, " pos=\"%s %s %s %s\"", xPos, yPos, zPos, angle); 
  fprintf(yyout, " open=\"%s\" close=\"%s\" speed=\"%s\">", 
                angOpen, angClos, speed); 
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  fprintf(yyout, " />");     
  
  // terminate line
  fprintf(yyout, "</door>\n");     
  free(attrFalc); 
}

/** NO LIFT NEEDED AS THERE IS NO DESCRIPTION OF LIFT Available */


/** BUTTON */
//TODO
Token *Button::readTable(char *name)
{
  if (strcasecmp(name, "button") != 0)
    return NULL;
  return new Button;
}

void Button::write2CFG(Table *data)
{
}

/** WALLS */
Token *Walls::readTable(char *name)
{
  if (strcasecmp(name, "walls") != 0)
    return NULL;
  return new Walls;
}

void Walls::write2CFG(Table *data)
{
  char * urlOfConfigFile;
  urlOfConfigFile = data->getEntreeValeur(0);
  fprintf(yyout, "<walls ");
  fprintf(yyout, "url=\"%s\">", rognerGuillemets(urlOfConfigFile));
  fprintf(yyout, "</walls>\n");
}

/** MIRAGE*/
Token *Mirage::readTable(char *name)
{
  if (strcasecmp(name, "mirage") != 0)
    return NULL;
  return new Mirage;
}

void Mirage::write2CFG(Table *data)
{
  const int cNrofOblAttr = 8;  // MUST attributes
  const int cNrofOptAttr = 5;  // CAN  attributes
  // MUST
  char *xPos, *yPos, *zPos, *aZ, *aX;// get attr by char *
  char *lx,   *ly,   *lz;
  // CAN
  AttributFacultatif ** attrFalc; // table to hunt through later
  attrFalc = (AttributFacultatif **) 
             malloc(cNrofOptAttr * sizeof(AttributFacultatif *));
  char *tmpFacAttrName; 
  char *tmpFacAttrVal; 

  attrFalc[0] = new BoxTextures(); // initialize table
  attrFalc[1] = new Apparence((char *)"diffuse");
  attrFalc[2] = new Apparence((char *)"ambient");
  attrFalc[3] = new Apparence((char *)"specular");
  attrFalc[4] = new Apparence((char *)"shininess");

  // print key word to tell VRENG parser what is coming.
  fprintf(yyout, "<mirage");

  // Treatment of MUST parameters
  xPos    = data->getEntreeValeur(0);
  yPos    = data->getEntreeValeur(1);
  zPos    = data->getEntreeValeur(2);
  aZ      = data->getEntreeValeur(3);
  aX      = data->getEntreeValeur(4);
  lx      = data->getEntreeValeur(5);
  ly      = data->getEntreeValeur(6);
  lz      = data->getEntreeValeur(7);
  
  // printing to file in proper format: See VRENG doc.
  fprintf(yyout, " pos=\"%s %s %s %s %s\">", xPos, yPos, zPos, aZ, aX);
  fprintf(yyout, " <solid shape=\"box\" dim=\"%s %s %s\"", lx, ly, lz);

  // Treatment of CAN parameters
  // point on first CAN. MUST are numbered 0..cNrofOblGateAttr-1 !
  int i = cNrofOblAttr;

  while ((tmpFacAttrName = data->getEntreeNom(i)) != NULL) {
  // As long as there is a variable(name) found go through the attrFalc table 
    for (int j = 0 ; j < cNrofOptAttr ; j++) {
      tmpFacAttrVal = data->getEntreeValeur(i);
      if (attrFalc[j]->read(tmpFacAttrName, tmpFacAttrVal) != 0)
	break; // found it - no further search needed.
    } 
    i++; 
  } 

  // print all  
  for (int j = 0 ; j < cNrofOptAttr ; j++)
      attrFalc[j]->print(yyout);
  
  // terminate line
  fprintf(yyout, " /></mirage>\n");   
  free(attrFalc);  
}

//************************* ATTRIBUTS FACULTATIFS **********************

//** Apparence APP **
Apparence::Apparence(char *type)
{
  nom = strdup(type);
  v = (char **)malloc(4 * sizeof(char*));
  v[1] = v[2] = v[3] = NULL; // terminate the strings with \0 !   
  nrOfReadParams = 0;
}

int Apparence::read(char *var, char *val)
{  
  int paramNr = 0;
  int nlgth = strlen(nom);
  
  // printf("**** %s *** %s ****\n", nom, var);

  if (strncmp(nom, var, nlgth) == 0){
    char nextCharAfterName = var[nlgth];
    switch(nextCharAfterName) {
    case '1': paramNr = 1; break; // eg. shininess1
    case 'R': paramNr = 1; break; // eg. ambientRED
    case '2': paramNr = 2; break; // eg. shininess2
    case 'G': paramNr = 2; break; // eg. ambientGREEN
    case '3': paramNr = 3; break; // eg. shininess3
    case 'B': paramNr = 3; break; // eg. ambientBLUE
    }

    // UNE SOLUTION POURRIE

    if (strcmp(val, "0.00")) {
      v[paramNr] = strdup(val);  // read the coulorComponentValue in v[]
      nrOfReadParams++;
    } 
  }
  return paramNr;
}

void Apparence::print(FILE *f)
{
  if (nrOfReadParams != 0)
    fprintf(f, " %s=\"%s %s %s\"", nom, v[1], v[2], v[3]);
  // else there has been made no use of this kind of 'Appearance'
  // so the 'nrOfReadParams' counter is zero!
}

/** Texture BOX_TEX */
BoxTextures::BoxTextures()
{
  compteur = 0;

  v = (char **)malloc(7 * sizeof(char *));
  nom = (char **)malloc(7 * sizeof(char *));
  strcpy(nom[1], "xp"); v[1] = NULL;
  strcpy(nom[2], "xn"); v[2] = NULL;
  strcpy(nom[3], "yp"); v[3] = NULL;
  strcpy(nom[4], "yn"); v[4] = NULL;
  strcpy(nom[5], "zp"); v[5] = NULL;
  strcpy(nom[6], "zn"); v[6] = NULL;
  strcpy(nom[7], "tx"); v[7] = NULL;
}

int BoxTextures::read(char *var, char *val)
{
  int i;
  for (i = 1 ; i < 7 ; i++)
    if (strcmp(nom[i], var) == 0) {
      v[i] = (char *)malloc(strlen(val) + sizeof(char)); 
      strcpy(v[i], rognerGuillemets(val));  
      compteur++;
      break;
    }
  return ((i < 7) ? i : 0);
}

void BoxTextures::print(FILE *f)
{
  int i;
  if (compteur != 0)
    for (i = 1 ; i < 7 ; i++)
      if ((v[i] != NULL) && (strcmp(v[i], "0.00")))
	fprintf(f, " %s=\"%s\"", nom[i], v[i]);
}

//---------------------- Sphere texture ---------------------------HK

Texture::Texture()
{
  strcpy(name, "texture");
  gifUrl = NULL;
  isThereGifUrl = 0;
}

int Texture::read(char * varName, char * potentialGifUrl)
{
  if (strcmp(varName, name) == 0 ) {

    gifUrl = (char*) malloc(strlen(potentialGifUrl) + sizeof(char));
    strcpy(gifUrl, rognerGuillemets(potentialGifUrl));
    isThereGifUrl = 1;
    // ca veut dire gifUrl = potentialGifUrl;
  }
  return 0;
}

void Texture::print(FILE *f)
{
  if ((isThereGifUrl == 1) && (strcmp(gifUrl, "0.00")))
    fprintf(f, " %s=\"%s\"", name, gifUrl);
}

//---------------------- une petite fonction de correction --------VG
char *rognerGuillemets(char *_nom)
{
  char *nom = (char *)malloc(100 * sizeof(char));

  nom = strtok(_nom, "\"");
  
  return nom;
}
