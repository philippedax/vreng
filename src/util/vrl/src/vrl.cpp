#include "global.hpp"

#include "vrl.hpp"
#include "vre.hpp"


extern int main();
extern Fichier *fvrl;
extern int yyparse();

Table *tablePrincipal;
PointeurPile *pileDesPointeurs;

char *createName(char *monNom, int taille);
int longueur (int taille);
void erreur(char *nom) ;

#if 0 //dax
int main()
{
  return 0;
}
#endif

// ***************************le fichier ************************************

// CONSTRUCTEUR
Fichier::Fichier(void) {
  
  maStructureListe = new StructureListe();
  maFonctionListe = new FonctionListe();
  monPrg = (PointDEntree *)NULL;
}

void
Fichier::addStructure(Structure *maStructure) {
  maStructureListe->addStructure(maStructure);
}

void
Fichier::addFonction(Fonction *maFonction) {
  maFonctionListe->addFonction(maFonction);
}

void
Fichier::addPointDEntree(PointDEntree *monPointDEntree) {
  monPrg = monPointDEntree;
}

Structure *
Fichier::getStructure(char *nomStructure) {
  return maStructureListe->getElement(nomStructure);
}
  
Structure *
Fichier::getStructure(int i) {
  return maStructureListe->getElement(i);
}

Structure *
Fichier::getPremierStructure(void) {
  return maStructureListe->getPremier();
}

Structure *
Fichier::getDernierStructure(void) {
  return maStructureListe->getDernier();
}


Fonction
*Fichier::getByNameFonction(char *nom){
  return maFonctionListe->getByNameElement(nom);
}

Fonction *
Fichier::getFonction(int i) {
  return maFonctionListe->getElement(i);
}

Fonction *
Fichier::getPremierFonction(void) {
  return maFonctionListe->getPremier();
}

Fonction *
Fichier::getDernierFonction(void) {
  return maFonctionListe->getDernier();
}

PointDEntree 
*Fichier::getPointDEntree(void){return monPrg;}

// *************************** les structures *******************************

// StructureListe
// CONSTRUCTEUR
StructureListe::StructureListe(void) {
  
  premier = (Structure *)NULL;
  dernier = (Structure *)NULL;
}

void
StructureListe::addStructure(Structure *monElement) {
  if (premier == (Structure *)NULL)
    premier = dernier = monElement;
  else {
    dernier->structSuivant = monElement;
    dernier = monElement;
  }
  monElement->structSuivant = (Structure *)NULL;
}

Structure *
StructureListe::getElement(char *nomStructure) {
  Structure *temp;
  temp = premier;
 //  if (temp == (Structure *)NULL) {
//     cout << "ma Structureliste est vide!!!\n";
//     return (Structure *)NULL;
//   }
  while (temp != (Structure *)NULL) {
    if ( strcmp(temp->getNom(), nomStructure) == 0) return temp;
    else temp = temp->structSuivant;
  }
  cout << "pas de structure possedant le nom voulu\n";
  return (Structure *)NULL;
}

Structure *
StructureListe::getElement(int i) {
  Structure *temp;
  temp = premier;
  if (temp == (Structure *)NULL) {
    cout << "ma Structureliste est vide!!!\n";
    return (Structure *)NULL;
  }
  for(int j=1 ; j<=(i-1) ; j++){
    if (temp->structSuivant == (Structure *)NULL){
      cout << "pas assez d'elements!!!";
      return (Structure *)NULL;}
    else
      temp = temp->structSuivant;
  }
  return temp;
}

//Structure
// CONSTRUCTEUR
Structure::Structure()
{
  //nbConstr = 0;
  monNom=(char *)NULL;
  maDeclarationListe = new DeclarationListe();
  monConstructeur = (Constructeur *)NULL;
  structSuivant = (Structure *)NULL;
  structHeritees = (NomListe *)NULL;

}

void Structure::setNom(char *monNom)
{
  this->monNom = monNom;
  //  this->monNom = strdup(monNom);
  //strcpy(this->monNom, monNom);
}

void Structure::addDeclaration(Declaration *maDeclaration)
{
  maDeclarationListe->addDeclaration(maDeclaration);
}

void Structure::addFonction(Fonction *fonction)
{
  fonctionListe->addFonction(fonction);
}

Declaration * Structure::getDeclaration(int i)
{
  return maDeclarationListe->getElement(i);
}

Declaration * Structure::getPremierDeclaration()
{
  return maDeclarationListe->getPremier();
}

Declaration * Structure::getDernierDeclaration()
{
  return maDeclarationListe->getDernier();
}

Nom *Structure::getPremierNom()
{
  if (!structHeritees)
    return (Nom *)NULL;

  return structHeritees->getPremier() ;
}

NomListe *Structure::getHeritage() 
{ return structHeritees ; }


// **************************************************************************
//FonctionListe
// CONSTRUCTEUR
FonctionListe::FonctionListe() {
  premier = (Fonction *)NULL;
  dernier = (Fonction *)NULL;
}

void
FonctionListe::addFonction(Fonction *monElement) {
   if (premier == (Fonction *)NULL)
    premier = dernier = monElement;
  else {
    dernier->fonctionSuivant = monElement;
    dernier = monElement;
  }
   monElement->fonctionSuivant = (Fonction *)NULL;
}


Fonction
*FonctionListe::getByNameElement(char *nom) {
  Fonction *fonction = premier;

  do
    if(strcmp(fonction->monNom, nom) == 0)
      return fonction;
  while ((fonction = fonction->fonctionSuivant)!= NULL);
  
  return (Fonction *)NULL;

}

Fonction*
FonctionListe::getElement(int i) {
 Fonction *temp;
  temp = premier;
  if (temp == (Fonction *)NULL) {
    cout << "ma Fonctionliste est vide!!!\n";
    return (Fonction *)NULL;
  }
  for(int j=1 ; j<=(i-1) ; j++){
    if (temp->fonctionSuivant == (Fonction *)NULL){
      cout << "pas assez d'elements!!!";
      return (Fonction *)NULL;}
    else
      temp = temp->fonctionSuivant;
  }
  return temp;
}


//Fonction
// CONSTRUCTEUR
Fonction::Fonction()
{
  monNom = (char *)NULL;
  maInstructionListe = (InstructionListe *)NULL;
  maNomListe = (NomListe *)NULL;
  fonctionSuivant = (Fonction *)NULL;
}

// CONSTRUCTEUR
Fonction::Fonction(char *monNom, InstructionListe *maInstructionListe)
{
  this->monNom = monNom;
  this->maInstructionListe = maInstructionListe;
  maNomListe = (NomListe *)NULL;
  fonctionSuivant = (Fonction *)NULL;
}

// CONSTRUCTEUR
Fonction::Fonction(char *monNom, NomListe *maNomListe, InstructionListe *maInstructionListe)
{
  this->monNom = monNom;
  this->maInstructionListe = maInstructionListe;
  this->maNomListe = maNomListe;
  fonctionSuivant = (Fonction *)NULL;
}

Instruction * Fonction::getInstruction(int i)
{
  return maInstructionListe->getElement(i);
}

Instruction * Fonction::getPremierInstruction()
{
  return maInstructionListe->getPremier();
}

Instruction * Fonction::getDernierInstruction()
{
  return maInstructionListe->getDernier();
}


// **************************************************************************
//PointDEntree
// CONSTRUCTEUR
PointDEntree::PointDEntree(void) {
  
  maInstructionListe = (InstructionListe *)NULL;
}

// CONSTRUCTEUR
PointDEntree::PointDEntree(InstructionListe *maInstructionListe) {
  
  this->maInstructionListe = maInstructionListe;
}

Instruction *
PointDEntree::getInstruction(int i) {
  return maInstructionListe->getElement(i);
}

Instruction *
PointDEntree::getPremierInstruction(void) {
  return maInstructionListe->getPremier();
}

Instruction *
PointDEntree::getDernierInstruction(void) {
  return maInstructionListe->getDernier();
}

// **************************************************************************
//Constructeur
// CONSTRUCTEUR
Constructeur::Constructeur(void) {
  
  maInstructionListe = new InstructionListe();
  maNomListe = (NomListe *)NULL;
}

void
Constructeur::addInstruction(Instruction *maInstruction) {
  maInstructionListe->addInstruction(maInstruction);
} 
 
Instruction *
Constructeur::getInstruction(int i) {
  return maInstructionListe->getElement(i);
}

Instruction *
Constructeur::getPremierInstruction(void) {
  return maInstructionListe->getPremier();
}

Instruction *
Constructeur::getDernierInstruction(void) {
  return maInstructionListe->getDernier();
}

Nom *
Constructeur::getNom(int i) {
  return maNomListe->getElement(i);
}

Nom *
Constructeur::getPremierNom(void) {
  return maNomListe->getPremier();
}

Nom *
Constructeur::getDernierNom(void) {
  return maNomListe->getDernier();
}

//AppelDeConstructeur
// CONSTRUCTEUR
AppelDeConstructeur::AppelDeConstructeur(void) {
  
  monNom = (char *)NULL;
  maExpressionListe = (ExpressionListe *)NULL;
}

// CONSTRUCTEUR
AppelDeConstructeur::AppelDeConstructeur(char *monNom) {
  
  this->monNom = monNom;
  maExpressionListe = (ExpressionListe *)NULL;
}

// CONSTRUCTEUR
AppelDeConstructeur::AppelDeConstructeur(char *monNom, ExpressionListe *maExpressionListe) {
  
  this->monNom = monNom;
  this->maExpressionListe = maExpressionListe;
}

Expression *
AppelDeConstructeur::getExpression(int i) {
  return maExpressionListe->getElement(i);
}

Expression *
AppelDeConstructeur::getPremierExpression(void) {
  return maExpressionListe->getPremier();
}

Expression *
AppelDeConstructeur::getDernierExpression(void) {
  return maExpressionListe->getDernier();
}

// ************************ AppelDeConstructeur *****************************
// *********************** methode update ***********************************

void
AppelDeConstructeur::updateTable()
{
  // la table empilee prend le nom de la structure appelee
  Table  *tableCourante = pileDesPointeurs->getTopPointeur() ;
  tableCourante->setNom(monNom);

  // recherche de la structure appelee
  Structure *structure = fvrl->getStructure(monNom) ;

  if (structure == NULL){
    cout << " erreur : " << monNom << " non declaree\n";
    exit(0);
  }

  // recuperation du constructeur
  Constructeur *constructeur = structure->getConstructeur() ;
  
  AppelDeConstructeur *appel = constructeur->getAppelDeConstructeur();

  NomListe *arguments = constructeur->getNomListe();
  ExpressionListe *parametres = maExpressionListe;

  if (arguments->getTaille() != (parametres->getTaille())) {
    cout << "le constructeur " << monNom << "n'est pas appele avec le bon nombre d'arguments\n" ;
    exit(1) ;
  }
  
  AssignationListe *initialisations = new AssignationListe();
   
  Nom *arg = arguments->getPremier();
  Expression *param = parametres->getPremier();


  // on y declare les attributs de la structure
  Declaration *attribut = structure->getPremierDeclaration();
  while(attribut) {
    attribut->updateTable() ;
    attribut = attribut->getSuivant() ;
  }

  // creation d'une assignation entre arguments et parametres
  while(arg) {	// traite aussi le cas ou il n'y a pas d'argument
    initialisations->addAssignation(param->creerAssignation(arg)); 
    arg = arg->getSuivant();
    param = param->getSuivant();
  }

  // on y declare les arguments du constructeur, puis on realise les assignations de la liste cree precedemment
  DeclarationSimple *declarerArgs = new DeclarationSimple(arguments); 
  declarerArgs->updateTable() ;

  Assignation *initialisation = initialisations->getPremier();
  while(initialisation) {
    initialisation->updateTable();
    initialisation = initialisation->getSuivant();
  }
  


   ///////  SI HERITAGE ////////
   // la methode update de AppelDeConstructeur realise les instructions dans la table
   // actuelle et change le nom de cette table actuelle. Elle prend le nom 
   // de la structure heritee

   if(appel) {
     // verification de l'implementation de l'heritage
     // recherche de la structure heritee
     Nom *nomTmp =  structure->getPremierNom();
     if(!nomTmp) {
       cout << "appel de constructeur dans " << monNom << " sans heritage !\n";
       exit(1);
     }
     if(strcmp(nomTmp->getNom(), appel->getNom())) {
       cout << monNom << " : incoherence entre appel de constructeur et heritage\n";
       exit(1);
     }

     // realisation de l'appel au constructeur
     appel->updateTable();
   } 

  
     // on realise les instructions du constructeur
     Instruction *instruction = constructeur->getPremierInstruction() ;

     while(instruction) {
       instruction->updateTable() ;
       instruction = instruction->getSuivant() ;
     }
}



// ************************ AppelDeConstructeur *****************************
// *********************** methode declareAttribut **************************
void
AppelDeConstructeur::declareAttribut(void) {
  // recuperation de la structure
  Structure *structure = fvrl->getStructure(monNom) ;

  // si c'est une structure heritee
  if(structure->getHeritage()) {
    AppelDeConstructeur *appel = new AppelDeConstructeur(structure->getPremierNom()->getNom());
    appel->declareAttribut();
  }
  
  Declaration *attributs = structure->getPremierDeclaration();

  while(attributs) {
    attributs->updateTable();
    attributs = attributs->getSuivant();
  }
  
}

// *************************** les instructions *****************************
//InstructionListe
// CONSTRUCTEUR
InstructionListe::InstructionListe(void) {
  
  taille = 0;
  premier = (Instruction *)NULL;
  dernier = (Instruction *)NULL; 
}

void
InstructionListe::addInstruction(Instruction *monElement) {
  taille++;
  //  cout << "instruction " << taille << " ajoutee\n";
   if (premier == (Instruction *)NULL)
    premier = dernier = monElement;
  else {
    dernier->instructionSuivant = monElement;
    dernier = monElement;
  }
   monElement->instructionSuivant = (Instruction *)NULL;
}

Instruction *
InstructionListe::getElement(int i) {
  Instruction *temp;
  temp = premier;
  if (temp == (Instruction *)NULL) {
    cout << "ma Instructionliste est vide!!!\n";
    return (Instruction *)NULL;
  }
  for(int j=1 ; j<=(i-1) ; j++){
    if (temp->instructionSuivant == (Instruction *)NULL){
      cout << "pas assez d'elements!!!";
      return (Instruction *)NULL;}
    else
      temp = temp->instructionSuivant;
  }
  return temp;
}

Instruction
*Instruction::getSuivant(void){
  return instructionSuivant;
}

// *************************** les statements *******************************
//Retour
// CONSTRUCTEUR
Retour::Retour(void) {
  
  monExpression = (Expression *)NULL;
  instructionSuivant = (Instruction *)NULL;
}

// CONSTRUCTEUR
Retour::Retour(Expression *monExpression) {
  
  instructionSuivant = (Instruction *)NULL;
  this->monExpression = monExpression;
}

//BoucleIf
// CONSTRUCTEUR
BoucleIf::BoucleIf(void) {
  
  instructionSuivant = (Instruction *)NULL;
  monExpressionLogique = (ExpressionLogique *)NULL;
  maInstructionListeSi = (InstructionListe *)NULL;
  maInstructionListeSinon = (InstructionListe *)NULL;
}

// CONSTRUCTEUR
BoucleIf::BoucleIf(ExpressionLogique *monExpressionLogique, InstructionListe *maInstructionListeSi) {
  
  instructionSuivant = (Instruction *)NULL;
  this->monExpressionLogique = monExpressionLogique;
  this->maInstructionListeSi = maInstructionListeSi;
  maInstructionListeSinon = (InstructionListe *)NULL;
}

// CONSTRUCTEUR
BoucleIf::BoucleIf(ExpressionLogique *monExpressionLogique, InstructionListe *maInstructionListeSi, InstructionListe *maInstructionListeSinon) {
  
  instructionSuivant = (Instruction *)NULL;
  this->monExpressionLogique = monExpressionLogique;
  this->maInstructionListeSi = maInstructionListeSi;
  this->maInstructionListeSinon = maInstructionListeSinon;
}

Instruction *
BoucleIf::getInstructionSi(int i) {
  return maInstructionListeSi->getElement(i);
}

Instruction *
BoucleIf::getPremierInstructionSi(void) {
  return maInstructionListeSi->getPremier();
}

Instruction *
BoucleIf::getDernierInstructionSi(void) {
  return maInstructionListeSi->getDernier();
}

Instruction *
BoucleIf::getInstructionSinon(int i) {
  return maInstructionListeSinon->getElement(i);
}

Instruction *
BoucleIf::getPremierInstructionSinon(void) {
  return maInstructionListeSinon->getPremier();
}

Instruction *
BoucleIf::getDernierInstructionSinon(void) {
  return maInstructionListeSinon->getDernier();
}
    
//BoucleFor
// CONSTRUCTEUR
BoucleFor::BoucleFor(void) {
  
  instructionSuivant = (Instruction *)NULL;
  monAssignationExpressionDebut = (AssignationExpression *)NULL;
  monExpressionLogiqueFin = (ExpressionLogique *)NULL;
  monAssignationExpressionIncrementation = (AssignationExpression *)NULL;
  monInstructionListe = (InstructionListe *)NULL;
}

// CONSTRUCTEUR
BoucleFor::BoucleFor(AssignationExpression *monAssignationExpressionDebut, ExpressionLogique *monExpressionLogiqueFin, AssignationExpression *monAssignationExpressionIncrementation, InstructionListe *monInstructionListe) {
  
  instructionSuivant = (Instruction *)NULL;
  this->monAssignationExpressionDebut = monAssignationExpressionDebut;
  this->monExpressionLogiqueFin = monExpressionLogiqueFin;
  this->monAssignationExpressionIncrementation = monAssignationExpressionIncrementation;
  this->monInstructionListe = monInstructionListe;
}

Instruction *
BoucleFor::getInstruction(int i) {
  return monInstructionListe->getElement(i);
}

Instruction *
BoucleFor::getPremierExpression(void) {
  return monInstructionListe->getPremier();
}

Instruction *
BoucleFor::getDernierExpression(void) {
  return monInstructionListe->getDernier();
}

// **************************** les declarations ****************************
//DeclarationListe
// CONSTRUCTEUR
DeclarationListe::DeclarationListe(void) {
  
  premier = (Declaration *)NULL;
  dernier = (Declaration *)NULL;
}

void
DeclarationListe::addDeclaration(Declaration *monElement) {
  if (premier == (Declaration *)NULL)
    premier = dernier = monElement;
  else {
    dernier->declarationSuivant = monElement;
    dernier = monElement;
  }
  monElement->declarationSuivant = (Declaration *)NULL;
}

Declaration *
DeclarationListe::getElement(int i) {
  Declaration *temp;
  temp = premier;
  if (temp == (Declaration *)NULL) {
    cout << "ma Declarationliste est vide!!!\n";
    return (Declaration *)NULL;
  }

  for(int j=1 ; j<=(i-1) ; j++){
    if (temp->declarationSuivant == (Declaration *)NULL){
      cout << "pas assez d'elements!!!";
      return (Declaration *)NULL;}
    else
      temp = temp->declarationSuivant;
  }
  return temp;
}

//DeclarationSimple
// CONSTRUCTEUR
DeclarationSimple::DeclarationSimple(void) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  maNomListe = (NomListe *)NULL;
  monNomDeTableau = (NomDeTableau *)NULL;
}

// CONSTRUCTEUR
DeclarationSimple::DeclarationSimple(NomListe *maNomListe) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  monNomDeTableau = (NomDeTableau *)NULL;
  this->maNomListe = maNomListe;
}

// CONSTRUCTEUR
DeclarationSimple::DeclarationSimple(NomDeTableau *monNomDeTableau) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  maNomListe = (NomListe *)NULL;
  this->monNomDeTableau = monNomDeTableau;
}

Nom *
DeclarationSimple::getNom(int i) {
  return maNomListe->getElement(i);
}

Nom *
DeclarationSimple::getPremierNom(void) {
  return maNomListe->getPremier();
}

Nom *
DeclarationSimple::getDernierNom(void) {
  return maNomListe->getDernier();
}

//DeclarationComplexe
// CONSTRUCTEUR
DeclarationComplexeChaine::DeclarationComplexeChaine(void) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  maVariable = (Variable *)NULL;
  maChaine = (char *)NULL;
}

// CONSTRUCTEUR
DeclarationComplexeChaine::DeclarationComplexeChaine(Variable *maVariable, char *maChaine) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  this->maVariable = maVariable;
  this->maChaine = maChaine;
}

// CONSTRUCTEUR
DeclarationComplexeExpression::DeclarationComplexeExpression(void) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  maVariable = (Variable *)NULL;
  monExpression = (Expression *)NULL;
}

// CONSTRUCTEUR
DeclarationComplexeExpression::DeclarationComplexeExpression(Variable *maVariable, Expression *monExpression) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  this->maVariable = maVariable;
  this->monExpression = monExpression;
}

// CONSTRUCTEUR
DeclarationComplexeNouvelObjet::DeclarationComplexeNouvelObjet(void) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  maVariable = (Variable *)NULL;
  monNouvelObjet = (NouvelObjet *)NULL;
}

// CONSTRUCTEUR
DeclarationComplexeNouvelObjet::DeclarationComplexeNouvelObjet(Variable *maVariable, NouvelObjet *monNouvelObjet) {
  
  instructionSuivant = (Instruction *)NULL;
  declarationSuivant = (Declaration *)NULL;
  this->maVariable = maVariable;
  this->monNouvelObjet = monNouvelObjet;
}


// ***************************** les assignations ***************************
//AssigantionListe
//Constructeur
AssignationListe::AssignationListe(void) : premier((Assignation *)NULL), dernier((Assignation *)NULL) {}

void
AssignationListe::addAssignation(Assignation *assignation) {
 if (premier == (Assignation *)NULL)
    premier = dernier = assignation;
  else {
    dernier->suivant = assignation;
    dernier = assignation;
  }
  assignation->suivant = (Assignation *)NULL;
}


//Assignation
//Constructeur
Assignation::Assignation(void) : maVariable((Variable *)NULL) {}
Assignation::Assignation(Variable *_variable) : maVariable(_variable) {}



//AssignationChaine
// CONSTRUCTEUR
AssignationChaine::AssignationChaine(void) {
  
  instructionSuivant = (Instruction *)NULL;
  maVariable = (Variable *)NULL;
  maChaine = (char *)NULL;
}

// CONSTRUCTEUR
AssignationChaine::AssignationChaine(Variable *maVariable, char *maChaine) {
  
  instructionSuivant = (Instruction *)NULL;
  this->maVariable = maVariable;
  this->maChaine = maChaine;
}

//AssignationExpresssion
// CONSTRUCTEUR
AssignationExpression::AssignationExpression(void) {
  
  instructionSuivant = (Instruction *)NULL;
  maVariable = (Variable *)NULL;
  monExpression = (Expression *)NULL;
}
// CONSTRUCTEUR
AssignationExpression::AssignationExpression(Variable *maVariable, Expression *monExpression) {
  
  instructionSuivant = (Instruction *)NULL;
  this->maVariable = maVariable;
  this->monExpression = monExpression;
}

//AssignationVariable
// Constructeur
AssignationVariable::AssignationVariable(void) : Assignation() {}
AssignationVariable::AssignationVariable(Variable *_variable1, Variable *_variable2) : Assignation(_variable1), variableAAssigner(_variable2) {}
  

//AssignationNouvelObjet
// CONSTRUCTEUR
AssignationNouvelObjet::AssignationNouvelObjet(void) {
  
  instructionSuivant = (Instruction *)NULL;
  maVariable = (Variable *)NULL;
  monObjet = (NouvelObjet *)NULL;
}

// CONSTRUCTEUR
AssignationNouvelObjet::AssignationNouvelObjet(Variable *maVariable, NouvelObjet *monObjet) {
  
  instructionSuivant = (Instruction *)NULL;
  this->maVariable = maVariable;
  this->monObjet = monObjet;
}

// ************************** nouvel object *********************************
// CONSTRUCTEUR
NouvelObjet::NouvelObjet(void) {
  
  monNom = (char *)NULL;
  maExpressionListe = (ExpressionListe *)NULL;
}

// CONSTRUCTEUR
NouvelObjet::NouvelObjet(char *monNom) {
  
  this->monNom = monNom;
  maExpressionListe = (ExpressionListe *)NULL;
}

// CONSTRUCTEUR
NouvelObjet::NouvelObjet(char *monNom, ExpressionListe *maExpressionListe) {
  
  this->monNom = monNom;
  this->maExpressionListe = maExpressionListe;
}

Expression *
NouvelObjet::getExpression(int i) {
  return maExpressionListe->getElement(i);
}

Expression *
NouvelObjet::getPremierExpression(void) {
  return maExpressionListe->getPremier();
}

Expression *
NouvelObjet::getDernierExpression(void) {
  return maExpressionListe->getDernier();
}

// **************************************************************************
//void
//Entree DeclarationSimple::updateTable(Table *table) {

//};

//***************************** FIN 1****************************************


//***************************** DEBUT 2**************************************

//***************************** ExpressionListe *****************************
// template <class Element> void
// Liste<Element>::addElement(Element *element){
//   if (premier == (Element *)NULL)
//     premier = dernier = element;
//   else {
//     dernier->suivant = element;
//     dernier = dernier->suivant;
//   }
// }


// template <class Element> Element
// *Liste<Element>::getElementRecursivement(Element *element, int i){
//   if(element == (Element *)NULL)
//     return (Element *)NULL;
    

//   return getElementRecursivement(element->suivant, i-1);
// }

ExpressionListe::ExpressionListe(Expression *monExpression)  : premier((Expression *)NULL), dernier((Expression *)NULL), taille(0) {
  this->addExpression(monExpression);
}

void
ExpressionListe::addExpression(Expression *expression){
  taille++;
  if (premier == (Expression *)NULL)
    premier = dernier = expression;
  else {
    dernier->suivant = expression;
    dernier = dernier->suivant;
  }
  expression->suivant = (Expression *)NULL;
}

Expression
*ExpressionListe::getElementRecursivement(Expression *expression, int i){
  if(expression == (Expression *)NULL)
    return (Expression *)NULL;

  return getElementRecursivement(expression->suivant, i-1);
}


//************************* VariableSimpleListe *****************************
void
VariableSimpleListe::addVariable(VariableSimple *variable){
  if (premier == (VariableSimple *)NULL)
    premier = dernier = variable;
  else {
    dernier->suivant = variable;
    dernier = dernier->suivant;
  }
  variable->suivant = (VariableSimple *)NULL;
}

VariableSimple
*VariableSimpleListe::getElementRecursivement(VariableSimple *variable, int i){
  if(variable == (VariableSimple *)NULL)
    return (VariableSimple *)NULL;

  return getElementRecursivement(variable->suivant, i-1);
}

VariableSimple
*VariableSimpleListe::getElement(int i) {
  VariableSimple *var = getElementRecursivement(premier, i);
  return var;
}

VariableSimple
*VariableSimpleListe::getPremier(void) {
  return premier;
}

VariableSimple
*VariableSimpleListe::getDernier(void) {
  return dernier;
}


//************************* ExpressionCalculable ****************************

Assignation 
*ExpressionCalculable::creerAssignation(Variable *variable) {
  Assignation *assignation = new AssignationExpression(variable, this);
  return assignation;
}
//************************* Variable ********** *****************************
Assignation
*Variable::creerAssignation(Variable *variable) {
  Assignation *assignation = new AssignationVariable(variable, this);
  return assignation;
}

//************************* AppelDeFonction - evaluate **********************
float
AppelDeFonction::evaluate(void) {
  // creer une table locale
  tableLocale = new Table(nom);	
  // empile un pointeur sur cette table locale
  pileDesPointeurs->addPointeur(new Pointeur(tableLocale));
  
  fonction = fvrl->getByNameFonction(nom);

  NomListe *nomListe = fonction->getNomListe();
  Nom *nomVar = nomListe->getPremier();
  Expression *expressionVar = liste->getPremier();
  
  // 1- verification du nombre d'argument passe a la fonction
  if(nomListe->getTaille() != liste->getTaille()) {
    cout << "erreur : nombre de parametres de la fonction " << nom << " incorrect\n";
    exit(0);
  }

  // 2- initialisation de la table
  do {
    tableLocale->addEntree(new EntreeFloat(nomVar->getNom(), expressionVar->evaluate()));
    expressionVar = expressionVar->getSuivant();
  } while((nomVar = nomVar->getSuivant()) != (Nom *)NULL);
    
  // 3- deroulement des instructions. Test si on est tombe sur un return
  Instruction *instruction = fonction->getPremierInstruction();

  do {
    instruction->updateTable();

    // si on n'est plus sur la meme table que la tablelocale alors il y a eu un return
    if(pileDesPointeurs->getTopPointeur() != tableLocale)
      break;

  } while((instruction = instruction->getSuivant()) != (Instruction *)NULL);


  
  // 4- recuperation de la valeur associee au return
  return tableLocale->getByNameEntree((char *)"return")->toFloat();;
}

//***************************** NomListe ************************************
void
NomListe::addNom(char *nom){
  taille++;
  if (premier == (Nom *)NULL)
    premier = dernier = new Nom(nom);
  else {
    dernier->suivant = new Nom(nom);
    dernier = dernier->suivant;
  }
  dernier->suivant = (Nom *)NULL;
}

Nom
*NomListe::getElementRecursivement(Nom *nom, int i){
  if(nom == (Nom *)NULL)
    return (Nom *)NULL;

  return getElementRecursivement(nom->suivant, i-1);
}

//***************************** Nom - methode evaluate **********************
float
Nom::evaluate(void){

  //cout << "evaluation de la variable " << nom << "\n";


  Pointeur *pointeur = pileDesPointeurs->getTop();
  Table *table;
  Entree *entree = (Entree *)NULL;

  do {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(nom);
    if (entree != (Entree *)NULL)
      break;

    pointeur = pointeur->getSuivant();
    
  } while (pointeur != NULL);
  
  if (entree == (Entree *)NULL){
    cout << "erreur : la variable " << nom << " n'est pas declaree !\n";
    exit(1);
  }
  cout << nom << " = " << entree->toFloat() << "\n";

  return entree->toFloat();
}

//************************ NomDeTableau - constructor **** ******************
NomDeTableau::NomDeTableau(char *nomDeTableau, Expression *_expression){
  taille = -1;
  nom =nomDeTableau;
  index = _expression;
}

char
*NomDeTableau::getRealNom(void) {
  char *monVraiNom;
  int n;

  if (taille < 0)
    n = (int)index->evaluate();
  else n = taille;

  monVraiNom = createName(nom, n) ;

  return monVraiNom;

}

//************************ NomDeTableau - methode evaluate ******************
float
NomDeTableau::evaluate(void){
  char *nomVar = getRealNom();
  Pointeur *pointeurBottom = pileDesPointeurs->getBottom();
  Pointeur *pointeur = pileDesPointeurs->getTop();
  Table *table;
  Entree *entree = (Entree *)NULL;
  
  do {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(nomVar);
    if (entree != (Entree *)NULL)
      break;

    pointeur = pointeur->getSuivant();
    
  } while (pointeur != pointeurBottom);
  
  if (entree == (Entree *)NULL){
    cout << "erreur : la variable " << nomVar << " n'est pas declaree !\n";
    exit(1);
  }
  return entree->toFloat();
}

//********************* VariableComplexe ************************************
VariableComplexe::VariableComplexe(VariableSimple *var1, VariableSimple *var2) : Variable((char *)NULL) { 

  liste = (NomListe *)NULL;
  liste2 = new VariableSimpleListe();
  addNomComposant(var1); 
  addNomComposant(var2); 
}

void
VariableComplexe::addNomComposant(VariableSimple *var) {
  liste2->addVariable(var);
}


Nom 
*VariableComplexe::getPremierVariable(void) {
  VariableSimple *var;

  // on commence par creer la liste des noms a partir des variables
  // dont on evalue le nom (getRealNom())
  liste = new NomListe();
  var = liste2->getPremier();
  
  while(var) {
    liste->addNom(var->getRealNom());
    var = var->getSuivant();
  }
  
  // on retourn le premier
  return liste->getPremier();
}


Nom 
*VariableComplexe::getDernierVariable(void) {
  if(!liste)
    return NULL;
  
  return liste->getDernier();
}

//********************* VariableComplexe - methode evaluate *****************
float 
VariableComplexe::evaluate(void){
  // 1- recherche montante de la racine de la VariableComplex (notation pointee)

  Nom *nomRacine = liste->getPremier();
  Pointeur *pointeurBottom = pileDesPointeurs->getBottom();
  Pointeur *pointeur = pileDesPointeurs->getTop();
  Table *table;
  Entree *entree = (Entree *)NULL;
  
  do {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(nomRacine->getNom());
    if (entree != (Entree *)NULL)
      break;
    
    pointeur = pointeur->getSuivant();
    
  } while (pointeur != pointeurBottom);
  
  if (entree == (Entree *)NULL){
    cout << "erreur : la variable " << nomRacine->getNom() << " n'est pas declaree !\n";
    exit(1);
  }
  
  // 2- recherche descendante de la bonne entree
  // le pointeur actuel est pointeur
  // on se trouve dans la Table table
  // avec l'Entree entree

  Nom *nomSuivant = nomRacine;
  
  while((nomSuivant = nomSuivant->getSuivant()) != NULL) {
    
    if((table = entree->getTable()) == (Table *)NULL) {
      cout << "erreur : " << nomRacine->getNom() << "..." << nomSuivant->getNom() << " n'existe pas\n";
      exit(0);
    }
    
    if((entree = table->getByNameEntree(nomSuivant->getNom())) == (Entree *)NULL) {
      cout << "erreur : la variable " << nomRacine->getNom() << " n'est pas declaree !\n";
      exit(1);
    }
  } 

  return entree->toFloat();
  
}

//****************************** ExpressionLogiqueListe *********************
void
ExpressionLogiqueListe::addExpressionLogique(ExpressionLogique *expression){
  if (premier == (ExpressionLogique *)NULL)
    premier = dernier = expression;
  else {
    dernier->suivant = expression;
    dernier = dernier->suivant;
  }
  dernier->suivant = (ExpressionLogique *)NULL;
}

ExpressionLogique
*ExpressionLogiqueListe::getElementRecursivement(ExpressionLogique *expression, int i){
  if(expression == (ExpressionLogique *)NULL)
    return (ExpressionLogique *)NULL;

  return getElementRecursivement(expression->suivant, i-1);
}

//***************************** VariableComplexe ****************************
// VariableSimple
// *VariableComplexe::getVariable(int i){
//   return liste->getElement(i);
// }

// VariableSimple
// *VariableComplexe::getPremierVariable(void){
//   return liste->getPremier();
// }

// VariableSimple
// *VariableComplexe::getDernierVariable(void){
//   return liste->getDernier();
// }

//***************************** Operations **********************************
//---------------------------- valeur absolue -------------------------------
float
Absolu::evaluate(void){
  float valeur = operande->evaluate();
  return (valeur > 0 ? valeur : -valeur);
}

//***************************** FIN 2 ****************************************


// ************************* les methodes triviales *************************
// --------------------------- PointeurPile --------------------------------
PointeurPile::PointeurPile(void){
  
  top = bottom = (Pointeur *)NULL;
  taille = 0;
}

void
PointeurPile::addPointeur(Pointeur *pointeur){
  taille++;

  if(top == (Pointeur *)NULL){
    top = bottom = pointeur;
    pointeur->pointeurSuivant = (Pointeur *)NULL;
    return;
  }

  pointeur->pointeurSuivant = top;	// on ajoute le pointeur sur le dessus
  top = pointeur;  
}

Table
*PointeurPile::takeTopPointeur(void){
  if(taille <= 0){
    cout << "*** erreur : pile vide\n" ;
    exit(1) ;
  }
    
  taille--;

  Pointeur *topPointeur = top;
  top = top->pointeurSuivant;

  return (topPointeur->getValeur());
}

Table
*PointeurPile::getTopPointeur(void){
  return top->getValeur();
}

// ------------------------------ Pointeur ---------------------------------
Pointeur::Pointeur(Table *table){
  
  valeur = table;
}

void
Pointeur::setValeur(Table *table){
  valeur = table;
}

Table 
*Pointeur::getValeur(void){
  return valeur;
}

Pointeur 
*Pointeur::getSuivant(void){
  return pointeurSuivant;
}

void
Pointeur::setSuivant(Pointeur *pointeur){
  pointeurSuivant = pointeur;
}

Pointeur *Pointeur::dupliquer()
{
  Pointeur *pointeur = new Pointeur(valeur);
  pointeur->setSuivant((Pointeur *)NULL);
  return pointeur;
}

// -------------------------- Table ----------------------------------------

Table::~Table()
{
  Entree *entree1 = premier;
  Entree *entree2 = premier->suivant;

  while (entree2 != (Entree *)NULL) {
    //delete entree1;
    entree1 = entree2;
    entree2 = entree2->suivant;
  }

  //delete entree1;
}

Entree *Table::getEntreeRecursivement(Entree *entree, int i)
{
  if (entree == (Entree *)NULL)
    return (Entree *)NULL;

  if(i == 0)
    return entree;

  return getEntreeRecursivement(entree->suivant, i-1);
}

char *Table::getEntreeNom(int i)
{
  Entree *entree = getEntree(i);
  if (!entree)
    return NULL;

  return entree->getNom();
}

char *Table::getEntreeValeur(int i)
{
  Entree *entree = getEntree(i);
  if(!entree)
    return NULL;
  
  return entree->toString();
}

void Table::addEntree(Entree *entree)
{
 if (premier == (Entree *)NULL)
    premier = dernier = entree;
  else {
    dernier->suivant = entree;
    dernier = dernier->suivant;
  }
 dernier->suivant = (Entree *)NULL;
}

void Table::removeDernierEntree()
{
  Entree *entree = premier;
  
  if (premier == (Entree *)NULL){
    cout << "erreur dans Table::removeDernierEntree() -> table vide\n";
    exit(1);
  }

  if (premier->suivant == (Entree *)NULL) {
    //delete premier;
    premier = dernier = (Entree *)NULL;
    return;
  }

  while (entree->suivant->suivant != (Entree *)NULL)
    entree = entree->suivant;
  
  //dax delete entree->suivant;
  entree->suivant = (Entree *)NULL;
  dernier = entree;
  return;
}

int Table::isFeuille()
{
  Entree *entree = premier;

  if (!entree) {
    cout << "la table " << nom << " est vide !\n";
    return 0;
  }

  do
    if (entree->table != 0)
      return 0;
  while ((entree = entree->suivant)!= NULL);
    
  return 1;
}

Entree *Table::getByNameEntree(char *nom)
{
  Entree *entree = premier;

  if (entree == (Entree *)NULL)
    return (Entree *)NULL;

  do
    if (!strcmp(entree->nom, nom))
      return entree;
  while ((entree = entree->suivant)!= NULL);
    
  return (Entree *)NULL;
}

int Table::checkNom(char *nom)
{
  int i = 0;
  Entree *entree = premier;

  do {
    if (!strcmp(entree->nom, nom))
      return i;
    i++;
  } while ((entree = entree->suivant)!= NULL);
    
  return -1;
}

Table *Table::dupliquer()
{
  Table *table = new Table(nom);
  Entree *entree = premier;
  Entree *entreeDupliquee;
  
  do {
    entreeDupliquee = entree->dupliquer();
    entreeDupliquee->setNom(entree->getNom());
    table->addEntree(entreeDupliquee);
  } while((entree = entree->suivant) != (Entree *)NULL);

  return table;
}


// ----------------------- les Entree ---------------------------------------
Entree::Entree() : table((Table *)NULL), suivant((Entree *)NULL) {}

Entree::Entree(char *monNom) : nom(monNom), table((Table *)NULL), suivant((Entree *)NULL) {}

// --------------------------------------------------------------------------
EntreeString::EntreeString(char *maValeur) : Entree()
{
  valeur = strdup(maValeur);
} 
EntreeString::EntreeString(char *monNom, char *maValeur) : Entree(monNom), valeur(maValeur) {}

float EntreeString::toFloat()
{
  cout << "erreur : float -> string\n";
  exit(0);
  return 0;
}

Entree *EntreeString::dupliquer()
{
  Entree *entree = new EntreeString(valeur);
  return entree;
}

Entree *EntreeString::dupliquer(char *_nom)
{
  Entree *entree = new EntreeString(_nom, valeur);
  return entree;
}

// --------------------------------------------------------------------------
EntreeFloat::EntreeFloat(char *monNom) : Entree(monNom), valeur(0) {}
EntreeFloat::EntreeFloat(float x) : valeur(x){} 
EntreeFloat::EntreeFloat(char *monNom, float x) : Entree(monNom), valeur(x) {}

char *EntreeFloat::toString()
{
  char *buf = (char *)malloc(100*sizeof(char));
  sprintf(buf,"%.2f",valeur);
  return buf;
}

Entree *EntreeFloat::dupliquer()
{
  Entree *entree = new EntreeFloat(valeur);
  return entree;
}

Entree *EntreeFloat::dupliquer(char *_nom)
{
  Entree *entree = new EntreeFloat(_nom, valeur);
  return entree;
}

// --------------------------------------------------------------------------
float EntreeNouvelObjet::toFloat()
{
  cout << "erreur : float -> string\n";
  exit(0);
  return 0;
}

EntreeNouvelObjet::EntreeNouvelObjet() : Entree() {;}
EntreeNouvelObjet::EntreeNouvelObjet(char *monNom) :Entree() {nom = monNom;}

void EntreeNouvelObjet::setTable(Table *maTable)
{
  table = maTable ;
}

Entree *EntreeNouvelObjet::dupliquer()
{
  EntreeNouvelObjet *entree = new EntreeNouvelObjet();
  entree->setTable(table->dupliquer()); 
  return (Entree *)entree;
}

Entree *EntreeNouvelObjet::dupliquer(char *_nom)
{
  EntreeNouvelObjet *entree = new EntreeNouvelObjet(_nom);
  entree->setTable(table->dupliquer()); 
  return (Entree *)entree;
}

// *********************** fin des methodes triviales ***********************

//Methodes de l'arbre 2

void Fichier::createTable()
{
  cout << "-> createTable de Fichier\n" ;
  if (!monPrg) {
    cout << "le programme ne contient pas de main() !";
    exit(0);
  }

  monPrg->createTable() ;
  cout << "<- createTable de Fichier\n" ;
}

void PointDEntree::createTable()
{
  cout << "-> createTable de PointDEntree\n" ;
  pileDesPointeurs = new PointeurPile();
  pileDesPointeurs->addPointeur(new Pointeur(tablePrincipal));

  Instruction *monInstruction = getPremierInstruction() ;
  while (monInstruction != NULL) {
    monInstruction->updateTable() ;
    monInstruction = monInstruction->getSuivant() ;
  }
}

void InstructionListe::updateTable()
{
  Instruction *monInstruction = premier ;
  while (monInstruction != NULL) {
    monInstruction->updateTable() ;
    monInstruction = monInstruction->getSuivant() ;
  }
}

void DeclarationSimple::updateTable()
{
  Table *tableCourante = pileDesPointeurs->getTopPointeur() ;	
  
  if (monNomDeTableau == NULL) { // Si c'est une liste de noms
    
    Nom *monNom = maNomListe->getPremier() ;
    while (monNom != NULL) {
      EntreeFloat *monEntree = new EntreeFloat(monNom->getNom()) ;
      tableCourante->addEntree(monEntree) ;
      monNom = monNom->getSuivant() ;
    }
  }
  else { // Si c'est un nom de tableau

    char *monNom = strdup(monNomDeTableau->getNom()) ;
    int taille = monNomDeTableau->getTaille() ;
    int n;

    if (taille < 0 ) {
      n =  monNomDeTableau->getIndex();
    } else n = taille;


    for (int i = 0 ; i < n ; i++) {
      char *monVraiNom = strdup(createName(monNom, i)) ; // createName("tab", 3) renvoie "tab[3]"
      EntreeFloat *monEntree = new EntreeFloat(monVraiNom) ;
      tableCourante->addEntree(monEntree) ;
    }
  }
}

void DeclarationComplexeChaine::updateTable()
{
  EntreeString *monEntreeString = new EntreeString(maChaine) ;
  maVariable->creer(monEntreeString) ;
}

void DeclarationComplexeExpression::updateTable()
{
  EntreeFloat *monEntreeFloat = new EntreeFloat(monExpression->evaluate()) ;
  maVariable->creer(monEntreeFloat) ;
}

void DeclarationComplexeNouvelObjet::updateTable()
{
  ////
}

void AssignationChaine::updateTable()
{
  EntreeString *monEntreeString = new EntreeString(maChaine) ;
  maVariable->substituer(monEntreeString) ;
}

void AssignationExpression::updateTable()
{
  EntreeFloat *monEntreeFloat = new EntreeFloat(monExpression->evaluate()) ;
  maVariable->substituer(monEntreeFloat) ;
}

void AssignationVariable::updateTable()
{
  // 1- rechercher la variable variablAAssigner et recuperer une duplication de l'Element

  Pointeur *pointeur = pileDesPointeurs->getTop();
  Entree *entree ;
  Table *table;
  
  do {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(variableAAssigner->getRealNom());
    if (entree != (Entree *)NULL)
      break;

    pointeur = pointeur->getSuivant();
    
  } while (pointeur != NULL);

  if(!entree) {
    cout << "la variable " << variableAAssigner->getRealNom() << " n'a pas ete trouvee.\n";
    exit(1);
  }

  // 2- et recuperer une duplication de l'Element
  Entree *entreeNouvelle = entree->dupliquer(maVariable->getNom());

  // 2- substituer dans la table courante
  maVariable->substituer(entreeNouvelle);
}

void AssignationNouvelObjet::updateTable()
{
  char *monObjetNom = monObjet->getNom() ;
  Structure *structure = fvrl->getStructure(monObjetNom) ;
  if(structure == NULL){
    cout << " erreur : " << monObjetNom << " non declaree\n";
    exit(0);
  }
  Constructeur *constructeur = structure->getConstructeur() ;

  AppelDeConstructeur *appel = constructeur->getAppelDeConstructeur();

  NomListe *arguments = constructeur->getNomListe();
  ExpressionListe *parametres = monObjet->getExpressionListe();
  
  if (arguments->getTaille() != (parametres->getTaille())) {
    cout << "le constructeur " << monObjetNom << "n'est pas appele avec le bon nombre d'arguments\n" ;
    exit(1) ;
  }
  
  AssignationListe *initialisations = new AssignationListe();
   
  Nom *arg = arguments->getPremier();
  Expression *param = parametres->getPremier();

  // creation d'une table provisoire ou seront stockes le resultat de l'appel au constructeur
  char * nomDeTable = (char *)malloc(100 * sizeof(char));
  strcat(nomDeTable, "Constructeur ");
  strcat(nomDeTable, monObjetNom);

   Table *tableTmp = new Table(nomDeTable);
   Table *tableFinale = new Table(monObjetNom);

   // on pointe dessus pour realiser des operations
   pileDesPointeurs->addPointeur(new Pointeur(tableTmp));

   // on y declare les attributs de la structure
   Declaration *attribut = structure->getPremierDeclaration();
   while(attribut) {
     attribut->updateTable() ;
     attribut = attribut->getSuivant() ;
    }

   // creation d'une assignation entre arguments et parametres
   while(arg) {	// traite aussi le cas ou il n'y a pas d'argument
     initialisations->addAssignation(param->creerAssignation(arg)); 
     arg = arg->getSuivant();
     param = param->getSuivant();
   }

   // on y declare les arguments du constructeur, puis on realise les assignations de la liste cree precedemment
   DeclarationSimple *declarerArgs = new DeclarationSimple(arguments); 
   declarerArgs->updateTable() ;

   Assignation *initialisation = initialisations->getPremier();
   while(initialisation) {
     initialisation->updateTable();
     initialisation = initialisation->getSuivant();
   }

   ///////  SI HERITAGE ////////
   // la methode update de AppelDeConstructeur realise les instructions dans la table
   // actuelle et change le nom de cette table actuelle. Elle prend le nom 
   // de la structure heritee

   if (appel) {
     // verification de l'implementation de l'heritage
     // recherche de la structure heritee
     Nom *nomTmp =  structure->getPremierNom();
     if (!nomTmp) {
       cout << "appel de constructeur dans " << monObjetNom << " sans heritage !\n";
       exit(1);
     }
     if(strcmp(nomTmp->getNom(), appel->getNom())) {
       cout << monObjetNom << " : incoherence entre appel de constructeur et heritage\n";
       exit(1);
     }

     // realisation de l'appel au constructeur
     appel->updateTable();
     tableFinale->setNom(pileDesPointeurs->getTopPointeur()->getNom());
   } 

   // on realise les instructions du constructeur
   Instruction *instruction = constructeur->getPremierInstruction() ;

   while(instruction) {
     instruction->updateTable() ;
     instruction = instruction->getSuivant() ;
   }
   
   // il faut maintenant creer la table correspondant a la structure a partir de la table temporaire
   // on depile la table temporaire
   tableTmp = pileDesPointeurs->takeTopPointeur();
   
   // on empile la table finale
    
   pileDesPointeurs->addPointeur(new Pointeur(tableFinale));

   // on declare les attributs dans la table finale
   attribut = structure->getPremierDeclaration();
   while (attribut) {
     attribut->updateTable() ;
     attribut = attribut->getSuivant() ;
   }

   if(appel)
     appel->declareAttribut();

   // on cherche dans la tableTmp les entrees correspondantes, on les duplique puis on les ajoute
   Entree *entreeFinale = tableFinale->getPremier();
   Nom *nomVar = new Nom((char *)"");
   Entree *entreeTmp;

   while(entreeFinale) {
     nomVar->setNom(entreeFinale->getNom()); 
     entreeTmp = tableTmp->getByNameEntree(entreeFinale->getNom());
     nomVar->substituer(entreeTmp->dupliquer());
     entreeFinale = entreeFinale->getSuivant();
   }

   // on depile
   tableFinale = pileDesPointeurs->takeTopPointeur();
   
   EntreeNouvelObjet *entreeNouvelle = new EntreeNouvelObjet();
   entreeNouvelle->setTable(tableFinale);
   
   maVariable->substituer(entreeNouvelle);

   return;
}

//////////////////////////////////////////////////
//   char *monObjetNom = monObjet->getNom() ;
//   Expression *monObjetExpression = monObjet->getPremierExpression() ;
//   Structure *maStructure = fvrl->getStructure(monObjetNom) ;
//   if(maStructure == NULL){
//     cout << " erreur : " << monObjetNom << " non declare\n";
//     exit(0);
//   }


//   Constructeur *monConstructeur = maStructure->getConstructeur() ;
//   Nom *monArgumentFormel = monConstructeur->getPremierNom() ;
//   AppelDeConstructeur *monAppel = monConstructeur->getAppelDeConstructeur() ;


//   if (monConstructeur->getNomListe()->getTaille() != (monObjet->getExpressionListe())->getTaille()) {
//     cout << "le constructeur " << monObjetNom << "n'est pas appele avec le bon nombre d'arguments\n" ;
//     exit(1) ;
//   }

//   PointeurPile *monAnciennePile = pileDesPointeurs->dupliquer() ;
//   EntreeNouvelObjet *monEntreeNouvelObjet = new EntreeNouvelObjet() ;
//   pileDesPointeurs = maVariable->substituer(monEntreeNouvelObjet) ;

//   Table *maTable = new Table(monObjetNom) ;
//   monEntreeNouvelObjet->setTable(maTable) ;

//   pileDesPointeurs->addPointeur(new Pointeur(maTable));
  
//   if (monAppel == NULL) {
//     Declaration *maDeclaration = maStructure->getPremierDeclaration() ;
//     while(maDeclaration) {
//       maDeclaration->updateTable() ;
//       maDeclaration = maDeclaration->getSuivant() ;
//     }
//     while(monArgumentFormel) {
//       NomListe *nomListe = new NomListe();
//       nomListe->addNom(monArgumentFormel->getNom());
//       maDeclaration = new DeclarationSimple(nomListe) ;
//       maDeclaration->updateTable() ;

//       Table *tableTmp = pileDesPointeurs->takeTopPointeur();/////
//       Nombre *nombre = new Nombre(monObjetExpression->evaluate());//////
//       pileDesPointeurs->addPointeur(new Pointeur(tableTmp));//////

//       AssignationExpression *monAssignationExpression = new AssignationExpression(monArgumentFormel,nombre) ;

//       monAssignationExpression->updateTable() ;


//       monArgumentFormel = monArgumentFormel->getSuivant() ;
//       monObjetExpression = monObjetExpression->getSuivant() ;
//     }
//     Instruction *monInstruction = monConstructeur->getPremierInstruction() ;
//     while(monInstruction) {
//       monInstruction->updateTable() ;
//       monInstruction = monInstruction->getSuivant() ;
//     }
//  }

//   pileDesPointeurs = monAnciennePile ;
//}

void Retour::updateTable()
{
 Table *tableCourante = pileDesPointeurs->getTopPointeur();
  tableCourante->addEntree(new EntreeFloat((char *)"return", monExpression->evaluate()));
  
  pileDesPointeurs->takeTopPointeur();
}

void BoucleIf::updateTable()
{
  if (monExpressionLogique->evaluate()) {
    if (getPremierInstructionSi() != NULL)
      maInstructionListeSi->updateTable();
  }
  else {
    if (maInstructionListeSinon != NULL)
      maInstructionListeSinon->updateTable();
  }
}

void BoucleFor::updateTable()
{
  Table *tableCourante = pileDesPointeurs->getTopPointeur();
  EntreeNouvelObjet *monEntree = new EntreeNouvelObjet((char *)"for") ;
  tableCourante->addEntree(monEntree) ;

  monAssignationExpressionDebut->updateTable() ;
  
  while (monExpressionLogiqueFin->evaluate()) {
    Table *maTable = new Table((char *)"for") ;
    monEntree->setTable(maTable) ;
    pileDesPointeurs->addPointeur(new Pointeur(maTable)); // pile->addPointeur(maTable) ;	je suis passe par la..(VG)
    monInstructionListe->updateTable() ;

    pileDesPointeurs->takeTopPointeur() ;
    monAssignationExpressionIncrementation->updateTable() ;
  }

  tableCourante->removeDernierEntree() ;
}

PointeurPile * Nom::substituer(Entree *monEntree)
{
  monEntree->setNom(nom) ;
  Table *tableConcernee = this->emplacement() ;
  tableConcernee->substituer(nom, monEntree) ;

  return pileDesPointeurs;
}

PointeurPile * NomDeTableau::substituer(Entree *monEntree)
{
  char *monVraiNom;
  int n;
  
  if (taille < 0)
    n = (int)index->evaluate();
  else n = taille;
  
  monVraiNom = createName(nom, n) ;

  monEntree->setNom(monVraiNom) ;
  Table *tableConcernee = this->emplacement() ;
  tableConcernee->substituer(monVraiNom, monEntree) ;

  return pileDesPointeurs;
}

// PointeurPile *
// VariableComplexe::substituer(Entree *monEntree) {
//  Nom *monNom = this->getPremierVariable() ;
//  Table *tableConcernee = monNom->emplacement() ;
//  monNom = monNom->getSuivant() ;
//  EntreeNouvelObjet *monEntreeNouvelObjet ;

//  while (monNom->getSuivant()) {
//    monEntreeNouvelObjet = (EntreeNouvelObjet *)tableConcernee->getByNameEntree(monNom->getNom()) ;
//    tableConcernee = monEntreeNouvelObjet->getTable() ;
//    monNom = monNom->getSuivant() ;
//  }
//  monEntree->setNom(monNom->getNom()) ;
//  tableConcernee->substituer(monNom->getNom(), monEntree) ;
// }

void Nom::creer(Entree *monEntree)
{
  monEntree->setNom(nom) ;
  Table *tableCourante = pileDesPointeurs->getTopPointeur();
  tableCourante->addEntree(monEntree) ;
}

void NomDeTableau::creer(Entree *monEntree)
{
  char *monVraiNom;
  int n;

  if (taille < 0)
    n = (int)index->evaluate();
  else n = taille;

  monVraiNom = createName(nom, n) ;
  monEntree->setNom(monVraiNom) ;
  Table *tableCourante = pileDesPointeurs->getTopPointeur();
  tableCourante->addEntree(monEntree) ;
}

void Table::substituer(char *monNom, Entree *monEntree)
{
  Entree *entreeCourante = premier ;
  char *nomCourant = entreeCourante->getNom() ;

  // 1- si c'est le premier...

  if (!strcmp(monNom, nomCourant)) {
    monEntree->setSuivant(entreeCourante->getSuivant()) ;
    premier = monEntree ;
  }

  // 2- sinon
  else {
    Entree *entreeSuivante = entreeCourante->getSuivant() ;
    while (entreeSuivante) {
      if (!strcmp(monNom, entreeSuivante->getNom())) {
	entreeCourante->setSuivant(monEntree) ;
	monEntree->setSuivant(entreeSuivante->getSuivant()) ;
	
	if(entreeSuivante == dernier)
	  dernier = monEntree;
      }
      entreeCourante = entreeSuivante ;
      entreeSuivante = entreeCourante->getSuivant() ;
    }
  }

  if(monEntree->suivant == NULL) 
    dernier = monEntree;
}

Table * Nom::emplacement()
{
  Pointeur *pointeur = pileDesPointeurs->getTop();
  Table *table;
  Entree *entree = (Entree *)NULL;
  while (pointeur != NULL) {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(nom);
    if (entree != (Entree *)NULL)
      return table ;
    pointeur = pointeur->getSuivant();
  }

  erreur(nom) ;
  return NULL;
}

PointeurPile * PointeurPile::inverser()
{
  PointeurPile *maPointeurPile = new PointeurPile() ;
  Pointeur *monPointeur = top;

  while (monPointeur) {
    maPointeurPile->addPointeur(monPointeur->dupliquer()) ;
    monPointeur = monPointeur->getSuivant() ;
  }
  return maPointeurPile ;
}

PointeurPile * PointeurPile::dupliquer()
{
  PointeurPile *interm = pileDesPointeurs->inverser();
  return interm->inverser() ;
}

Table * NomDeTableau::emplacement()
{
 char *monVraiNom;
  int n;

  if (taille < 0)
    n = (int)index->evaluate();
  else n = taille;

  monVraiNom = createName(nom, n) ;
  Nom *monNom = new Nom(monVraiNom) ;
  return monNom->emplacement() ;
}

PointeurPile * VariableComplexe::substituer(Entree *monEntree)
{
  // je duplique la pile actuelle (mise en memoire)
  PointeurPile *pileSauv = pileDesPointeurs->dupliquer();

  // je recupere la table actuelle
  Table *tableConcernee = pileDesPointeurs->getTopPointeur();

  // je retire le premier composant de la variable complexe (salle.mur1.x -> salle)
  Nom *nom = this->getPremierVariable() ;

  // je reitere sur le nomComposant suivant, tant qu'on n'est pas a la variable terminale
  do {
    if (nom->getSuivant() == NULL) 
      break;

      // je recupere un pointeur sur l'entree correspondante (si elle existe)
    EntreeNouvelObjet *entreeTmp = (EntreeNouvelObjet *)tableConcernee->getByNameEntree(nom->getNom());
    // si elle n'existe pas , je vais voir dans la table precedente
    // (on est peut etre dans une boucle for)
    if(!entreeTmp) {
      Table *tableInterm = ((pileDesPointeurs->getTop())->getSuivant())->getValeur();
      entreeTmp = (EntreeNouvelObjet *)tableInterm->getByNameEntree(nom->getNom());
    }

    if (!entreeTmp) {
      cout << "erreur : la variable " << nom << " n'existe pas dans " << tableConcernee->getNom() << "\n";
      exit(1);
    }
    // on descend
    tableConcernee = entreeTmp->getTable() ;
    pileDesPointeurs->addPointeur(new Pointeur(tableConcernee)) ;
    
  } while ((nom = nom->getSuivant()) != NULL);
  
  monEntree->setNom(nom->getNom()) ;
  tableConcernee->substituer(nom->getNom(), monEntree) ;

  // restauration de la pile originale
  pileDesPointeurs = pileSauv;

  return pileSauv ;
}
 
 //  PointeurPile *maPointeurPile ;
 //  maPointeurPile = monNom->emplacementPile() ;
 //  Table *tableConcernee = monNom->emplacement() ;
 //monNom = monNom->getSuivant() ;
//  EntreeNouvelObjet *monEntreeNouvelObjet ;

//  do{
//    if (monNom->getSuivant() == NULL) 
//       break;
//    monEntreeNouvelObjet = (EntreeNouvelObjet *)tableConcernee->getByNameEntree(monNom->getNom()) ;
//    tableConcernee = monEntreeNouvelObjet->getTable() ;
//    maPointeurPile->addPointeur(new Pointeur(tableConcernee)) ;
//  } while ((monNom = monNom->getSuivant()) != NULL);
//  monEntree->setNom(monNom->getNom()) ;
//  tableConcernee->substituer(monNom->getNom(), monEntree) ;
//  return maPointeurPile ;
 //}


PointeurPile * Nom::emplacementPile()
{
  PointeurPile *maPointeurPile = pileDesPointeurs->dupliquer() ;
  Pointeur *pointeur = pileDesPointeurs->getTop();
  Table *table;
  Entree *entree = (Entree *)NULL;
  while (pointeur != NULL) {
    table = pointeur->getValeur();
    entree = table->getByNameEntree(nom);
    if (entree != (Entree *)NULL)
      return maPointeurPile ;
    pointeur = pointeur->getSuivant() ;
    maPointeurPile->takeTopPointeur() ;
  }

  erreur(nom) ;
  return NULL ;
}


//***************************** FIN *****************************************

char *createName(char *monNom, int taille)
{
  char *nom = (char *)malloc(100 * sizeof(char));
  char *buff = (char *)malloc(10 * sizeof(char));

  strcat(nom, monNom);
  strcat(nom, "[");
  sprintf(buff, "%d", taille);
  strcat(nom, buff);
  strcat(nom, "]");
  
  //cout << "createName!!!!! ->  " << nom << "\n";
  return nom ;
}

int longueur (int taille)
{
  int x;
  cout << "longueur!!!! <-  " << taille << "\n";
  if ((x = taille/10))
    return 1+longueur(x) ;
  else
    return 1 ;
}

void erreur(char* nom)
{
  cout << "la variable " << nom << " n'existe pas !!!\n" ;
  exit(1) ;
}


//*********************** Parcours final ************************************
void Table::parcoursFinal()
{
  if (this->isFeuille()) {
    TokenListe *listeDesTokens = new TokenListe() ;
    Token *tokenResultat = listeDesTokens->readTable(this->nom) ;
    if (tokenResultat)
      tokenResultat->write2CFG(this) ;
    return ;
  }

  Table *tableSuivante ;
  Entree *entree = this->premier ;
  
  while (entree != NULL) {
    if ((tableSuivante = entree->getTable()) != NULL)
      tableSuivante->parcoursFinal();
    entree = entree->suivant;
  }
  return;
}

//***************************** FIN 3****************************************
