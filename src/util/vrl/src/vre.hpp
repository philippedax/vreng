#ifndef VRE_HPP
#define VRE_HPP

#include "global.hpp"
#include "vrl.hpp"

#define DEBBOUT if(1==1) fprintf


/**
 * Class declarations
 */
class TokenListe;
class Token;

class Wall;
class Gate;
class Web;
class Board;
class Host;
class Doc;
class Cauldron;
class Step;
class Aoi;
class Door;
class Button;
class Walls;
class Mirage;

class AttributFacultatif;
class Apparence;
class BoxTextures;


/**
 * Token
 */
class Token {
 public:
  virtual Token *readTable(char *name)=0;
  virtual void write2CFG(Table *data)=0;
};

/**
 * TokenListe
 */
class TokenListe {
 public:
  Token **array;
  int length;

  TokenListe();
  Token *readTable(char *name);
};

/**
 * Wall
 */
class Wall : public Token {
 public:
  Token *readTable(char *name);	// Est-on oblige de les redeclaree ?
  void write2CFG(Table *data);
};

/**
 * Gate
 */
class Gate : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Web
 */
class Web : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Board
 */
class Board : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Host
 */
class Host : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Doc
 */
class Doc : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Cauldron
 */
class Cauldron : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Step
 */
class Step : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Aoi
 */
class Aoi : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Door
 */
class Door : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Button
 */
class Button : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Walls
 */
class Walls : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * Mirage
 */
class Mirage : public Token {
 public:
  Token *readTable(char *name);
  void write2CFG(Table *data);
};

/**
 * AttributFacultatif
 */
class AttributFacultatif {
 public:
  virtual int read(char *var, char *val) = 0;
  virtual void print(FILE *f) = 0;
};

/**
 * Apparence
 */
class Apparence : public AttributFacultatif {
  char *nom;
  char **v;
  int nrOfReadParams;

 public:  
  Apparence(char *type);
  int read(char *var, char *val);
  void print(FILE *f);
};

/**
 * BoxTextures
 */
class BoxTextures : public AttributFacultatif {
  char **nom;
  char **v;
  int compteur;

 public:
  BoxTextures();
  int read(char *var, char *val);
  void print(FILE *f);
};

/**
 * Texture
 */
class Texture : public AttributFacultatif {
  char * name;
  char * gifUrl;
  int isThereGifUrl;

 public:
  Texture();
  int read(char *varName, char * potentialGifUrl);
  void print(FILE *f);
};

#endif
