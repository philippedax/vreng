dnl
dnl configure.in for VRL
dnl
dnl report bugs to dax@inf.enst.fr
dnl
dnl Process this file with autoconf to produce a configure script.
dnl checks for version info

dnl --------------------------------------------------------------------------
dnl
dnl name, version, config
dnl

AC_INIT(src/vrl.y)
AM_CONFIG_HEADER(config.h)

dnl where to find install.sh, config.sub, and config.guess
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE(vrl,3.0)

dnl --------------------------------------------------------------------------
dnl
dnl Checks for compiler
dnl
#AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
#AC_PROG_CXXCPP
AM_PROG_LEX
AC_PROG_YACC

dnl --------------------------------------------------------------------------
dnl
dnl programs needed by the build and install process
dnl
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_AIX
AC_ISC_POSIX

dnl --------------------------------------------------------------------------
dnl
dnl Check for optimization
dnl
AC_ARG_ENABLE(optimize,
            [  --enable-optimize       allow to optimize and make code faster [default=no]],
            [optimize_ok=$enableval], [optimize_ok=no])

CFLAGS="-O"
CXXFLAGS="-O"
if test "$GCC" = yes; then
  if test "$optimize_ok" = yes; then
    CFLAGS="-O2 -Wall -ffast-math"
    CXXFLAGS="-O2 -Wall -ffast-math"
  else
    CFLAGS="-g -Wall"
    CXXFLAGS="-g -Wall"
  fi
fi

dnl --------------------------------------------------------------------------
dnl
dnl check headers
dnl
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(stdlib.h unistd.h string.h strings.h ctype.h malloc.h math.h errno.h)

dnl --------------------------------------------------------------------------
dnl
dnl Checks for sizeofs.
dnl

dnl --------------------------------------------------------------------------
dnl
dnl Checks for typedefs.
dnl

dnl --------------------------------------------------------------------------
dnl
dnl Checks for library functions.
dnl
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strchr strncasecmp strdup strerror memcpy)
if test x$ac_cv_func_strerror = xno; then
dnl Replace `main' with a function in -liberty:
  AC_CHECK_LIB(iberty, main)
fi

dnl --------------------------------------------------------------------------
dnl
dnl Make Makefiles
dnl
AC_OUTPUT([Makefile 
        src/Makefile
        lib/Makefile
        config/Makefile
        examples/Makefile
        doc/Makefile],
[
# Makefile uses this timestamp to record whether config.h is up to date.
echo `date` > stamp-h
])
