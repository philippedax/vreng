#include <GL/glut.h>

#include "ui.hpp"
#include "vred.hpp"
#include "vre.hpp"
#include "safe.hpp"
#include "raycaster.hpp"


int UI::mainWin;
const char *UI::objectTypes[] = { "Wall", "Gate", "Step", "Web", "Board", "Host", "Doc", "Earth" };
int UI::currentObject = 0;
Solid *UI::item = NULL;
struct objectChain *UI::selected = NULL;
float UI::center[3] = {0., 0., 0.}, UI::centerXButton = 0;
float UI::size[3];
float UI::angleZ;
float UI::radius;
char UI::urlXp[128]="", UI::urlXn[128]="", UI::urlYp[128]="", UI::urlYn[128]="";
char UI::urlZp[128]="", UI::urlZn[128]="";
float UI::dif[3], UI::amb[3], UI::shi[3], UI::spe[3];
char UI::url[128]="", UI::ipmc[32]="";
int UI::collis = true;
Camera *UI::camera;
bool UI::motionEnabled = false;
int UI::mouseX, UI::mouseY;
char *UI::dialStr = NULL;
int UI::dialUsage;

GLUI *UI::topWin, *UI::sideWin, *UI::botWin, *UI::dialGlui=NULL;
GLUI_EditText *UI::dialText;
GLUI_EditText *UI::centerGlui[3];
GLUI_EditText *UI::sizeGlui[3];
GLUI_EditText *UI::angleZGlui;
GLUI_EditText *UI::radiusGlui;
GLUI_EditText *UI::difGlui[3], *UI::ambGlui[3], *UI::shiGlui[3], *UI::speGlui[3];
GLUI_EditText *UI::texXp, *UI::texXn, *UI::texYp, *UI::texYn, *UI::texZp, *UI::texZn, *UI::texSph;
GLUI_EditText *UI::urlGlui, *UI::ipmcGlui;
GLUI_Button *UI::grpButton, *UI::ungrpButton, *UI::delButton;
GLUI_Translation *UI::transXYButton, *UI::transZButton;
GLUI_Translation *UI::sizeButton[3];
GLUI_Translation *UI::angleZButton;
GLUI_Rollout *UI::texRollout;
GLUI_Rollout *UI::appRollout;
GLUI_StaticText *UI::objectDescr;


/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */
void UI::control( int event )
{
  switch (event) {

    case ADD_OBJECT:
      createObject();
      break;

    case GROUP:
      {
	Group *newGroup = new Group("newGroup", COLORED, Color::green);
	Vred::treeRoot->add(newGroup);
	int i=0;

	while (selected) {
	    newGroup->add(selected->current);
	    i++;
	    struct objectChain *next = selected->next;
	    free(selected);
	    selected = next;
	}
	printf("%d in new group\n", i);
	item = newGroup;
	updateControls();
      }
      break;

    case UNGROUP:
      {
	Group *group = dynamic_cast<Group*>(item);
	if (group) {
	    group->explode();
	    Vred::treeRoot->remove(group);
	    delete group;
	}
	item = NULL;
	updateControls();
      }
      break;

    case DELETE:
      if (item) {
	Vred::treeRoot->remove(item);
	item = NULL;
	updateControls();

	printf("Object deleted from treeRoot\n");
      }
      break;

    case OPEN:
      askFor("File to open :", OPEN_FILE);
      break;

    case SAVE:
      askFor("File to save :", SAVE_FILE, "output.vre");
      break;

    case QUIT:
      exit(0);
      break;

    case OK_BTN:
      switch (dialUsage) {
	case OPEN_FILE:
	  delete Vred::treeRoot;
	  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);
	  printf("loading file.................");
	  if ( Vred::treeRoot->loadFile(dialStr) != 0 )
	    printf( "FAILED TO LOAD FILE : %s\n", dialStr );
	  else
	    printf("OK\n");
	  break;
	case SAVE_FILE:
	  Vred::treeRoot->printFile(dialStr);
	  break;
      }
      stopAskingFor();
      item = NULL;
      updateControls();
      break;

    case CANCEL_BTN:
      stopAskingFor();
      break;

    case CENTER:
      if (item) {
	Vect oldCenter = item->getCenter();

	if (collis) {	      
	  int wasColliding = Vred::treeRoot->collide(*item);
	  item->setCenter( Vect(center[0], center[1], center[2]) );
	  if (!wasColliding && Vred::treeRoot->collide(*item)) {
	    // there's a true collision : it wasn't colliding but now it is
	    // so we restore previous state
	    //printf( "COLLISION !!\n");
	    item->setCenter(oldCenter);
	    center[0] = oldCenter[0];
	    center[1] = oldCenter[1];
	    center[2] = oldCenter[2];  
	  }
	}
	else
	  item->setCenter( Vect(center[0], center[1], center[2]) );

          centerGlui[0]->set_float_val(center[0]);
	  centerGlui[1]->set_float_val(center[1]);
	  centerGlui[2]->set_float_val(center[2]);
	  centerXButton = -center[0];
	}
      break;

    case CENTER_X_BTN:
      center[0] = -centerXButton;
      control(CENTER);
      break;

    case SIZE:
      if (item) {
        if (size[0] <MIN_SOLID_SIZE)
          size[0] = MIN_SOLID_SIZE;
        if (size[1] <MIN_SOLID_SIZE)
          size[1] = MIN_SOLID_SIZE;
        if (size[2] <MIN_SOLID_SIZE)
          size[2] = MIN_SOLID_SIZE;
	  
	if (dynamic_cast<Sphere*>(item)) {
	  //printf("resizing sphere\n");
	  size[0] = size[1] = size[2];
	  radiusGlui->set_float_val(size[2]);
	}
	  
	Vect oldSize = item->getSize();

	if (collis) {	      
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setSize(Vect(size[0], size[1], size[2]));  
	  if (!wasColliding && Vred::treeRoot->collide(*item)) {
	    // there's a true collision : it wasn't colliding but now it is
	    // so we restore previous state
	    //printf( "COLLISION !!\n");
	    item->setSize(oldSize);
	    size[0] = oldSize[0];
	    size[1] = oldSize[1];
	    size[2] = oldSize[2];  
	    radius = size[2];
	  }
	}
	else
	  item->setSize(Vect(size[0], size[1], size[2]));  
	  sizeGlui[0]->set_float_val(size[0]);
	  sizeGlui[1]->set_float_val(size[1]);
	  sizeGlui[2]->set_float_val(size[2]);
	  sideWin->sync_live();
	  botWin->sync_live();
	}
      break;

    case ROT_Z:
      if (item) {
	while (angleZ > 180)
	  angleZ -= 360;
	while (angleZ < -180)
	  angleZ += 360;

	double oldAngle = item->getOrient()[2];

	if (collis) {	      
	  int wasColliding = Vred::treeRoot->collide(*item);
	  item->setOrient( Vect(0, 0, angleZ) );

	  if (!wasColliding && Vred::treeRoot->collide(*item)) {
	    // there's a true collision : it wasn't colliding but now it is
	    // so we restore previous state
	    //printf( "COLLISION !!\n");
	    item->setOrient( Vect(0, 0, oldAngle) );
	    angleZ = oldAngle;
	  }
	}
	else
	 item->setOrient( Vect(0, 0, angleZ) );
	 angleZGlui->set_float_val(angleZ);
      }
      break;

    case RADIUS:
      if (item) {
	if (radius <MIN_SOLID_SIZE)
	  radius = MIN_SOLID_SIZE;
	  
	double oldRadius = item->getSize()[0];

	if (collis) {	      
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setSize( Vect(radius, radius, radius) );
	  if (!wasColliding && Vred::treeRoot->collide(*item)) {
	    // there's a true collision : it wasn't colliding but now it is
	    // so we restore previous state
	    //printf( "COLLISION !!\n");
	    item->setSize(Vect(oldRadius, oldRadius, oldRadius));
	    radius = oldRadius;
	  }
	}
	else
	  item->setSize( Vect(radius, radius, radius) );            
	  size[0] = size[1] = size[2] = radius;
	  topWin->sync_live();
      }
      break;

    case TEXTURE:
      item->setTexture(Tex(urlXp, urlXn, urlYp, urlYn, urlZp, urlZn));
      
      if (urlXp[0]!=0 || urlXn[0]!=0 || urlYp[0]!=0 || urlYn[0]!=0 || urlZp[0]!=0 || urlZn[0]!=0 )
        item->setStyle(TEXTURED);
      else
	item->setStyle(COLORED);

      //printf("Tex modified\n");
      break;

    case APPEARANCE:
      if (item) {
	Color difColor(dif[0], dif[1], dif[2], 1.0);
	Color ambColor(amb[0], amb[1], amb[2], 1.0);
	Color shiColor(shi[0], shi[1], shi[2], 1.0);
	Color speColor(spe[0], spe[1], spe[2], 1.0);
	App newApp( ambColor, difColor, speColor, shiColor );
	item->setApp( newApp );
	//printf("appearance modified\n");
      }
      break;

    case TARGET_URL:
      {
	Gate *gate = dynamic_cast<Gate*>(item);
	if (gate) {
	    gate->setUrl(url);
	    gate->setIpmc(ipmc);
        }
	Web *web = dynamic_cast<Web*>(item);
	if (web)
	  web->setUrl(url);
	Host *host = dynamic_cast<Host*>(item);
	if (host)
	  host->setHostname(url);
	Doc *doc = dynamic_cast<Doc*>(item);
	if (doc)
	  doc->setUrl(url);

	printf("Target URL updated\n");
      }
      break;

    default:
      printf("action sur un controle non gere\n");
  }
}

/***************************************** myGlutIdle() ***********/
void UI::myGlutIdle(void)
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != mainWin ) 
    glutSetWindow(mainWin);

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this application  */
  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/
void UI::myGlutMouse(int button, int button_state, int x, int y)
{
  if ( button_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON ) {      
    Solid *formerSelectedItem = item;

#if 0 //dax
    item = RayCaster::getSelection(camera->getCenter(),
                                 camera->getWatch(),
        		         x, glutGet((GLenum)GLUT_WINDOW_HEIGHT)-botWin->getH()-y,
				 0,
				 0,
				 glutGet((GLenum)GLUT_WINDOW_WIDTH)-sideWin->getW(),
				 glutGet((GLenum)GLUT_WINDOW_HEIGHT)-topWin->getH()-botWin->getH()
                                  );
#endif
 
    if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
      if (item) {
	struct objectChain *obj = (struct objectChain*) malloc(sizeof(struct objectChain));
	obj->current = item;
	obj->next = selected;
	selected = obj;
      }
      if (formerSelectedItem) {
	struct objectChain *obj = (struct objectChain*) malloc(sizeof(struct objectChain));
	obj->current = formerSelectedItem;
	obj->next = selected;
	selected = obj;
      }
    }
    else {
      // on desselectionne tout
      while (selected) {
	struct objectChain *next = selected->next;

	free(selected);
	selected = next;
      }
    }
    if (selected)
      item = NULL;

    updateControls();
  }

  if (button_state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
    motionEnabled = true;
    mouseX = x;
    mouseY = y;
  }
  if (button_state == GLUT_UP && button == GLUT_RIGHT_BUTTON) {
    motionEnabled = false;
  }

  glutPostRedisplay();
}

/***************************************** myGlutMotion() **********/
void UI::myGlutMotion(int x, int y)
{
  if (motionEnabled) {
    double dheading = 0.0, dpitch = 0.0;

    if (x < mouseX - 1) {
      dheading = 2.0;
      mouseX = x; 
    }
    else if (x > mouseX + 1) {
      dheading = -2.0;
      mouseX = x; 
    }
    if (y < mouseY - 1) {
      dpitch = -2.0;
      mouseY = y;
    }
    else if (y > mouseY + 1) {
      dpitch = 2.0;
      mouseY = y;
    }
    camera->move(Vect::null, Vect(0.0, dpitch, dheading));
    glutPostRedisplay();
  }
}

/***************************************** myGlutKeyboard() **********/
void UI::myGlutKeyboard(unsigned char key, int x, int y) 
{
  myGlutSpecialKeyboard(key, x, y);
}

void UI::myGlutSpecialKeyboard(int key, int x, int y) 
{
  switch (key) {
  case GLUT_KEY_UP:
    camera->move(Vect(0.5, 0.0, 0.0), Vect::null);
    break;
  case GLUT_KEY_DOWN:
    camera->move(Vect(-0.5, 0.0, 0.0), Vect::null);
    break;
  case GLUT_KEY_LEFT:
    camera->move(Vect(0.0, 0.5, 0.0), Vect::null);
    break;
  case GLUT_KEY_RIGHT:
    camera->move(Vect(0.0, -0.5, 0.0), Vect::null);
    break;
  case GLUT_KEY_PAGE_UP:
    camera->move(Vect(0.0, 0.0, 0.5), Vect::null);
    break;
  case GLUT_KEY_PAGE_DOWN:
    camera->move(Vect(0.0, 0.0, -0.5), Vect::null);
    break;
  case '4':
    camera->move(Vect::null, Vect(0.0, 0.0, 5.0));
    break;
  case '6':
    camera->move(Vect::null, Vect(0.0, 0.0, -5.0));
    break;
  case '8':
    camera->move(Vect::null, Vect(0.0, -5.0, 0.0));
    break;
  case '2':
    camera->move(Vect::null, Vect(0.0, 5.0, 0.0));
    break;
  default:
    //cout << "key: \"" << key << "\"" << endl;
    break;
  }
  glutPostRedisplay();
}

/**************************************** myGlutReshape() *************/
void UI::myGlutReshape( int x, int y )
{
  GLUI_Master.auto_set_viewport();
  glutPostRedisplay();
}

/***************************************** myGlutDisplay() *****************/
void UI::myGlutDisplay( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawGrid();
  Vred::treeRoot->render();

  // pour les objets selectionnes: changer en wired
  // redessiner et rechanger en textured.

  struct objectChain *currentObjChain = selected;
  while (currentObjChain) {
    renderSolid(currentObjChain->current);
    currentObjChain = currentObjChain->next;
  }
  if (item)
    renderSolid(item);

  glutSwapBuffers(); 
}

void UI::renderSolid(Solid *solid)
{
  if (solid == NULL) return;

  //printf("renderSolid: ");
  //solid->print();
  
  int oldStyle = solid->getStyle();
  solid->setStyle(WIRED);

  App app = solid->getApp();
  Color color = app.getAmbient();
  solid->setApp(App(Color::red, app.getDiffuse(), app.getSpecular(), app.getShininess()) );
  solid->render();
  solid->setStyle(oldStyle);
  solid->setApp(App(color, app.getDiffuse(), app.getSpecular(), app.getShininess()));
}

void UI::vertex(const Vect& v)
{
  glVertex3d(v[0], v[1], v[2]);
}

void UI::drawGrid()
{
  Vect ux(1., 0., 0.), uy(0., 1., 0.), v(-3., -3., 0.);

  glMaterialfv(GL_FRONT, GL_AMBIENT, Color::blue);
  glBegin(GL_LINES);
    vertex(v);
    vertex(v + 6 * ux);
    vertex(v + uy);
    vertex(v + uy + 6 * ux);
    vertex(v + 2 * uy);
    vertex(v + 2 * uy + 6 * ux);
    vertex(v + 3 * uy);
    vertex(v + 3 * uy + 6 * ux);
    vertex(v + 4 * uy);
    vertex(v + 4 * uy + 6 * ux);
    vertex(v + 5 * uy);
    vertex(v + 5 * uy + 6 * ux);
    vertex(v + 6 * uy);
    vertex(v + 6 * uy + 6 * ux);
  glEnd();
  v = Vect(-3.0, -3.0, 0.0);
  glBegin(GL_LINES);
    vertex(v);
    vertex(v + 6 * uy);
    vertex(v + ux);
    vertex(v + ux + 6 * uy);
    vertex(v + 2 * ux);
    vertex(v + 2 * ux + 6 * uy);
    vertex(v + 3 * ux);
    vertex(v + 3 * ux + 6 * uy);
    vertex(v + 4 * ux);
    vertex(v + 4 * ux + 6 * uy);
    vertex(v + 5 * ux);
    vertex(v + 5 * ux + 6 * uy);
    vertex(v + 6 * ux);
    vertex(v + 6 * ux + 6 * uy);
  glEnd();
}

/****************************************/
/*   Initialize GLUT and create window  */
/****************************************/
void UI::setupUI(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 50, 50 );
  //dax glutInitWindowSize( 800, 700 );
  glutInitWindowSize( 800, 500 );
 
  mainWin = glutCreateWindow( "VRed" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  GLUI_Master.set_glutSpecialFunc( myGlutSpecialKeyboard );
  glutMotionFunc( myGlutMotion );
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
  
  // setup de l'environnement GL
  glLightfv(GL_LIGHT0, GL_AMBIENT, Color::white);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, Color::white);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT2, GL_SPECULAR, Color(0.2, 0.2, 0.2, 1.0));
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHTING);

  glClearColor(0., 0., 0., 0.);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);	// enable z-buffering

  glMatrixMode(GL_PROJECTION);
  gluPerspective(60.0, 1.0, 0.2, 100.0);
  camera = new Camera("cam0", Vect(7.0, 6.0, 3.0), Vect(0.0, 20.0, -140.0));
  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  /* Create the top subwindow */
  topWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_TOP);
  topWin->set_main_gfx_window(mainWin);
  //printf("subwin top\n");

  /* Boxtype */  
  GLUI_Panel *objectPanel = topWin->add_panel("");
  GLUI_Listbox *objectTypeList = topWin->add_listbox_to_panel(objectPanel, "Object type:", &currentObject);

  for (unsigned int i=0; i < sizeof(objectTypes) / sizeof(const char*); i++)
    objectTypeList->add_item(i, (char *) objectTypes[i]);

  topWin->add_column_to_panel(objectPanel, false);
  topWin->add_button_to_panel(objectPanel, "Add", ADD_OBJECT, control);
  
  /* collision */
  topWin->add_separator();
  topWin->add_checkbox("Prevent collision while moving objects", &collis);

  /* placement */
  topWin->add_column(true);
  GLUI_Panel *xyzPanel = topWin->add_panel("");
  (centerGlui[0] = topWin->add_edittext_to_panel(xyzPanel,
                                                 "x",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[0],
                                                 CENTER,
                                                 control))->set_w(25);
  topWin->add_column_to_panel(xyzPanel, false);
  (centerGlui[1] = topWin->add_edittext_to_panel(xyzPanel,
                                                 "y",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[1],
                                                 CENTER,
                                                 control))->set_w(25);
  topWin->add_column_to_panel(xyzPanel, false);
  (centerGlui[2] = topWin->add_edittext_to_panel(xyzPanel,
                                                 "z",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[2],
                                                 CENTER,
                                                 control))->set_w(25);

  /* sizes */
  GLUI_Panel *sizeXyzPanel = topWin->add_panel("");
  (sizeGlui[0] = topWin->add_edittext_to_panel(sizeXyzPanel,
                                               "Dx",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[0],
                                               SIZE,
                                               control ) )->set_w(25);
  topWin->add_column_to_panel(sizeXyzPanel, false);
  (sizeGlui[1] = topWin->add_edittext_to_panel(sizeXyzPanel,
                                               "Dy",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[1],
                                               SIZE,
                                               control ) )->set_w(25);
  topWin->add_column_to_panel(sizeXyzPanel, false);
  (sizeGlui[2] = topWin->add_edittext_to_panel(sizeXyzPanel,
                                               "Dz",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[2], SIZE,
                                               control ) )->set_w(25);
  topWin->add_column(false);
  angleZGlui = topWin->add_edittext("Angle /Z",
                                    GLUI_EDITTEXT_FLOAT, 
				    &angleZ,
                                    ROT_Z,
                                    control);
  angleZGlui->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);

  radiusGlui = topWin->add_edittext("Radius",
                                    GLUI_EDITTEXT_FLOAT, 
				    &radius,
                                    RADIUS,
                                    control);

  /* Create the side subwindow */
  sideWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_RIGHT);
  sideWin->set_main_gfx_window(mainWin);
  //printf("subwin right\n");

  /* texture */
  texRollout = sideWin->add_rollout( "Texture" );
  (texXp = sideWin->add_edittext_to_panel(texRollout,
                                          "Xp",
                                          GLUI_EDITTEXT_TEXT,
                                          urlXp,
                                          TEXTURE,
                                          control))->set_w(128);
  (texXn = sideWin->add_edittext_to_panel(texRollout,
                                          "Xn",
                                          GLUI_EDITTEXT_TEXT,
                                          urlXn,
                                          TEXTURE,
                                          control))->set_w(128);
  (texYp = sideWin->add_edittext_to_panel(texRollout,
                                          "Yp",
                                          GLUI_EDITTEXT_TEXT,
                                          urlYp,
                                          TEXTURE,
                                          control))->set_w(128);
  (texYn = sideWin->add_edittext_to_panel(texRollout,
                                          "Yn",
                                          GLUI_EDITTEXT_TEXT,
                                          urlYn,
                                          TEXTURE,
                                          control))->set_w(128);
  (texZp = sideWin->add_edittext_to_panel(texRollout,
                                          "Zp",
                                          GLUI_EDITTEXT_TEXT,
                                          urlZp,
					  TEXTURE,
                                          control))->set_w(128);
  (texZn = sideWin->add_edittext_to_panel(texRollout,
                                          "Zn",
                                          GLUI_EDITTEXT_TEXT,
                                          urlZn,
                                          TEXTURE,
                                          control))->set_w(128);
  (texSph = sideWin->add_edittext_to_panel(texRollout,
                                           "Sp",
                                           GLUI_EDITTEXT_TEXT,
                                           urlXp,
                                           TEXTURE,
                                           control))->set_w(128);
  
  sideWin->add_button_to_panel(texRollout, "Load textures", TEXTURE, control);

  /* Appearance */
  appRollout = sideWin->add_rollout( "Appearance" );

  GLUI_Panel *ambientPanel = sideWin->add_panel_to_panel(appRollout, "Ambient");
  (ambGlui[0] = sideWin->add_edittext_to_panel(ambientPanel,
                                               "R",
                                               GLUI_EDITTEXT_FLOAT,
					       &amb[0],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  ambGlui[0]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(ambientPanel, false);  
  (ambGlui[1] = sideWin->add_edittext_to_panel(ambientPanel,
                                               "G",
                                               GLUI_EDITTEXT_FLOAT,
					       &amb[1],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  ambGlui[1]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(ambientPanel, false);
  (ambGlui[2] = sideWin->add_edittext_to_panel(ambientPanel,
                                               "B",
                                               GLUI_EDITTEXT_FLOAT,
					       &amb[2],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  ambGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *diffusePanel = sideWin->add_panel_to_panel(appRollout, "Diffuse");
  (difGlui[0] = sideWin->add_edittext_to_panel(diffusePanel,
                                               "R",
                                               GLUI_EDITTEXT_FLOAT,
					       &dif[0],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  difGlui[0]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(diffusePanel, false);  
  (difGlui[1] = sideWin->add_edittext_to_panel(diffusePanel,
                                               "G",
                                               GLUI_EDITTEXT_FLOAT, 
					       &dif[1],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  difGlui[1]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(diffusePanel, false);
  (difGlui[2] = sideWin->add_edittext_to_panel(diffusePanel,
                                               "B",
                                               GLUI_EDITTEXT_FLOAT,
					       &dif[2],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  difGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *specularPanel = sideWin->add_panel_to_panel(appRollout, "Specular");
  (speGlui[0] = sideWin->add_edittext_to_panel(specularPanel,
                                               "V1",
                                               GLUI_EDITTEXT_FLOAT,
					       &spe[0],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  speGlui[0]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(specularPanel, false);  
  (speGlui[1] = sideWin->add_edittext_to_panel(specularPanel,
                                               "V2",
                                               GLUI_EDITTEXT_FLOAT,
					       &spe[1],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  speGlui[1]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(specularPanel, false);
  (speGlui[2] = sideWin->add_edittext_to_panel(specularPanel,
                                               "V3",
                                               GLUI_EDITTEXT_FLOAT,
					       &spe[2],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  speGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *shininessPanel = sideWin->add_panel_to_panel(appRollout, "Shininess");
  (shiGlui[0] = sideWin->add_edittext_to_panel(shininessPanel,
                                               "V1",
                                               GLUI_EDITTEXT_FLOAT,
					       &shi[0],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  shiGlui[0]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(shininessPanel, false);  
  (shiGlui[1] = sideWin->add_edittext_to_panel(shininessPanel,
                                               "V2",
                                               GLUI_EDITTEXT_FLOAT,
					       &shi[1],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  shiGlui[1]->set_float_limits(0, 1);
  sideWin->add_column_to_panel(shininessPanel, false);
  (shiGlui[2] = sideWin->add_edittext_to_panel(shininessPanel,
                                               "V3",
                                               GLUI_EDITTEXT_FLOAT,
					       &shi[2],
                                               APPEARANCE,
                                               control ) )->set_w(20);
  shiGlui[2]->set_float_limits(0, 1);

  (urlGlui = sideWin->add_edittext("To Where (URL)",
                                    GLUI_EDITTEXT_TEXT,
                                    url, 
				    TARGET_URL,
                                    control ) )->set_w(128);
  (ipmcGlui = sideWin->add_edittext("IP Multicast",
                                    GLUI_EDITTEXT_TEXT,
                                    ipmc, 
				    TARGET_URL,
                                    control ) )->set_w(32);
  
  sideWin->add_separator();
  objectDescr = sideWin->add_statictext("Object type :");
  

  /* Create the Bot subwindow */
  botWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_BOTTOM);
  botWin->set_main_gfx_window(mainWin);
  //printf("subwin bot\n");

  transXYButton = botWin->add_translation("Object YZ",
                                          GLUI_TRANSLATION_XY,
                                          &center[1],
                                          CENTER,
                                          control);
  transXYButton->set_speed(0.1);
  botWin->add_column(false);
  transZButton = botWin->add_translation("Object X",
                                         GLUI_TRANSLATION_Z,
                                         &centerXButton,
                                         CENTER_X_BTN,
                                         control);
  transZButton->set_speed(0.1);
  botWin->add_column(false);
  angleZButton = botWin->add_translation("Rot /Z",
                                         GLUI_TRANSLATION_X,
                                         &angleZ,
                                         ROT_Z,
                                         control);
  botWin->add_column(true);
  sizeButton[0] = botWin->add_translation("Size X",
                                          GLUI_TRANSLATION_Z, 
        				  &size[0],
                                          SIZE,
                                          control);
  sizeButton[0]->set_speed(0.1);
  botWin->add_column(false);
  sizeButton[1] =  botWin->add_translation("Size Y",
                                           GLUI_TRANSLATION_X, 
					   &size[1],
                                           SIZE,
                                           control);
  sizeButton[1]->set_speed(0.1);
  botWin->add_column(false);
  sizeButton[2] =  botWin->add_translation("Size Z",
                                           GLUI_TRANSLATION_Y, 
					   &size[2],
                                           SIZE,
                                           control);
  sizeButton[2]->set_speed(0.1);

  botWin->add_column(true);
  grpButton = botWin->add_button("Group", GROUP, control);
  ungrpButton = botWin->add_button("Ungroup", UNGROUP, control);
  botWin->add_column(false);
  delButton = botWin->add_button("Delete", DELETE, control);

  /*  buttons save and quit */
  botWin->add_column(true);
  botWin->add_button("Open", OPEN, control);
  botWin->add_button("Save", SAVE, control);
  botWin->add_separator();
  botWin->add_button("Quit", QUIT, control);

  updateControls();
}

void UI::createObject()
{
  item = NULL;
  switch (currentObject) {
    case WALL_TYPE:
      item = new Wall("unMur",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case GATE_TYPE:
      item = new Gate("unMur",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case EARTH_TYPE:
      item = new Earth("uneSphere",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App(),10,10);
      break;
    case WEB_TYPE:
      item = new Web("unWeb",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case BOARD_TYPE:
      item = new Board("unBoard",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case HOST_TYPE:
      item = new Host("unHost",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case DOC_TYPE:
      item = new Board("unDoc",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case STEP_TYPE:
      item = new Board("unStep",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    default:
      printf("object type unknown\n");
  }
  
  if (item)
    Vred::treeRoot->add(item);

  updateControls();
  glutPostRedisplay();
}

void UI::updateControls()
{
  angleZGlui->disable();
  centerGlui[0]->disable();
  centerGlui[1]->disable();
  centerGlui[2]->disable();
  sizeGlui[0]->disable();
  sizeGlui[1]->disable();
  sizeGlui[2]->disable();
  sizeButton[0]->disable();
  sizeButton[1]->disable();
  sizeButton[2]->disable();

  texRollout->disable();
  appRollout->disable();
  urlGlui->disable();
  ipmcGlui->disable();

  angleZButton->disable();
  transXYButton->disable();
  transZButton->disable();
  
  grpButton->disable();
  ungrpButton->disable();
  delButton->disable();

  if (item) {
    //dax radiusGlui->Hide();
    //dax radiusGlui->hide_internal(false);
     
    if (selected)
      grpButton->enable();
      topWin->refresh();
      return;
  }

  // it's at least a solid
  angleZGlui->enable();
  centerGlui[0]->enable();
  centerGlui[1]->enable();
  centerGlui[2]->enable();
  angleZButton->enable();
  transXYButton->enable();
  transZButton->enable();
  delButton->enable();
  
  // box
  Box *box = dynamic_cast<Box*>(item);
  if (box) {
    sizeGlui[0]->enable();
    sizeGlui[1]->enable();
    sizeGlui[2]->enable();
    sizeButton[0]->enable();
    sizeButton[1]->enable();
    sizeButton[2]->enable();
    //dax radiusGlui->Hide();
 
    Tex tex = item->getTexture();
    texRollout->enable(); 
    //dax texSph->Hide();
    //dax texXp->Show();
    Safe::strcpy(urlXp, tex.getTex_xp() );
    //dax texXn->Show();
    Safe::strcpy(urlXn, tex.getTex_xn() );
    //dax texYp->Show();
    Safe::strcpy(urlYp, tex.getTex_yp() );
    //dax texYn->Show();
    Safe::strcpy(urlYn, tex.getTex_yn() );
    //dax texZp->Show();
    Safe::strcpy(urlZp, tex.getTex_zp() );
    //dax texZn->Show();
    Safe::strcpy(urlZn, tex.getTex_zn() );
    
    appRollout->enable();
    
    sideWin->refresh();
    topWin->refresh();
    sideWin->sync_live();
  }

  Sphere *sphere = dynamic_cast<Sphere*>(item);
  if (sphere) {
    //dax radiusGlui->Show();
    sizeButton[2]->enable();

    texRollout->enable(); 
    //dax texXp->Hide();
    //dax texXn->Hide();
    //dax texYp->Hide();
    //dax texYn->Hide();
    //dax texZp->Hide();
    //dax texZn->Hide();
    //dax texSph->Show();

    Tex tex = item->getTexture();
    Safe::strcpy(urlXp, tex.getTex_xp() );
    urlXn[0] = urlYp[0] = urlYn[0] = urlZp[0] = urlZn[0] = 0;
    appRollout->enable();
    topWin->refresh();
    sideWin->refresh();
  }

  Group *group = dynamic_cast<Group*>(item);
  if (group) {
    ungrpButton->enable();
    //dax radiusGlui->Hide();
    sizeGlui[0]->enable();
    sizeGlui[1]->enable();
    sizeGlui[2]->enable();
    sizeButton[0]->enable();
    sizeButton[1]->enable();
    sizeButton[2]->enable();
  }

  if (! item) {
    printf("updateControls: item null\n");
    return;
  }
  int classId = item->getClassId();
  if (classId == GATE) {
    urlGlui->enable();
    Safe::strcpy( url, ((Gate*)item)->getUrl() );
    ipmcGlui->enable();
    Safe::strcpy( ipmc, ((Gate*)item)->getIpmc() );
  }
  if (classId == WEB || classId == DOC || classId == HOST)
    urlGlui->enable();
  if (classId == WEB)
    Safe::strcpy( url, ((Web*)item)->getUrl() );
  if (classId == DOC)
    Safe::strcpy( url, ((Doc*)item)->getUrl() );
  if (classId == HOST)
    Safe::strcpy( url, ((Host*)item)->getHostname() );
  
  Vect vect;
  vect = item->getCenter();
  center[0] = vect[0];
  center[1] = vect[1];
  center[2] = vect[2];

  vect = item->getSize();
  size[0] = vect[0];
  size[1] = vect[1];
  size[2] = vect[2];
  radius = size[2];
  
  vect = item->getOrient();
  angleZ = vect[2];
 
  char str[128];
  sprintf( str, "Object type : %s", item->getClassName() );
  objectDescr->set_text(str);
  
  Color tempColor = item->getApp().getAmbient();
  amb[0] = tempColor[0];
  amb[1] = tempColor[1];
  amb[2] = tempColor[2];

  tempColor = item->getApp().getDiffuse();
  dif[0] = tempColor[0];
  dif[1] = tempColor[1];
  dif[2] = tempColor[2];

  tempColor = item->getApp().getSpecular();
  spe[0] = tempColor[0];
  spe[1] = tempColor[1];
  spe[2] = tempColor[2];

  tempColor = item->getApp().getShininess();
  shi[0] = tempColor[0];
  shi[1] = tempColor[1];
  shi[2] = tempColor[2];

  GLUI_Master.sync_live_all();
}

void UI::askFor( const char *_question, int _dlogUsage, const char *_answer )
{
  if (dialGlui == NULL) {
    if (dialStr == NULL)
    dialStr = (char*) malloc(128);

    dialGlui = GLUI_Master.create_glui("");
    dialText = dialGlui->add_edittext( (char*)_question, GLUI_EDITTEXT_TEXT, dialStr, OK_BTN);
    dialText->set_w(256);
    dialGlui->add_column(false);
    dialGlui->add_button("OK", OK_BTN, control);
    dialGlui->add_button("Cancel", CANCEL_BTN, control);
      
    dialGlui->set_main_gfx_window(mainWin);
  }
  dialText->set_name((char*)_question);
  strcpy(dialStr, _answer);
  dialUsage = _dlogUsage;
  //dax dialGlui->Show();
  dialGlui->sync_live();
}

void UI::stopAskingFor()
{
  //dax dialGlui->Hide();
  //dax dialGlui->hide_internal(0);
}
