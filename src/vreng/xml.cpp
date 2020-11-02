//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
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
//
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "xml.hpp"


#if HAVE_LIBXML2

#define XML_ERROR -1
#define DETECTION_ERROR -2
#define ALL_OK 0
#define _XML_DEBUG_ 0
#define _XML_DTD_ 0


int Xml::string2Coord(char *strpos, float *tmpx, float *tmpy, float *tmpz, float *tmporiz)
{
  char *ptok;

  *tmpx = 0;
  *tmpy = 0;
  *tmpz = 0;
  *tmporiz = 0;

  ptok = strtok(strpos, ",");
  if (ptok) {
    *tmpx = (float) atof(ptok);
  }
  else return -4;

  ptok =  strtok(NULL, ",");
  if (ptok) {
    *tmpy = (float) atof(ptok);
  }
  else return -3;

  ptok =  strtok(NULL, ",");
  if (ptok) {
    *tmpz = (float) atof(ptok);
  }
  else return -2;

  ptok =  strtok(NULL, ",");
  if (ptok) {
    *tmporiz = (float) atof(ptok);
  }
  else return -1;

  return 0;
}


int Xml::returnXmlDocument(char* filename, xmlDocPtr* doc)
{
  /* charge le document XML */
  *doc = xmlParseFile(filename);

  if ((*doc) == NULL) {
    error("unable to parse file \"%s\"", filename);
    return XML_ERROR;
  }

  return ALL_OK;
}


int Xml::selectXpathPoint(char *filename, float *position, char *type, char *result)
{
  xmlDocPtr doc;
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  const xmlChar *xpathExpr;
  char* xpathExpr_=strdup("//*/");

  strcat(xpathExpr_,type);
  xpathExpr = (const xmlChar *) xpathExpr_;

  /* charge le document XML */
  doc = xmlParseFile(filename);

  if (doc == NULL) {
    error("unable to parse file \"%s\"", filename);
    return XML_ERROR;
  }

  /* Creation du contexte d'evaluation */
  xpathCtx = xmlXPathNewContext(doc);
  if (xpathCtx == NULL) {
    error("unable to create new XPath context");
    xmlFreeDoc(doc);
    return XML_ERROR;
  }

  /* Evalue l'expression de xpathExpr ds le contexte xpathCtx */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if (xpathObj == NULL) {
    error("unable to evaluate xpath expression \"%s\"", xpathExpr);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    return XML_ERROR;
  }

  int detecterror = returnXpathPoint(xpathObj->nodesetval, position, result);

  free(xpathExpr_);
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  xmlFreeDoc(doc);

  return detecterror;
}


int Xml::returnXpathPoint(xmlNodeSetPtr nodes, float *position, char *result)
{
  return ALL_OK;
}


int Xml::selectProximity(char *filename,char *type, float *posx,float *posy,float *posz,float *ori)
{
  xmlDocPtr doc;
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  const xmlChar *xpathExpr;
  char* xpathExpr_=strdup("//");

  strcat(xpathExpr_,type);
  xpathExpr = (const xmlChar *) xpathExpr_;

  /* charge le document XML */
  doc = xmlParseFile(filename);
  if (doc == NULL) {
    error( "unable to parse file \"%s\"", filename);
    return XML_ERROR;
  }
  /* Creation du contexte d'evaluation */
  xpathCtx = xmlXPathNewContext(doc);
  if (xpathCtx == NULL) {
    error("unable to create new XPath context");
    xmlFreeDoc(doc);
    return XML_ERROR;
  }
  /* Evalue l'expression de xpathExpr ds le contexte xpathCtx */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if (xpathObj == NULL) {
    error("unable to evaluate xpath expression \"%s\"", xpathExpr);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    return XML_ERROR;
  }

  int detecterror = returnProximity(xpathObj->nodesetval, &(*posx),&(*posy),&(*posz),&(*ori));

  free(xpathExpr_);
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  xmlFreeDoc(doc);

  return detecterror;
}


int Xml::returnProximity(xmlNodeSetPtr nodes, float *posx, float *posy, float *posz, float *ori)
{
  xmlNodePtr cur;
  // xmlNodePtr tmp, soliddim;
  xmlAttrPtr tmpattr;
  char *foundpos;
  char *founddim;
  float positx = 0,tmpx =0;
  float posity = 0,tmpy =0;
  float positz = 0,tmpz =0;
  float orientz = 0 , tmporiz =0;
  double distance = MAXFLOAT;
  double tmpdistance = MAXFLOAT;
  bool found = false;

  int size = nodes ? nodes->nodeNr : 0;

  foundpos = strdup("N/A");
  founddim = strdup("0.0,0.0,0.0");

  if ((nodes == NULL)||(size < 1)){
    error("no xml nodes");
    return DETECTION_ERROR;
  }

  for (int i=0; i<size; ++i) {
    if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];
      tmpattr=cur->properties;
      found = false;
      /* recherche de la position la plus proche */
      while (tmpattr && !found) {
	if (!strncmp((const char*) tmpattr->name, "pos", 3)) {
	  found = true;

	  foundpos = strdup((const char*) tmpattr->children->content);
	  string2Coord(foundpos, &tmpx, &tmpy, &tmpz, &tmporiz);

	  /* Calcule de la distance a l'objet */
	  tmpdistance = sqrt(((tmpx-(*posx))*(tmpx-(*posx)))
			   + ((tmpy-(*posy))*(tmpy-(*posy)))
			   + ((tmpx-(*posz))*(tmpx-(*posz))));

	  /* Si l'objet est plus proche on le retient */
	  if (tmpdistance < distance) {
	    distance = tmpdistance;
	    positx = tmpx;
	    posity = tmpy;
	    positz = tmpz;
	    orientz = tmporiz;
	  }
	}
        else {
	  tmpattr = tmpattr->next;
	}
      }
    }
  }
  free(foundpos);
  free(founddim);

  /* Si on a bien trouve un objet */
  if (distance < MAXFLOAT) {
    (*posx) = positx;
    (*posy) = posity;
    (*posz) = positz;
    (*ori)  = orientz;
    return ALL_OK;
  }
  return DETECTION_ERROR;
}


int Xml::selectXpathExpr(char *filename, const char *xpathExpr_, char *phrase, char *result)
{
  xmlDocPtr doc;
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;
  const xmlChar *xpathExpr;
  char str[200];

  if (! strcmp(xpathExpr_, "//*/@name")) {
    sprintf(str, "//*/*[@name=\"%s\"]", phrase);
    xpathExpr = (const xmlChar *) str;
  }
  else
    xpathExpr = (const xmlChar *) xpathExpr_;

  /* charge le document XML */
  doc = xmlParseFile(filename);
  if (doc == NULL) {
    error("unable to parse file \"%s\"", filename);
    return XML_ERROR;
  }
  /* Creation du contexte d'evaluation */
  xpathCtx = xmlXPathNewContext(doc);
  if (xpathCtx == NULL) {
    error("unable to create new XPath context");
    xmlFreeDoc(doc);
    return XML_ERROR;
  }
  /* Evalue l'expression de xpathExpr ds le contexte xpathCtx */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if (xpathObj == NULL) {
    error("unable to evaluate xpath expression \"%s\"", xpathExpr);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    return XML_ERROR;
  }

  /* Lancement de l'analyse sur les resultats */
  int detecterror;
  if (! strcmp(xpathExpr_, "//*/@name") && (xpathObj) && (xpathObj->nodesetval->nodeNr == 1)) {
    detecterror = returnXpathName(xpathObj->nodesetval, result);
  }
  else {
    detecterror = returnXpathExpr(xpathObj->nodesetval, phrase, result);
  }

  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  xmlFreeDoc(doc);

  return detecterror;
}


int Xml::returnXpathExpr(xmlNodeSetPtr nodes, char *phrase, char *result)
{
  xmlNodePtr cur, tmp, soliddim;
  xmlAttrPtr tmpattr;
  char *foundpos;
  char *founddim;
  float positx = 0;
  float posity = 0;
  float positz = 0;
  float orientz = 0;
  bool found = false;

  int size = nodes ? nodes->nodeNr : 0;

  foundpos = strdup("N/A");
  founddim = strdup("0.0,0.0,0.0");

  if ((nodes == NULL) || (size < 1)) {
    error("no xml nodes");
    return DETECTION_ERROR;
  }

  for (int i=0; i<size; ++i) {
    if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
      cur = nodes->nodeTab[i];

      found = false;
      if (! strcmp((const char*) cur->children->content, phrase)) {
	tmp = cur->next;
	while (tmp && !found) {
	  if ((!strcmp((const char*) tmp->name, "position")) || (!strcmp((const char*) tmp->name, "pos"))) {
	    found = true;
	    foundpos = strdup((const char*) tmp->children->content);
	    break;
	  }
	  tmp = tmp->next;
	}
	
	tmp = cur->prev;
	while (tmp && !found) {
	  if ((!strcmp((const char*) tmp->name, "position")) || (!strcmp((const char*) tmp->name, "pos"))) {
	    found = true;
	    foundpos = strdup((const char*) tmp->children->content);
	    break;
	  }
	  tmp = tmp->prev;
	}
	
	/* le nom de la table et la position sont trouvés,
	   il reste a trouver les dimensions de la table */
	/* si on est pas sur un solid on part a l'interieur de
	   la description de l'objet pour rechercher un solid */
	tmp = cur->parent;
	
	soliddim = NULL;
	if (strcmp((const char*) tmp->name, "solid")) {
	  //on remonte sur l'objet
	  found = false;
	  tmp = tmp->children;
	  while (tmp && !found) {
	    if (! strcmp((const char*) tmp->name, "solid")) {
	      found = true;
	      soliddim = tmp;
	      break;
	    }
	    tmp = tmp->next;
	  }
	}
	tmp = soliddim;
	
	/* Maintenant que le solid est trouvé on cherche la dimension */
	tmpattr = tmp->properties;
	found = false;
	while (tmpattr && !found) {
	  if ((!strcmp((const char*) tmpattr->name, "dimension")) || (!strcmp((const char*) tmpattr->name, "dim"))) {
	    found = true;
	    founddim = strdup((const char*) tmpattr->children->content);
	    break;
	  }
	  tmpattr = tmpattr->next;
	}
      }
    }
  }

  /* Pour le moment nous ne tenons pas compte de l'orientation de l'objet en question*/
  if ((foundpos) && (strcmp(foundpos, "N/A"))) {
    string2Coord(foundpos, &positx, &posity, &positz, &orientz);
    sprintf(result, "%.2f,%.2f,%.2f,%.2f,%s", positx, posity, positz, orientz, founddim);
    free(foundpos);
    free(founddim);
    return ALL_OK;
  }
  else {
    sprintf(result, "N/A");
    free(foundpos);
    free(founddim);
    return DETECTION_ERROR;
  }
  free(foundpos);
  free(founddim);

  return ALL_OK;
}


int Xml::returnXpathName(xmlNodeSetPtr nodes, char *result)
{
  xmlNodePtr cur, tmp, soliddim;
  xmlAttrPtr tmpattr,tmpattr2;
  char *foundpos;
  char *founddim;
  float positx = 0;
  float posity = 0;
  float positz = 0;
  float orientz = 0;
  bool found = false;

  int size = nodes ? nodes->nodeNr : 0;

  foundpos = strdup("N/A");
  founddim = strdup("0.0,0.0,0.0");

  if (!nodes || (size != 1)) {
    error("%d NODES problem: founded 2 same object name !", size);
    return DETECTION_ERROR;
  }

  if (nodes->nodeTab[0]->type == XML_ELEMENT_NODE) {
    cur = nodes->nodeTab[0];

    found = false;
    tmpattr2 = cur->properties;
    while (tmpattr2 && !found) {
      if ((!strcmp((const char*) tmpattr2->name, "position")) || (!strcmp((const char*) tmpattr2->name, "pos"))) {
	found = true;
	foundpos = strdup((const char*) tmpattr2->children->content);
	break;
      }
      tmpattr2 = tmpattr2->next;
    }
    /* le nom de la table et la position sont trouvés,
       il reste a trouver les dimensions de la table */
    /* si on est pas sur un solid on part a l'interieur de
       la description de l'objet pour rechercher un solid */
    tmp = cur;

    soliddim = NULL;
    if (strcmp((const char*) tmp->name, "solid")) {
      //on remonte sur l'objet
      found = false;
      tmp = tmp->children;
      while (tmp && !found) {
	if (! strcmp((const char*) tmp->name, "solid")) {
	  found = true;
	  soliddim = tmp;
	  break;
	}
	tmp = tmp->next;
      }
    }
    tmp = soliddim;

    /* Maintenant que le solid est trouvé on cherche la dimension */
    tmpattr = tmp->properties;
    found = false;
    while (tmpattr && !found) {
      if ((!strcmp((const char*) tmpattr->name, "dimension")) || (!strcmp((const char*) tmpattr->name, "dim"))) {
	found = true;
	founddim = strdup((const char*) tmpattr->children->content);
	break;
      }
      tmpattr = tmpattr->next;
    }
  }

  /* Pour le moment nous ne tenons pas compte de l'orientation de l'objet en question*/
  if ((foundpos) && (strcmp(foundpos,"N/A"))) {
    string2Coord(foundpos, &positx, &posity, &positz, &orientz);
    sprintf(result, "%.2f,%.2f,%.2f,%.2f,%s", positx, posity, positz, orientz, founddim);
    free(foundpos);
    free(founddim);
    return ALL_OK;
  }
  else {
    sprintf(result, "N/A");
    free(foundpos);
    free(founddim);
    return DETECTION_ERROR;
  }
  free(foundpos);
  free(founddim);

  return ALL_OK;
}


void Xml::dtdValidation(const char *pathname)
{
  /* On precise ici le mode de verification avec DTD */
  xmlTextReaderPtr xmltextptr = xmlReaderForFile(pathname, NULL,
			        XML_PARSE_DTDATTR |  // DTD attributes
			        XML_PARSE_NOENT |    // substitute entities
			        XML_PARSE_DTDVALID); // validation avec la DTD
  if (xmltextptr != NULL) {
    int ret;
    bool dtderr = false;
    char *filename = (char *) strrchr(pathname, '/');
    filename++;

    do {
      ret = xmlTextReaderRead(xmltextptr);
#if _DEBUG_DTD_
      processNode(xmltextptr);
#endif
    } while (ret == 1);

    /* Once the document has been fully parsed check the validation results */
    if (xmlTextReaderIsValid(xmltextptr) != 1) {
      warning("%s does not validate DTD!", filename);
      ret = 0;
      dtderr = true;
    }
    xmlFreeTextReader(xmltextptr);
    if (ret != 0) {
      warning("%s : failed to parse", filename);
      dtderr = true;
    }
    if (!dtderr)
      notice("%s is valid!", filename);
  }
  else {
    error("can't open %s", pathname);
  }
  xmlCleanupParser();
  xmlMemoryDump();
  return;
}

#if _DEBUG_DTD_ //evdebug DTD
void Xml::processNode(xmlTextReaderPtr reader)
{
  const xmlChar *name, *value;

  name = xmlTextReaderConstName(reader);
  if (name == NULL)
      name = BAD_CAST "--";

  value = xmlTextReaderConstValue(reader);

  printf("%d %d %s %d %d",
	 xmlTextReaderDepth(reader),
	 xmlTextReaderNodeType(reader),
         name,
         xmlTextReaderIsEmptyElement(reader),
         xmlTextReaderHasValue(reader));
  if (value == NULL)
    printf("\n");
  else {
    if (xmlStrlen(value) > 40)
      printf(" %.40s...\n", value);
    else
      printf(" %s\n", value);
  }
}
#endif //evdebug DTD

#endif //HAVE_LIBXML2
