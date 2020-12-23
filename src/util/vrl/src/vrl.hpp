#ifndef VRL_HPP
#define VRL_HPP

#include "global.hpp"


// *************************** DEBUT 1 **************************************


//REMARQUES :
// les listes doivent parfois etre creees dans les constructeurs
// faire un fprintf(stderr, ...) dans les constructeurs, pour le debogage


// **************** les classes pour l'arbre d'environnement ****************
class Table;	//liste d'instance Entree
class Entree;	//abstraite
class EntreeFloat;
class EntreeString;	// ou plutot une sorte d'union des trois car pbs !!
class EntreeObjet;
class PointeurPile;
class Pointeur;

// ***************************le fichier ************************************
class Fichier;

// *************************** les structures *******************************
class StructureListe;
class Structure;
class StructureHeritee; //herite de Structure
class FonctionListe;
class Fonction;
class PointDEntree;
class Constructeur;

// *************************** les instructions *****************************
class InstructionListe;
class Instruction;  //abstraite
class AppelDeConstructeur;

// *************************** les statements *******************************
//class StatementListe;
class Statement; //abstraite, herite de Instruction
class Retour; //herite de Statement
class BoucleIf; //herite de Statement    
class BoucleFor; //herite de Statement

// **************************** les declarations ****************************
class DeclarationListe;
class Declaration;	//abstraite, herite de Instruction
class DeclarationSimple; //herite de Declaration
class DeclarationComplexe; //abstraite, herite de Declaration
class DeclarationComplexeChaine; //herite de DeclarationComplexeChaine
class DeclarationComplexeExpression; //herite de DeclarationComplexeChaine
class DeclarationComplexeNouvelObjet; //herite de DeclarationComplexeChaine

// ***************************** les assignations ***************************
//class AssignationListe;
class Assignation;	//abstraite, herite de Instruction
class AssignationChaine; //herite de Assignation
class AssignationExpression; //herite de Assignation
class AssignationVariable;
class AssignationNouvelObjet; //herite de Assignation

// ************************** nouvel object *********************************
class NouvelObjet;

// *************************** les expressions ******************************
class ExpressionListe;
class Expression;	//abstraite
class ExpressionCalculable;
class Nombre;		// herite de expression calculable
class AppelDeFonction;

// ****************************** Variables *********************************
class Variable;		//abstraite    herite de expression
class VariableSimpleListe;
class VariableSimple;
class NomListe;		//liste de nom
class Nom;		//herite de variable
class NomDeTableau;	//herite de variable
class VariableComplexe;

// ****************************** Operations ********************************
class Operation;	//abstraite
class OperationBinaire;
class Addition;
class Soustraction;
class Multiplication;
class Division;
class OperationUnaire;
class Negation;		
class Absolu;

// ********************* les expressions logique ****************************
class ExpressionLogiqueListe;
class ExpressionLogique;	//abstraite

// ************************** comparaisons **********************************
class Comparaison;	//abstraite
class Superieur;
class Inferieur;
class SuperieurOuEgal;
class InferieurOuEgal;
class EgalEgal;
class Different;

// *************************** Operations logiques **************************
class OperationBinaireLogique;
class OuLogique;
class EtLogique;
class OperationUnaireLogique;
class NegationLogique;

// ****************************** FIN 1 *************************************
/* extern FILE *yyin ; */
//extern int   yyparse(); //dax
/* extern FILE *fichierCFG; */


//***************************** DEBUT 2**************************************

// ************************** la pile de pointeur ***************************

// *************************** la table d'environnement *********************

// ***************************le fichier ************************************

class Fichier {
  
  StructureListe *maStructureListe;  // il faut creer ces listes
  FonctionListe *maFonctionListe;    //
  PointDEntree *monPrg;

public:   
  Fichier();
  void addStructure(Structure *);
  void addFonction(Fonction *);
  void addPointDEntree(PointDEntree *);
  void createTable();

  Structure *getStructure(char *nomStructure); 
  Structure *getStructure(int i);
  Structure *getPremierStructure();
  Structure *getDernierStructure();

  Fonction *getByNameFonction(char *nom);
  Fonction *getFonction(int i); 
  Fonction *getPremierFonction(); 
  Fonction *getDernierFonction();

  PointDEntree *getPointDEntree();
};


// *************************** les structures *******************************

//---------------------------------------------------------------------------  
class StructureListe  { 
  Structure *premier; 
  Structure *dernier; 
  
 public: 
  StructureListe(); 
  void addStructure(Structure *);
  void checkNoms(); 
   
  Structure *getElement(char *); 
  Structure *getElement(int i); 
  Structure *getPremier() {return premier;} 
  Structure *getDernier() {return dernier;}
}; 


class Structure {
  // int nbConstr;
protected:
  char *monNom;
  DeclarationListe *maDeclarationListe;  //new dans le constructeur
  Constructeur *monConstructeur;
  Structure *structSuivant;
  FonctionListe *fonctionListe;
  
public:
  NomListe *structHeritees; //structures heritees
  Structure();
  void setNom(char *);
  void addDeclaration(Declaration *);
  void addConstructeur(Constructeur *constructeur) {monConstructeur = constructeur;}
  void addFonction(Fonction *fonction);
  void setNomListe(NomListe *maNomListe) {structHeritees = maNomListe;} 

  char *getNom() {return monNom;}

  Declaration *getDeclaration(int i);
  Declaration *getPremierDeclaration();
  Declaration *getDernierDeclaration();

  Constructeur *getConstructeur() {return monConstructeur;}
  Structure *getSuivant() {return structSuivant;}

  Nom *getPremierNom();
  NomListe *getHeritage();

  //void checkNom();       //regarde dans tous les precedents  
				//ou les antecedents si le nom
				//existe.  
  //void checkConstructeur();  //verifie l'unicite et l'existence
				//du constructeur.
  //virtual void setNomListe(NomListe *maNomListe) {;}

  friend class StructureListe;
};

//---------------------------------------------------------------------------  

class StructureHeritee : public Structure {
public:
  NomListe *structHeritees; //structures heritees

  // void setNomListe(NomListe *maNomListe) {structHeritees = maNomListe;}
  
 
};


class FonctionListe {
  Fonction *premier;
  Fonction *dernier;

public:
  FonctionListe();
  void addFonction(Fonction *);
  //void checkNoms();

  Fonction *getByNameElement(char *);
  Fonction *getElement(int i);
  Fonction *getPremier() {return premier;}
  Fonction *getDernier() {return dernier;}
};

//--------------------------------------------------------------------------

class Fonction {
  char *monNom;
  InstructionListe *maInstructionListe;
  NomListe *maNomListe;
  Fonction *fonctionSuivant;
  
public:
  Fonction();
  Fonction(char *, InstructionListe *);
  Fonction(char *, NomListe *, InstructionListe *);

  char *getNom() {return monNom;}
  Fonction *getSuivant() {return fonctionSuivant;}

  Instruction *getInstruction(int i);
  Instruction *getPremierInstruction();
  Instruction *getDernierInstruction();

  NomListe *getNomListe() {return maNomListe;}

  friend class FonctionListe;
};

// **************************************************************************

class PointDEntree {
  InstructionListe *maInstructionListe;

public:
  PointDEntree();
  PointDEntree(InstructionListe *);

  void createTable();

  Instruction *getInstruction(int i);
  Instruction *getPremierInstruction();
  Instruction *getDernierInstruction();
};

// **************************************************************************

class Constructeur {
  InstructionListe *maInstructionListe;
  NomListe *maNomListe;
  AppelDeConstructeur *monAppelDeConstructeur;

public:
  Constructeur();
  void addInstruction(Instruction *);
  void addAppelDeConstructeur(AppelDeConstructeur *monAppelDeConstructeur) { this->monAppelDeConstructeur = monAppelDeConstructeur;}
  void setNomListe(NomListe *maNomListe) { this->maNomListe = maNomListe;}

  Instruction *getInstruction(int i);
  Instruction *getPremierInstruction();
  Instruction *getDernierInstruction();

  Nom *getNom(int i);
  Nom *getPremierNom();
  Nom *getDernierNom();
  NomListe *getNomListe() { return maNomListe ; }

  AppelDeConstructeur *getAppelDeConstructeur(){return monAppelDeConstructeur;}
};

//--------------------------------------------------------------------------

class AppelDeConstructeur {
  char *monNom;
  ExpressionListe *maExpressionListe;

public:
  AppelDeConstructeur();
  AppelDeConstructeur(char *);
  AppelDeConstructeur(char *, ExpressionListe *);

  char *getNom() {return monNom;}
  
  Expression *getExpression(int i);
  Expression *getPremierExpression();
  Expression *getDernierExpression();

  ExpressionListe *getListe() {return maExpressionListe ;}

  void updateTable();
  void declareAttribut();
};

// **************************************************************************
// *************************** les instructions *****************************
// **************************************************************************

class InstructionListe {
  Instruction *premier;
  Instruction *dernier;
  int taille ;

 public:
  InstructionListe();
  void addInstruction(Instruction *);
  void updateTable();
  
  Instruction *getElement(int i);
  Instruction *getPremier() {return premier;}
  Instruction *getDernier() {return dernier;}

};

//--------------------------------------------------------------------------

class Instruction { //abstraite
 protected:
  Instruction *instructionSuivant;
 public:  
  Instruction *getSuivant();

  virtual void updateTable(){;}

  friend class InstructionListe;
};

// *************************** les statements *******************************

//class StatementListe {     Necessaire ???
//};

//--------------------------------------------------------------------------

class Statement : public Instruction { //abstraite
};

//--------------------------------------------------------------------------

class Retour : public Statement {
  Expression *monExpression;

 public:
  Retour();
  Retour(Expression *);

  void updateTable();

  Expression *getExpression() {return monExpression;}
};

//--------------------------------------------------------------------------

class BoucleIf : public Statement {
  ExpressionLogique *monExpressionLogique;
  InstructionListe *maInstructionListeSi;
  InstructionListe *maInstructionListeSinon;

public:
  BoucleIf();
  BoucleIf(ExpressionLogique *, InstructionListe *);
  BoucleIf(ExpressionLogique *, InstructionListe *, InstructionListe *);

  void updateTable();
  
  ExpressionLogique *getExpressionLogique() {return monExpressionLogique;}
  
  Instruction *getInstructionSi(int i);
  Instruction *getPremierInstructionSi();
  Instruction *getDernierInstructionSi();

  Instruction *getInstructionSinon(int i);
  Instruction *getPremierInstructionSinon();
  Instruction *getDernierInstructionSinon();
};    

//--------------------------------------------------------------------------

class BoucleFor : public Statement {
  AssignationExpression *monAssignationExpressionDebut;
  ExpressionLogique *monExpressionLogiqueFin;
  AssignationExpression *monAssignationExpressionIncrementation;
  InstructionListe *monInstructionListe;

 public:
  BoucleFor();
  BoucleFor(AssignationExpression *, ExpressionLogique *, AssignationExpression *, InstructionListe *);

  void updateTable();

  AssignationExpression *getAssignationExpressionDebut() {return monAssignationExpressionDebut;}
  ExpressionLogique *getExpressionLogiqueFin() {return monExpressionLogiqueFin;}
  AssignationExpression *getAssignationExpressionIncrementation() {return monAssignationExpressionIncrementation;}
  InstructionListe *getInstructionListe() {return monInstructionListe;}

  Instruction *getInstruction(int i);
  Instruction *getPremierExpression();
  Instruction *getDernierExpression();
};

// **************************** les declarations ****************************

class DeclarationListe {
  Declaration *premier;
  Declaration *dernier;

 public:
  DeclarationListe();
  void addDeclaration(Declaration *);

  Declaration *getElement(int i);
  Declaration *getPremier() {return premier;}
  Declaration *getDernier() {return dernier;}
};

//--------------------------------------------------------------------------

class Declaration : public Instruction {	//abstraite
protected:  
  Declaration *declarationSuivant;
 public:
  Declaration *getSuivant() {return declarationSuivant;}
  friend class DeclarationListe;
};

//--------------------------------------------------------------------------

class DeclarationSimple : public Declaration {
  NomListe *maNomListe;
  NomDeTableau *monNomDeTableau;

 public:
  DeclarationSimple();
  DeclarationSimple(NomListe *);
  DeclarationSimple(NomDeTableau *);

  void updateTable();

  Nom *getNom(int i);
  Nom *getPremierNom();
  Nom *getDernierNom();

};

//--------------------------------------------------------------------------

class DeclarationComplexe : public Declaration { //abstraite
 protected:
  Variable *maVariable;
 public:
  virtual Variable *getVariable() = 0;
  
};

class DeclarationComplexeChaine : public DeclarationComplexe { 
  char *maChaine;

 public:
  DeclarationComplexeChaine();
  DeclarationComplexeChaine(Variable *, char *);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  char *getChaine() {return maChaine;}
};


class DeclarationComplexeExpression : public DeclarationComplexe { 
  Expression *monExpression;

 public:
  DeclarationComplexeExpression();
  DeclarationComplexeExpression(Variable *, Expression *);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  Expression *getExpression() {return monExpression;}
};

class DeclarationComplexeNouvelObjet : public DeclarationComplexe { 
  NouvelObjet *monNouvelObjet;

 public:
  DeclarationComplexeNouvelObjet();
  DeclarationComplexeNouvelObjet(Variable *, NouvelObjet *);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  NouvelObjet *getNouvelObjet() {return monNouvelObjet;}
};



// ***************************** les assignations ***************************

// class AssignationListe ??? Necessaire ???

class AssignationListe {
  Assignation *premier;
  Assignation *dernier;

 public:
  AssignationListe();
  void addAssignation(Assignation *);

  Assignation *getPremier() {return premier;}
  Assignation *getDernier() {return dernier;}
};


//--------------------------------------------------------------------------

class Assignation : public Instruction {     //abstraite
  Assignation *suivant;
protected: 
  Variable *maVariable;
public:
  Assignation();
  Assignation(Variable *_variable);
  
  Assignation *getSuivant() {return suivant;}

  virtual Variable *getVariable() = 0;
  
  friend class AssignationListe;
};

//--------------------------------------------------------------------------

class AssignationChaine : public Assignation {
  char *maChaine; //char * ??

 public:
  AssignationChaine();
  AssignationChaine(Variable *,char *);
  
  void updateTable();
  
  Variable *getVariable() {return maVariable;}
  char *getChaine() {return maChaine;}
};

//--------------------------------------------------------------------------

class AssignationExpression : public Assignation {
  Expression *monExpression;

 public:
  AssignationExpression() ;
  AssignationExpression(Variable *_variable1, Expression *_variable2);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  Expression *getExpression() {return monExpression;}
};
//--------------------------------------------------------------------------
class AssignationVariable : public Assignation {
  Variable *variableAAssigner;

public:
  AssignationVariable() ;
  AssignationVariable(Variable *, Variable *);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  Variable *getVariableAAssigner() {return variableAAssigner;}
};

//--------------------------------------------------------------------------

class AssignationNouvelObjet : public Assignation {
  NouvelObjet *monObjet;

 public:
  AssignationNouvelObjet();
  AssignationNouvelObjet(Variable *, NouvelObjet *);

  void updateTable();

  Variable *getVariable() {return maVariable;}
  NouvelObjet *getObjet() {return monObjet;}
};

// ************************** nouvel object *********************************

class NouvelObjet {
  char *monNom;
  ExpressionListe *maExpressionListe;

public:
  NouvelObjet();
  NouvelObjet(char *);
  NouvelObjet(char *, ExpressionListe *);

  Expression *getExpression(int i);
  Expression *getPremierExpression();
  Expression *getDernierExpression();
  ExpressionListe *getExpressionListe() { return maExpressionListe ; }

  char *getNom() {return monNom;}
};

//**************************** FIN 2 ****************************************


//***************************** DEBUT 3**************************************

// *************************** les expressions ******************************

class ExpressionListe{ 
  Expression *premier; 
  Expression *dernier;
  int taille;

  Expression *getElementRecursivement(Expression *expression, int i);
public:
  ExpressionListe() : premier((Expression *)NULL), dernier((Expression *)NULL), taille(0) {  }
  ExpressionListe(Expression *monExpression);
  void addExpression(Expression *expression);
  Expression *getElement(int i){return getElementRecursivement(premier, i);}
  Expression *getPremier(){return premier;}
  Expression *getDernier(){return dernier;}
  int getTaille() {return taille;}
};


/* class ExpressionListe : public Liste<Expression> { */
/* }; */

//--------------------------------------------------------------------------

class Expression{   	//abstraite
protected:
  Expression *suivant;
public :
  Expression() : suivant((Expression *)NULL) {} //non necessaire
  Expression *getSuivant() {return suivant;}
  virtual float evaluate() = 0;
  virtual Assignation *creerAssignation(Variable *variable) = 0;

  friend class ExpressionListe;
};
//--------------------------------------------------------------------------

class ExpressionCalculable : public Expression {
private: 
  Expression *expression;
public:
  ExpressionCalculable() : Expression(){}
  ExpressionCalculable(Expression *_expression) : Expression(), expression(_expression) {}
  float evaluate() {return expression->evaluate();}
  virtual Assignation *creerAssignation(Variable *variable);
};

//--------------------------------------------------------------------------

class Nombre : public ExpressionCalculable { 
  float valeur;
public:
  Nombre(float x) : ExpressionCalculable(), valeur(x) { }
  Nombre(int x) : ExpressionCalculable(), valeur(x) { }
  float getValeur(){return valeur;}
  float evaluate(){return valeur;}
};

//--------------------------------------------------------------------------
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//methode evaluate non implementee


class AppelDeFonction : public ExpressionCalculable{ //dur !!!!!!!!!!!!!!!!!!!!!!!!!!!
  char *nom;
  ExpressionListe *liste;
  Table *tableLocale;
  Fonction *fonction;
public:
  AppelDeFonction(char *_nom) : ExpressionCalculable(), nom(_nom) {  }
  AppelDeFonction(char *_nom, ExpressionListe *_liste) : ExpressionCalculable(), nom(_nom), liste(_liste) {}
  char *getNom(){return nom;};
  ExpressionListe *getExpressionListe(){return liste;};
  Expression *getExpression(int i){return liste->getElement(i);};
  Expression *getPremierExpression(){return liste->getPremier();}
  Expression *getDernierExpression(){return liste->getDernier();}

  float evaluate();
};

//--------------------------------------------------------------------------
class Variable : public Expression{ 	
protected:	
  char *nom;
public:
  Variable(char *_nom) : Expression(), nom(_nom) {  }
  virtual Table *emplacement() =0;
  virtual void creer(Entree *entree) =0;
  virtual PointeurPile *substituer(Entree *entree) = 0;
  virtual char *getNom() = 0 ;
  virtual char *getRealNom() {return nom;}
  virtual Assignation *creerAssignation(Variable *variable);
};

//--------------------------------------------------------------------------
class VariableSimpleListe{
  VariableSimple *premier;
  VariableSimple *dernier;

  VariableSimple *getElementRecursivement(VariableSimple *variable, int i);
public:
  VariableSimpleListe() : premier((VariableSimple *)NULL), dernier((VariableSimple *)NULL) {}
  void addVariable(VariableSimple *variable);
  VariableSimple *getElement(int i);
  VariableSimple *getPremier();
  VariableSimple *getDernier();

  friend class VariableSimple;
};
// cette classe est uniquement implemente pour les variables complexes
// aussi les methodes sont un peu speciales
// (dans l'urgence du deboggage)

//--------------------------------------------------------------------------
class VariableSimple : public Variable {
protected:
  VariableSimple *suivant;
public:
  VariableSimple()  : Variable((char *)NULL){  }
  VariableSimple(char *_nom) : Variable(_nom), suivant((VariableSimple *)NULL) {}
  VariableSimple *getSuivant(){return suivant;}
  virtual char *getRealNom() { return NULL;}

  friend class VariableSimpleListe;
};

//--------------------------------------------------------------------------
class NomListe{ 
  Nom *premier;
  Nom *dernier;
  int taille;

  Nom *getElementRecursivement(Nom *nom, int i);
public:
  NomListe() : premier((Nom *)NULL), dernier((Nom *)NULL), taille(0) {  }
  NomListe(char *_nom) : premier((Nom *)NULL), dernier((Nom *)NULL), taille(0){ addNom(_nom);}
  void addNom(char *nom);
  Nom *getElement(int i){return getElementRecursivement(premier, i);}

  Nom *getPremier(){return premier;}
  Nom *getDernier(){return dernier;}
  int getTaille() {return taille;}

};

// class NomListe : Liste<Nom>{
// };		

//--------------------------------------------------------------------------

class Nom : public VariableSimple { 
  //char *nom;  herite de Variable
  Nom *suivant;
 public: 
  Nom(char *_nom) : VariableSimple(_nom), suivant((Nom *)NULL) {  }
  Nom *getSuivant(){return this->suivant;}
  float evaluate();

  Table *emplacement();
  void creer(Entree *entree);
  void setNom(char *_nom) {nom = _nom;}
  PointeurPile *substituer(Entree *entree);
  PointeurPile * emplacementPile();
  char *getNom() {return nom;}
  char *getRealNom() {return nom;}

  friend class NomListe;
};		

//--------------------------------------------------------------------------

class NomDeTableau : public VariableSimple {
  char *nom;   // attribut herite
  int taille;
  Expression *index;

public:
  // recupere le nom d'une part, la taille de l'autre
  NomDeTableau(char *_nom, int _taille) { nom = _nom; taille = _taille; index = (Expression *)NULL ;} 
  NomDeTableau(char *nom1, Expression *_expression) ;
  int getTaille(){return taille;};	//est-ce utile
  float evaluate(); //dur
  char *getNom(){return nom;};	//est-ce utile
  Table *emplacement();
  void creer(Entree *entree);
  PointeurPile *substituer(Entree *entree);
  int getIndex() {return (int)index->evaluate();}
  char *getRealNom();
};

//--------------------------------------------------------------------------

class VariableComplexe : public Variable {
  char *nom;
  NomListe *liste;	// dans le meme ordre premier = gauche
  VariableSimpleListe *liste2;

public:
  // VariableComplexe(char *nom1, char *nom2);
  VariableComplexe(VariableSimple *var1, VariableSimple *var2) ;

  void addNomComposant(VariableSimple *var);
  char *getNom(){return nom;}
  //char *getNomComposant(int i){return liste->getElement(i)->getRealNom();}
  //char *getPremierNomComposant(){return liste->getPremier()->getRealNom();}
  //char *getDernierNomComposant(){return liste->getDernier()->getRealNom();}
  //Nom *getVariable(int i){return liste->getElement(i);}
  Nom *getPremierVariable();
  Nom *getDernierVariable();
  float evaluate(); 

  Table *emplacement(){return NULL;} ///////////////////
  void creer(Entree *entree){;} ////////////////////
  PointeurPile *substituer(Entree *entree);
};

// ****************************** Operations ********************************
class Operation : public ExpressionCalculable {	//abstraite
  virtual float evaluate() =0;
};

//--------------------------------------------------------------------------

class OperationBinaire : public Operation {
protected:
  Expression *operandeGauche;
  Expression *operandeDroit;
public:
  OperationBinaire(Expression *expressionGauche, Expression *expressionDroit) : operandeGauche(expressionGauche), operandeDroit(expressionDroit) {  }
  Expression *getOperandeGauche(){return operandeGauche;}
  Expression *getOperandeDroit(){return operandeDroit;}
}; 

//--------------------------------------------------------------------------

class Addition : public OperationBinaire {
public:
  Addition(Expression *expressionGauche, Expression *expressionDroit) : OperationBinaire(expressionGauche, expressionDroit) {  }
  float evaluate(){return ((operandeGauche->evaluate()) + (operandeDroit->evaluate()));}
};

//--------------------------------------------------------------------------

class Soustraction : public OperationBinaire {
public:
  Soustraction(Expression *expressionGauche, Expression *expressionDroit) : OperationBinaire(expressionGauche, expressionDroit) {  }
  float evaluate(){return ((operandeGauche->evaluate()) - (operandeDroit->evaluate()));}
};

//--------------------------------------------------------------------------

class Multiplication : public OperationBinaire {
public:
  Multiplication(Expression *expressionGauche, Expression *expressionDroit) : OperationBinaire(expressionGauche, expressionDroit) {  }
  float evaluate(){return ((operandeGauche->evaluate()) * (operandeDroit->evaluate()));}
};

//--------------------------------------------------------------------------

class Division : public OperationBinaire {
public:
  Division(Expression *expressionGauche, Expression *expressionDroit) : OperationBinaire(expressionGauche, expressionDroit) {  }
  float evaluate(){return ((operandeGauche->evaluate()) / (operandeDroit->evaluate()));}
 };

//--------------------------------------------------------------------------

class OperationUnaire : public Operation {
protected:
  Expression *operande;
public:
  OperationUnaire(Expression *expression) : operande(expression) {  }
  Expression *getOperande(){return operande;}
 };

//--------------------------------------------------------------------------
class Negation : public OperationUnaire {
public:
  Negation(Expression *expression) : OperationUnaire(expression) {  };
  float evaluate(){return (- (operande->evaluate()));}
};		

//--------------------------------------------------------------------------
class Absolu : public OperationUnaire {
public:
  Absolu(Expression *expression) : OperationUnaire(expression) {  }
  float evaluate();
};

// ********************* les expressions logiques ***************************

class ExpressionLogiqueListe {
  ExpressionLogique *premier;
  ExpressionLogique *dernier;
  
  ExpressionLogique *getElementRecursivement(ExpressionLogique *expression, int i);
public:
  ExpressionLogiqueListe() : premier((ExpressionLogique *)NULL), dernier(premier) {}
  void addExpressionLogique(ExpressionLogique *expression);
  ExpressionLogique *getElement(int i){return getElementRecursivement(premier, i);}
  ExpressionLogique *getPremier(){return premier;}
  ExpressionLogique *getDernier(){return dernier;}
};

// class ExpressionLogiqueListe : Liste<ExpressionLogique> {
// };

//--------------------------------------------------------------------------

class ExpressionLogique { 	//abstraite
protected:
  ExpressionLogique *suivant;
public:
  ExpressionLogique *getSuivant() {return suivant;}
  ExpressionLogique() : suivant((ExpressionLogique *)NULL) {  }
  virtual int evaluate() = 0;

  friend class ExpressionLogiqueListe;
};

// ************************** comparaisons **********************************
class Comparaison : public ExpressionLogique {
protected:
  Expression *operandeGauche;
  Expression *operandeDroit;
public:
  Comparaison(Expression *expressionGauche, Expression *expressionDroit) : operandeGauche(expressionGauche), operandeDroit(expressionDroit) {  }
  //virtual int evaluate();
  Expression *getOperandeGauche(){return operandeGauche;}
  Expression *getOperandeDroit(){return operandeDroit;}
};

//--------------------------------------------------------------------------

class Superieur : public Comparaison {
public:
  Superieur(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return (operandeGauche->evaluate() > operandeDroit->evaluate());}
};

//--------------------------------------------------------------------------

class Inferieur : public Comparaison {
public:
  Inferieur(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return (operandeGauche->evaluate() < operandeDroit->evaluate());}
};

//--------------------------------------------------------------------------

class SuperieurOuEgal : public Comparaison {
public:
  SuperieurOuEgal(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return (operandeGauche->evaluate() >= operandeDroit->evaluate());}
};

//--------------------------------------------------------------------------

class InferieurOuEgal : public Comparaison {
public:
  InferieurOuEgal(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return (operandeGauche->evaluate() <= operandeDroit->evaluate());};
};

//--------------------------------------------------------------------------

class EgalEgal : public Comparaison {
public:
  EgalEgal(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return (operandeGauche->evaluate() == operandeDroit->evaluate());}
};

//--------------------------------------------------------------------------

class Different : public Comparaison {
public:
  Different(Expression *expressionGauche, Expression *expressionDroit) : Comparaison(expressionGauche, expressionDroit) {  }
  int evaluate(){return ((operandeGauche->evaluate()) != (operandeDroit->evaluate()));}
};

// *************************** Operations logiques **************************

class OperationBinaireLogique : public ExpressionLogique {
protected:
  ExpressionLogique *operandeGauche;
  ExpressionLogique *operandeDroit;
public:
  OperationBinaireLogique(ExpressionLogique *expressionGauche, ExpressionLogique *expressionDroit) : operandeGauche(expressionGauche), operandeDroit(expressionDroit){  }
  ExpressionLogique *getOperandeGauche() {return operandeGauche;}
  ExpressionLogique *getOperandeDroit(){return operandeDroit;}
 };

//--------------------------------------------------------------------------

class OuLogique : public OperationBinaireLogique {
public:
  OuLogique(ExpressionLogique *expressionGauche, ExpressionLogique *expressionDroit) : OperationBinaireLogique(expressionGauche, expressionDroit){  }
  int evaluate(){return ((1 +  (operandeGauche->evaluate()) + (operandeDroit->evaluate())) / 2);}
};

//--------------------------------------------------------------------------

class EtLogique : public OperationBinaireLogique {
public:
  EtLogique(ExpressionLogique *expressionGauche, ExpressionLogique *expressionDroit) : OperationBinaireLogique(expressionGauche, expressionDroit){  }
  int evaluate(){return ((operandeGauche->evaluate()) * (operandeDroit->evaluate()));}
};

//--------------------------------------------------------------------------

class OperationUnaireLogique : public ExpressionLogique {
protected:
  ExpressionLogique *operande;
public:
  OperationUnaireLogique(ExpressionLogique *expression) : operande(expression){  }
  ExpressionLogique *getOperande(){return operande;}  
};
 
//--------------------------------------------------------------------------

class NegationLogique : public OperationUnaireLogique {
public:
  NegationLogique (ExpressionLogique *expression) : OperationUnaireLogique(expression){  }
  int evaluate(){return ((1 + (operande->evaluate())) % 2);}
};

//--------------------------------------------------------------------------

//***************************** FIN 3****************************************


//***************************** DEBUT ***************************************

//************************ la pile de pointeur ******************************
class PointeurPile {
  Pointeur *top ;
  Pointeur *bottom ;	// je l'ai remis pour mes implementations...
  int taille ;

public:
  PointeurPile() ;
  void addPointeur(Pointeur *pointeur) ;
  //depile un pointeur et renvoie sa valeur
  Table *takeTopPointeur() ;
  //renvoie la valeur du pointeur du dessus
  Table *getTopPointeur();
  
  Pointeur *getTop() {return top;}
  Pointeur *getBottom() {return bottom;}
  PointeurPile *dupliquer() ;
  PointeurPile *inverser();
} ;

// **************************************************************************
class Pointeur {
  Table *valeur ;
  Pointeur *pointeurSuivant ;

public:
  Pointeur(Table *table);
  void setValeur(Table *table);
  Table *getValeur();
  Pointeur *getSuivant();
  Pointeur *dupliquer();
  void setSuivant(Pointeur *pointeur);

  friend class PointeurPile ;
} ;

//************************ la table d'environnement *************************
class Table {
  char *nom;
  Entree *premier;
  Entree *dernier;

  Entree *getEntreeRecursivement(Entree *entree, int i);
public:
  Table(char *_nom) : nom(_nom), premier((Entree *)NULL), dernier((Entree *)NULL) {}
  ~Table();
  char *getNom(){return nom;}
  void setNom(char *_nom) {nom = _nom;}

  // renvoie le nom de l'Entree i
  char *getEntreeNom(int i);

  // renvoie la valeur de l'entree i
  char *getEntreeValeur(int i);

  void addEntree(Entree *monEntree) ;
  Entree *getEntree(int i){return getEntreeRecursivement(premier, i);}
  Entree *getPremier() {return premier;}
  Entree *getDernier() {return dernier;}
  void removeDernierEntree() ;

  // renvoie vrai si pour toutes les Entree, Entree->table = NULL
  int isFeuille();

  // renvoie l'Entree dont le nom vaut nom
  Entree *getByNameEntree(char *nom);

  // renvoie la position i>=0 si le nom existe dans la table, -1 sinon
  int checkNom(char *nom);

  // et pour le parcours final...
  void parcoursFinal();

  void substituer(char *_nom, Entree *entree);

  // renvoie une copie de la table
  Table *dupliquer();
};

// **************************************************************************
class Entree  {	//class abstraite racine des Entrees
protected:
  char *nom;
  Table *table;
  Entree *suivant ;
public:
  Entree(); 
  Entree(char *monNom);

  char *getNom() { return nom ; }
  Table *getTable(){return table;}
  Entree *getSuivant() { return suivant ; }
  void setSuivant(Entree *monSuivant) { suivant = monSuivant ; }
  void setNom(char *_nom) {nom = strdup(_nom);}

  virtual Entree *dupliquer() {return NULL;}
  virtual Entree *dupliquer(char *nom) {return NULL;}

  virtual char *toString() {return NULL;}
  virtual float toFloat() {return 0;}

  friend class Table;
};

// **************************************************************************
class EntreeFloat : public Entree {
  float valeur;

 public:
  EntreeFloat(char *monNom);
  EntreeFloat(float x);
  EntreeFloat(char *monNom, float x);
 
  Entree *dupliquer();
  Entree *dupliquer(char *);

  void setNom(char *monNom) ;
  void setValeur(float valeur) { this->valeur = valeur ; }
  // convertie valeur en char*.    il faut une bibliotheque <stdlib.h>
  char *toString();
  float toFloat() {return valeur;}
} ;

// **************************************************************************
class EntreeString : public Entree {
  char *valeur;
 public:
  EntreeString(char *maValeur);
  EntreeString(char *monNom, char *maValeur); 

  Entree *dupliquer();
  Entree *dupliquer(char *);

  void setValeur(char *valeur) { this->valeur = valeur ; }
  //renvoie valeur
  char *toString() {return valeur ; }
  float toFloat();
} ;

// **************************************************************************
class EntreeNouvelObjet : public Entree {

 public:
  EntreeNouvelObjet();
  EntreeNouvelObjet(char *monNom);

  Entree *dupliquer();
  Entree *dupliquer(char *);


  void setTable(Table *maTable);
  char *toString() {cout << "erreur : table non terminale\n";exit(1);return NULL;}
  float toFloat();

} ;

#endif
