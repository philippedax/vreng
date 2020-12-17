#include <GL/glut.h>

#include "ui.hpp"
#include "vred.hpp"
#include "vre.hpp"
#include "safe.hpp"
#include "raycaster.hpp"

#define MIN_SOLID_SIZE  0.1


const char *UI::kObjectTypesString[] =
                 {"Wall", "Gate", "Step", "Web", "Board", "Host", "Doc", "Earth"};
enum UIObjectTypes {UI_WALL_TYPE, UI_GATE_TYPE, UI_STEP_TYPE, UI_WEB_TYPE, 
		 UI_BOARD_TYPE, UI_HOST_TYPE, UI_DOC_TYPE,UI_EARTH_SPHERE_TYPE  };
int UI::sCurrentObjectType = 0;
int UI::sMainWindowID;
GLUI *UI::sNorthGlui, *UI::sEastGlui, *UI::sSouthGlui, *UI::sDialogGlui=NULL;
Solid *UI::sSelectedItem=NULL;
struct ObjectChain *UI::sSelectedSet=NULL;
GLUI_EditText *UI::sCenterField[3];
GLUI_Translation *UI::sObjectTransXYButton;
GLUI_Translation *UI::sObjectTransZButton;
float UI::sCenterVal[3] = {0., 0., 0.}, UI::sCenterXButtonVal=0;
GLUI_EditText *UI::sSizeField[3];
GLUI_Translation *UI::sSizeButton[3];
float UI::sSizeVal[3];
GLUI_EditText *UI::sAngleZField;
GLUI_Translation *UI::sAngleZButton;
float UI::sAngleZVal;
GLUI_EditText *UI::sRadiusField;
float UI::sRadiusVal;
GLUI_Rollout *UI::sTextureRollout;
GLUI_EditText *UI::sTextureXp, *UI::sTextureXn, *UI::sTextureYp, *UI::sTextureYn;
GLUI_EditText *UI::sTextureZp, *UI::sTextureZn, *UI::sTextureSphere;
char UI::sTexURLXp[300]="", UI::sTexURLXn[300]="", UI::sTexURLYp[300]="", UI::sTexURLYn[300]="";
char UI::sTexURLZp[300]="", UI::sTexURLZn[300]="";
GLUI_StaticText *UI::sObjectClassDescription;
GLUI_Rollout *UI::sAppearanceRollout;
GLUI_EditText *UI::sDiffuseField[3], *UI::sAmbientField[3], *UI::sShininessField[3], *UI::sSpecularField[3];
float UI::sDiffuseVal[3], UI::sAmbientVal[3], UI::sShininessVal[3], UI::sSpecularVal[3];
GLUI_EditText *UI::sToWhereField, *UI::sIPMulticastField;
char UI::sToWhereVal[300]="", UI::sIPMulticastVal[300]="";
float UI::sViewRotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
GLUI_Button *UI::sGroupButton, *UI::sUngroupButton, *UI::sDeleteButton;
int UI::sPreventCollision=true;
Camera *UI::sCamera;
bool UI::sMotionEnabled = false;
int UI::sMouseX, UI::sMouseY;
char *UI::sDialogString=NULL;
GLUI_EditText *UI::sDialogEditText;
int UI::sDialogUsage;


/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */
void UI::HandleControlEvent( int control )
{
  switch (control) {

    case UI_ADD_OBJECT:
      CreateNewObject();
      break;

    case UI_GROUP:
      {
	Group *newGroup = new Group("newGroup", COLORED, Color::green);
	Vred::treeRoot->Add(newGroup);
	int i=0;

	while (sSelectedSet != NULL) {
	    newGroup->Add(sSelectedSet->current);
	    i++;
	    struct ObjectChain *suivant = sSelectedSet->next;
	    free(sSelectedSet);
	    sSelectedSet = suivant;
	}
	printf("%d in new group\n", i);
	sSelectedItem = newGroup;
	UpdateControls();
      }
      break;

    case UI_UNGROUP:
      {
	Group *group = dynamic_cast<Group*>(sSelectedItem);
	if (group != NULL) {
	    group->Explode();
	    Vred::treeRoot->Remove(group);
	    delete group;
	}
	sSelectedItem = NULL;
	UpdateControls();
      }
      break;

    case UI_DELETE:
      if (sSelectedItem != NULL) {
	  Vred::treeRoot->Remove(sSelectedItem);
	  sSelectedItem = NULL;
	  UpdateControls();

	  printf("Object deleted from treeRoot\n");
      }
      break;

    case UI_OPEN:
      AskForString("File to open :", DLOG_OPEN_FILE);
      break;

    case UI_SAVE:
      AskForString("File to save :", DLOG_SAVE_FILE, "output.vre");
      break;

    case UI_QUIT:
      exit(0);
      break;

    case UI_OK_BTN:
      switch (sDialogUsage) {
	case DLOG_OPEN_FILE:
	  delete Vred::treeRoot;
	  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);
	  printf("Loading file.................");
	  if( Vred::treeRoot->LoadFile(sDialogString) != 0 )
	    printf( "FAILED TO LOAD FILE : %s\n", sDialogString );
	  else
	    printf("OK\n");
	  sSelectedItem = NULL;
	  
	  break;
	case DLOG_SAVE_FILE:
	  Vred::treeRoot->PrintFile(sDialogString);
	  break;
      }
      StopAskingForString();
      sSelectedItem = NULL;
      UpdateControls();
      break;

    case UI_CANCEL_BTN:
      StopAskingForString();
      break;

    case UI_CENTER:
      if (sSelectedItem != NULL) {
	  Vect oldCenter = sSelectedItem->GetCenter();

	  if (sPreventCollision) {	      
	      int wasColliding = Vred::treeRoot->Collide(*sSelectedItem);
	      sSelectedItem->SetCenter( Vect(sCenterVal[0], sCenterVal[1], sCenterVal[2]) );
	      if (!wasColliding && Vred::treeRoot->Collide(*sSelectedItem)) {
		  // there's a true collision : it wasn't colliding but now it is
		  // so we restore previous state
		  //printf( "COLLISION !!\n");
		  sSelectedItem->SetCenter(oldCenter);
		  sCenterVal[0] = oldCenter[0];
		  sCenterVal[1] = oldCenter[1];
		  sCenterVal[2] = oldCenter[2];  
		}
	    }
	  else
	    sSelectedItem->SetCenter( Vect(sCenterVal[0], sCenterVal[1], sCenterVal[2]) );
	  
	  sCenterField[0]->set_float_val(sCenterVal[0]);
	  sCenterField[1]->set_float_val(sCenterVal[1]);
	  sCenterField[2]->set_float_val(sCenterVal[2]);
	  sCenterXButtonVal = -sCenterVal[0];
	}
      break;

    case UI_CENTER_X_BTN:
      sCenterVal[0] = -sCenterXButtonVal;
      HandleControlEvent(UI_CENTER);
      break;

    case UI_SIZE:
      if (sSelectedItem != NULL) {
	  if( sSizeVal[0] <MIN_SOLID_SIZE)
	    sSizeVal[0] = MIN_SOLID_SIZE;
	  if( sSizeVal[1] <MIN_SOLID_SIZE)
	    sSizeVal[1] = MIN_SOLID_SIZE;
	  if( sSizeVal[2] <MIN_SOLID_SIZE)
	    sSizeVal[2] = MIN_SOLID_SIZE;
	  
	  if (dynamic_cast<Sphere*>(sSelectedItem) != NULL) {
	      //printf("resizing sphere\n");
	      sSizeVal[0] = sSizeVal[1] = sSizeVal[2];
	      sRadiusField->set_float_val(sSizeVal[2]);
	  }
	  
	  Vect oldSize = sSelectedItem->GetSize();

	  if (sPreventCollision) {	      
	      int wasColliding = Vred::treeRoot->Collide(*sSelectedItem);

	      sSelectedItem->SetSize(Vect(sSizeVal[0], sSizeVal[1], sSizeVal[2]));  
	      if (!wasColliding && Vred::treeRoot->Collide(*sSelectedItem)) {
		  // there's a true collision : it wasn't colliding but now it is
		  // so we restore previous state
		  //printf( "COLLISION !!\n");
		  sSelectedItem->SetSize(oldSize);
		  sSizeVal[0] = oldSize[0];
		  sSizeVal[1] = oldSize[1];
		  sSizeVal[2] = oldSize[2];  
		  sRadiusVal = sSizeVal[2];
	      }
	  }
	  else
	    sSelectedItem->SetSize(Vect(sSizeVal[0], sSizeVal[1], sSizeVal[2]));  
	  sSizeField[0]->set_float_val(sSizeVal[0]);
	  sSizeField[1]->set_float_val(sSizeVal[1]);
	  sSizeField[2]->set_float_val(sSizeVal[2]);
	  sEastGlui->sync_live();
	  sSouthGlui->sync_live();
	}
      break;

    case UI_ROT_Z:
      if (sSelectedItem != NULL) {
	  while(sAngleZVal>180)
	    sAngleZVal -= 360;
	  while(sAngleZVal<-180)
	    sAngleZVal += 360;
	  
	  double oldAngle = sSelectedItem->GetOrientation()[2];

	  if (sPreventCollision) {	      
	      int wasColliding = Vred::treeRoot->Collide(*sSelectedItem);
	      sSelectedItem->SetOrientation( Vect(0, 0, sAngleZVal) );

	      if (!wasColliding && Vred::treeRoot->Collide(*sSelectedItem)) {
		  // there's a true collision : it wasn't colliding but now it is
		  // so we restore previous state
		  //printf( "COLLISION !!\n");
		  sSelectedItem->SetOrientation( Vect(0, 0, oldAngle) );
		  sAngleZVal = oldAngle;
	      }
	  }
	  else
	    sSelectedItem->SetOrientation( Vect(0, 0, sAngleZVal) );
	  
	  sAngleZField->set_float_val(sAngleZVal);
      }
      break;

    case UI_RADIUS:
      if (sSelectedItem != NULL) {
	  if( sRadiusVal <MIN_SOLID_SIZE)
	    sRadiusVal = MIN_SOLID_SIZE;
	  
	  double oldRadius = sSelectedItem->GetSize()[0];

	  if (sPreventCollision) {	      
	      int wasColliding = Vred::treeRoot->Collide(*sSelectedItem);

	      sSelectedItem->SetSize( Vect(sRadiusVal, sRadiusVal, sRadiusVal) );
	      if (!wasColliding && Vred::treeRoot->Collide(*sSelectedItem)) {
		  // there's a true collision : it wasn't colliding but now it is
		  // so we restore previous state
		  //printf( "COLLISION !!\n");
		  sSelectedItem->SetSize(Vect(oldRadius, oldRadius, oldRadius));
		  sRadiusVal = oldRadius;
	      }
	  }
	  else
	    sSelectedItem->SetSize( Vect(sRadiusVal, sRadiusVal, sRadiusVal) );            
	  sSizeVal[0] = sSizeVal[1] = sSizeVal[2] = sRadiusVal;
	  sNorthGlui->sync_live();
      }
      break;

    case UI_TEXTURE:
      sSelectedItem->SetTexture(Tex(sTexURLXp, sTexURLXn, sTexURLYp, sTexURLYn, sTexURLZp, sTexURLZn) );
      
      if( sTexURLXp[0]!=0 || sTexURLXn[0]!=0 || sTexURLYp[0]!=0 || sTexURLYn[0]!=0 || sTexURLZp[0]!=0 || sTexURLZn[0]!=0 )
	  sSelectedItem->SetRenderStyle(TEXTURED);
      else
	sSelectedItem->SetRenderStyle(COLORED);

      //printf("Texture modified\n");
      break;

    case APPEARANCE:
      if (sSelectedItem != NULL) {	  
	Color diffColor(sDiffuseVal[0], sDiffuseVal[1], sDiffuseVal[2], 1.0);
	Color ambientColor(sAmbientVal[0], sAmbientVal[1], sAmbientVal[2], 1.0);
	Color shininessColor(sShininessVal[0], sShininessVal[1], sShininessVal[2], 1.0);
	Color specularColor(sSpecularVal[0], sSpecularVal[1], sSpecularVal[2], 1.0);
	App newAppearance( ambientColor, diffColor, specularColor, shininessColor );
	sSelectedItem->SetApp( newAppearance );
	//printf("appearance modified\n");
      }
      break;

    case TARGET_URL:
      {
	Gate *gate = dynamic_cast<Gate*>(sSelectedItem);
	if (gate != NULL) {
	    gate->SetToWhere(sToWhereVal);
	    gate->SetIp_multi(sIPMulticastVal);
	}
	
	Web *web = dynamic_cast<Web*>(sSelectedItem);
	if (web != NULL)
	  web->SetUrl(sToWhereVal);
	
	Host *host = dynamic_cast<Host*>(sSelectedItem);
	if (host != NULL)
	  host->SetHostname(sToWhereVal);
	
	Doc *doc = dynamic_cast<Doc*>(sSelectedItem);
	if (doc != NULL)
	  doc->SetUrl(sToWhereVal);

	printf("Target URL updated\n");
      }
      break;
    default:
      printf("action sur un controle non gere\n");
    }
}

/***************************************** myGlutIdle() ***********/
void UI::MyGlutIdle(void)
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  /* if ( glutGetWindow() != sMainWindowID ) 
    glutSetWindow(sMainWindowID);  */

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this application  */
 // glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/
void UI::MyGlutMouse(int button, int button_state, int x, int y)
{
  if ( button_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON ) {      
      Solid *formerSelectedItem = sSelectedItem;

      sSelectedItem = RayCaster::GetSelection(sCamera->GetCenter(), sCamera->GetWatch(),
					      x, glutGet((GLenum)GLUT_WINDOW_HEIGHT)-sSouthGlui->getH()-y,
					      0,
					      0,
					      glutGet((GLenum)GLUT_WINDOW_WIDTH)-sEastGlui->getW(),
					      glutGet((GLenum)GLUT_WINDOW_HEIGHT)-sNorthGlui->getH()-sSouthGlui->getH() );
 
      if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
	  if (sSelectedItem != NULL) {
	      struct ObjectChain *maillon = (struct ObjectChain*)malloc(sizeof(struct ObjectChain));
	      maillon->current = sSelectedItem;
	      maillon->next = sSelectedSet;
	      sSelectedSet = maillon;
	  }
	  if (formerSelectedItem != NULL) {
	      struct ObjectChain *maillon = (struct ObjectChain*)malloc(sizeof(struct ObjectChain));
	      maillon->current = formerSelectedItem;
	      maillon->next = sSelectedSet;
	      sSelectedSet = maillon;
	  }
      }
      else {
	  // on desselectionne tout
	  while (sSelectedSet != NULL) {
	      struct ObjectChain *suivant = sSelectedSet->next;

	      free(sSelectedSet);
	      sSelectedSet = suivant;
	  }
      }
      if (sSelectedSet != NULL)
	sSelectedItem = NULL;

      UpdateControls();
    }
  
  if (button_state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
      sMotionEnabled = true;
      sMouseX = x;
      sMouseY = y;
  }
  if (button_state == GLUT_UP && button == GLUT_RIGHT_BUTTON) {
      sMotionEnabled = false;
  }

  glutPostRedisplay();
}

/***************************************** myGlutMotion() **********/
void UI::MyGlutMotion(int x, int y)
{
  if (sMotionEnabled) {
    double dheading = 0.0, dpitch = 0.0;

    if (x < sMouseX - 1) {
      dheading = 2.0;
      sMouseX = x; 
    }
    else if (x > sMouseX + 1) {
      dheading = -2.0;
      sMouseX = x; 
    }
    if (y < sMouseY - 1) {
      dpitch = -2.0;
      sMouseY = y;
    }
    else if (y > sMouseY + 1) {
      dpitch = 2.0;
      sMouseY = y;
    }
    sCamera->Move(Vect::null, Vect(0.0, dpitch, dheading));
    glutPostRedisplay();
  }
}

/***************************************** myGlutKeyboard() **********/
void UI::MyGlutKeyboard(unsigned char key, int x, int y) 
{
  MyGlutSpecialKeyboard(key, x, y);
}

void UI::MyGlutSpecialKeyboard(int key, int x, int y) 
{
  switch (key) {
  case GLUT_KEY_UP:
    sCamera->Move(Vect(0.5, 0.0, 0.0), Vect::null);
    break;
  case GLUT_KEY_DOWN:
    sCamera->Move(Vect(-0.5, 0.0, 0.0), Vect::null);
    break;
  case GLUT_KEY_LEFT:
    sCamera->Move(Vect(0.0, 0.5, 0.0), Vect::null);
    break;
  case GLUT_KEY_RIGHT:
    sCamera->Move(Vect(0.0, -0.5, 0.0), Vect::null);
    break;
  case GLUT_KEY_PAGE_UP:
    sCamera->Move(Vect(0.0, 0.0, 0.5), Vect::null);
    break;
  case GLUT_KEY_PAGE_DOWN:
    sCamera->Move(Vect(0.0, 0.0, -0.5), Vect::null);
    break;
  case '4':
    sCamera->Move(Vect::null, Vect(0.0, 0.0, 5.0));
    break;
  case '6':
    sCamera->Move(Vect::null, Vect(0.0, 0.0, -5.0));
    break;
  case '8':
    sCamera->Move(Vect::null, Vect(0.0, -5.0, 0.0));
    break;
  case '2':
    sCamera->Move(Vect::null, Vect(0.0, 5.0, 0.0));
    break;
  default:
    //cout << "key: \"" << key << "\"" << endl;
    break;
  }
  glutPostRedisplay();
}

/**************************************** myGlutReshape() *************/
void UI::MyGlutReshape( int x, int y )
{
  GLUI_Master.auto_set_viewport();
  glutPostRedisplay();
}

/***************************************** myGlutDisplay() *****************/
void UI::MyGlutDisplay( void )
{
  printf("myGlutDisplay\n");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawGrid();
  Vred::treeRoot->Render();

  // pour les objets selectionnes: changer en wired
  // redessiner et rechanger en textured.

  struct ObjectChain *currentObjChain = sSelectedSet;
  while (currentObjChain != NULL) {
    RenderSolidAsSelected(currentObjChain->current);
    currentObjChain = currentObjChain->next;
  }
  if (sSelectedItem != NULL)
    RenderSolidAsSelected(sSelectedItem);

  glutSwapBuffers(); 
}

void UI::RenderSolidAsSelected(Solid *solid)
{
  if (solid == NULL) return;

  //printf("RenderSolidAsSelected : ");
  //solid->Print();
  
  int oldRenderStyle = solid->GetRenderStyle();
  solid->SetRenderStyle(WIRED);
  App app = solid->GetApp();
  Color color = app.GetAmbient();
  solid->SetApp(App(Color::red, app.GetDiffuse(), app.GetSpecular(), app.GetShininess()) );
  solid->Render();
  solid->SetRenderStyle(oldRenderStyle);
  solid->SetApp(App(color, app.GetDiffuse(), app.GetSpecular(), app.GetShininess()) );
}

void UI::Vertex(const Vect& v) {
  glVertex3d(v[0], v[1], v[2]);
}

void UI::DrawGrid()
{
  Vect ux(1., 0., 0.), uy(0., 1., 0.), v(-3., -3., 0.);

  glMaterialfv(GL_FRONT, GL_AMBIENT, Color::blue);
  glBegin(GL_LINES);
    Vertex(v);
    Vertex(v + 6 * ux);
    Vertex(v + uy);
    Vertex(v + uy + 6 * ux);
    Vertex(v + 2 * uy);
    Vertex(v + 2 * uy + 6 * ux);
    Vertex(v + 3 * uy);
    Vertex(v + 3 * uy + 6 * ux);
    Vertex(v + 4 * uy);
    Vertex(v + 4 * uy + 6 * ux);
    Vertex(v + 5 * uy);
    Vertex(v + 5 * uy + 6 * ux);
    Vertex(v + 6 * uy);
    Vertex(v + 6 * uy + 6 * ux);
  glEnd();
  v = Vect(-3.0, -3.0, 0.0);
  glBegin(GL_LINES);
    Vertex(v);
    Vertex(v + 6 * uy);
    Vertex(v + ux);
    Vertex(v + ux + 6 * uy);
    Vertex(v + 2 * ux);
    Vertex(v + 2 * ux + 6 * uy);
    Vertex(v + 3 * ux);
    Vertex(v + 3 * ux + 6 * uy);
    Vertex(v + 4 * ux);
    Vertex(v + 4 * ux + 6 * uy);
    Vertex(v + 5 * ux);
    Vertex(v + 5 * ux + 6 * uy);
    Vertex(v + 6 * ux);
    Vertex(v + 6 * ux + 6 * uy);
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
 
  sMainWindowID = glutCreateWindow( "VRed" );
  glutDisplayFunc( MyGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( MyGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( MyGlutKeyboard );
  GLUI_Master.set_glutMouseFunc( MyGlutMouse );
  GLUI_Master.set_glutSpecialFunc( MyGlutSpecialKeyboard );
  glutMotionFunc( MyGlutMotion );
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

  sCamera = new Camera("cam0", Vect(7.0, 6.0, 3.0), Vect(0.0, 20.0, -140.0));
  Vred::treeRoot = new Group("treeRoot", COLORED, Color::white);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  /*** Create the top subwindow ***/
  sNorthGlui = GLUI_Master.create_glui_subwindow(sMainWindowID, GLUI_SUBWINDOW_TOP);
  printf("subwindow top\n");
  sNorthGlui->set_main_gfx_window(sMainWindowID);

  /* Boxtype */  
  GLUI_Panel *objectTypePanel = sNorthGlui->add_panel("");
  GLUI_Listbox *objectTypeList = sNorthGlui->add_listbox_to_panel(objectTypePanel, "Object type:", &sCurrentObjectType);
  for (unsigned int i=0; i < sizeof(kObjectTypesString) / sizeof(const char*); i++)
    objectTypeList->add_item(i, (char *) kObjectTypesString[i]);

  sNorthGlui->add_column_to_panel(objectTypePanel, false);
  sNorthGlui->add_button_to_panel(objectTypePanel, "Add", UI_ADD_OBJECT, HandleControlEvent);
  
  /* collision */
  sNorthGlui->add_separator();
  sNorthGlui->add_checkbox("Prevent collision while moving objects", &sPreventCollision);

  /* placement */
  sNorthGlui->add_column(true);
  GLUI_Panel *xyzPanel = sNorthGlui->add_panel("");
  (sCenterField[0] = sNorthGlui->add_edittext_to_panel(xyzPanel,
                                                      "x",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &sCenterVal[0],
                                                      UI_CENTER,
                                                      HandleControlEvent))->set_w(25);
  sNorthGlui->add_column_to_panel(xyzPanel, false);
  (sCenterField[1] = sNorthGlui->add_edittext_to_panel(xyzPanel,
                                                      "y",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &sCenterVal[1],
                                                      UI_CENTER,
                                                      HandleControlEvent))->set_w(25);
  sNorthGlui->add_column_to_panel(xyzPanel, false);
  (sCenterField[2] = sNorthGlui->add_edittext_to_panel(xyzPanel,
                                                      "z",
                                                      GLUI_EDITTEXT_FLOAT,
                                                      &sCenterVal[2],
                                                      UI_CENTER,
                                                      HandleControlEvent))->set_w(25);

  GLUI_Panel *deltaXyzPanel = sNorthGlui->add_panel("");
  (sSizeField[0] = sNorthGlui->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dx",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &sSizeVal[0],
                                                    UI_SIZE,
                                                    HandleControlEvent ) )->set_w(25);
  sNorthGlui->add_column_to_panel(deltaXyzPanel, false);
  (sSizeField[1] = sNorthGlui->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dy",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &sSizeVal[1],
                                                    UI_SIZE,
                                                    HandleControlEvent ) )->set_w(25);
  sNorthGlui->add_column_to_panel(deltaXyzPanel, false);
  (sSizeField[2] = sNorthGlui->add_edittext_to_panel(deltaXyzPanel,
                                                    "Dz",
                                                    GLUI_EDITTEXT_FLOAT, 
						    &sSizeVal[2], UI_SIZE,
                                                    HandleControlEvent ) )->set_w(25);
  sNorthGlui->add_column(false);
  sAngleZField = sNorthGlui->add_edittext("Angle /Z",
                                          GLUI_EDITTEXT_FLOAT, 
					  &sAngleZVal,
                                          UI_ROT_Z,
                                          HandleControlEvent);
  sAngleZField->set_float_limits(-180, 180, GLUI_LIMIT_WRAP);

  sRadiusField = sNorthGlui->add_edittext("Radius",
                                           GLUI_EDITTEXT_FLOAT, 
					   &sRadiusVal,
                                           UI_RADIUS,
                                           HandleControlEvent);

  /*** Create the side subwindow ***/
  sEastGlui = GLUI_Master.create_glui_subwindow(sMainWindowID, GLUI_SUBWINDOW_RIGHT);
  printf("subwindow right\n");

  sEastGlui->set_main_gfx_window(sMainWindowID);
  /* texture */
  sTextureRollout = sEastGlui->add_rollout( "Texture" );
  (sTextureXp = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Xp",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLXp,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureXn = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Xn",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLXn,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureYp = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Yp",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLYp,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureYn = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Yn",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLYn,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureZp = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Zp",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLZp,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureZn = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Zn",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLZn,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  (sTextureSphere = sEastGlui->add_edittext_to_panel(sTextureRollout,
                                                "Sp",
                                                GLUI_EDITTEXT_TEXT,
                                                sTexURLXp,
                                                UI_TEXTURE,
                                                HandleControlEvent))->set_w(200);
  
  sEastGlui->add_button_to_panel(sTextureRollout, "Load textures", UI_TEXTURE, HandleControlEvent);

  /* Appearance */
  sAppearanceRollout = sEastGlui->add_rollout( "Appearance" );

  GLUI_Panel *ambientPanel = sEastGlui->add_panel_to_panel(sAppearanceRollout, "Ambient");
  (sAmbientField[0] = sEastGlui->add_edittext_to_panel(ambientPanel,
                                                      "R",
                                                      GLUI_EDITTEXT_FLOAT,
						      &sAmbientVal[0],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sAmbientField[0]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(ambientPanel, false);  
  (sAmbientField[1] = sEastGlui->add_edittext_to_panel(ambientPanel,
                                                      "G",
                                                      GLUI_EDITTEXT_FLOAT,
						      &sAmbientVal[1],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sAmbientField[1]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(ambientPanel, false);
  (sAmbientField[2] = sEastGlui->add_edittext_to_panel(ambientPanel,
                                                      "B",
                                                      GLUI_EDITTEXT_FLOAT,
						      &sAmbientVal[2],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sAmbientField[2]->set_float_limits(0, 1);

  GLUI_Panel *diffusePanel = sEastGlui->add_panel_to_panel(sAppearanceRollout, "Diffuse");
  (sDiffuseField[0] = sEastGlui->add_edittext_to_panel(diffusePanel,
                                                      "R",
                                                      GLUI_EDITTEXT_FLOAT,
					              &sDiffuseVal[0],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sDiffuseField[0]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(diffusePanel, false);  
  (sDiffuseField[1] = sEastGlui->add_edittext_to_panel(diffusePanel,
                                                      "G",
                                                      GLUI_EDITTEXT_FLOAT, 
						      &sDiffuseVal[1],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sDiffuseField[1]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(diffusePanel, false);
  (sDiffuseField[2] = sEastGlui->add_edittext_to_panel(diffusePanel,
                                                      "B",
                                                      GLUI_EDITTEXT_FLOAT,
						      &sDiffuseVal[2],
                                                      APPEARANCE,
                                                      HandleControlEvent ) )->set_w(20);
  sDiffuseField[2]->set_float_limits(0, 1);

  GLUI_Panel *specularPanel = sEastGlui->add_panel_to_panel(sAppearanceRollout, "Specular");
  (sSpecularField[0] = sEastGlui->add_edittext_to_panel(specularPanel,
                                                       "V1",
                                                        GLUI_EDITTEXT_FLOAT,
							&sSpecularVal[0],
                                                        APPEARANCE,
                                                        HandleControlEvent ) )->set_w(20);
  sSpecularField[0]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(specularPanel, false);  
  (sSpecularField[1] = sEastGlui->add_edittext_to_panel(specularPanel,
                                                        "V2",
                                                        GLUI_EDITTEXT_FLOAT,
							&sSpecularVal[1],
                                                        APPEARANCE,
                                                        HandleControlEvent ) )->set_w(20);
  sSpecularField[1]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(specularPanel, false);
  (sSpecularField[2] = sEastGlui->add_edittext_to_panel(specularPanel,
                                                        "V3",
                                                        GLUI_EDITTEXT_FLOAT,
							&sSpecularVal[2],
                                                        APPEARANCE,
                                                        HandleControlEvent ) )->set_w(20);
  sSpecularField[2]->set_float_limits(0, 1);

  GLUI_Panel *shininessPanel = sEastGlui->add_panel_to_panel(sAppearanceRollout, "Shininess");
  (sShininessField[0] = sEastGlui->add_edittext_to_panel(shininessPanel,
                                                         "V1",
                                                         GLUI_EDITTEXT_FLOAT,
							 &sShininessVal[0],
                                                         APPEARANCE,
                                                         HandleControlEvent ) )->set_w(20);
  sShininessField[0]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(shininessPanel, false);  
  (sShininessField[1] = sEastGlui->add_edittext_to_panel(shininessPanel,
                                                         "V2",
                                                         GLUI_EDITTEXT_FLOAT,
							 &sShininessVal[1],
                                                         APPEARANCE,
                                                         HandleControlEvent ) )->set_w(20);
  sShininessField[1]->set_float_limits(0, 1);
  sEastGlui->add_column_to_panel(shininessPanel, false);
  (sShininessField[2] = sEastGlui->add_edittext_to_panel(shininessPanel,
                                                         "V3",
                                                         GLUI_EDITTEXT_FLOAT,
							 &sShininessVal[2],
                                                         APPEARANCE,
                                                         HandleControlEvent ) )->set_w(20);
  sShininessField[2]->set_float_limits(0, 1);

  (sToWhereField = sEastGlui->add_edittext("To Where (URL)",
                                           GLUI_EDITTEXT_TEXT,
                                           sToWhereVal, 
					   TARGET_URL,
                                           HandleControlEvent ) )->set_w(250);
  (sIPMulticastField = sEastGlui->add_edittext("IP Multicast",
                                               GLUI_EDITTEXT_TEXT,
                                               sIPMulticastVal, 
					       TARGET_URL,
                                               HandleControlEvent ) )->set_w(250);
  
  sEastGlui->add_separator();
  sObjectClassDescription = sEastGlui->add_statictext("Object type :");
  
  /*********** South subWindow **********************************/

  sSouthGlui = GLUI_Master.create_glui_subwindow(sMainWindowID, GLUI_SUBWINDOW_BOTTOM);
  sSouthGlui->set_main_gfx_window(sMainWindowID);
  printf("subwindow bottom\n");
  
  sObjectTransXYButton = sSouthGlui->add_translation("Object YZ",
                                                     GLUI_TRANSLATION_XY,
                                                     &sCenterVal[1],
                                                     UI_CENTER,
                                                     HandleControlEvent);
  sObjectTransXYButton->set_speed(0.1);
  sSouthGlui->add_column(false);
  sObjectTransZButton = sSouthGlui->add_translation("Object X",
                                                     GLUI_TRANSLATION_Z,
                                                     &sCenterXButtonVal,
                                                     UI_CENTER_X_BTN,
                                                     HandleControlEvent);
  sObjectTransZButton->set_speed(0.1);
  sSouthGlui->add_column(false);
  sAngleZButton = sSouthGlui->add_translation("Rot /Z",
                                              GLUI_TRANSLATION_X,
                                              &sAngleZVal,
                                              UI_ROT_Z,
                                              HandleControlEvent);
  sSouthGlui->add_column(true);
  sSizeButton[0] =  sSouthGlui->add_translation("Size X",
                                                GLUI_TRANSLATION_Z, 
						&sSizeVal[0],
                                                UI_SIZE,
                                                HandleControlEvent);
  sSizeButton[0]->set_speed(0.1);
  sSouthGlui->add_column(false);
  sSizeButton[1] =  sSouthGlui->add_translation("Size Y",
                                                GLUI_TRANSLATION_X, 
						&sSizeVal[1],
                                                UI_SIZE,
                                                HandleControlEvent);
  sSizeButton[1]->set_speed(0.1);
  sSouthGlui->add_column(false);
  sSizeButton[2] =  sSouthGlui->add_translation("Size Z",
                                                GLUI_TRANSLATION_Y, 
						&sSizeVal[2],
                                                UI_SIZE,
                                                HandleControlEvent);
  sSizeButton[2]->set_speed(0.1);

  sSouthGlui->add_column(true);
  sGroupButton = sSouthGlui->add_button("Group", UI_GROUP, HandleControlEvent);
  sUngroupButton = sSouthGlui->add_button("Ungroup", UI_UNGROUP, HandleControlEvent);
  sSouthGlui->add_column(false);
  sDeleteButton = sSouthGlui->add_button("Delete", UI_DELETE, HandleControlEvent);

  /*  buttons save and quit */
  sSouthGlui->add_column(true);
  sSouthGlui->add_button("Open", UI_OPEN, HandleControlEvent);
  sSouthGlui->add_button("Save", UI_SAVE, HandleControlEvent);
  sSouthGlui->add_separator();
  sSouthGlui->add_button("Quit", UI_QUIT, HandleControlEvent);

  UpdateControls();
}

void UI::CreateNewObject()
{
  sSelectedItem = NULL;
  switch(sCurrentObjectType) {
    case UI_WALL_TYPE:
      sSelectedItem = new Wall("unMur", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case UI_GATE_TYPE:
      sSelectedItem = new Gate("unMur", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case UI_EARTH_SPHERE_TYPE:
      sSelectedItem = new Earth("uneSphere", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
				Tex(), App(),10, 10 );
      break;
    case UI_WEB_TYPE:
      sSelectedItem = new Web("unWeb", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case UI_BOARD_TYPE:
      sSelectedItem = new Board("unBoard", Vect::null, Vect::null, Vect::unit, COLORED, Color::white,
			       Tex(), App() );
      break;
    case UI_HOST_TYPE:
      sSelectedItem = new Host("unHost", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case UI_DOC_TYPE:
      sSelectedItem = new Board("unDoc", Vect::null, Vect::null, Vect::unit, COLORED, Color::white, 
			       Tex(), App() );
      break;
    case UI_STEP_TYPE:
      sSelectedItem = new Board("unStep", Vect::null, Vect::null, Vect::unit, COLORED, Color::white,
			       Tex(), App() );
      break;
    default:
      printf("object type unknown\n");
  }
  
  if (sSelectedItem != NULL)
    Vred::treeRoot->Add(sSelectedItem);

  UpdateControls();
  glutPostRedisplay();
}

void UI::UpdateControls()
{
  sAngleZField->disable();
  sCenterField[0]->disable();
  sCenterField[1]->disable();
  sCenterField[2]->disable();
  sSizeField[0]->disable();
  sSizeField[1]->disable();
  sSizeField[2]->disable();
  sSizeButton[0]->disable();
  sSizeButton[1]->disable();
  sSizeButton[2]->disable();

  sTextureRollout->disable();
  sAppearanceRollout->disable();
  sToWhereField->disable();
  sIPMulticastField->disable();

  sAngleZButton->disable();
  sObjectTransXYButton->disable();
  sObjectTransZButton->disable();
  
  sGroupButton->disable();
  sUngroupButton->disable();
  sDeleteButton->disable();

  if (sSelectedItem == NULL) {
    sRadiusField->Hide();
     
    if (sSelectedSet != NULL)
      sGroupButton->enable();
      sNorthGlui->refresh();
      return;
  }

  // it's at least a solid
  sAngleZField->enable();
  sCenterField[0]->enable();
  sCenterField[1]->enable();
  sCenterField[2]->enable();
  sAngleZButton->enable();
  sObjectTransXYButton->enable();
  sObjectTransZButton->enable();
  sDeleteButton->enable();
  
  /* box */
  Box *box = dynamic_cast<Box*>(sSelectedItem);
  if (box != NULL) { //on a bien un objet de type box
    sSizeField[0]->enable();
    sSizeField[1]->enable();
    sSizeField[2]->enable();
    sSizeButton[0]->enable();
    sSizeButton[1]->enable();
    sSizeButton[2]->enable();
    sRadiusField->Hide();
 
    Tex tex = sSelectedItem->GetTexture();
    sTextureRollout->enable(); 
    sTextureSphere->Hide();

    sTextureXp->Show();
    Safe::strcpy(sTexURLXp, tex.GetTex_xp() );
    sTextureXn->Show();
    Safe::strcpy(sTexURLXn, tex.GetTex_xn() );
    sTextureYp->Show();
    Safe::strcpy(sTexURLYp, tex.GetTex_yp() );
    sTextureYn->Show();
    Safe::strcpy(sTexURLYn, tex.GetTex_yn() );
    sTextureZp->Show();
    Safe::strcpy(sTexURLZp, tex.GetTex_zp() );
    sTextureZn->Show();
    Safe::strcpy(sTexURLZn, tex.GetTex_zn() );
    
    sAppearanceRollout->enable();
    
    sEastGlui->refresh();
    sNorthGlui->refresh();
    sEastGlui->sync_live();
  }

  Sphere *sphere = dynamic_cast<Sphere*>(sSelectedItem);
  if (sphere != NULL) {
    sRadiusField->Show();
    sSizeButton[2]->enable();

    sTextureRollout->enable(); 
   
    sTextureXp->Hide();
    sTextureXn->Hide();
    sTextureYp->Hide();
    sTextureYn->Hide();
    sTextureZp->Hide();
    sTextureZn->Hide();
    sTextureSphere->Show();

    Tex tex = sSelectedItem->GetTexture();
    Safe::strcpy(sTexURLXp, tex.GetTex_xp() );
    sTexURLXn[0] = sTexURLYp[0] = sTexURLYn[0] = sTexURLZp[0] = sTexURLZn[0] = 0;
    sAppearanceRollout->enable();
    sNorthGlui->refresh();
    sEastGlui->refresh();
  }

  Group *group = dynamic_cast<Group*>(sSelectedItem);
  if (group != NULL) {
    sUngroupButton->enable();

    sRadiusField->Hide();
    sSizeField[0]->enable();
    sSizeField[1]->enable();
    sSizeField[2]->enable();
    sSizeButton[0]->enable();
    sSizeButton[1]->enable();
    sSizeButton[2]->enable();
  }

  int classId = sSelectedItem->GetClassId();
  if (classId == GATE) {
    sToWhereField->enable();
    Safe::strcpy( sToWhereVal, ((Gate*)sSelectedItem)->ToWhere() );
    sIPMulticastField->enable();
    Safe::strcpy( sIPMulticastVal, ((Gate*)sSelectedItem)->Ip_multi() );
  }
  if (classId == WEB || classId==DOC || classId==HOST)
    sToWhereField->enable();
  if (classId == WEB)
    Safe::strcpy( sToWhereVal, ((Web*)sSelectedItem)->GetUrl() );
  if (classId == DOC)
    Safe::strcpy( sToWhereVal, ((Doc*)sSelectedItem)->GetUrl() );
  if (classId == HOST)
    Safe::strcpy( sToWhereVal, ((Host*)sSelectedItem)->GetHostname() );
  
  Vect tempVect;
  tempVect = sSelectedItem->GetCenter();
  sCenterVal[0] = tempVect[0];
  sCenterVal[1] = tempVect[1];
  sCenterVal[2] = tempVect[2];

  tempVect = sSelectedItem->GetSize();
  sSizeVal[0] = tempVect[0];
  sSizeVal[1] = tempVect[1];
  sSizeVal[2] = tempVect[2];
  sRadiusVal = sSizeVal[2];
  
  tempVect = sSelectedItem->GetOrientation();
  sAngleZVal = tempVect[2];
 
  char tempStr[300];
  sprintf( tempStr, "Object type : %s", sSelectedItem->GetClassName() );
  sObjectClassDescription->set_text(tempStr);
  
  Color tempColor = sSelectedItem->GetApp().GetAmbient();
  sAmbientVal[0] = tempColor[0];
  sAmbientVal[1] = tempColor[1];
  sAmbientVal[2] = tempColor[2];

  tempColor = sSelectedItem->GetApp().GetDiffuse();
  sDiffuseVal[0] = tempColor[0];
  sDiffuseVal[1] = tempColor[1];
  sDiffuseVal[2] = tempColor[2];

  tempColor = sSelectedItem->GetApp().GetSpecular();
  sSpecularVal[0] = tempColor[0];
  sSpecularVal[1] = tempColor[1];
  sSpecularVal[2] = tempColor[2];

  tempColor = sSelectedItem->GetApp().GetShininess();
  sShininessVal[0] = tempColor[0];
  sShininessVal[1] = tempColor[1];
  sShininessVal[2] = tempColor[2];

  GLUI_Master.sync_live_all();
}

void UI::AskForString( const char *_question, int _dlogUsage, const char *_defaultAnswer )
{
  if (sDialogGlui == NULL) {
    if (sDialogString == NULL)
    sDialogString = (char*)malloc(300);

    sDialogGlui = GLUI_Master.create_glui("");
    sDialogEditText = sDialogGlui->add_edittext( (char*)_question, GLUI_EDITTEXT_TEXT, sDialogString, UI_OK_BTN);
    sDialogEditText->set_w(400);
    sDialogGlui->add_column(false);

    sDialogGlui->add_button("OK", UI_OK_BTN, HandleControlEvent);
    sDialogGlui->add_button("Cancel", UI_CANCEL_BTN, HandleControlEvent);
      
    sDialogGlui->set_main_gfx_window(sMainWindowID);
  }

  sDialogEditText->set_name((char*)_question);
  strcpy(sDialogString, _defaultAnswer);
  sDialogUsage = _dlogUsage;
  sDialogGlui->show();
  sDialogGlui->sync_live();
}

void UI::StopAskingForString()
{
  sDialogGlui->hide();
}
