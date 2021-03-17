/************************************************************************
 *
 *  uconf.cpp : configuration of the UAppli
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include "config.h"	// LINUX
#include <locale.h>
#include <iostream>
#include <ubit/udefs.hpp>
#include <ubit/unode.hpp>
#include <ubit/uconf.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/upix.hpp>
#include <ubit/ustyleparser.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uclassImpl.hpp>  // for UStyleSheet (bizarrement)
#include <ubit/uevent.hpp>
#include <ubit/uima.hpp>
#include <ubit/ubackground.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


// TRUETYPE font dirs (in /usr/X11R6/ /usr/include/ /usr/openwin/include )
#define XFT_DIR "/usr/X11/share/fonts/Type1/" // macosx
#if LINUX
#  if HAVE_TTF_PFA
#    define LFT_DIR TTFPATH
#  else
#    define LFT_DIR "./fonts/"
//DAX#    error Sorry, but the TTF font aith .pfa suffix must exist
#  endif
#else
#  define LFT_DIR ""
#endif
//#if UBUNTU | DEBIAN
//#define LFT_DIR "/usr/share/fonts/X11/Type1/" //ubunto
//#else
//#define LFT_DIR "/usr/share/X11/fonts/Type1/" // fedora
//#endif
#define OFT_DIR "/usr/openwin/lib/X11/fonts/TrueType/" // solaris
// /System/Library/Frameworks/JavaVM.framework/Versions/1.3.1/Home/lib/fonts/LucidaSansRegular.ttf";
  
// these definitions MUST be HERE and in THIS order to avoid init incoherencies
UFontFamily UFontFamily::defaults("defaults", UCONST);    // private constr!
UFontFamily UFontFamily::sans_serif("sans_serif",UCONST);
UFontFamily UFontFamily::serif("serif",UCONST);
UFontFamily UFontFamily::monospace("monospace",UCONST);
  
UConf UAppli::conf;   // configuration of the UAppli

/* ==================================================== ===== ======= */

UConf::~UConf() {
  // on detruit rien car il peut y avoir des pointeurs partages
  // tant pis pour la memoire (cout negligeable en pratique)
}

UConf::UConf() : 
freeze_options(false),

#if UBIT_WITH_GLUT
windowing_toolkit("GLUT"),
is_using_gl(true),
is_using_freetype(true),
  
#elif UBIT_WITH_X11
windowing_toolkit("X11"),
#  if UBIT_WITH_GL    // X11 with GL
is_using_gl(true),  // GL est le mode par defaut pour X11, si disponible
is_using_freetype(true),  // FT idem
#  else          // X11 without GL
is_using_gl(false),
is_using_freetype(false),
#  endif

#elif UBIT_WITH_GDK
windowing_toolkit("GDK"),
is_using_gl(false),
is_using_freetype(false),

#else
#  error "One of the UBIT_WITH_GLUT, UBIT_WITH_X11, UBIT_WITH_GDK macros must be defined and set to 1"
#endif

bpp(24),             // default=24 : use 24 bits whenever possible
depth_size(0),       // openGL depth buffer
stencil_size(0),     // openGL stencil buffer
verbosity(false),
scale(1.),	         // default=1. : no scaling factor
filebox_width(400),
filebox_height(230),
filebox_line_count(10),
mouse_select_button(UMouseEvent::LeftButton),
mouse_menu_button(UMouseEvent::RightButton),
mouse_alt_button(UMouseEvent::MidButton),
locale(""),   // default="" : the locale is given by the environment (ex: "iso_8859_1")
default_background(*new UBackground(UColor::lightgrey))
{
  // - - - fonts - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  // specifies default font characteristics used when FontFamilies can't be found
  UFontFamily::defaults.setFamily("helvetica,arial,fixed"); // coma separated list of family names
  UFontFamily::defaults.setStyles("r", "o,i");        // normal_style, italic_style
  UFontFamily::defaults.setWeights("medium", "bold"); // normal_weight, bold_weight
  UFontFamily::defaults.setCompression("normal");
  UFontFamily::defaults.setEncoding("1");
  UFontFamily::defaults.setPreferredSizes("2,6,7,8,9,10,11,12,14,16,18,20,24,34,48,72");
  UFontFamily::defaults.setFreeType
#if LINUX
  //regular
  (XFT_DIR"UTRG____.pfa,"LFT_DIR"UTRG____.pfa",   //-adobe-utopia-medium-r-normal--0-0-0-0-p-0-iso8859-1
   //bold
   XFT_DIR"UTB_____.pfa,"LFT_DIR"UTB_____.pfa",   //-adobe-utopia-bold-r-normal--0-0-0-0-p-0-iso8859-1
   //italic
   XFT_DIR"UTI_____.pfa,"LFT_DIR"UTI_____.pfa",   //-adobe-utopia-medium-i-normal--0-0-0-0-p-0-iso8859-1
   //bolditalic
   XFT_DIR"UTBI____.pfa,"LFT_DIR"UTBI____.pfa");  //-adobe-utopia-bold-i-normal--0-0-0-0-p-0-iso8859-1
#else
  //regular
  (XFT_DIR"l048013t.pfa,"LFT_DIR"l048013t.pfa",   //b&h-luxi sans-medium-r-**-iso8859-1
   //bold
   XFT_DIR"l048016t.pfa,"LFT_DIR"l048016t.pfa",   //b&h-luxi sans-bold-r-**-iso8859-1
   //italic
   XFT_DIR"l048033t.pfa,"LFT_DIR"l048033t.pfa",   //b&h-luxi sans-medium-o-**-iso8859-1
   //bolditalic
   XFT_DIR"l048036t.pfa,"LFT_DIR"l048036t.pfa");  //b&h-luxi sans-bold-o-**-iso8859-1
#endif
  
  UFontFamily::sans_serif = UFontFamily::defaults;
  UFontFamily::sans_serif.setFamily("helvetica,arial");
  UFontFamily::sans_serif.setFreeType
#if LINUX
    //regular
    (XFT_DIR"UTRG____.pfa,"LFT_DIR"UTRG____.pfa,"OFT_DIR"Arial.ttf",  //-adobe-utopia-medium-r-normal--0-0-0-0-p-0-iso8859-1
     //bold
     XFT_DIR"UTB_____.pfa,"LFT_DIR"UTB_____.pfa,"OFT_DIR"Arial-Bold.ttf", //-adobe-utopia-bold-r-normal--0-0-0-0-p-0-iso8859-1
     //italic
     XFT_DIR"UTI_____.pfa,"LFT_DIR"UTI_____.pfa,"OFT_DIR"Arial-Italic.ttf", //-adobe-utopia-medium-i-normal--0-0-0-0-p-0-iso8859-1
     //bolditalic
     XFT_DIR"UTBI____.pfa,"LFT_DIR"UTBI____.pfa,"OFT_DIR"Arial-BoldItalic.ttf"); //-adobe-utopia-bold-i-normal--0-0-0-0-p-0-iso8859-1
#else
    //regular
    (XFT_DIR"l048013t.pfa,"LFT_DIR"l048013t.pfa,"OFT_DIR"Arial.ttf",  //b&h-luxi sans-medium-r-**-iso8859-1
     //bold
     XFT_DIR"l048016t.pfa,"LFT_DIR"l048016t.pfa,"OFT_DIR"Arial-Bold.ttf", //b&h-luxi sans-bold-r-**-iso8859-1
     //italic
     XFT_DIR"l048033t.pfa,"LFT_DIR"l048033t.pfa,"OFT_DIR"Arial-Italic.ttf", //b&h-luxi sans-medium-o-**-iso8859-1
     //bolditalic
     XFT_DIR"l048036t.pfa,"LFT_DIR"l048036t.pfa,"OFT_DIR"Arial-BoldItalic.ttf"); //b&h-luxi sans-bold-o-**-iso8859-1
#endif
  
  UFontFamily::serif = UFontFamily::defaults;
  UFontFamily::serif.setFamily("times");
  UFontFamily::serif.setStyles("r", "i,o");
  UFontFamily::serif.setFreeType
    //regular
    (XFT_DIR"l049013t.pfa,"LFT_DIR"l049013t.pfa,"XFT_DIR"UTRG____.pfa", //b&h-luxi serif-medium-r-**-iso8859-1
     //bold
     XFT_DIR"l049016t.pfa,"LFT_DIR"l049016t.pfa,"XFT_DIR"UTB_____.pfa", //b&h-luxi serif-bold-r-**-iso8859-1
     //italic
     XFT_DIR"l049033t.pfa,"LFT_DIR"l049033t.pfa,"XFT_DIR"UTI_____.pfa", //b&h-luxi serif-medium-o-**-iso8859-1
     //bolditalic
     XFT_DIR"l049036t.pfa,"LFT_DIR"l049036t.pfa,"XFT_DIR"UTBI____.pfa"); //b&h-luxi serif-bold-o-**-iso8859-1
  
  UFontFamily::monospace = UFontFamily::defaults;
  UFontFamily::monospace.setFamily("courier");
  UFontFamily::monospace.setFreeType
    //regular
    (XFT_DIR"l047013t.pfa,"LFT_DIR"l047013t.pfa,"XFT_DIR"cour.pfa", //b&h-luxi mono-medium-r-**-iso8859-1
     //bold
     XFT_DIR"l047016t.pfa,"LFT_DIR"l047016t.pfa,"XFT_DIR"courb.pfa", //b&h-luxi mono-bold-r-**-iso8859-1
     //italic
     XFT_DIR"l047033t.pfa,"LFT_DIR"l047033t.pfa,"XFT_DIR"couri.pfa", //b&h-luxi mono-medium-o-**-iso8859-1
     //bolditalic
     XFT_DIR"l047036t.pfa,"LFT_DIR"l047036t.pfa,"XFT_DIR"courbi.pfa"); //b&h-luxi mono-bold-o-**-iso8859-1
  
   // - - - defaults - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  default_font = &UFont::sans_serif;     // CANT be null!
  setDefaultBackground(UPix::metal);     // UPix::velin, UColor::lightgrey
  unknow_image = &UPix::question;  
  selection_color  = null;               // none if pointer is null
  selection_bgcolor= &UColor::lightblue; // idem
  selection_font = &UFont::fill;         // idem
  
  // - - - modes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  xsync = usync = false;
  menu_grab = true;
  postpone_delete = true;
  soft_dbf = true;
#if UBIT_WITH_GLUT
  soft_menus = true;
  soft_wins = true;
#else
  soft_menus = false;
  soft_wins = false;
#endif
  transp_scrollbars = false;
  tele_pointers = false;      // group mode sets this mode to true
  //linear_gamma = false;     // linear gamma visual for Solaris
  generic_textsel = false;
  click_radius        = 15;	  // MUST be defined
  // NB: attention a ne pas mettre une valeur trop grande sinon les
  // click/action vont devenir des multiclicks
  multi_click_delay  = 250;	  // millisec
  auto_repeat_delay  = 150;	  // millisec
  open_tip_delay     = 500;	  // millisec
  open_submenu_delay = 300;	  // millisec
 }

/* ==================================================== ===== ======= */

void UConf::setLocale(const UStr& s) {locale = s;}
void UConf::setScale(float v) {scale = v;}
void UConf::setVerbose(int v) {verbosity = v;}
  
void UConf::setDefaultBackground(const UBackground& bg) {
  default_background = bg;
}
/*
void UConf::setDefaultBackground(const UColor& c) {
  default_background.setColor(c);
}
void UConf::setDefaultBackground(UIma& i) {
  default_background.setIma(i);
}
 */

void UConf::setTransparentScrollbars(bool s) {transp_scrollbars = s;}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void freezed_error(const char* fct) {
  UAppli::error(fct,"this function must be called before the UAppli is created");
}

#ifndef UBIT_WITH_GL
static void withoutgl_error(const char* fct, const char* ext = "") {
  UAppli::error(fct,"this option is not available because the Ubit toolkit has not been compiled with OpenGL %s", ext);
}
#endif

void UConf::useGL(bool state) {
  if (freeze_options) {freezed_error("UConf::useGL"); return;}
  if (!state) is_using_gl = false;
  else
#if UBIT_WITH_GL
    is_using_gl = true;
#else
    withoutgl_error("UConf::useGL");
#endif
}

void UConf::setDepthBuffer(int s) {
  if (freeze_options) {freezed_error("UConf::setDepthBuffer"); return;}
  if (s <= 0) depth_size = 0;
  else
#if UBIT_WITH_GL
    depth_size = s;
#else
    withoutgl_error("UConf::setDepthBuffer");
#endif
}

void UConf::setStencilBuffer(int s) {
  if (freeze_options) {freezed_error("UConf::setStencilBuffer"); return;}
  if (s <= 0) stencil_size = 0;
  else
#if UBIT_WITH_GL
    stencil_size = s;
#else
    withoutgl_error("UConf::setStencilBuffer");
#endif
}

void UConf::useFreeType(bool state) {
  if (freeze_options) {freezed_error("UConf::useFreeType"); return;}
  if (!state) is_using_freetype = false;
  else
#if UBIT_WITH_GL && UBIT_WITH_FREETYPE
    is_using_freetype = true;
#else
   withoutgl_error("UConf::useFreeType", "and FreeType");
#endif
}

void UConf::setBpp(int b) {
  if (freeze_options) freezed_error("UConf::setBpp");
  else bpp = b;
}

/* ==================================================== ===== ======= */

void UConf::parseOptions(int& argc, char** argv, UOption* options) {
  UOption::parseOptions(argc, argv, options);
}

void UConf::parseUbitOptions(int& argc, char** argv)
{
  const char* conf_file = null;
  bool help = false, version = false, group = false, all_sync = false;
  
  UOption ubit_opts[] = {
  {"conf","",     UOption::Arg(conf_file)},
  {"help","-",    UOption::Arg(help)},
  {"ubit","",     UOption::Arg(help)},
  {"vers","ion",  UOption::Arg(version)},
  {"verb","ose",  UOption::Arg(verbosity)},
    
  {"disp","lay",  UOption::Arg(display)},
  {"gl","",       UOption::Arg(is_using_gl)},
  {"free","type", UOption::Arg(is_using_freetype)},
  {"sync","hronous",UOption::Arg(all_sync)},
  {"xsync","",    UOption::Arg(xsync)},
  {"usync","",    UOption::Arg(usync)},
  
  {"locale","",   UOption::Arg(locale)},
  {"scale","",    UOption::Arg(scale)},
  {"bpp","",      UOption::Arg(bpp)},
  {"mgrab","",    UOption::Arg(menu_grab)},
  {"dbf","",      UOption::Arg(soft_dbf)},
  {"sfw","",      UOption::Arg(soft_wins)},
  {"sfm","",      UOption::Arg(soft_menus)},
  {"tsb","",      UOption::Arg(transp_scrollbars)},
  {"telep","",    UOption::Arg(tele_pointers)},
  {"group","ware",UOption::Arg(group)},

  {null, null, null}
  };
  
  parseOptions(argc, argv, ubit_opts);
  
  if (help) {printHelp(); exit(0);}
  if (conf_file) readAttributes(conf_file);

  if (all_sync) xsync = usync = true;
  if (is_using_gl) useGL(true);
  if (is_using_freetype) useFreeType(true);  // no effect if -no-gl
    
  if (is_using_gl) {
    soft_dbf = false;
  }
  else if (is_using_freetype) {   // without GL
    UAppli::warning("UConf", "FreeType option deactivated because it requires OpenGL");
    is_using_freetype = false;
  }
  
  if (group) soft_wins = soft_menus = tele_pointers = true;
  
  if (version || verbosity != 0) {
    cerr
    << "Ubit version: " << UAppli::getVersion() << endl
    << "(run the application with --help-ubit to display all options)" << endl
    << "Current configuration: " << endl
    << "- underlying windowing toolkit: " << windowing_toolkit << endl
    << "- using Open GL: " << (is_using_gl ? "yes" : "no") << endl
    << "- using FreeType Fonts: " << (is_using_freetype ? "yes" : "no") << endl
    //<< "- using 3D mode: " << (using_3d ? "yes" : "no") 
    << endl;
    if (version) exit(0);
  }
}

/* ==================================================== ===== ======= */

void UConf::printHelp()
{
  cout
  << "Ubit options: "
  << "\n  --help-x, --help-ubit : prints this message"
  << "\n  --vers[ion]           : prints the current version and configuration"
  << "\n  --verb[ose] int       : sets warning messages verbosity"
  << "\n  --display host[:scr]  : displays the application on this host"
  << "\n  --locale string       : sets the locale [default = environment locale]"
  << "\n  --scale float         : sets the global rendering scale [default = 1.0]"
  << "\n  --bpp int             : sets the number of bits per pixel [default = 24]"
  << "\n  --[no-]gl             : enable/disable OpenGL [when available]"
  << "\n  --[no-]sync           : X synchronous mode [default = disabled]"
  << "\n  --[no-]mgrab          : mouse grabs in menus [default = enabled]"
  << "\n  --[no-]dbf            : soft double buffering for X11 [default = enabled]"
  << "\n  --[no-]tsb            : transparent scrollbars [default = disabled]"
  << "\n  --[no-]sfm            : soft menus [default = disabled]"
  << "\n  --[no-]sfd            : soft dialogs [default = disabled]"
  << "\n  --[no-]group          : groupware mode [default = disabled]"
  << "\n  --[no-]telep          : tele pointer mode [default = disabled]"
  << endl << endl;
}

/* ==================================================== [(C)Elc] ======= */

struct DefaultStyleMaker: public UStyleParser::StyleMaker {
  UStyleSheet& smap;
  UAttrList* props;
  
  DefaultStyleMaker(UStyleSheet& _smap) : smap(_smap), props(null) {}
  virtual void begin() {props = null;}
  virtual void end(bool ok) {props = null;}
  
  virtual void create() {
    for (unsigned int k = 0; k < count; k++) {
      // att le selecteur peut etre vide!
      const UClass* c = smap.obtainClass(*selectors[k]);
      if (k == 0) {
        props = c->getAttributes();      // props partagee !!!
        if (!props) props = new UAttrList();
      }
      c->setAttributes(props);
    }
  }
  
  virtual void addProp(const UStr& name, const UStr& value) {
    if (!props)
      UAppli::error("DefaultStyleMaker","this DefaultStyleMaker object (%p) has no prop list", this);
    props->addAttr(name, value);
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void UConf::readAttributes(const char* conf_file) {
  if (!conf_file || !*conf_file) return;
  UStr buf;
  if (buf.read(conf_file) <= 0) return;
  
  DefaultStyleMaker dsm(UAppli::getStyleSheet());
  UStyleParser parser;
  parser.parseImpl(dsm, buf);
}

UAttrList* UConf::getAttributes(const char* category) {
  const UClass* ec = UAppli::getStyleSheet().findClass(category);
  if (ec) return ec->getAttributes() ; else return null;
}

/* ==================================================== [(C)Elc] ======= */

static const char* testOption(const char* arg, const char* name,
                              const char* ending, const char*& stat) {
  const char *pa = arg, *pn;
  stat = "true";
    
  if (pa[0] == 'n' && pa[1] == 'o' && pa[2] == '-') {
    stat = null;
    pa = arg+3;
  }
  
  for (pn = name; *pn != 0; pn++) {
    if (*pa == *pn) pa++;
    else return null;
  }
  
  for (pn = ending; *pn != 0; pn++) {
    if (*pa == *pn) pa++;
    else if (*pa == 0) return pa;
    else return null;
  }

  return pa;
}

static void removeOption(int& card, char** tab, int k) {
  card--;
  for (int j = k; j < card; j++) tab[j] = tab[j+1];
}

static UOption* matchOption(const char* arg, UOption* options, const char*& stat) {
  for (UOption* p = options; p->begname != null; p++) {
    if (testOption(arg, p->begname, p->endname, stat)) return p;
  }
  return null;  // not found
}

/* ==================================================== ===== ======= */

class UOptionArg {
public:
  virtual ~UOptionArg() {}
  virtual int getArgCount() = 0;
  virtual void set(const char*) = 0;
};

class UOptionBoolArg : public UOptionArg {
public:
  bool& val;
  UOptionBoolArg(bool& _val) : val(_val) {}
  virtual int getArgCount() {return 0;}
  virtual void set(const char* _val) {val = (_val ? true : false);}
};

class UOptionIntArg : public UOptionArg {
public:
  int& val;
  UOptionIntArg(int& _val) : val(_val) {}
  virtual int getArgCount() {return 1;}
  virtual void set(const char* _val) {val = (_val ? atoi(_val) : 0);}
};

class UOptionFloatArg : public UOptionArg {
public:
  float& val;
  UOptionFloatArg(float& _val) : val(_val) {}
  virtual int getArgCount() {return 1;}
  virtual void set(const char* _val) {val = (_val ? atof(_val) : 0);}
};

class UOptionStrArg : public UOptionArg {
public:
  UStr& val;
  UOptionStrArg(UStr& _val) : val(_val) {}
  virtual int getArgCount() {return 1;}
  virtual void set(const char* _val) {val = _val;}
};

class UOptionC_StrArg : public UOptionArg {
public:
  char*& val;
  UOptionC_StrArg(char*& _val) : val(_val) {}
  virtual int getArgCount() {return 1;}
  virtual void set(const char* _val) {val = (_val ? UCstr::dup(_val) : null);}
};

class UOptionCC_StrArg : public UOptionArg {
public:
  const char*& val;
  UOptionCC_StrArg(const char*& _val) : val(_val) {}
  virtual int getArgCount() {return 1;}
  virtual void set(const char* _val) {val = (_val ? UCstr::dup(_val) : null);}
};

/* ==================================================== ======== ======= */

class UOptionArg* UOption::Arg(bool& _val) {
  return new UOptionBoolArg(_val);
}

class UOptionArg* UOption::Arg(int& _val) {
  return new UOptionIntArg(_val);
}

class UOptionArg* UOption::Arg(float& _val) {
  return new UOptionFloatArg(_val);
}

class UOptionArg* UOption::Arg(UStr& _val) {
  return new UOptionStrArg(_val);
}

class UOptionArg* UOption::Arg(char*& _val) {
  return new UOptionC_StrArg(_val);
}

class UOptionArg* UOption::Arg(const char*& _val) {
  return new UOptionCC_StrArg(_val);
}

/* ==================================================== ===== ======= */
 
void UOption::parseOptions(int& argc, char** argv, UOption* options) {
  if (!options) return;

  int k = 1;
  while (k < argc) { 
    const char *p = argv[k];
    const char* stat = null;
    
    if (*p != '-') {k++; continue;}     // un - obligatoire
    else p++;
    if (*p == '-') p++;                 // un - optionnel
    if (*p == 0) {k++; continue;}
    
    UOption* opt = null;
    
    if ((opt = matchOption(p, options, stat))) {
      removeOption(argc, argv, k);
      if (opt->arg->getArgCount() == 0) opt->arg->set(stat);
      
      else if (opt->arg->getArgCount() > 0) {
        if (k < argc) {
          opt->arg->set(argv[k]);
          removeOption(argc, argv, k);
        }
        else {
          std::string optname;
          if (opt->begname) {
            optname = " --";
            optname += opt->begname;
          }
          if (opt->endname) {
            optname += "[";
            optname += opt->endname;
            optname += "]";
          }
          cerr << "Warning: option" << optname <<  " requires an argument" << endl;
        }
      }
    }
    else k++;   // autre options: a traiter ailleurs
  }
}


}
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
