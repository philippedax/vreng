################################################################################
#
# X11 header and library
#
AC_DEFUN([AC_X11_CHECK],
[ 
  have_x11="no"
  AC_PATH_X
  AC_PATH_XTRA
  saved_cflags=$CFLAGS
  saved_ldflags=$LDFLAGS
  CFLAGS=$X_CFLAGS
  LDFLAGS="$X_LDFLAGS $X_LIBS"
  # 
  # Check for X libraries
  #
  AH_TEMPLATE(HAVE_LIBX11)
  AC_CHECK_LIB([X11], [XOpenDisplay], 
               [x_libs="$X_LIBS -lX11 $X_EXTRA_LIBS" AC_DEFINE(HAVE_LIBX11)],
               [AC_MSG_WARN([X11 library not found, check config.log! try --with-x])],
               [$X_LIBS $X_EXTRA_LIBS])
  # needed by ubit: XmuClientWindow
  AH_TEMPLATE(HAVE_LIBXMU)
  AC_CHECK_LIB([Xmu], [XmuClientWindow],
               [x_libs="$X_LIBS -lXmu $x_libs" AC_DEFINE(HAVE_LIBXMU)],
               [AC_MSG_WARN([Xmu library not found, check config.log! try apt|dnf|yum|brew install libXmu-devel | apt-get install libxmu-dev])],
               [$x_libs])
  
  x_libs="$X_LIBS $X_PRE_LIBS $x_libs"
  AH_TEMPLATE(HAVE_XMU_WINUTIL_H)
  AH_TEMPLATE(HAVE_XPM_H)
  case "`$canonical $build`" in
    *-*-solaris*) 
      AC_CHECK_HEADER([openwin/Xmu/WinUtil.h], [AC_DEFINE([HAVE_XMU_WINUTIL_H])],
        AC_MSG_WARN([unable to find Xmu/WinUtil.h vreng/ubit need it: install libxmu-dev]))
      AC_CHECK_HEADER([openwin/xpm.h], [AC_DEFINE([HAVE_XPM_H])], 
        AC_MSG_WARN([unable to find Xpm/xpm.h vreng/ubit need it: install libxpm-dev]))
      ;;
    *)
      AC_CHECK_HEADER([X11/Xmu/WinUtil.h], [AC_DEFINE([HAVE_XMU_WINUTIL_H])], 
        AC_MSG_WARN([unable to find Xmu/WinUtil.h vreng/ubit need it: install libxmu-dev]))
      AC_CHECK_HEADER([X11/xpm.h], [AC_DEFINE([HAVE_XPM_H])], 
        AC_MSG_WARN([unable to find Xpm/xpm.h vreng/ubit need it: install libxpm-dev]))
      ;;
  esac
  CFLAGS=$saved_cflags
  LDFLAGS=$saved_ldflags
  have_x11="yes"
])

################################################################################
#
# Glut
#
AC_DEFUN([AC_GLUT_CHECK],
[
am_libglut="no"
AH_TEMPLATE(HAVE_LIBGLUT)

case "`$canonical $build`" in
*-*-darwin*)
  AC_CHECK_HEADERS([GLUT/glut.h])
  OPENGL_LIBS=-framework GLUT /System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib  /System/Library/Frameworks/OpenGL.framework/Libraries/libGLU.dylib
  LIBS="$LIBS $(OPENGL_LIBS)"
  # search in MacPorts and Fink installs
  if test -d /wudr/X11/include ; then
    OSDEP_CFLAGS="-I/wudr/X11/include"
  elif test -d /opt/X11/include ; then
    OSDEP_CFLAGS="-I/opt/X11/include"
  elif test -d /opt/local/include ; then
    OSDEP_CFLAGS="-I/opt/local/include"
  elif test -d /sw/include ; then
    OSDEP_CFLAGS="$OSDEP_CFLAGS -I/sw/include"
  fi
  if test -d /usr/X11/lib ; then
    OSDEP_LIBS="-L/opt/X11/lib"
  elif test -d /opt/X11/lib ; then
    OSDEP_LIBS="-L/opt/X11/lib"
  elif test -d /opt/local/lib ; then
    OSDEP_LIBS="-L/opt/local/lib"
  elif test -d /sw/lib ; then
    OSDEP_LIBS="$OSDEP_LIBS -L/sw/lib"
  fi
  LDFLAGS="$OSDEP_LIBS $LDFLAGS"
  AC_DEFINE(HAVE_LIBGLU)
  ;;
*)
  case "`$canonical $build`" in
    *-*-solaris*) OSDEP_LIBS="-L/usr/openwin/lib" OSDEP_CFLAGS="-I/usr/local/include" ;;
    *-*-linux*)   OSDEP_LIBS=""                   OSDEP_CFLAGS="" ;;
    *-*-netbsd*)  OSDEP_LIBS="-L/usr/pkg/lib"     OSDEP_CFLAGS="-I/usr/pkg/include" ;;
    *)            OSDEP_LIBS="-L/usr/local/lib"   OSDEP_CFLAGS="-I/usr/local/include" ;;
  esac
  AC_CHECK_LIB([glut], [glutMainLoop])
  AC_CHECK_HEADERS([GL/glut.h])
  LDFLAGS="$OSDEP_LIBS $LDFLAGS"
  if test "ac_cv_lib_glut_glutMainLoop" = no; then
    AC_MSG_WARN([Could not find glut lib!])
  else
    CPPFLAGS="$OSDEP_CFLAGS $CPPFLAGS"
    LIBS="$OSDEP_LIBS -lglut $LIBS"
    GLUT_LIB="$OSDEP_LIBS -lglut"
    AC_SUBST(GNUT_LIB)
  fi
  ;;
esac
AC_DEFINE(HAVE_LIBGLUT)
am_libglut="yes"

CPPFLAGS="$OSDEP_CFLAGS $CPPFLAGS"
])

################################################################################
#
# FreeType
#
dnl AC_CHECK_FREETYPE([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for FreeType2, and define FREETYPE_CFLAGS, FREETYPE_LIBS
dnl and HAVE_FREETYPE, HAVE_LIBFREETYPE
AC_DEFUN([AC_CHECK_FREETYPE],
[
dnl
dnl Get the cflags and libraries from the freetype-config script
dnl
AC_ARG_WITH(freetype-prefix,
[  --with-freetype-prefix=PFX      Prefix where FreeType is installed (optional)],
            ft_config_prefix="$withval", ft_config_prefix="")

if test x$ft_config_prefix != x ; then
  ft_config_args="$ft_config_args --prefix=$ft_config_prefix"
  if test x${FREETYPE_CONFIG+set} != xset ; then
    FREETYPE_CONFIG=$ft_config_prefix/bin/freetype-config
  fi
fi
AC_PATH_PROG(FREETYPE_CONFIG, freetype-config, no)

min_ft_version=ifelse([$1], ,6.1.0,$1)
AC_MSG_CHECKING(for FreeType - version >= $min_ft_version)
no_ft=""
if test "$FREETYPE_CONFIG" = "no" ; then
  no_ft=yes
else
  FREETYPE_CFLAGS=`$FREETYPE_CONFIG $ft_config_args --cflags`
  FREETYPE_LIBS=`$FREETYPE_CONFIG $ft_config_args --libs`
  ft_config_version=`$FREETYPE_CONFIG $ft_config_args --version`
  ft_config_major_version=`echo $ft_config_version | cut -d . -f 1`
  ft_config_minor_version=`echo $ft_config_version | cut -d . -f 2`
  ft_config_micro_version=`echo $ft_config_version | cut -d . -f 3`
  ft_min_major_version=`echo $min_ft_version | cut -d . -f 1`
  ft_min_minor_version=`echo $min_ft_version | cut -d . -f 2`
  ft_min_micro_version=`echo $min_ft_version | cut -d . -f 3`
  ft_config_version=`expr $ft_config_major_version \* 10000 + $ft_config_minor_version \* 100 + $ft_config_micro_version`
  ft_min_version=`expr $ft_min_major_version \* 10000 + $ft_min_minor_version \* 100 + $ft_min_micro_version`
  if test $ft_config_version -lt $ft_min_version ; then
    ifelse([$3], , :, [$3])
  else
    CPPFLAGS="$FREETYPE_CFLAGS $CPPFLAGS"
    LIBS="$FREETYPE_LIBS $LIBS"
  fi             # test $ft_config_version -lt $ft_min_version
fi               # test "$FREETYPE_CONFIG" = "no"
if test "x$no_ft" = x ; then
   AC_MSG_RESULT(yes)
   ifelse([$2], , :, [$2])     
else
   AC_MSG_RESULT(no)
   if test "$FREETYPE_CONFIG" = "no" ; then
     echo "*** The freetype-config script installed by FT2 could not be found"
     echo "*** If FT2 was installed in PREFIX, make sure PREFIX/bin is in"
     echo "*** your path, or set the FT2_CONFIG environment variable to the"
     echo "*** full path to freetype-config."
   else
     echo "*** The FreeType test program failed to run.  If your system uses"
     echo "*** shared libraries and they are installed outside the normal"
     echo "*** system library path, make sure the variable LD_LIBRARY_PATH"
     echo "*** (or whatever is appropiate for your system) is correctly set."
   fi
   FREETYPE_CFLAGS=""
   FREETYPE_LIBS=""
   ifelse([$3], , :, [$3])
fi
AC_SUBST(FREETYPE_CFLAGS)
AC_SUBST(FREETYPE_LIBS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LIBS)
AH_TEMPLATE(HAVE_FREETYPE)
AC_DEFINE(HAVE_FREETYPE)
AH_TEMPLATE(HAVE_LIBFREETYPE)
AC_CHECK_LIB(freetype, FT_Init_FreeType, [AC_DEFINE(HAVE_LIBFREETYPE)],
             AC_MSG_WARN([unable to link with FreeType lib!]), )
])

################################################################################
#
# SDL
#
# Configure paths for SDL
# Sam Lantinga 9/21/99
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor
#
dnl AM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
#
AC_DEFUN([AM_PATH_SDL],
[dnl 
dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  if test x$sdl_exec_prefix != x ; then
    sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
    if test x${SDL_CONFIG+set} != xset ; then
      SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
    fi
  fi
  if test x$sdl_prefix != x ; then
    sdl_args="$sdl_args --prefix=$sdl_prefix"
    if test x${SDL_CONFIG+set} != xset ; then
      SDL_CONFIG=$sdl_prefix/bin/sdl-config
    fi
  fi

  if test "x$prefix" != xNONE; then
    PATH="$prefix/bin:$prefix/usr/bin:$PATH"
  fi
  AC_PATH_PROG(SDL_CONFIG, sdl-config, no, [$PATH])
  min_sdl_version=ifelse([$1], ,0.11.0,$1)
  AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no" ; then
    no_sdl=yes
  else
    SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
    SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`

    sdl_major_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sdl_minor_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_version=`$SDL_CONFIG $sdl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sdltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SDL_CFLAGS"
      CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
      LIBS="$LIBS $SDL_LIBS"
dnl
dnl Now check if the installed SDL is sufficiently new. (Also sanity
dnl checks the results of sdl-config to some extent
dnl
      rm -f conf.sdltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.sdltest");
  */
  { FILE *fp = fopen("conf.sdltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sdl_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sdl_version");
     exit(1);
   }

   if (($sdl_major_version > major) ||
      (($sdl_major_version == major) && ($sdl_minor_version > minor)) ||
      (($sdl_major_version == major) && ($sdl_minor_version == minor) && ($sdl_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sdl-config --version' returned %d.%d.%d, but the minimum version\n", $sdl_major_version, $sdl_minor_version, $sdl_micro_version);
      printf("*** of SDL required is %d.%d.%d. If sdl-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If sdl-config was wrong, set the environment variable SDL_CONFIG\n");
      printf("*** to point to the correct copy of sdl-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_sdl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SDL_CONFIG" = "no" ; then
       echo "*** The sdl-config script installed by SDL could not be found"
       echo "*** If SDL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SDL_CONFIG environment variable to the"
       echo "*** full path to sdl-config."
     else
       if test -f conf.sdltest ; then
        :
       else
          echo "*** Could not run SDL test program, checking why..."
          CFLAGS="$CFLAGS $SDL_CFLAGS"
          CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
          LIBS="$LIBS $SDL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "SDL.h"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SDL or finding the wrong"
          echo "*** version of SDL. If it is not finding SDL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SDL was incorrectly installed"
          echo "*** or that you have moved SDL since it was installed. In the latter case, you"
          echo "*** may want to edit the sdl-config script: $SDL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
  rm -f conf.sdltest
])

################################################################################
#
# Java - Class
#
dnl @synopsis AC_CHECK_CLASS
dnl
dnl AC_CHECK_CLASS tests the existence of a given Java class, either in
dnl a jar or in a '.class' file.
dnl
dnl *Warning*: its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_check_class.m4,v 1.3 2000/01/28 11:54:26 bortz Exp $
dnl

AC_DEFUN([AC_CHECK_CLASS],[
AC_REQUIRE([AC_PROG_JAVA])dnl
ac_var_name=`echo $1 | sed 's/\./_/g'`
dnl Normaly I'd use a AC_CACHE_CHECK here but since the variable name is
dnl dynamic I need an extra level of extraction
AC_MSG_CHECKING([for $1 class])
AC_CACHE_VAL(ac_cv_class_$ac_var_name, [
if test x$ac_cv_prog_uudecode_base64 = xyes; then
dnl /**
dnl  * Test.java: used to test dynamicaly if a class exists.
dnl  */
dnl public class Test
dnl {
dnl
dnl public static void
dnl main( String[] argv )
dnl {
dnl 	Class lib;
dnl 	if (argv.length < 1)
dnl 	 {
dnl 		System.err.println ("Missing argument");
dnl 		System.exit (77);
dnl 	 }
dnl 	try
dnl 	 {
dnl 		lib = Class.forName (argv[0]);
dnl 	 }
dnl 	catch (ClassNotFoundException e)
dnl 	 {
dnl 		System.exit (1);
dnl 	 }
dnl 	lib = null;
dnl 	System.exit (0);
dnl }
dnl
dnl }
cat << \EOF > Test.uue
begin-base64 644 Test.class
yv66vgADAC0AKQcAAgEABFRlc3QHAAQBABBqYXZhL2xhbmcvT2JqZWN0AQAE
bWFpbgEAFihbTGphdmEvbGFuZy9TdHJpbmc7KVYBAARDb2RlAQAPTGluZU51
bWJlclRhYmxlDAAKAAsBAANlcnIBABVMamF2YS9pby9QcmludFN0cmVhbTsJ
AA0ACQcADgEAEGphdmEvbGFuZy9TeXN0ZW0IABABABBNaXNzaW5nIGFyZ3Vt
ZW50DAASABMBAAdwcmludGxuAQAVKExqYXZhL2xhbmcvU3RyaW5nOylWCgAV
ABEHABYBABNqYXZhL2lvL1ByaW50U3RyZWFtDAAYABkBAARleGl0AQAEKEkp
VgoADQAXDAAcAB0BAAdmb3JOYW1lAQAlKExqYXZhL2xhbmcvU3RyaW5nOylM
amF2YS9sYW5nL0NsYXNzOwoAHwAbBwAgAQAPamF2YS9sYW5nL0NsYXNzBwAi
AQAgamF2YS9sYW5nL0NsYXNzTm90Rm91bmRFeGNlcHRpb24BAAY8aW5pdD4B
AAMoKVYMACMAJAoAAwAlAQAKU291cmNlRmlsZQEACVRlc3QuamF2YQAhAAEA
AwAAAAAAAgAJAAUABgABAAcAAABtAAMAAwAAACkqvgSiABCyAAwSD7YAFBBN
uAAaKgMyuAAeTKcACE0EuAAaAUwDuAAasQABABMAGgAdACEAAQAIAAAAKgAK
AAAACgAAAAsABgANAA4ADgATABAAEwASAB4AFgAiABgAJAAZACgAGgABACMA
JAABAAcAAAAhAAEAAQAAAAUqtwAmsQAAAAEACAAAAAoAAgAAAAQABAAEAAEA
JwAAAAIAKA==
====
EOF
		if uudecode$EXEEXT Test.uue; then
			:
		else
			echo "configure: __oline__: uudecode had trouble decoding base 64 file 'Test.uue'" >&AS_MESSAGE_LOG_FD
			echo "configure: failed file was:" >&AS_MESSAGE_LOG_FD
			cat Test.uue >&AS_MESSAGE_LOG_FD
			ac_cv_prog_uudecode_base64=no
		fi
	rm -f Test.uue
	if AC_TRY_COMMAND($JAVA $JAVAFLAGS Test $1) >/dev/null 2>&1; then
		eval "ac_cv_class_$ac_var_name=yes"
	else
		eval "ac_cv_class_$ac_var_name=no"
	fi
	rm -f Test.class
else
	AC_TRY_COMPILE_JAVA([$1], , [eval "ac_cv_class_$ac_var_name=yes"],
		[eval "ac_cv_class_$ac_var_name=no"])
fi
eval "ac_var_val=$`eval echo ac_cv_class_$ac_var_name`"
eval "HAVE_$ac_var_name=$`echo ac_cv_class_$ac_var_val`"
HAVE_LAST_CLASS=$ac_var_val
if test x$ac_var_val = xyes; then
	ifelse([$2], , :, [$2])
else
	ifelse([$3], , :, [$3])
fi
])
dnl for some reason the above statment didn't fall though here?
dnl do scripts have variable scoping?
eval "ac_var_val=$`eval echo ac_cv_class_$ac_var_name`"
AC_MSG_RESULT($ac_var_val)
])

################################################################################
dnl @synopsis AC_CHECK_CLASSPATH
dnl
dnl AC_CHECK_CLASSPATH just displays the CLASSPATH, for the edification
dnl of the user.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_check_classpath.m4,v 1.3 1999/12/29 10:45:03 bortz Exp $
dnl
AC_DEFUN([AC_CHECK_CLASSPATH],[
CLASSPATH=$CLASSPATH:.
if test "x$CLASSPATH" = x; then
   echo "You have no CLASSPATH,\n I hope it is good"
else
   echo "You have CLASSPATH $CLASSPATH, hope it is correct"
fi
AC_SUBST(CLASSPATH)dnl
])

################################################################################
dnl @synopsis AC_CHECK_RQRD_CLASS
dnl
dnl AC_CHECK_RQRD_CLASS tests the existence of a given Java class, either in
dnl a jar or in a '.class' file and fails if it doesn't exist.
dnl Its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_check_rqrd_class.m4,v 1.1 1999/12/29 09:22:26 bortz Exp $
dnl
AC_DEFUN([AC_CHECK_RQRD_CLASS],[
CLASS=`echo $1|sed 's/\./_/g'`
AC_CHECK_CLASS($1)
if test "$HAVE_LAST_CLASS" = "no"; then
        AC_MSG_ERROR([Required class $1 missing, exiting.])
fi
])

################################################################################
dnl @synopsis AC_JAVA_OPTIONS
dnl
dnl AC_JAVA_OPTIONS adds configure command line options used for Java m4
dnl macros. This Macro is optional.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Devin Weaver <ktohg@tritarget.com>
dnl @version $Id: ac_java_options.m4,v 1.2 2000/01/02 16:07:12 bortz Exp $
dnl
AC_DEFUN([AC_JAVA_OPTIONS],[
AC_ARG_WITH(java-prefix,
			[  --with-java-prefix=PFX  prefix where Java runtime is installed (optional)])
AC_ARG_WITH(javac-flags,
			[  --with-javac-flags=FLAGS flags to pass to the Java compiler (optional)])
AC_ARG_WITH(java-flags,
			[  --with-java-flags=FLAGS flags to pass to the Java VM (optional)])
JAVAPREFIX=$with_java_prefix
JAVACFLAGS=$with_javac_flags
JAVAFLAGS=$with_java_flags
AC_SUBST(JAVAPREFIX)dnl
AC_SUBST(JAVACFLAGS)dnl
AC_SUBST(JAVAFLAGS)dnl
AC_SUBST(JAVA)dnl
AC_SUBST(JAVAC)dnl
])

################################################################################
dnl @synopsis AC_PROG_JAVA
dnl
dnl Here is a summary of the main macros:
dnl
dnl AC_PROG_JAVAC: finds a Java compiler.
dnl
dnl AC_PROG_JAVA: finds a Java virtual machine.
dnl
dnl AC_CHECK_CLASS: finds if we have the given class (beware of CLASSPATH!).
dnl
dnl AC_CHECK_RQRD_CLASS: finds if we have the given class and stops otherwise.
dnl
dnl AC_TRY_COMPILE_JAVA: attempt to compile user given source.
dnl
dnl AC_TRY_RUN_JAVA: attempt to compile and run user given source.
dnl
dnl AC_JAVA_OPTIONS: adds Java configure options.
dnl
dnl AC_PROG_JAVA tests an existing Java virtual machine. It uses the
dnl environment variable JAVA then tests in sequence various common Java
dnl virtual machines. For political reasons, it starts with the free ones.
dnl You *must* call [AC_PROG_JAVAC] before.
dnl
dnl If you want to force a specific VM:
dnl
dnl - at the configure.in level, set JAVA=yourvm before calling AC_PROG_JAVA
dnl   (but after AC_INIT)
dnl
dnl - at the configure level, setenv JAVA
dnl
dnl You can use the JAVA variable in your Makefile.in, with @JAVA@.
dnl
dnl *Warning*: its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl TODO: allow to exclude virtual machines (rationale: most Java programs
dnl cannot run with some VM like kaffe).
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl
dnl A Web page, with a link to the latest CVS snapshot is at
dnl <http://www.internatif.org/bortzmeyer/autoconf-Java/>.
dnl
dnl This is a sample configure.in
dnl Process this file with autoconf to produce a configure script.
dnl
dnl    AC_INIT(UnTag.java)
dnl
dnl    dnl Checks for programs.
dnl    AC_CHECK_CLASSPATH
dnl    AC_PROG_JAVAC
dnl    AC_PROG_JAVA
dnl
dnl    dnl Checks for classes
dnl    AC_CHECK_RQRD_CLASS(org.xml.sax.Parser)
dnl    AC_CHECK_RQRD_CLASS(com.jclark.xml.sax.Driver)
dnl
dnl    AC_OUTPUT(Makefile)
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_prog_java.m4,v 1.5 2000/01/02 16:07:12 bortz Exp $
dnl
AC_DEFUN([AC_PROG_JAVA],[
AC_REQUIRE([AC_EXEEXT])dnl
if test x$JAVAPREFIX = x; then
	test x$JAVA = x && AC_CHECK_PROGS(JAVA, java$EXEEXT kaffe$EXEEXT )
else
	test x$JAVA = x && AC_CHECK_PROGS(JAVA, java$EXEEXT kaffe$EXEEXT, $JAVAPREFIX)
fi
test x$JAVA = x && AC_MSG_ERROR([no acceptable Java virtual machine found in \$PATH])
AC_PROG_JAVA_WORKS
AC_PROVIDE([$0])dnl
])

#####################################################################################
dnl @synopsis AC_PROG_JAVA_WORKS
dnl
dnl Internal use ONLY.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_prog_java_works.m4,v 1.2 1999/12/29 09:58:48 bortz Exp $
dnl
AC_DEFUN([AC_PROG_JAVA_WORKS], [
AC_CHECK_PROG(uudecode, uudecode$EXEEXT, yes)
if test x$uudecode = xyes; then
AC_CACHE_CHECK([if uudecode can decode base 64 file], ac_cv_prog_uudecode_base64, [
dnl /**
dnl  * Test.java: used to test if java compiler works.
dnl  */
dnl public class Test
dnl {
dnl
dnl public static void
dnl main( String[] argv )
dnl {
dnl 	System.exit (0);
dnl }
dnl
dnl }
cat << \EOF > Test.uue
begin-base64 644 Test.class
yv66vgADAC0AFQcAAgEABFRlc3QHAAQBABBqYXZhL2xhbmcvT2JqZWN0AQAE
bWFpbgEAFihbTGphdmEvbGFuZy9TdHJpbmc7KVYBAARDb2RlAQAPTGluZU51
bWJlclRhYmxlDAAKAAsBAARleGl0AQAEKEkpVgoADQAJBwAOAQAQamF2YS9s
YW5nL1N5c3RlbQEABjxpbml0PgEAAygpVgwADwAQCgADABEBAApTb3VyY2VG
aWxlAQAJVGVzdC5qYXZhACEAAQADAAAAAAACAAkABQAGAAEABwAAACEAAQAB
AAAABQO4AAyxAAAAAQAIAAAACgACAAAACgAEAAsAAQAPABAAAQAHAAAAIQAB
AAEAAAAFKrcAErEAAAABAAgAAAAKAAIAAAAEAAQABAABABMAAAACABQ=
====
EOF
if uudecode$EXEEXT Test.uue; then
	ac_cv_prog_uudecode_base64=yes
else
	echo "configure: __oline__: uudecode had trouble decoding base 64 file 'Test.uue'" >&AS_MESSAGE_LOG_FD
	echo "configure: failed file was:" >&AS_MESSAGE_LOG_FD
	cat Test.uue >&AS_MESSAGE_LOG_FD
	ac_cv_prog_uudecode_base64=no
fi
rm -f Test.uue])
fi
if test x$ac_cv_prog_uudecode_base64 != xyes; then
	rm -f Test.class
	#AC_MSG_WARN([I have to compile Test.class from scratch])
	if test x$ac_cv_prog_javac_works = xno; then
		AC_MSG_ERROR([Cannot compile java source. $JAVAC does not work properly])
	fi
	if test x$ac_cv_prog_javac_works = x; then
		AC_PROG_JAVAC
	fi
fi
AC_CACHE_CHECK(if $JAVA works, ac_cv_prog_java_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
TEST=Test
changequote(, )dnl
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
public static void main (String args[]) {
	System.exit (0);
} }
EOF
changequote([, ])dnl
if test x$ac_cv_prog_uudecode_base64 != xyes; then
	if AC_TRY_COMMAND(CLASSPATH=$CLASSPATH $JAVAC $JAVACFLAGS $JAVA_TEST) && test -s $CLASS_TEST; then
		:
	else
	  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
	  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
	  AC_MSG_ERROR(The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?))
	fi
fi
if AC_TRY_COMMAND(CLASSPATH=$CLASSPATH $JAVA $JAVAFLAGS $TEST) >/dev/null 2>&1; then
  ac_cv_prog_java_works=yes
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
  AC_MSG_ERROR(The Java VM $JAVA failed (see config.log, check the CLASSPATH?))
fi
rm -fr $JAVA_TEST $CLASS_TEST Test.uue
])
AC_PROVIDE([$0])dnl
]
)

################################################################################
dnl @synopsis AC_PROG_JAVAC
dnl
dnl AC_PROG_JAVAC tests an existing Java compiler. It uses the environment
dnl variable JAVAC then tests in sequence various common Java compilers. For
dnl political reasons, it starts with the free ones.
dnl
dnl If you want to force a specific compiler:
dnl
dnl - at the configure.in level, set JAVAC=yourcompiler before calling
dnl AC_PROG_JAVAC
dnl
dnl - at the configure level, setenv JAVAC
dnl
dnl You can use the JAVAC variable in your Makefile.in, with @JAVAC@.
dnl
dnl *Warning*: its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl TODO: allow to exclude compilers (rationale: most Java programs cannot compile
dnl with some compilers like guavac).
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_prog_javac.m4,v 1.3 1999/12/29 10:46:15 bortz Exp $
dnl
AC_DEFUN([AC_PROG_JAVAC],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
	test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, javac$EXEEXT jikes$EXEEXT "gcj$EXEEXT -C" guavac$EXEEXT)
else
	test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, javac$EXEEXT jikes$EXEEXT "gcj$EXEEXT -C" guavac$EXEEXT , $JAVAPREFIX)
fi
test "x$JAVAC" = x && AC_MSG_ERROR([no acceptable Java compiler found in \$PATH])
AC_PROG_JAVAC_WORKS
AC_PROVIDE([$0])dnl
])

################################################################################
dnl @synopsis AC_PROG_JAVAC_WORKS
dnl
dnl Internal use ONLY.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
dnl @version $Id: ac_prog_javac_works.m4,v 1.2 1999/12/29 09:58:48 bortz Exp $
dnl
AC_DEFUN([AC_PROG_JAVAC_WORKS],[
AC_CACHE_CHECK([if $JAVAC works], ac_cv_prog_javac_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) >/dev/null 2>&1; then
  ac_cv_prog_javac_works=yes
else
  AC_MSG_ERROR([The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?)])
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
fi
rm -f $JAVA_TEST $CLASS_TEST
])
AC_PROVIDE([$0])dnl
])

################################################################################
dnl @synopsis AC_TRY_COMPILE_JAVA
dnl
dnl AC_TRY_COMPILE_JAVA attempt to compile user given source.
dnl
dnl *Warning*: its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Devin Weaver <ktohg@tritarget.com>
dnl @version $Id: ac_try_compile_java.m4,v 1.1 1999/12/29 09:59:34 bortz Exp $
dnl
AC_DEFUN([AC_TRY_COMPILE_JAVA],[
AC_REQUIRE([AC_PROG_JAVAC])dnl
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
ifelse([$1], , , [import $1;])
public class Test {
[$2]
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS Test.java) && test -s Test.class
then
dnl Don't remove the temporary files here, so they can be examined.
  ifelse([$3], , :, [$3])
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat Test.java >&AS_MESSAGE_LOG_FD
ifelse([$4], , , [  rm -fr Test*
  $4
])dnl
fi
rm -fr Test*])

################################################################################
dnl @synopsis AC_TRY_RUN_JAVA
dnl
dnl AC_TRY_RUN_JAVA attempt to compile and run user given source.
dnl
dnl *Warning*: its success or failure can depend on a proper setting of the
dnl CLASSPATH env. variable.
dnl
dnl Note: This is part of the set of autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download the whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl The general documentation, as well as the sample configure.in, is
dnl included in the AC_PROG_JAVA macro.
dnl
dnl @author Devin Weaver <ktohg@tritarget.com>
dnl @version $Id: ac_try_run_java.m4,v 1.1 1999/12/29 09:59:34 bortz Exp $
dnl
AC_DEFUN([AC_TRY_RUN_JAVA],[
AC_REQUIRE([AC_PROG_JAVAC])dnl
AC_REQUIRE([AC_PROG_JAVA])dnl
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
ifelse([$1], , , [include $1;])
public class Test {
[$2]
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS Test.java) && test -s Test.class && ($JAVA $JAVAFLAGS Test; exit) 2>/dev/null
then
dnl Don't remove the temporary files here, so they can be examined.
  ifelse([$3], , :, [$3])
else
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat Test.java >&AS_MESSAGE_LOG_FD
ifelse([$4], , , [  rm -fr Test*
  $4
])dnl
fi
rm -fr Test*])

################################################################################
dnl @synopsis AC_PROG_JAR
dnl
dnl AC_PROG_JAR tests for an existing jar program. It uses the environment
dnl variable JAR then tests in sequence various common jar programs.
dnl
dnl If you want to force a specific compiler:
dnl
dnl - at the configure.in level, set JAR=yourcompiler before calling
dnl AC_PROG_JAR
dnl
dnl - at the configure level, setenv JAR
dnl
dnl You can use the JAR variable in your Makefile.in, with @JAR@.
dnl
dnl Note: This macro depends on the autoconf M4 macros for Java programs.
dnl It is VERY IMPORTANT that you download that whole set, some
dnl macros depend on other. Unfortunately, the autoconf archive does not
dnl support the concept of set of macros, so I had to break it for
dnl submission.
dnl
dnl The general documentation of those macros, as well as the sample
dnl configure.in, is included in the AC_PROG_JAVA macro.
dnl
dnl @author Egon Willighagen <egonw@sci.kun.nl>
dnl @version $Id: ac_prog_jar.m4,v 1.2 2000/07/19 13:03:32 simons Exp $
dnl
AC_DEFUN([AC_PROG_JAR],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar$EXEEXT)
else
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar, $JAVAPREFIX)
fi
test "x$JAR" = x && AC_MSG_ERROR([no acceptable jar program found in \$PATH])
AC_PROVIDE([$0])dnl
])

################################################################################
