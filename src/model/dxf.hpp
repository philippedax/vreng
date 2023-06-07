//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
//---------------------------------------------------------------------------
/*------------------------------------------------------------------------
                                /\
                             __/__\__
                            /\      /\
                           /  \    /  \
                          /    \  /    \
                         /______\/______\
                          /____________\
                          M   M OOOO X  X
                          M M M 0  0  XX
                          M   M OOOO X  X

          MOX - Centro de Computacion Avanzada en Ingenieria.
                        Facultad de Ingenieria
                       Universidad de los Andes
                         Bogota, Colombia.
------------------------------------------------------------------------*/
#ifndef DXF_HPP
#define DXF_HPP
/*************************************************************************
  Nombre: common3D.h      Version: 0.5                  Fecha: Abril-1996
  Descripcion:
     Funciones para la matematica de puntos en 3D, asi como la
     definicion del estandar de putno flotante utilizado por la libreria
     tambien se ecuentran las funciones para el manejo de color por RGB.
  Tipos/Clases:
     FPOINT    objeto de punto flotante.
     Point3D   Punto en el espacio R3.
     SRGB      Estructura para el manejo del espacio de color RGB.
  Funciones:
     Las funciones definidas en este modulo estan definidas por cada tipo
     de objeto (ver mas adelantre en la definicion de cada uno).
  Observaciones:
     FPOINT corresponde al tipo estandar double, pero se puede modificar
     facilmente a otro formato (p.ej. punto fijo para eficiencia, todo
     programa que utilize la libreria al declarar una variable de punto
     flotante deberia utilizar este objeto para consistencia.

     SRGB corraponde al manjo del espacio RGB, pero por el momento este
     equivale a un punto 3D (3 valores de tipo FPOINT), por lo tanto esta
     clase se manipula por medio de las de la clase Point3D.
 Autor: Daniel Barrero                                  Fecha: Abril-1996
**************************************************************************/

/*------------------  Object Point3D -----------------------------------*/
typedef struct Point3D_s {
  float x;
  float y;
  float z;
} Point3D;

#define FEQ(x,y)	(fabs(x-y)<=(float)1e-06)
Point3D * newPoint3D(float x, float y, float z);
Point3D * deletePoint3D(Point3D *p);
Point3D * setPoint3D(Point3D *p, float x, float y, float z);
Point3D * copyPoint3D(Point3D *p1, Point3D *p2);
Point3D * addPoint3D(Point3D *p1, Point3D *p2);
Point3D * subPoint3D(Point3D *p1, Point3D *p2);
#define multPoint3D(p1,p2)   ((p1!=NULL && p2!=NULL)?((p1->x*p2->x)+(p1->y*p2->y)+(p1->z*p2->z)):0;
Point3D * addPoint3DFPOINT(Point3D *p, float s);
Point3D * multPoint3DFPOINT(Point3D *p, float f);
#define distPoint3D(p1,p2)   ((p1!=NULL && p2!=NULL)?(sqr(sqr(p2->x-p1->x) + sqr(p2->y-p1->y) + sqr(p2->z-p1->z)))?-1)
#define eqPoint3D(p1,p2)     ((p1==p2)?TRUE:((p1!=NULL && p2!=NULL) && (FEQ(p1->x,p2->x) && FEQ(p1->y,p2->y) && FEQ(p1->z,p2->z))))

/* Object SRGB */
#define SRGB   Point3D

/*------------------  Object Surface  ----------------------------------*/
typedef struct Surface_s {
  float ambient;	///< ambient
  float diffuse;	///< diffuse
  float brilliance;	///< brilliance
  float specular;	///< specular RGB
  float roughness;	///< specular coef
  float reflection;	///< reflection 0.0 - 1.0
  float refraction;	///< refraction 0.0 - 1.0
} Surface;

Surface * newSurface(float ambient,float diffuse, float brilliance,
                     float specular, float roughness, float reflection,
                     float refraction);
Surface * deleteSurface( Surface *surf);
Surface * setSurface(Surface *surf, float ambient,float diffuse,
                     float brilliance, float specular ,float roughness,
                     float reflection, float refraction);
Surface * copySurface(Surface *surf1, Surface *surf2);

typedef enum { START         = 0,
	       TEXT          = 1,
	       NAME0         = 2,
	       NAME1         = 3,
	       NAMEN         = 4,
	       ENTITY_HANDLE = 5,
	       LINE_TYPE     = 6,
	       TEXT_STYLE    = 7,
	       LAYER_NAME    = 8,
	       VAR_NAME      = 9,
	       X0            = 10,
	       X1            = 11,
	       X2            = 12,
	       X3            = 13,
	       X4            = 14,
	       X5            = 15,
	       X6            = 16,
	       X7            = 17,
	       XN            = 18,
	       Y0            = 20,
	       Y1            = 21,
	       Y2            = 22,
	       Y3            = 23,
	       Y4            = 24,
	       Y5            = 25,
	       Y6            = 26,
	       Y7            = 27,
	       YN            = 28,
	       Z0            = 30,
	       Z1            = 31,
	       Z2            = 32,
	       Z3            = 33,
	       Z4            = 34,
	       Z5            = 35,
	       Z6            = 36,
	       ZN            = 37,
	       ELEVATION     = 38,
	       THICKNESS     = 39,
	       FLOATVAL0     = 40,
	       FLOATVAL1     = 41,
	       FLOATVAL2     = 42,
	       FLOATVAL3     = 43,
	       FLOATVAL4     = 44,
	       FLOATVAL5     = 45,
	       FLOATVAL6     = 47,
	       FLOATVAL7     = 47,
	       FLOATVALN     = 48,
	       REPEAT        = 49,
	       ANG0          = 50,
	       ANG1          = 51,
	       ANG2          = 52,
	       ANG3          = 53,
	       ANG4          = 54,
	       ANG5          = 55,
	       ANG6          = 56,
	       ANG7          = 57,
	       ANGN          = 58,
	       COLORNUM      = 62,
	       ENTITIES      = 66,
	       ENT_SPAC      = 67,
	       VIEWPORT_STATE= 68,
	       VIEWPORT_ID   = 69,
	       FLAGS70      = 70,
	       FLAGS71      = 71,
	       FLAGS72      = 72,
	       FLAGS73      = 73,
	       FLAGS74      = 74,
	       FLAGS75      = 75,
	       FLAGS76      = 76,
	       FLAGS77      = 77,
	       FLAGS7N      = 78,
	       EXTRUC_DIR_X = 210,
	       EXTRUC_DIR_Y = 210,
	       EXTRUC_DIR_Z = 210,
               COMMENT       = 999,
               STRING        = 1000,
               REG_AP_NAME   = 1001,
               EXT_ENTITIE_ST= 1002,  /*   puede ser { o } */
               EXT_ENTITIE_X_CORD = 1010,
               EXT_ENTITIE_Y_CORD = 1020,
               EXT_ENTITIE_Z_CORD = 1030,
               EXT_ENTITIE_X_POS  = 1011,
               EXT_ENTITIE_Y_POS  = 1021,
               EXT_ENTITIE_Z_POS  = 1031,
               EXT_ENTITIE_X_DISP = 1012,
               EXT_ENTITIE_Y_DISP = 1022,
               EXT_ENTITIE_Z_DISP = 1032,
               EXT_ENTITIE_X_DIR  = 1013,
               EXT_ENTITIE_Y_DIR  = 1023,
               EXT_ENTITIE_Z_DIR  = 1033,
               EXT_ENTITIE_FLOAT  = 1040,
               EXT_ENTITIE_DIST   = 1041,
               EXT_ENTITIE_SCALE  = 1042,
               EXT_ENTITIE_16INT  = 1070,
               EXT_ENTITIE_32LONG = 1071,
               DXF_FILE           = 9999
} DXF_gr_id;

/* limpia un string de los tokens dados (lo compacta).*/
char *_xstrclean(char *in,char *tok);
/* convierte un srting a lowercase (minusculas) */
char *_xstrlwr(char *in);
/* convierte un srting a uppercase (mayusculas) */
char *_xstrupr(char *in);

/*------------------  Object Polygon -----------------------------------*/
#define POL_CLOSED        0
#define POL_OPENED        1
#define POL_POINT_VALS    0
#define POL_POINT_REFS    1

typedef struct Polygon_s {
  char pol_type;	///< Polygon type: CLOSED (default), OPENED (polyline)
  union {
     Point3D *pnts;
     Point3D **refs;
  } points;		///< List of points
  char points_type;	///< List of POINT_VALS or POINT_REFS
  int num_points;	///< Number of points
  SRGB *color;		///< Polygon color
  Surface *surface;	///< Surface
} Polygon;

/* Creator / Destructor Functions Object Polygon */
Polygon * newPolygon(char pol_type, char points_type, SRGB *color);
Polygon * deletePolygon(Polygon *pol);

int addPolygonPoint(Polygon *pol, Point3D *p);
int delPolygonPoint(Polygon *pol);
int insertPolygonPoint(Polygon *pol, Point3D *p, int pos);
int removePolygonPoint(Polygon *pol, int pos);
bool changePolygonType(Polygon *pol, char pol_type);
bool changePoligonPointsType(Polygon *pol, char points_type);

Point3D * maxPolygon(Polygon *pol);
Point3D * minPolygon(Polygon *pol);

/*------------------  Object PolyMesh ----------------------------------*/
typedef struct PolyMeshObj_s {
  char *name;		///< Name of the polymesh object
  Point3D **points;	///< Points of the polymesh
  int num_points;	///< Number of points in the polymesh
  Polygon **polygons;	///< Polygons contructing the mesh
  int num_polygons;	///< Number of polygons constructiong the mesh
  SRGB *color;		///< General color of the polymesh
  Surface *surface;	///< Surface
} PolyMeshObj;

/* Creator / Destructor Funtions Object Polymesh) */
PolyMeshObj * newPolyMesh(char *name, SRGB *color);
PolyMeshObj * deletePolyMesh(PolyMeshObj *pmesh);
PolyMeshObj * changePolyMeshObjName(PolyMeshObj *pmesh, char *name);
int addPolyMeshObjPolygon(PolyMeshObj *pmesh, Polygon *pol);
int delPolyMeshObjPolygon(PolyMeshObj *pmesh);
int insertPolyMeshObjPolygon(PolyMeshObj *pmesh, Polygon *pol, int pos);
int removePolyMeshObjPolygon(PolyMeshObj *pmesh, int pos);
int addPolyMeshObjPoint(PolyMeshObj *pmesh, Point3D *p);
int delPolyMeshObjPoint(PolyMeshObj *pmesh);
int insertPolyMeshObjPoint(PolyMeshObj *pmesh, Point3D *p, int pos);
int removePolyMeshObjPoint(PolyMeshObj *pmesh, int pos);
int addPolyMeshObjPointEx(PolyMeshObj *pmesh, Point3D *p);

int addPolyMeshObjPolygonPoint(PolyMeshObj *pmesh, Point3D *p);
int delPolyMeshObjPolygonPoint(PolyMeshObj *pmesh);
int insertPolyMeshObjPolygonPoint(PolyMeshObj *pmesh, int pol_num, Point3D *p, int point_num);
int removePolyMeshObjPolygonPoint(PolyMeshObj *pmesh, int pol_num, int point_num);

Point3D * maxPolyMeshObj(PolyMeshObj *pmesh);
Point3D * minPolyMeshObj(PolyMeshObj *pmesh);

/*------------------  Object SceneObj ----------------------------------*/
typedef struct SceneObj_s {
     char *name;            /* Name of the scene */
     PolyMeshObj **objects;  /* Objects of the scene */
     int num_objects;        /* Number of objects in the scene */
     SRGB *color;            /* General ambient color of the scene  */
} SceneObj;

/* Creator / Destructor Funtions Object Polymesh) */
SceneObj * newScene(char *name, SRGB *color);
SceneObj * deleteScene(SceneObj *scene );
SceneObj * changeSceneObjName(SceneObj *scene, char *name);
int addSceneObjPolyMesh(SceneObj *scene, PolyMeshObj *pmesh);
int delSceneObjPolyMesh(SceneObj *scene);
int insertSceneObjPolyMesh(SceneObj *scene, PolyMeshObj *pmesh, int pos);
int removeSceneObjPolyMesh(SceneObj *scene, int pos);

Point3D * maxSceneObj(SceneObj *scene);
Point3D * minSceneObj(SceneObj *scene);

/*------------------ Object DXF_token ----------------------------------*/
typedef struct DXF_token_s {
  DXF_gr_id gr_id;	///< Group ID
  char *data;		///< Token Data
  int datalen;		///< lenght of data
} DXF_token;

DXF_token * newTokenDXF(DXF_gr_id gr_id, const char *data, int datalen);
DXF_token * deleteTokenDXF(DXF_token *tok);
DXF_token * readTokenDXF(FILE *fp);
DXF_token * writeTokenDXF(FILE *fp, DXF_token *tok);
#define TOK_EQ      1
#define TOK_GR_EQ   2
#define TOK_DATA_EQ 3
char eqTokenDXF(DXF_token *tok1,DXF_token *tok2);

/*------------------ Object DXF_file -----------------------------------*/
typedef struct DXF_file_s {
  char *filename;
  FILE *fp;
  SceneObj *objects;///< Scene containing the list of objects in polymeshed form
} DXF_file;

DXF_file * newDXF(char *filename);
DXF_file * deleteDXF(DXF_file *dxf);
DXF_file * openDXF(DXF_file *dxf);
DXF_file * closeDXF(DXF_file *dxf);
DXF_file * readDXF(DXF_file *dxf);

/*------------------ Object DXF_rule -----------------------------------*/
#define funcRule(f)         char (*f)(DXF_file *,DXF_rule *, DXF_token *)

typedef struct DXF_rule_s DXF_rule;

struct DXF_rule_s {
  DXF_token *tok;
  int num_rules;
  DXF_rule **lrules;
  int act_rule;
  funcRule(parse);
};

DXF_rule * newRuleDXF(DXF_token *tok, funcRule(parse));
DXF_rule * deleteRuleDXF( DXF_rule *rule);
int addRuleDXF(DXF_rule *rule, DXF_rule *rul);
int delRuleDXF(DXF_rule *rule);
DXF_rule * initParserRulesDXF();
DXF_file * parseDXF(DXF_file *dxf, DXF_rule *rule);

char parseDXFfile(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFHEADER(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFENTITIES(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFBLOCKS(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXF3DFACE(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFXVALS(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFYVALS(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);
char parseDXFZVALS(DXF_file *dxf, DXF_rule *rule, DXF_token *tok);


/**
 * Dxf class
 */
class Dxf {

 public:
  DXF_file *dxffile;

 private:
  bool loaded;                  ///< flag loaded or not
  float currentScale;           ///< current scale
  float desiredScale;           ///< desired scale
  GLint dlist;                  ///< display list
  char *url;                    ///< dxf url
  bool flgcolor;        	///< flag color setted
  float mat_diffuse[4];		///<
  float mat_ambient[4]; 	///<
  float mat_specular[4];	///<
  int texture;			///<

 private:
  const char * getUrl() const;
  /**< get an Url */

  static void reader(void *adxf, class Http *http);
  /**< Downloads */

 public:
  Dxf(const char *url);

  virtual ~Dxf();

  void setScale(float scale);
  /**< Sets the scale */

  void setColor(int type, float *color);
  /**< Sets the color */

  float getScale();
  /**< Gets the spheric BB */

  GLint displaylist();
  /**< Draws in displaylist */

  bool draw(DXF_file *dxffile);
  /**< Draws */

  void render();
  /**< Renders */
};

#endif
