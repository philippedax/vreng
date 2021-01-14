#include <GL/glut.h>

#include "ui.hpp"
#include "vred.hpp"
#include "vre.hpp"
#include "safe.hpp"
#include "raycaster.hpp"

#define MIN_SOLID_SIZE  0.1


const char *UI::objectTypesString[] =
                 { "Wall", "Gate", "Step", "Web", "Board", "Host", "Doc", "Earth" };
enum UIObjectTypes { WALL_TYPE, GATE_TYPE, STEP_TYPE, WEB_TYPE, 
		     BOARD_TYPE, HOST_TYPE, DOC_TYPE, EARTH_TYPE };

int UI::mainWindowID;
int UI::currentObjectType = 0;
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
float UI::viewRotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
int UI::preventCollision = true;
Camera *UI::camera;
bool UI::motionEnabled = false;
int UI::mouseX, UI::mouseY;
char *UI::dialogString = NULL;
int UI::dialogUsage;

GLUI *UI::topSubwin, *UI::rightSubwin, *UI::bottomSubwin, *UI::dialogGlui=NULL;
GLUI_EditText *UI::dialogEditText;
GLUI_EditText *UI::centerField[3];
GLUI_EditText *UI::sizeField[3];
GLUI_EditText *UI::angleZField;
GLUI_EditText *UI::radiusField;
GLUI_EditText *UI::difField[3], *UI::ambField[3], *UI::shiField[3], *UI::speField[3];
GLUI_EditText *UI::texXp, *UI::texXn, *UI::texYp, *UI::texYn, *UI::texZp, *UI::texZn, *UI::texSph;
GLUI_EditText *UI::urlField, *UI::ipmcField;
GLUI_Button *UI::groupButton, *UI::ungroupButton, *UI::deleteButton;
GLUI_Translation *UI::objectTransXYButton, *UI::objectTransZButton;
GLUI_Translation *UI::sizeButton[3];
GLUI_Translation *UI::angleZButton;
GLUI_Rollout *UI::texRollout;
GLUI_Rollout *UI::appearanceRollout;
GLUI_StaticText *UI::objectClassDescription;


/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */
void UI::controlEvent( int control )
{
  switch (control) {

    case UI_ADD_OBJECT:
      createNewObject();
      break;

    case UI_GROUP:
      {
	Group *newGroup = new Group("newGroup", COLORED, Color::green);
	Vred::treeRoot->add(newGroup);
	int i=0;

	while (selected != NULL) {
	    newGroup->add(selected->current);
	    i++;
	    struct objectChain *suivant = selected->next;
	    free(selected);
	    selected = suivant;
	}
	printf("%d in new group\n", i);
	item = newGroup;
	updateControls();
      }
      break;

    case UI_UNGROUP:
      {
	Group *group = dynamic_cast<Group*>(item);
	if (group != NULL) {
	    group->explode();
	    Vred::treeRoot->remove(group);
	    delete group;
	}
	item = NULL;
	updateControls();
      }
      break;

    case UI_DELETE:
      if (item != NULL) {
	  Vred::treeRoot->remove(item);
	  item = NULL;
	  updateControls();

	  printf("Object deleted from treeRoot\n");
      }
      break;

    case UI_OPEN:
      askFor("File to open :", DLOG_OPEN_FILE);
      break;

    case UI_SAVE:
      askFor("File to save :", DLOG_SAVE_FILE, "output.vre");
      break;

    case UI_QUIT:
      exit(0);
      break;

    case UI_OK_BTN:
      switch (dialogUsage) {
	case DLOG_OPEN_FILE:
	  delete Vred::treeRoot;
	  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);
	  printf("loading file.................");
	  if ( Vred::treeRoot->loadFile(dialogString) != 0 )
	    printf( "FAILED TO LOAD FILE : %s\n", dialogString );
	  else
	    printf("OK\n");
	  item = NULL;
	  break;
	case DLOG_SAVE_FILE:
	  Vred::treeRoot->printFile(dialogString);
	  break;
      }
      stopAskingFor();
      item = NULL;
      updateControls();
      break;

    case UI_CANCEL_BTN:
      stopAskingFor();
      break;

    case UI_CENTER:
      if (item != NULL) {
	  Vect oldCenter = item->getCenter();

	  if (preventCollision) {	      
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
	  
	  centerField[0]->set_float_val(center[0]);
	  centerField[1]->set_float_val(center[1]);
	  centerField[2]->set_float_val(center[2]);
	  centerXButton = -center[0];
	}
      break;

    case UI_CENTER_X_BTN:
      center[0] = -centerXButton;
      controlEvent(UI_CENTER);
      break;

    case UI_SIZE:
      if (item != NULL) {
	  if (size[0] <MIN_SOLID_SIZE)
	    size[0] = MIN_SOLID_SIZE;
	  if (size[1] <MIN_SOLID_SIZE)
	    size[1] = MIN_SOLID_SIZE;
	  if (size[2] <MIN_SOLID_SIZE)
	    size[2] = MIN_SOLID_SIZE;
	  
	  if (dynamic_cast<Sphere*>(item) != NULL) {
	      //printf("resizing sphere\n");
	      size[0] = size[1] = size[2];
	      radiusField->set_float_val(size[2]);
	  }
	  
	  Vect oldSize = item->getSize();

	  if (preventCollision) {	      
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
	  sizeField[0]->set_float_val(size[0]);
	  sizeField[1]->set_float_val(size[1]);
	  sizeField[2]->set_float_val(size[2]);
	  rightSubwin->sync_live();
	  bottomSubwin->sync_live();
	}
      break;

    case UI_ROT_Z:
      if (item != NULL) {
	  while(angleZ>180)
	    angleZ -= 360;
	  while(angleZ<-180)
	    angleZ += 360;
	  
	  double oldAngle = item->getOrientation()[2];

	  if (preventCollision) {	      
	      int wasColliding = Vred::treeRoot->collide(*item);
	      item->setOrientation( Vect(0, 0, angleZ) );

	      if (!wasColliding && Vred::treeRoot->collide(*item)) {
		  // there's a true collision : it wasn't colliding but now it is
		  // so we restore previous state
		  //printf( "COLLISION !!\n");
		  item->setOrientation( Vect(0, 0, oldAngle) );
		  angleZ = oldAngle;
	      }
	  }
	  else
	    item->setOrientation( Vect(0, 0, angleZ) );
	  
	  angleZField->set_float_val(angleZ);
      }
      break;

    case UI_RADIUS:
      if (item != NULL) {
	  if (radius <MIN_SOLID_SIZE)
	    radius = MIN_SOLID_SIZE;
	  
	  double oldRadius = item->getSize()[0];

	  if (preventCollision) {	      
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
	  topSubwin->sync_live();
      }
      break;

    case UI_TEXTURE:
      item->setTexture(Tex(urlXp, urlXn, urlYp, urlYn, urlZp, urlZn));
      
      if (urlXp[0]!=0 || urlXn[0]!=0 || urlYp[0]!=0 || urlYn[0]!=0 || urlZp[0]!=0 || urlZn[0]!=0 )
	  item->setStyle(TEXTURED);
      else
	item->setStyle(COLORED);

      //printf("Tex modified\n");
      break;

    case APPEARANCE:
      if (item != NULL) {	  
	Color difColor(dif[0], dif[1], dif[2], 1.0);
	Color ambColor(amb[0], amb[1], amb[2], 1.0);
	Color shiColor(shi[0], shi[1], shi[2], 1.0);
	Color speColor(spe[0], spe[1], spe[2], 1.0);
	App newAppearance( ambColor, difColor, speColor, shiColor );
	item->setApp( newAppearance );
	//printf("appearance modified\n");
      }
      break;

    case TARGET_URL:
      {
	Gate *gate = dynamic_cast<Gate*>(item);
	if (gate != NULL) {
	    gate->setUrl(url);
	    gate->setIpmc(ipmc);
	}
	
	Web *web = dynamic_cast<Web*>(item);
	if (web != NULL)
	  web->setUrl(url);
	
	Host *host = dynamic_cast<Host*>(item);
	if (host != NULL)
	  host->setHostname(url);
	
	Doc *doc = dynamic_cast<Doc*>(item);
	if (doc != NULL)
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
#if 0 //dax
  if ( glutGetWindow() != mainWindowID ) 
    glutSetWindow(mainWindowID);
#endif

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this application  */
  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/
void UI::myGlutMouse(int button, int button_state, int x, int y)
{
  if ( button_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON ) {      
      Solid *formerSelectedItem = item;

      item = RayCaster::getSelection(camera->getCenter(), camera->getWatch(),
					     x, glutGet((GLenum)GLUT_WINDOW_HEIGHT)-bottomSubwin->getH()-y,
					     0,
					     0,
					     glutGet((GLenum)GLUT_WINDOW_WIDTH)-rightSubwin->getW(),
					     glutGet((GLenum)GLUT_WINDOW_HEIGHT)-topSubwin->getH()-bottomSubwin->getH() );
 
      if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
	if (item != NULL) {
	  struct objectChain *maillon = (struct objectChain*)malloc(sizeof(struct objectChain));
	  maillon->current = item;
	  maillon->next = selected;
	  selected = maillon;
	}
	if (formerSelectedItem != NULL) {
	  struct objectChain *maillon = (struct objectChain*)malloc(sizeof(struct objectChain));
	  maillon->current = formerSelectedItem;
	  maillon->next = selected;
	  selected = maillon;
	}
      }
      else {
	// on desselectionne tout
	while (selected != NULL) {
	  struct objectChain *suivant = selected->next;

	  free(selected);
	  selected = suivant;
	}
      }
      if (selected != NULL)
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
  printf("myGlutReshape: glutPostRedisplay\n");
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
  while (currentObjChain != NULL) {
    renderSolid(currentObjChain->current);
    currentObjChain = currentObjChain->next;
  }
  if (item != NULL)
    renderSolid(item);

  glutSwapBuffers(); 
}

void UI::renderSolid(Solid *solid)
{
  if (solid == NULL) return;

  //printf("renderSolid: ");
  //solid->Print();
  
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
void UI::SetupUI(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 800, 700 );
 
  mainWindowID = glutCreateWindow( "VRed" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  GLUI_Master.set_glutSpecialFunc( myGlutSpecialKeyboard );
  glutMotionFunc( myGlutMotion );
  GLUI_Master.set_glutIdleFunc( NULL );
  
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

  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  gluPerspective(60.0, 1.0, 0.2, 100.0);

  camera = new Camera("cam0", Vect(7.0, 6.0, 3.0), Vect(0.0, 20.0, -140.0));
  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  /*** Create the top subwindow ***/
  topSubwin = GLUI_Master.create_glui_subwindow(mainWindowID, GLUI_SUBWINDOW_TOP);
  printf("subwindow top\n");
  topSubwin->set_main_gfx_window(mainWindowID);

  /* Boxtype */  
  GLUI_Panel *objectTypePanel = topSubwin->add_panel("");
  GLUI_Listbox *objectTypeList = topSubwin->add_listbox_to_panel(objectTypePanel, "Object type:", &currentObjectType);
  for (unsigned int i=0; i < sizeof(objectTypesString) / sizeof(const char*); i++)
    objectTypeList->add_item(i, (char *) objectTypesString[i]);

  topSubwin->add_column_to_panel(objectTypePanel, false);
  topSubwin->add_button_to_panel(objectTypePanel, "Add", UI_ADD_OBJECT, controlEvent);
  
  /* collision */
  topSubwin->add_separator();
  topSubwin->add_checkbox("Prevent collision while moving objects", &preventCollision);

  /* placement */
  topSubwin->add_column(true);
  GLUI_Panel *xyzPanel = topSubwin->add_panel("");
  (centerField[0] = topSubwin->add_edittext_to_panel(xyzPanel,
                                                      "x",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &center[0],
                                                      UI_CENTER,
                                                      controlEvent))->set_w(25);
  topSubwin->add_column_to_panel(xyzPanel, false);
  (centerField[1] = topSubwin->add_edittext_to_panel(xyzPanel,
                                                      "y",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &center[1],
                                                      UI_CENTER,
                                                      controlEvent))->set_w(25);
  topSubwin->add_column_to_panel(xyzPanel, false);
  (centerField[2] = topSubwin->add_edittext_to_panel(xyzPanel,
                                                      "z",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &center[2],
                                                      UI_CENTER,
                                                      controlEvent))->set_w(25);

  GLUI_Panel *deltaXyzPanel = topSubwin->add_panel("");
  (sizeField[0] = topSubwin->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dx",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &size[0],
                                                    UI_SIZE,
                                                    controlEvent ) )->set_w(25);
  topSubwin->add_column_to_panel(deltaXyzPanel, false);
  (sizeField[1] = topSubwin->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dy",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &size[1],
                                                    UI_SIZE,
                                                    controlEvent ) )->set_w(25);
  topSubwin->add_column_to_panel(deltaXyzPanel, false);
  (sizeField[2] = topSubwin->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dz",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &size[2], UI_SIZE,
                                                    controlEvent ) )->set_w(25);
  topSubwin->add_column(false);
  angleZField = topSubwin->add_edittext("Angle /Z",
                                          GLUI_EDITTEXT_FLOAT, 
					  &angleZ,
                                          UI_ROT_Z,
                                          controlEvent);
  angleZField->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);

  radiusField = topSubwin->add_edittext("Radius",
                                           GLUI_EDITTEXT_FLOAT, 
					   &radius,
                                           UI_RADIUS,
                                           controlEvent);

  /*** Create the side subwindow ***/
  rightSubwin = GLUI_Master.create_glui_subwindow(mainWindowID, GLUI_SUBWINDOW_RIGHT);
  printf("subwindow right\n");

  rightSubwin->set_main_gfx_window(mainWindowID);
  /* texture */
  texRollout = rightSubwin->add_rollout( "Texture" );
  (texXp = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Xp",
                                                GLUI_EDITTEXT_TEXT,
                                                urlXp,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texXn = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Xn",
                                                GLUI_EDITTEXT_TEXT,
                                                urlXn,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texYp = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Yp",
                                                GLUI_EDITTEXT_TEXT,
                                                urlYp,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texYn = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Yn",
                                                GLUI_EDITTEXT_TEXT,
                                                urlYn,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texZp = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Zp",
                                                GLUI_EDITTEXT_TEXT,
                                                urlZp,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texZn = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Zn",
                                                GLUI_EDITTEXT_TEXT,
                                                urlZn,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  (texSph = rightSubwin->add_edittext_to_panel(texRollout,
                                                "Sp",
                                                GLUI_EDITTEXT_TEXT,
                                                urlXp,
                                                UI_TEXTURE,
                                                controlEvent))->set_w(128);
  
  rightSubwin->add_button_to_panel(texRollout, "Load textures", UI_TEXTURE, controlEvent);

  /* Appearance */
  appearanceRollout = rightSubwin->add_rollout( "Appearance" );

  GLUI_Panel *ambientPanel = rightSubwin->add_panel_to_panel(appearanceRollout, "Ambient");
  (ambField[0] = rightSubwin->add_edittext_to_panel(ambientPanel,
                                                      "R",
                                                      GLUI_EDITTEXT_FLOAT,
						      &amb[0],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  ambField[0]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(ambientPanel, false);  
  (ambField[1] = rightSubwin->add_edittext_to_panel(ambientPanel,
                                                      "G",
                                                      GLUI_EDITTEXT_FLOAT,
						      &amb[1],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  ambField[1]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(ambientPanel, false);
  (ambField[2] = rightSubwin->add_edittext_to_panel(ambientPanel,
                                                      "B",
                                                      GLUI_EDITTEXT_FLOAT,
						      &amb[2],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  ambField[2]->set_float_limits(0, 1);

  GLUI_Panel *diffusePanel = rightSubwin->add_panel_to_panel(appearanceRollout, "Diffuse");
  (difField[0] = rightSubwin->add_edittext_to_panel(diffusePanel,
                                                      "R",
                                                      GLUI_EDITTEXT_FLOAT,
					              &dif[0],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  difField[0]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(diffusePanel, false);  
  (difField[1] = rightSubwin->add_edittext_to_panel(diffusePanel,
                                                      "G",
                                                      GLUI_EDITTEXT_FLOAT, 
						      &dif[1],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  difField[1]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(diffusePanel, false);
  (difField[2] = rightSubwin->add_edittext_to_panel(diffusePanel,
                                                      "B",
                                                      GLUI_EDITTEXT_FLOAT,
						      &dif[2],
                                                      APPEARANCE,
                                                      controlEvent ) )->set_w(20);
  difField[2]->set_float_limits(0, 1);

  GLUI_Panel *specularPanel = rightSubwin->add_panel_to_panel(appearanceRollout, "Specular");
  (speField[0] = rightSubwin->add_edittext_to_panel(specularPanel,
                                                       "V1",
                                                        GLUI_EDITTEXT_FLOAT,
							&spe[0],
                                                        APPEARANCE,
                                                        controlEvent ) )->set_w(20);
  speField[0]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(specularPanel, false);  
  (speField[1] = rightSubwin->add_edittext_to_panel(specularPanel,
                                                        "V2",
                                                        GLUI_EDITTEXT_FLOAT,
							&spe[1],
                                                        APPEARANCE,
                                                        controlEvent ) )->set_w(20);
  speField[1]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(specularPanel, false);
  (speField[2] = rightSubwin->add_edittext_to_panel(specularPanel,
                                                        "V3",
                                                        GLUI_EDITTEXT_FLOAT,
							&spe[2],
                                                        APPEARANCE,
                                                        controlEvent ) )->set_w(20);
  speField[2]->set_float_limits(0, 1);

  GLUI_Panel *shininessPanel = rightSubwin->add_panel_to_panel(appearanceRollout, "Shininess");
  (shiField[0] = rightSubwin->add_edittext_to_panel(shininessPanel,
                                                         "V1",
                                                         GLUI_EDITTEXT_FLOAT,
							 &shi[0],
                                                         APPEARANCE,
                                                         controlEvent ) )->set_w(20);
  shiField[0]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(shininessPanel, false);  
  (shiField[1] = rightSubwin->add_edittext_to_panel(shininessPanel,
                                                         "V2",
                                                         GLUI_EDITTEXT_FLOAT,
							 &shi[1],
                                                         APPEARANCE,
                                                         controlEvent ) )->set_w(20);
  shiField[1]->set_float_limits(0, 1);
  rightSubwin->add_column_to_panel(shininessPanel, false);
  (shiField[2] = rightSubwin->add_edittext_to_panel(shininessPanel,
                                                         "V3",
                                                         GLUI_EDITTEXT_FLOAT,
							 &shi[2],
                                                         APPEARANCE,
                                                         controlEvent ) )->set_w(20);
  shiField[2]->set_float_limits(0, 1);

  (urlField = rightSubwin->add_edittext("To Where (URL)",
                                           GLUI_EDITTEXT_TEXT,
                                           url, 
					   TARGET_URL,
                                           controlEvent ) )->set_w(128);
  (ipmcField = rightSubwin->add_edittext("IP Multicast",
                                               GLUI_EDITTEXT_TEXT,
                                               ipmc, 
					       TARGET_URL,
                                               controlEvent ) )->set_w(32);
  
  rightSubwin->add_separator();
  objectClassDescription = rightSubwin->add_statictext("Object type :");
  
  /*********** South subWindow **********************************/

  bottomSubwin = GLUI_Master.create_glui_subwindow(mainWindowID, GLUI_SUBWINDOW_BOTTOM);
  bottomSubwin->set_main_gfx_window(mainWindowID);
  printf("subwindow bottom\n");
  
  objectTransXYButton = bottomSubwin->add_translation("Object YZ",
                                                     GLUI_TRANSLATION_XY,
                                                     &center[1],
                                                     UI_CENTER,
                                                     controlEvent);
  objectTransXYButton->set_speed(0.1);
  bottomSubwin->add_column(false);
  objectTransZButton = bottomSubwin->add_translation("Object X",
                                                     GLUI_TRANSLATION_Z,
                                                     &centerXButton,
                                                     UI_CENTER_X_BTN,
                                                     controlEvent);
  objectTransZButton->set_speed(0.1);
  bottomSubwin->add_column(false);
  angleZButton = bottomSubwin->add_translation("Rot /Z",
                                              GLUI_TRANSLATION_X,
                                              &angleZ,
                                              UI_ROT_Z,
                                              controlEvent);
  bottomSubwin->add_column(true);
  sizeButton[0] =  bottomSubwin->add_translation("Size X",
                                                GLUI_TRANSLATION_Z, 
						&size[0],
                                                UI_SIZE,
                                                controlEvent);
  sizeButton[0]->set_speed(0.1);
  bottomSubwin->add_column(false);
  sizeButton[1] =  bottomSubwin->add_translation("Size Y",
                                                GLUI_TRANSLATION_X, 
						&size[1],
                                                UI_SIZE,
                                                controlEvent);
  sizeButton[1]->set_speed(0.1);
  bottomSubwin->add_column(false);
  sizeButton[2] =  bottomSubwin->add_translation("Size Z",
                                                GLUI_TRANSLATION_Y, 
						&size[2],
                                                UI_SIZE,
                                                controlEvent);
  sizeButton[2]->set_speed(0.1);

  bottomSubwin->add_column(true);
  groupButton = bottomSubwin->add_button("Group", UI_GROUP, controlEvent);
  ungroupButton = bottomSubwin->add_button("Ungroup", UI_UNGROUP, controlEvent);
  bottomSubwin->add_column(false);
  deleteButton = bottomSubwin->add_button("Delete", UI_DELETE, controlEvent);

  /*  buttons save and quit */
  bottomSubwin->add_column(true);
  bottomSubwin->add_button("Open", UI_OPEN, controlEvent);
  bottomSubwin->add_button("Save", UI_SAVE, controlEvent);
  bottomSubwin->add_separator();
  bottomSubwin->add_button("Quit", UI_QUIT, controlEvent);

  updateControls();
}

void UI::createNewObject()
{
  item = NULL;
  switch(currentObjectType) {
    case WALL_TYPE:
      item = new Wall("unMur", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case GATE_TYPE:
      item = new Gate("unMur", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case EARTH_TYPE:
      item = new Earth("uneSphere", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
				Tex(), App(), 10, 10 );
      break;
    case WEB_TYPE:
      item = new Web("unWeb", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case BOARD_TYPE:
      item = new Board("unBoard", Vect::null, Vect::null, Vect::unit, COLORED, Color::white,
			       Tex(), App() );
      break;
    case HOST_TYPE:
      item = new Host("unHost", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case DOC_TYPE:
      item = new Board("unDoc", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case STEP_TYPE:
      item = new Board("unStep", Vect::null, Vect::null, Vect::unit, COLORED, Color::white,
			       Tex(), App() );
      break;
    default:
      printf("object type unknown\n");
  }
  
  if (item != NULL)
    Vred::treeRoot->add(item);

  updateControls();
  printf("createNewObject: glutPostRedisplay\n");
  glutPostRedisplay();
}

void UI::updateControls()
{
  angleZField->disable();
  centerField[0]->disable();
  centerField[1]->disable();
  centerField[2]->disable();
  sizeField[0]->disable();
  sizeField[1]->disable();
  sizeField[2]->disable();
  sizeButton[0]->disable();
  sizeButton[1]->disable();
  sizeButton[2]->disable();

  texRollout->disable();
  appearanceRollout->disable();
  urlField->disable();
  ipmcField->disable();

  angleZButton->disable();
  objectTransXYButton->disable();
  objectTransZButton->disable();
  
  groupButton->disable();
  ungroupButton->disable();
  deleteButton->disable();

  if (item == NULL) {
    radiusField->Hide();
     
    if (selected != NULL)
      groupButton->enable();
      topSubwin->refresh();
      return;
  }

  // it's at least a solid
  angleZField->enable();
  centerField[0]->enable();
  centerField[1]->enable();
  centerField[2]->enable();
  angleZButton->enable();
  objectTransXYButton->enable();
  objectTransZButton->enable();
  deleteButton->enable();
  
  /* box */
  Box *box = dynamic_cast<Box*>(item);
  if (box != NULL) { //on a bien un objet de type box
    sizeField[0]->enable();
    sizeField[1]->enable();
    sizeField[2]->enable();
    sizeButton[0]->enable();
    sizeButton[1]->enable();
    sizeButton[2]->enable();
    radiusField->Hide();
 
    Tex tex = item->getTexture();
    texRollout->enable(); 
    texSph->Hide();

    texXp->Show();
    Safe::strcpy(urlXp, tex.getTex_xp() );
    texXn->Show();
    Safe::strcpy(urlXn, tex.getTex_xn() );
    texYp->Show();
    Safe::strcpy(urlYp, tex.getTex_yp() );
    texYn->Show();
    Safe::strcpy(urlYn, tex.getTex_yn() );
    texZp->Show();
    Safe::strcpy(urlZp, tex.getTex_zp() );
    texZn->Show();
    Safe::strcpy(urlZn, tex.getTex_zn() );
    
    appearanceRollout->enable();
    
    rightSubwin->refresh();
    topSubwin->refresh();
    rightSubwin->sync_live();
  }

  Sphere *sphere = dynamic_cast<Sphere*>(item);
  if (sphere != NULL) {
    radiusField->Show();
    sizeButton[2]->enable();

    texRollout->enable(); 
   
    texXp->Hide();
    texXn->Hide();
    texYp->Hide();
    texYn->Hide();
    texZp->Hide();
    texZn->Hide();
    texSph->Show();

    Tex tex = item->getTexture();
    Safe::strcpy(urlXp, tex.getTex_xp() );
    urlXn[0] = urlYp[0] = urlYn[0] = urlZp[0] = urlZn[0] = 0;
    appearanceRollout->enable();
    topSubwin->refresh();
    rightSubwin->refresh();
  }

  Group *group = dynamic_cast<Group*>(item);
  if (group != NULL) {
    ungroupButton->enable();

    radiusField->Hide();
    sizeField[0]->enable();
    sizeField[1]->enable();
    sizeField[2]->enable();
    sizeButton[0]->enable();
    sizeButton[1]->enable();
    sizeButton[2]->enable();
  }

  int classId = item->getClassId();
  if (classId == GATE) {
    urlField->enable();
    Safe::strcpy( url, ((Gate*)item)->getUrl() );
    ipmcField->enable();
    Safe::strcpy( ipmc, ((Gate*)item)->getIpmc() );
  }
  if (classId == WEB || classId==DOC || classId==HOST)
    urlField->enable();
  if (classId == WEB)
    Safe::strcpy( url, ((Web*)item)->getUrl() );
  if (classId == DOC)
    Safe::strcpy( url, ((Doc*)item)->getUrl() );
  if (classId == HOST)
    Safe::strcpy( url, ((Host*)item)->getHostname() );
  
  Vect tempVect;
  tempVect = item->getCenter();
  center[0] = tempVect[0];
  center[1] = tempVect[1];
  center[2] = tempVect[2];

  tempVect = item->getSize();
  size[0] = tempVect[0];
  size[1] = tempVect[1];
  size[2] = tempVect[2];
  radius = size[2];
  
  tempVect = item->getOrientation();
  angleZ = tempVect[2];
 
  char tempStr[128];
  sprintf( tempStr, "Object type : %s", item->getClassName() );
  objectClassDescription->set_text(tempStr);
  
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

void UI::askFor( const char *_question, int _dlogUsage, const char *_defaultAnswer )
{
  if (dialogGlui == NULL) {
    if (dialogString == NULL)
    dialogString = (char*)malloc(128);

    dialogGlui = GLUI_Master.create_glui("");
    dialogEditText = dialogGlui->add_edittext( (char*)_question, GLUI_EDITTEXT_TEXT, dialogString, UI_OK_BTN);
    dialogEditText->set_w(256);
    dialogGlui->add_column(false);
    dialogGlui->add_button("OK", UI_OK_BTN, controlEvent);
    dialogGlui->add_button("Cancel", UI_CANCEL_BTN, controlEvent);
      
    dialogGlui->set_main_gfx_window(mainWindowID);
  }

  dialogEditText->set_name((char*)_question);
  strcpy(dialogString, _defaultAnswer);
  dialogUsage = _dlogUsage;
  dialogGlui->show();
  dialogGlui->sync_live();
}

void UI::stopAskingFor()
{
  dialogGlui->hide();
}
