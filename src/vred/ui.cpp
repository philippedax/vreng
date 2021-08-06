#include <GL/glut.h>

#include "ui.hpp"
#include "vred.hpp"
#include "vre.hpp"
#include "safe.hpp"
#include "raycaster.hpp"


int UI::mainWin;
const char *UI::objectTypes[] = { "Wall",
                                  "Gate",
                                  "Step",
                                  "Web",
                                  "Board",
                                  "Host",
                                  "Doc",
                                  "Mirage",
                                  "Thing",
                                  "Earth"
                                };
int UI::currObject = 0;
Solid *UI::item = NULL;
struct objectChain *UI::selected = NULL;
float UI::center[3] = {0, 0, 0};
float UI::centerXButton = 0;
float UI::size[3];
float UI::angleZ;
float UI::radius;
char UI::urlXp[128] = "", UI::urlXn[128] = "";
char UI::urlYp[128] = "", UI::urlYn[128] = "";
char UI::urlZp[128] = "", UI::urlZn[128] = "";
float UI::dif[3], UI::amb[3], UI::shi[3], UI::spe[3];
char UI::url[128] = "";
char UI::ipmc[32] = "";
char *UI::dialStr = NULL;
Camera *UI::camera;
bool UI::motion = false;
int UI::mouseX, UI::mouseY;
int UI::dialUsage;
int UI::collis = true;

GLUI *UI::topWin, *UI::rightWin, *UI::botWin, *UI::dialGlui = NULL;
GLUI_EditText *UI::dialText;
GLUI_EditText *UI::centerGlui[3];
GLUI_EditText *UI::sizeGlui[3];
GLUI_EditText *UI::angleZGlui;
GLUI_EditText *UI::radiusGlui;
GLUI_EditText *UI::difGlui[3], *UI::ambGlui[3], *UI::shiGlui[3], *UI::speGlui[3];
GLUI_EditText *UI::texXp,*UI::texXn,*UI::texYp,*UI::texYn,*UI::texZp,*UI::texZn,*UI::texSph;
GLUI_EditText *UI::urlGlui;
GLUI_EditText *UI::ipmcGlui;
GLUI_Button *UI::grpButton, *UI::ungrpButton, *UI::delButton;
GLUI_Translation *UI::transXYButton, *UI::transZButton;
GLUI_Translation *UI::sizeButton[3];
GLUI_Translation *UI::rotZButton;
//GLUI_Rollout *UI::texRollout;
GLUI_Panel *UI::texPanel;
GLUI_Rollout *UI::appRollout;
GLUI_StaticText *UI::objectDescr;


/****************************** control_cb() *****************************/
/* GLUI control callback                                                 */
void UI::control_cb( int event )
{
  switch (event) {

    case ADD_OBJECT:
      createObject();
      break;

    case GROUP:
      {
	Group *newGroup = new Group("newGroup", COLORED, Color::green);
	Vred::treeRoot->add(newGroup);

	for ( ; selected; ) {
	  newGroup->add(selected->current);
	  struct objectChain *next = selected->next;
	  free(selected);
	  selected = next;
	}
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
      }
      break;

    case OPEN:
      openDial("File to open:", OPEN_FILE);
      break;

    case SAVE:
      openDial("File to save:", SAVE_FILE, "output.vre");
      break;

    case QUIT:
      exit(0);
      break;

    case OK_BTN:
      switch (dialUsage) {

	case OPEN_FILE:
	  delete Vred::treeRoot;
	  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);
	  printf("loading file......");
	  if ( Vred::treeRoot->loadFile(dialStr) != 0 )
	    printf("failed to load file: %s\n", dialStr);
	  else
	    printf("file loaded\n");
	  break;

	case SAVE_FILE:
	  Vred::treeRoot->printFile(dialStr);
	  break;
      }
      closeDial();
      item = NULL;
      updateControls();
      break;

    case CANCEL_BTN:
      closeDial();
      break;

    case CENTER_XY:
      if (item) {
	if (collis) {
	  Vect oldcenter = item->getCenter();
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setCenter( Vect(center[0], center[1], center[2]) );
	  if (! wasColliding && Vred::treeRoot->collide(*item)) {
	    //printf( "COLLISION !!\n");
	    item->setCenter(oldcenter);
	    center[0] = oldcenter[0];
	    center[1] = oldcenter[1];
	    center[2] = oldcenter[2];  
	  }
	}
	else {
	  item->setCenter( Vect(center[0], center[1], center[2]) );
	}
        centerGlui[0]->set_float_val(center[0]);
	centerGlui[1]->set_float_val(center[1]);
	centerGlui[2]->set_float_val(center[2]);
	centerXButton = -center[0];
      }
      break;

    case CENTER_Z:
      center[0] = -centerXButton;
      //control_cb(CENTER_Z);
      break;

    case SIZE:
      if (item) {
        if (size[0] < MIN_SOLID_SIZE) {
          size[0] = MIN_SOLID_SIZE;
	}
        if (size[1] < MIN_SOLID_SIZE) {
          size[1] = MIN_SOLID_SIZE;
	}
        if (size[2] < MIN_SOLID_SIZE) {
          size[2] = MIN_SOLID_SIZE;
	}

	if (dynamic_cast<Sphere*>(item)) {
	  // resizing sphere
	  size[0] = size[1] = size[2];
	  radiusGlui->set_float_val(size[2]);
	}
	if (collis) {	      
	  Vect oldsize = item->getSize();
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setSize(Vect(size[0], size[1], size[2]));  
	  if (! wasColliding && Vred::treeRoot->collide(*item)) {
	    //printf( "COLLISION !!\n");
	    item->setSize(oldsize);
	    size[0] = oldsize[0];
	    size[1] = oldsize[1];
	    size[2] = oldsize[2];  
	    radius = size[2];
	  }
	}
	else
	  item->setSize(Vect(size[0], size[1], size[2]));  
	}
        sizeGlui[0]->set_float_val(size[0]);
	sizeGlui[1]->set_float_val(size[1]);
	sizeGlui[2]->set_float_val(size[2]);

	rightWin->sync_live();
	botWin->sync_live();
      break;

    case ROT_Z:
      if (item) {
	while (angleZ > 180) {
	  angleZ -= 360;
        }
	while (angleZ < -180) {
	  angleZ += 360;
        }
	if (collis) {	      
	  float oldangle = item->getOrient()[2];
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setOrient( Vect(0, 0, angleZ) );
	  if (! wasColliding && Vred::treeRoot->collide(*item)) {
	    //printf( "COLLISION !!\n");
	    item->setOrient( Vect(0, 0, oldangle) );
	    angleZ = oldangle;
	  }
	}
	else {
	  item->setOrient( Vect(0, 0, angleZ) );
	}
	angleZGlui->set_float_val(angleZ);
      }
      break;

    case RADIUS:
      if (item) {
	if (radius < MIN_SOLID_SIZE) {
	  radius = MIN_SOLID_SIZE;
        }
	if (collis) {	      
	  float oldradius = item->getSize()[0];
	  int wasColliding = Vred::treeRoot->collide(*item);

	  item->setSize( Vect(radius, radius, radius) );
	  if (! wasColliding && Vred::treeRoot->collide(*item)) {
	    //printf( "COLLISION !!\n");
	    item->setSize(Vect(oldradius, oldradius, oldradius));
	    radius = oldradius;
	  }
	}
	else {
	  item->setSize( Vect(radius, radius, radius) );            
	}
	size[0] = size[1] = size[2] = radius;
	topWin->sync_live();
      }
      break;

    case TEXTURE:
      item->setTexture(Tex(urlXp, urlXn, urlYp, urlYn, urlZp, urlZn));
      
      if ( urlXp[0] || urlXn[0] || urlYp[0] || urlYn[0] || urlZp[0] || urlZn[0] ) {
        item->setStyle(TEXTURED);
      }
      else {
	item->setStyle(COLORED);
      }
      break;

    case APPEARANCE:
      if (item) {
	Color difColor(dif[0], dif[1], dif[2], 1.0);
	Color ambColor(amb[0], amb[1], amb[2], 1.0);
	Color shiColor(shi[0], shi[1], shi[2], 1.0);
	Color speColor(spe[0], spe[1], spe[2], 1.0);
	App newApp( ambColor, difColor, speColor, shiColor );
	item->setApp( newApp );
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
	if (web) {
	  web->setUrl(url);
        }
	Host *host = dynamic_cast<Host*>(item);
	if (host) {
	  host->setHostname(url);
        }
	Doc *doc = dynamic_cast<Doc*>(item);
	if (doc) {
	  doc->setUrl(url);
        }
	//printf("Target URL updated\n");
      }
      break;

    default:
      printf("action unknown\n");
  }
}

/***************************** myGlutIdle() ***********************/
void UI::myGlutIdle(void)
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback. So we need to explicitly change it if necessary */
  if ( glutGetWindow() != mainWin ) {
    glutSetWindow(mainWin);
  }

  /* GLUI_Master.sync_live_all(); - not needed - nothing to sync */
  glutPostRedisplay();
}

/***************************** myGlutMouse() **********************/
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
				 glutGet((GLenum)GLUT_WINDOW_WIDTH)-rightWin->getW(),
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

    updateControls();
  }

  if (button_state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
    motion = true;
    mouseX = x;
    mouseY = y;
  }
  if (button_state == GLUT_UP && button == GLUT_RIGHT_BUTTON) {
    motion = false;
  }

  glutPostRedisplay();
}

/***************************** myGlutMotion() **********************/
void UI::myGlutMotion(int x, int y)
{
  if (motion) {
    float dheading = 0, dpitch = 0;

    if (x < mouseX - 1) {
      dheading = 2;
      mouseX = x; 
    }
    else if (x > mouseX + 1) {
      dheading = -2;
      mouseX = x; 
    }
    if (y < mouseY - 1) {
      dpitch = -2;
      mouseY = y;
    }
    else if (y > mouseY + 1) {
      dpitch = 2;
      mouseY = y;
    }
    camera->move(Vect::null, Vect(0, dpitch, dheading));
    glutPostRedisplay();
  }
}

/***************************** myGlutKeyboard() **********************/
void UI::myGlutKeyboard(unsigned char key, int x, int y) 
{
  myGlutSpecialKeyboard(key, x, y);
}

void UI::myGlutSpecialKeyboard(int key, int x, int y) 
{
  switch (key) {
  case GLUT_KEY_UP:
    camera->move(Vect(0.5, 0, 0), Vect::null);
    break;
  case GLUT_KEY_DOWN:
    camera->move(Vect(-0.5, 0, 0), Vect::null);
    break;
  case GLUT_KEY_LEFT:
    camera->move(Vect(0, 0.5, 0), Vect::null);
    break;
  case GLUT_KEY_RIGHT:
    camera->move(Vect(0, -0.5, 0), Vect::null);
    break;
  case GLUT_KEY_PAGE_UP:
    camera->move(Vect(0, 0, 0.5), Vect::null);
    break;
  case GLUT_KEY_PAGE_DOWN:
    camera->move(Vect(0, 0, -0.5), Vect::null);
    break;
  case '4':
    camera->move(Vect::null, Vect(0, 0, 5.0));
    break;
  case '6':
    camera->move(Vect::null, Vect(0, 0, -5.0));
    break;
  case '8':
    camera->move(Vect::null, Vect(0, -5.0, 0));
    break;
  case '2':
    camera->move(Vect::null, Vect(0, 5.0, 0));
    break;
  default:
    //cout << "key: \"" << key << "\"" << endl;
    break;
  }
  glutPostRedisplay();
}

/**************************** myGlutReshape() *************************/
void UI::myGlutReshape( int x, int y )
{
  GLUI_Master.auto_set_viewport();
  glutPostRedisplay();
}

/**************************** myGlutDisplay() *************************/
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
  if (item) {
    renderSolid(item);
  }

  glutSwapBuffers(); 
}

void UI::renderSolid(Solid *solid)
{
  if (solid == NULL) return;

  //solid->print();
  
  int oldStyle = solid->getStyle();
  solid->setStyle(WIRED);

  App app = solid->getApp();
  Color color = app.getAmbient();
  solid->setApp( App(Color::red, app.getDiffuse(), app.getSpecular(), app.getShininess()) );
  solid->render();
  solid->setStyle(oldStyle);
  solid->setApp( App(color, app.getDiffuse(), app.getSpecular(), app.getShininess()) );
}

void UI::vertex(const Vect& v)
{
  glVertex3d(v[0], v[1], v[2]);
}

void UI::drawGrid()
{
  Vect ux(1, 0, 0), uy(0, 1, 0), v(-4, -4, 0.);

  glMaterialfv(GL_FRONT, GL_AMBIENT, Color::blue);
  glBegin(GL_LINES);
    vertex(v);
    vertex(v + 8 * ux);
    vertex(v + uy);
    vertex(v + uy + 8 * ux);
    vertex(v + 2 * uy);
    vertex(v + 2 * uy + 8 * ux);
    vertex(v + 3 * uy);
    vertex(v + 3 * uy + 8 * ux);
    vertex(v + 4 * uy);
    vertex(v + 4 * uy + 8 * ux);
    vertex(v + 5 * uy);
    vertex(v + 5 * uy + 8 * ux);
    vertex(v + 6 * uy);
    vertex(v + 6 * uy + 8 * ux);
    vertex(v + 7 * uy);
    vertex(v + 7 * uy + 8 * ux);
    vertex(v + 8 * uy);
    vertex(v + 8 * uy + 8 * ux);
  glEnd();
  v = Vect(-4, -4, 0);
  glBegin(GL_LINES);
    vertex(v);
    vertex(v + 8 * uy);
    vertex(v + ux);
    vertex(v + ux + 8 * uy);
    vertex(v + 2 * ux);
    vertex(v + 2 * ux + 8 * uy);
    vertex(v + 3 * ux);
    vertex(v + 3 * ux + 8 * uy);
    vertex(v + 4 * ux);
    vertex(v + 4 * ux + 8 * uy);
    vertex(v + 5 * ux);
    vertex(v + 5 * ux + 8 * uy);
    vertex(v + 6 * ux);
    vertex(v + 6 * ux + 8 * uy);
    vertex(v + 7 * ux);
    vertex(v + 7 * ux + 8 * uy);
    vertex(v + 8 * ux);
    vertex(v + 8 * ux + 8 * uy);
  glEnd();
}


/********************************************/
/*   Initializes GLUT and creates windows   */
/********************************************/

void UI::setupUI(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 820, 580 );
 
  mainWin = glutCreateWindow( "VRed" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  GLUI_Master.set_glutSpecialFunc( myGlutSpecialKeyboard );
  glutMotionFunc( myGlutMotion );
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
  
  // setup lightings
  glLightfv(GL_LIGHT0, GL_AMBIENT, Color::white);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, Color::white);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT2, GL_SPECULAR, Color(.2, .2, .2, 1));
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHTING);

  glClearColor(0, 0, 0, 0);	// black background
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);	// enable depth-buffering

  /* view */
  glMatrixMode(GL_PROJECTION);
  gluPerspective(60, 1, 0.2, 100);
  camera = new Camera("cam0", Vect(7, 6, 3), Vect(0, 20, -140));
  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);


  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  /* Create the top subwindow */
  topWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_TOP);
  topWin->set_main_gfx_window(mainWin);

  /* Objtype */  
  GLUI_Panel *objectPanel = topWin->add_panel("");
  GLUI_Listbox *objectTypeList = topWin->add_listbox_to_panel(objectPanel, "Object type:", &currObject);

  for (int i=0; i < sizeof(objectTypes) / sizeof(char*); i++) {
    objectTypeList->add_item(i, (char *) objectTypes[i]);
  }

  /* button Add */
  topWin->add_column_to_panel(objectPanel, false);
  topWin->add_button_to_panel(objectPanel, "Add", ADD_OBJECT, control_cb);
  
  /* collision */
  topWin->add_separator();
  topWin->add_checkbox("Prevent collision while moving objects", &collis);

  /* position */
  topWin->add_column(true);
  GLUI_Panel *xyzPanel = topWin->add_panel("");
  (centerGlui[0] = topWin->add_edittext_to_panel(xyzPanel, "x",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[0], CENTER_XY,
                                                 control_cb)
  )->set_w(10);
  topWin->add_column_to_panel(xyzPanel, false);
  (centerGlui[1] = topWin->add_edittext_to_panel(xyzPanel, "y",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[1], CENTER_XY,
                                                 control_cb)
  )->set_w(10);
  topWin->add_column_to_panel(xyzPanel, false);
  (centerGlui[2] = topWin->add_edittext_to_panel(xyzPanel, "z",
                                                 GLUI_EDITTEXT_FLOAT,
                                                 &center[2], CENTER_XY,
                                                 control_cb)
  )->set_w(10);

  /* dimensions */
  GLUI_Panel *sizePanel = topWin->add_panel("");
  (sizeGlui[0] = topWin->add_edittext_to_panel(sizePanel, "Dx",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[0], SIZE,
                                               control_cb)
  )->set_w(10);
  topWin->add_column_to_panel(sizePanel, false);
  (sizeGlui[1] = topWin->add_edittext_to_panel(sizePanel, "Dy",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[1], SIZE,
                                               control_cb)
  )->set_w(10);
  topWin->add_column_to_panel(sizePanel, false);
  (sizeGlui[2] = topWin->add_edittext_to_panel(sizePanel, "Dz",
                                               GLUI_EDITTEXT_FLOAT, 
					       &size[2], SIZE,
                                               control_cb)
  )->set_w(10);
  topWin->add_column(false);
  angleZGlui = topWin->add_edittext("Angle/Z",
                                    GLUI_EDITTEXT_FLOAT, 
				    &angleZ, ROT_Z,
                                    control_cb);
  angleZGlui->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);

  radiusGlui = topWin->add_edittext("Radius",
                                    GLUI_EDITTEXT_FLOAT, 
				    &radius, RADIUS,
                                    control_cb);


  /* Create the right subwindow */
  rightWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_RIGHT);
  rightWin->set_main_gfx_window(mainWin);

  /* texture */
  //texRollout = rightWin->add_rollout( "Tex");
  texPanel = rightWin->add_panel( "Tex");
  rightWin->add_column_to_panel(texPanel, true);
  (texXp = rightWin->add_edittext_to_panel(texPanel, "Xp",
                                           GLUI_EDITTEXT_TEXT,
                                           urlXp, TEXTURE,
                                           control_cb)
  )->set_w(100);
  (texXn = rightWin->add_edittext_to_panel(texPanel, "Xn",
                                           GLUI_EDITTEXT_TEXT,
                                           urlXn, TEXTURE,
                                           control_cb)
  )->set_w(100);
  rightWin->add_column_to_panel(texPanel, false);
  (texYp = rightWin->add_edittext_to_panel(texPanel, "Yp",
                                           GLUI_EDITTEXT_TEXT,
                                           urlYp, TEXTURE,
                                           control_cb)
  )->set_w(100);
  (texYn = rightWin->add_edittext_to_panel(texPanel, "Yn",
                                           GLUI_EDITTEXT_TEXT,
                                           urlYn, TEXTURE,
                                           control_cb)
  )->set_w(100);
  rightWin->add_column_to_panel(texPanel, false);
  (texZp = rightWin->add_edittext_to_panel(texPanel, "Zp",
                                           GLUI_EDITTEXT_TEXT,
                                           urlZp, TEXTURE,
                                           control_cb)
  )->set_w(100);
  (texZn = rightWin->add_edittext_to_panel(texPanel, "Zn",
                                           GLUI_EDITTEXT_TEXT,
                                           urlZn, TEXTURE,
                                           control_cb)
  )->set_w(100);
  (texSph = rightWin->add_edittext_to_panel(texPanel, "Sp",
                                            GLUI_EDITTEXT_TEXT,
                                            urlXp, TEXTURE,
                                            control_cb)
  )->set_w(100);
  rightWin->add_button_to_panel(texPanel, "Load textures", TEXTURE, control_cb);

  /* Appearance */
  appRollout = rightWin->add_rollout("Appearance");

  GLUI_Panel *difPanel = rightWin->add_panel_to_panel(appRollout, "Diffuse");
  (difGlui[0] = rightWin->add_edittext_to_panel(difPanel, "R",
                                                GLUI_EDITTEXT_FLOAT,
					        &dif[0], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  difGlui[0]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(difPanel, false);  
  (difGlui[1] = rightWin->add_edittext_to_panel(difPanel, "G",
                                                GLUI_EDITTEXT_FLOAT, 
					        &dif[1], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  difGlui[1]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(difPanel, false);
  (difGlui[2] = rightWin->add_edittext_to_panel(difPanel, "B",
                                                GLUI_EDITTEXT_FLOAT,
					        &dif[2], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  difGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *ambPanel = rightWin->add_panel_to_panel(appRollout, "Ambient");
  (ambGlui[0] = rightWin->add_edittext_to_panel(ambPanel, "R",
                                                GLUI_EDITTEXT_FLOAT,
					        &amb[0], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  ambGlui[0]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(ambPanel, false);  
  (ambGlui[1] = rightWin->add_edittext_to_panel(ambPanel, "G",
                                                GLUI_EDITTEXT_FLOAT,
					        &amb[1], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  ambGlui[1]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(ambPanel, false);
  (ambGlui[2] = rightWin->add_edittext_to_panel(ambPanel, "B",
                                                GLUI_EDITTEXT_FLOAT,
					        &amb[2], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  ambGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *spePanel = rightWin->add_panel_to_panel(appRollout, "Specular");
  (speGlui[0] = rightWin->add_edittext_to_panel(spePanel, "R",
                                                GLUI_EDITTEXT_FLOAT,
					        &spe[0], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  speGlui[0]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(spePanel, false);  
  (speGlui[1] = rightWin->add_edittext_to_panel(spePanel, "G",
                                                GLUI_EDITTEXT_FLOAT,
					        &spe[1], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  speGlui[1]->set_float_limits(0, 1);
  rightWin->add_column_to_panel(spePanel, false);
  (speGlui[2] = rightWin->add_edittext_to_panel(spePanel, "B",
                                                GLUI_EDITTEXT_FLOAT,
					        &spe[2], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  speGlui[2]->set_float_limits(0, 1);

  GLUI_Panel *shiPanel = rightWin->add_panel_to_panel(appRollout, "Shininess");
  (shiGlui[0] = rightWin->add_edittext_to_panel(shiPanel, "S",
                                                GLUI_EDITTEXT_FLOAT,
					        &shi[0], APPEARANCE,
                                                control_cb)
  )->set_w(10);
  shiGlui[0]->set_float_limits(0, 20);

  (urlGlui = rightWin->add_edittext("url",
                                    GLUI_EDITTEXT_TEXT,
                                    url, TARGET_URL,
                                    control_cb)
  )->set_w(128);
  (ipmcGlui = rightWin->add_edittext("IP Multicast",
                                     GLUI_EDITTEXT_TEXT,
                                     ipmc, TARGET_URL,
                                     control_cb)
  )->set_w(64);
  
  //rightWin->add_separator();
  //objectDescr = rightWin->add_statictext("Object type:");
  

  /* Create the Bot subwindow */
  botWin = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_BOTTOM);
  botWin->set_main_gfx_window(mainWin);

  transXYButton = botWin->add_translation("Trans YZ",
                                          GLUI_TRANSLATION_XY,
                                          &center[1], CENTER_XY,
                                          control_cb);
  transXYButton->set_speed(0.1);
  botWin->add_column(false);
  transZButton = botWin->add_translation("Trans X",
                                         GLUI_TRANSLATION_Z,
                                         &centerXButton, CENTER_Z,
                                         control_cb);
  transZButton->set_speed(0.1);
  botWin->add_column(false);
  rotZButton = botWin->add_translation("Rot/Z",
                                       GLUI_TRANSLATION_X,
                                       &angleZ, ROT_Z,
                                       control_cb);
  botWin->add_column(true);
  sizeButton[0] = botWin->add_translation("Size X",
                                          GLUI_TRANSLATION_Z, 
        				  &size[0], SIZE,
                                          control_cb);
  sizeButton[0]->set_speed(0.1);
  botWin->add_column(false);
  sizeButton[1] = botWin->add_translation("Size Y",
                                          GLUI_TRANSLATION_X, 
					  &size[1], SIZE,
                                          control_cb);
  sizeButton[1]->set_speed(0.1);
  botWin->add_column(false);
  sizeButton[2] = botWin->add_translation("Size Z",
                                          GLUI_TRANSLATION_Y, 
					  &size[2], SIZE,
                                          control_cb);
  sizeButton[2]->set_speed(0.1);

  botWin->add_column(true);
  grpButton = botWin->add_button("Group", GROUP, control_cb);
  ungrpButton = botWin->add_button("Ungroup", UNGROUP, control_cb);
  botWin->add_column(false);
  delButton = botWin->add_button("Delete", DELETE, control_cb);

  /*  buttons open/save and quit */
  botWin->add_column(true);
  botWin->add_button("Open", OPEN, control_cb);
  botWin->add_button("Save", SAVE, control_cb);
  botWin->add_separator();
  botWin->add_button("Quit", QUIT, control_cb);

  updateControls();
}

void UI::createObject()
{
  item = NULL;
  switch (currObject) {
    case WALL_TYPE:
      item = new Wall("Box",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case GATE_TYPE:
      item = new Gate("Box",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case EARTH_TYPE:
      item = new Earth("Sphere",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App(),10,10);
      break;
    case WEB_TYPE:
      item = new Web("Web",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case BOARD_TYPE:
      item = new Board("Board",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case HOST_TYPE:
      item = new Host("Host",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case DOC_TYPE:
      item = new Doc("Doc",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case STEP_TYPE:
      item = new Step("Step",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case MIRAGE_TYPE:
      item = new Mirage("Mirage",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    case THING_TYPE:
      item = new Thing("Thing",
                     Vect::null, Vect::null, Vect::unit, COLORED, Color::white, Tex(), App());
      break;
    default:
      printf("object type unknown\n");
  }

  if (item) {
    Vred::treeRoot->add(item);
  }

  updateControls();
}

void UI::updateControls()
{
  urlGlui->disable();
  ipmcGlui->disable();

  if (item) {
    if (selected)
      grpButton->enable();
      //topWin->refresh();
      return;
  }

  // it's at least a solid
  angleZGlui->enable();
  centerGlui[0]->enable();
  centerGlui[1]->enable();
  centerGlui[2]->enable();
  rotZButton->enable();
  transXYButton->enable();
  transZButton->enable();
  sizeButton[0]->enable();
  sizeButton[1]->enable();
  sizeButton[2]->enable();
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
 
    Tex tex = item->getTexture();
    texPanel->enable(); 
    //texRollout->enable(); 
    Safe::strcpy(urlXp, tex.getTex_xp());
    Safe::strcpy(urlXn, tex.getTex_xn());
    Safe::strcpy(urlYp, tex.getTex_yp());
    Safe::strcpy(urlYn, tex.getTex_yn());
    Safe::strcpy(urlZp, tex.getTex_zp());
    Safe::strcpy(urlZn, tex.getTex_zn());
    
    appRollout->enable();
    
    //rightWin->refresh();
    topWin->refresh();
    rightWin->sync_live();
  }

  Sphere *sphere = dynamic_cast<Sphere*>(item);
  if (sphere) {
    sizeButton[2]->enable();

    texPanel->enable(); 
    //texRollout->enable(); 

    Tex tex = item->getTexture();
    Safe::strcpy(urlXp, tex.getTex_xp());
    urlXn[0] = urlYp[0] = urlYn[0] = urlZp[0] = urlZn[0] = 0;
    appRollout->enable();

    //topWin->refresh();
    //rightWin->refresh();
  }

  Group *group = dynamic_cast<Group*>(item);
  if (group) {
    ungrpButton->enable();
    sizeGlui[0]->enable();
    sizeGlui[1]->enable();
    sizeGlui[2]->enable();
    sizeButton[0]->enable();
    sizeButton[1]->enable();
    sizeButton[2]->enable();
  }

  if (! item) {
    return;
  }

  int classId = item->getClassId();
  switch (classId) {
  case GATE:
    urlGlui->enable();
    Safe::strcpy( url, ((Gate*)item)->getUrl() );
    ipmcGlui->enable();
    Safe::strcpy( ipmc, ((Gate*)item)->getIpmc() );
    break;
  case WEB:
    urlGlui->enable();
    Safe::strcpy( url, ((Web*)item)->getUrl() );
    break;
  case DOC:
    urlGlui->enable();
    Safe::strcpy( url, ((Doc*)item)->getUrl() );
    break;
  case HOST:
    urlGlui->enable();
    Safe::strcpy( url, ((Host*)item)->getHostname() );
    break;
  }
  
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
  sprintf(str, "Object type: %s", item->getClassName());
  objectDescr->set_text(str);
  
  Color color = item->getApp().getAmbient();
  amb[0] = color[0];
  amb[1] = color[1];
  amb[2] = color[2];

  color = item->getApp().getDiffuse();
  dif[0] = color[0];
  dif[1] = color[1];
  dif[2] = color[2];

  color = item->getApp().getSpecular();
  spe[0] = color[0];
  spe[1] = color[1];
  spe[2] = color[2];

  color = item->getApp().getShininess();
  shi[0] = color[0];
  shi[1] = color[1];
  shi[2] = color[2];

  GLUI_Master.sync_live_all();

  glutPostRedisplay();
}

void UI::openDial( const char *question, int action, const char *answer )
{
  if (dialGlui == NULL) {
    if (dialStr == NULL) {
      dialStr = (char*) malloc(128);
    }

    dialGlui = GLUI_Master.create_glui("");
    dialText = dialGlui->add_edittext((char*)question, GLUI_EDITTEXT_TEXT, dialStr, OK_BTN);
    dialText->set_w(256);
    dialGlui->add_column(false);
    dialGlui->add_button("Ok", OK_BTN, control_cb);
    dialGlui->add_button("Cancel", CANCEL_BTN, control_cb);
    dialGlui->set_main_gfx_window(mainWin);
  }

  dialText->set_name((char*) question);
  strcpy(dialStr, answer);
  dialUsage = action;
  dialGlui->show();
  dialGlui->sync_live();
}

void UI::closeDial()
{
  dialGlui->disable();
  //dialGlui->hide();
}
