#ifndef UI_H
#define UI_H

#include "glui.h"

class Solid;
/*
class GLUI;
class GLUI_EditText;
class GLUI_StaticText;
class GLUI_Translation;
class GLUI_Rollout;
class GLUI_Button;
*/

enum UIenum
{
  UI_ADD_OBJECT,
  UI_GROUP,
  UI_UNGROUP,
  UI_DELETE,
  UI_OPEN,
  UI_SAVE,
  UI_QUIT,
  UI_OK_BTN,
  UI_CANCEL_BTN,
  UI_CENTER,
  UI_CENTER_X_BTN,
  UI_SIZE, 
  UI_ROT_Z,
  UI_RADIUS,
  UI_TEXTURE,
  APPEARANCE,
  TARGET_URL
};

enum UIDialogUsage
{
  DLOG_OPEN_FILE,
  DLOG_SAVE_FILE
};

struct ObjectChain
{
  Solid *current;
  struct ObjectChain *next;
};

class Camera;
class Vect;

class UI
{
 private:
  static int sMainWindowID;
  static GLUI *sNorthGlui, *sEastGlui, *sSouthGlui, *sDialogGlui;
  static Solid *sSelectedItem;
  static struct ObjectChain *sSelectedSet;

  static const char *kObjectTypesString[];
  static int sCurrentObjectType;
  
  
  static GLUI_EditText *sCenterField[3];
  static GLUI_Translation *sObjectTransXYButton;
  static GLUI_Translation *sObjectTransZButton;
  static float sCenterVal[3], sCenterXButtonVal;

  static GLUI_EditText *sSizeField[3];
  static GLUI_Translation *sSizeButton[3];
  static float sSizeVal[3];

  static GLUI_EditText *sAngleZField;
  static GLUI_Translation *sAngleZButton;
  static float sAngleZVal;
  
  static GLUI_EditText *sRadiusField;
  static float sRadiusVal;

  static GLUI_Rollout *sTextureRollout;
  static GLUI_EditText *sTextureXp, *sTextureXn, *sTextureYp, *sTextureYn;
  static GLUI_EditText *sTextureZp, *sTextureZn, *sTextureSphere;
  static char sTexURLXp[300], sTexURLXn[300], sTexURLYp[300], sTexURLYn[300];
  static char sTexURLZp[300], sTexURLZn[300];

  static GLUI_StaticText *sObjectClassDescription;

  static GLUI_Rollout *sAppearanceRollout;
  static GLUI_EditText *sDiffuseField[3], *sAmbientField[3], *sShininessField[3], *sSpecularField[3];
  static float sDiffuseVal[3], sAmbientVal[3], sShininessVal[3], sSpecularVal[3];

  static GLUI_EditText *sToWhereField, *sIPMulticastField;
  static char sToWhereVal[300], sIPMulticastVal[300];

  static float sViewRotate[16];

  static GLUI_Button *sGroupButton, *sUngroupButton, *sDeleteButton;

  static int sPreventCollision;

  static Camera *sCamera;

  static bool sMotionEnabled;
  static int sMouseX, sMouseY;

  static char *sDialogString;
  static GLUI_EditText *sDialogEditText;
  static int sDialogUsage;

  // ----------------------

  static void HandleControlEvent(int control);
  static void MyGlutIdle();
  static void MyGlutMouse(int button, int button_state, int x, int y );
  static void MyGlutMotion(int x, int y );
  static void MyGlutKeyboard(unsigned char key, int x, int y);
  static void MyGlutSpecialKeyboard(int key, int x, int y);

  static void MyGlutReshape( int x, int y );
  static void MyGlutDisplay( void );
  static void RenderSolidAsSelected(Solid *solid);
  static void DrawGrid();
  static void Vertex(const Vect& v);
  static void CreateNewObject();
  static void UpdateControls();
  static void AskForString(const char *_question, int _dlogUsage, const char *_defaultAnswer="");
  static void StopAskingForString();
public:
  static void SetupUI(int argc, char *argv[]);
};

#endif
