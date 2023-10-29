# Process this file with autoconf to produce a configure script.

# Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004
# Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# configure.ac for VREng	http://vreng.enst.fr/
#
# Written by Philippe Dax, report bugs to dax@telecom-paristech.fr


# --------------------------------------------------------------------------
#
# Checks name, version, config
#
AC_INIT([vreng],[9.4.3],[philippe.dax@gmail.com])

# Make sure we are using a recent autoconf version
#AC_PREREQ([2.71])

AC_CONFIG_SRCDIR([src/base/vreng.cpp])

# where to find install.sh, config.sub, and config.guess
AC_CONFIG_AUX_DIR(config)
# where to find macros m4
AC_CONFIG_MACRO_DIR(config)

# canonicalizing cpu:vendor:os
AC_CANONICAL_HOST([])

AM_INIT_AUTOMAKE([subdir-objects])

# version
echo $PACKAGE_VERSION > VERSION.md
set x `echo $PACKAGE_VERSION | tr '.' ' '`
major=$2
minor=$3
micro=$4
AH_TEMPLATE(VRENG_MAJOR_VERSION, [VReng major version])
AC_DEFINE_UNQUOTED(VRENG_MAJOR_VERSION, [$major])
AH_TEMPLATE(VRENG_MINOR_VERSION, [VReng minor version])
AC_DEFINE_UNQUOTED(VRENG_MINOR_VERSION, [$minor])
AH_TEMPLATE(VRENG_MICRO_VERSION, [VReng micro version])
AC_DEFINE_UNQUOTED(VRENG_MICRO_VERSION, [$micro])
ubit_features=""

# use config.h
AC_CONFIG_HEADERS([config.h])

#AM_MAINTAINER_MODE


# --------------------------------------------------------------------------
#
# Checks for compiler
#
#DAX#AC_USE_SYSTEM_EXTENSIONS
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_GCC_TRADITIONAL
AC_PROG_YACC

AH_TEMPLATE(GCC_VERSION, [gcc version])
AH_TEMPLATE(GCC_MAJOR_VERSION, [gcc major version])
if test "$GCC" = "yes"; then
  gcc_version=`${CXX} --version | sed 1q | cut -f3 -d' '`
  gcc_major_version=`echo ${gcc_version} | cut -f1 -d'.'`
  gcc_minor_version=`echo ${gcc_version} | cut -f2 -d'.'`
  AC_DEFINE_UNQUOTED(GCC_VERSION, ["${gcc_version}"])
  AC_DEFINE_UNQUOTED(GCC_MAJOR_VERSION, [${gcc_major_version}])
  compiler=${CXX}-${gcc_version}
else
  compiler=${CC}
fi


# --------------------------------------------------------------------------
#
# Programs needed by the build and install process
#
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_PROG(AR, ar, ar)
AC_PROG_RANLIB


# --------------------------------------------------------------------------
#
# Where to install by default, handle the --prefix=... option
#
if test "${prefix}" = "NONE"; then
  prefix=/usr/local
fi
if test "${exec_prefix}" = "NONE"; then
  exec_prefix=${prefix}
fi


CFLAGS="${CFLAGS}"
CXXFLAGS="${CXXFLAGS}"
CPPFLAGS="${CPPFLAGS}"
LDFLAGS="${LDFLAGS}"

if test "${GCC}" = "yes"; then
  CFLAGS="${CFLAGS} -Wall -fno-strict-aliasing -Wno-int-conversion -Wno-int-to-pointer-cast"
  CXXFLAGS="${CXXFLAGS} -Wall -fno-strict-aliasing -fpermissive -Wno-invalid-offsetof -Wno-address -Wno-deprecated  -Wno-macro-redefined -Wno-write-strings -Wno-unused-function -Wno-unused-variable -Wno-unknown-pragmas -Wno-infinite-recursion [-Wno-c++11-extensions]" ### -Wno-literal-suffix -Wno-misleading-indentation
  CXXFLAGS=`echo ${CXXFLAGS} | sed -e 's/-O2//'`
  LDFLAGS=`echo ${LDFLAGS} | sed -e 's/-O2//'`
fi


# --------------------------------------------------------------------------
#
# Check for debugging
#
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],[allow to debug [[default=no]]]))

if test "${enable_debug:=no}" = "yes"; then
  CXXFLAGS=`echo ${CXXFLAGS} | sed -e 's/-O2//'`
  LDFLAGS="`echo ${LDFLAGS} | sed -e 's/-O2//'` -g"
fi


# --------------------------------------------------------------------------
#
# Check for optimization
#
AC_ARG_ENABLE(optimize,
              AS_HELP_STRING([--enable-optimize],[allow to optimize and make code faster [[default=no]]]))

if test "${enable_optimize:=no}" = "yes"; then
  CFLAGS="${CFLAGS} -O"
  CXXFLAGS="${CXXFLAGS}"
  if test "${GCC}" = "yes"; then
    optim_flags="-ffast-math -fomit-frame-pointer -fexpensive-optimizations -funroll-loops -fpermissive"
    CFLAGS="${CFLAGS} ${optim_flags}"
    CXXFLAGS="${CXXFLAGS} ${optim_flags}"
  else
    CXXFLAGS="${CXXFLAGS} -O"
  fi
fi


# --------------------------------------------------------------------------
#
# Check for profiling compiler option
#
AC_ARG_ENABLE(profiling,
              AS_HELP_STRING([--enable-profiling],[add -pg gcc option [[default=no]]]))
 
if test "${enable_profiling:=no}" = "yes"; then
  if test "${GCC}" = "yes"; then
    CFLAGS="${CFLAGS} -pg"
    CXXFLAGS="${CXXFLAGS} -pg"
    LDFLAGS="${LDFLAGS} -pg"
  fi
fi


# --------------------------------------------------------------------------
#
# Check for permissive compiler option
#
AC_ARG_ENABLE(permissive,
              AS_HELP_STRING([--enable-permissive],[add --permissive gcc option [[default=no]]]))
 
if test "${enable_permissive:=no}" = "yes"; then
  if test "${GCC}" = "yes"; then
    CFLAGS="${CFLAGS}"
    CXXFLAGS="${CXXFLAGS} --permissive"
  fi
fi


# --------------------------------------------------------------------------
#
# Check for gcc
#
if test "${GCC}" = "yes"; then
  case "$gcc_major_version" in
  4)
    case "$gcc_minor_version" in
    0) ;;
    *) CXXFLAGS="${CXXFLAGS} -ffriend-injection" ;;
    esac
    ;;
  esac
fi


# --------------------------------------------------------------------------
#
# Check if support for 64 bits (tested with gcc-2.96, gcc-3.*, gcc4.*)
#
AC_ARG_ENABLE(64bit,
              AS_HELP_STRING([--enable-64bit],[enable 64 bits compilation [[default=no]]]))

if test "${enable_64bit:=no}" = "yes"; then
  AC_MSG_RESULT([--> Using 64 bits])
  if test "${GCC}" = "yes"; then
    CFLAGS="${CFLAGS} -m64"
    CXXFLAGS="${CXXFLAGS} -m64"
  fi
fi


# --------------------------------------------------------------------------
#
# Checks for OS types
#
AH_TEMPLATE(SOLARIS)
AH_TEMPLATE(SUNOS)
AH_TEMPLATE(LINUX)
AH_TEMPLATE(LINUX_SPARC)
AH_TEMPLATE(LINUX_ALPHA)
AH_TEMPLATE(LINUX_IPAQ)
AH_TEMPLATE(NETBSD)
AH_TEMPLATE(FREEBSD)
AH_TEMPLATE(OPENBSD)
AH_TEMPLATE(HPUX)
AH_TEMPLATE(AIX)
AH_TEMPLATE(OSF1)
AH_TEMPLATE(IRIX)
AH_TEMPLATE(MACOSX)
AH_TEMPLATE(NEXTSTEP)
AH_TEMPLATE(CYGWIN32)
AH_TEMPLATE(WIN32)
AH_TEMPLATE(FEDORA)
AH_TEMPLATE(UBUNTU)
AH_TEMPLATE(CENTOS)
AH_TEMPLATE(DEBIAN)

# OS dependent paths
OSDEP_LIBS=""
OSDEP_CFLAGS=""

# X11 paths
case "`$ac_config_sub $build`" in
*-*-darwin*)
  #if test -d /opt/X11/include ; then
  #  X_CFLAGS="-I/opt/X11/include"
  #fi
  #if test -d /opt/X11/lib ; then
  #  X_LIBS="-L/opt/X11/lib"
  #fi
  ;;
*)
  X_CFLAGS="-I/usr/X11R6/include"
  X_LIBS="-L/usr/X11R6/lib"
  if test "$x_includes" != "NONE" ; then
    X_CFLAGS="-I$x_includes"
  fi
  if test "$x_libraries" != "NONE" ; then
    X_LIBS="-L$x_libraries"
  fi
  ;;
esac

# defaults
libmpeg=mpeg
LDFLAGS_EXT=""

case "`$ac_config_sub $build`" in
*-*-solaris*)
  CPPFLAGS="-I/usr/X11R6/include -I/usr/openwin/include $CPPFLAGS"
  X_CFLAGS="-I/usr/X11R6/include -I/usr/openwin/include"
  X_LIBS="-L/usr/openwin/lib"
  if test -d /usr/local/lib ; then
    OSDEP_LIBS="-L/usr/local/lib"
  fi
  if test -d /usr/local/include ; then
    OSDEP_CFLAGS="-I/usr/local/include"
  fi
  if test -d /usr/sfw/lib ; then
    OSDEP_LIBS="${OSDEP_LIBS} -L/usr/sfw/lib"
  fi
  if test -d /usr/sfw/include ; then
    OSDEP_CFLAGS="${OSDEP_CFLAGS} -I/usr/sfw/include"
  fi
  AC_DEFINE(SOLARIS)
  ;;
sparc-sun-sunos*)
  AC_DEFINE(SUNOS)
  ;;
sparc-*-linux*)
  AC_DEFINE(LINUX_SPARC)
  AC_DEFINE(LINUX)
  ;;
alpha-*-linux*)
  AC_DEFINE(LINUX_ALPHA)
  AC_DEFINE(LINUX)
  ;;
armv4l-*-linux*)
  AC_DEFINE(LINUX_IPAQ)
  AC_DEFINE(LINUX)
  ;;
*-*-linux*)
  X_CFLAGS="-I/usr/include"
  if test -d /usr/lib64 ; then
    X_LIBS="-L/usr/lib64"
    OSDEP_LIBS="-L/usr/lib64"
  else
    X_LIBS="-L/usr/lib"
    OSDEP_LIBS="-L/usr/lib"
  fi
  AC_DEFINE(LINUX)
  distrib=`lsb_release -is`
  case $distrib in
  Fedora)
    AC_DEFINE(FEDORA)
    ;;
  Ubuntu)
    AC_DEFINE(UBUNTU)
    ;;
  Debian)
    AC_DEFINE(DEBIAN)
    ;;
  CentOS)
    AC_DEFINE(CENTOS)
    ;;
  esac
  ;;
*-*-netbsd*)
  OSDEP_LIBS="-L/usr/pkg/lib"
  if test -d /usr/pkg/include ; then
    OSDEP_CFLAGS="-I/usr/pkg/include"
  fi
  CPPFLAGS="${OSDEP_CFLAGS} $CPPFLAGS"
  AC_DEFINE(NETBSD)
  # special patch for pkgsrc -lmpeg become -lmpeg_lib, because of kde conflict
  libmpeg=mpeg_lib
  ;;
*-*-freebsd*)
  AC_DEFINE(FREEBSD)
  ;;
*-*-openbsd*)
  AC_DEFINE(OPENBSD)
  ;;
*-*-hpux*)
  AC_DEFINE(HPUX)
  ;;
*-*-aix*)
  AC_DEFINE(AIX)
  ;;
*-*-osf*)
AC_DEFINE(OSF1)
  ;;
*-*-irix*)
  AC_DEFINE(IRIX)
  ;;
*-*-darwin*|*-*-macosx*)
  # needed by ocaml linking
  LDFLAGS="-Wl,-no_compact_unwind"
  # search in MacPorts and Fink installs
  if test -d /usr/X11/include ; then
    OSDEP_CFLAGS="-I/usr/X11/include"
    X_CFLAGS="-I/usr/X11/include"
    CPPFLAGS="-I/usr/X11/include ${OSDEP_CFLAGS} $CPPFLAGS"
  elif test -d /opt/X11/include ; then
    OSDEP_CFLAGS="-I/opt/X11/include"
    X_CFLAGS="-I/opt/X11/include"
    CPPFLAGS="-I/opt/X11/include ${OSDEP_CFLAGS} $CPPFLAGS"
  elif test -d /opt/local/include ; then
    OSDEP_CFLAGS="-I/opt/local/include"
    CPPFLAGS="-I/opt/local/include ${OSDEP_CFLAGS} $CPPFLAGS"
  elif test -d /sw/include ; then
    OSDEP_CFLAGS="${OSDEP_CFLAGS} -I/sw/include"
    CPPFLAGS="-I/sw/include ${OSDEP_CFLAGS} $CPPFLAGS"
  fi
  if test -d /usr/X11/lib ; then
    OSDEP_LIBS="-L/usr/X11/lib"
    X_LIBS="-L/usr/X11/lib"
  elif test -d /opt/X11/lib ; then
    OSDEP_LIBS="-L/opt/X11/lib"
    X_LIBS="-L/opt/X11/lib"
  elif test -d /opt/local/lib ; then
    OSDEP_LIBS="-L/opt/local/lib"
  elif test -d /sw/lib ; then
    OSDEP_LIBS="${OSDEP_LIBS} -L/sw/lib"
  fi
  #CXXFLAGS="${CXXFLAGS} -bind_at_load"
  AC_DEFINE(MACOSX)
  case "`$ac_config_sub $build`" in
    *-*-darwin9*)
    # BUG leopard
    LDFLAGS_EXT="-dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
    ;;
  esac
  ;;
*-*-nextstep*)
  AC_DEFINE(NEXTSTEP)
  ;;
i*-pc-cygwin*|i*-pc-mingw*)
  AC_DEFINE(CYGWIN32)
  AC_DEFINE(WIN32)
  AC_DEFINE(HAVE_MULTICAST)
  ;;
esac

#
# Find TTF fonts (TrueType fonts)
#
AH_TEMPLATE(TTFPATH)
AH_TEMPLATE(HAVE_TTF_PFA)
TTFPATH=
if test -f /usr/share/X11/fonts/Type1/UTRG____.pfa; then
  TTFPATH=/usr/share/X11/fonts/Type1/
  AC_DEFINE(HAVE_TTF_PFA)
elif test -f /usr/share/fonts/X11/Type1/UTRG____.pfa; then
  TTFPATH=/usr/share/fonts/X11/Type1/
  AC_DEFINE(HAVE_TTF_PFA)
elif test -f /usr/X11/share/fonts/Type1/UTRG____.pfa; then
  TTFPATH=/usr/X11/share/fonts/Type1/
  AC_DEFINE(HAVE_TTF_PFA)
elif test -f /usr/X11/lib/fonts/Type1/UTRG____.pfa; then
  TTFPATH=/usr/X11/lib/fonts/Type1/
  AC_DEFINE(HAVE_TTF_PFA)
else
  AC_MSG_WARN(Vreng requires TTF freetype fonts with the pfs format see src/ubit/fonts directory)
fi
if test x$TTFPATH != x; then
  AC_DEFINE_UNQUOTED(TTFPATH, ["$TTFPATH"])
fi

AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)


# --------------------------------------------------------------------------
# Check if we need to have -lnsl and -lsocket in LIBS. This may be needed
# to get the functions below right (socket, gethostbyname).
AC_CHECK_LIB([nsl], [gethostbyname])
AC_CHECK_LIB([socket], [connect])

AC_CHECK_LIB([m], [sqrt])


# --------------------------------------------------------------------------
#
# Check headers
#
AC_INCLUDES_DEFAULT([])
AC_HEADER_STDBOOL
AC_HEADER_DIRENT
AC_HEADER_RESOLV
AC_HEADER_STAT
AC_HEADER_ASSERT
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS([unistd.h stddef.h float.h limits.h fcntl.h stdarg.h ctype.h utime.h malloc.h signal.h math.h errno.h setjmp.h getopt.h pwd.h pthread.h ieeefp.h locale.h wchar.h libintl.h floatingpoint.h sys/param.h sys/time.h sys/timeb.h sys/socket.h sys/select.h sys/poll.h sys/resource.h sys/utsname.h netinet/tcp.h arpa/inet.h])


# --------------------------------------------------------------------------
#
# Check for sizeofs and typedefs.
#
AC_C_BIGENDIAN
AC_C_CONST
AC_C_VOLATILE
AC_C_INLINE

AC_TYPE_OFF_T
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES(ptrdiff_t)

AC_MSG_CHECKING(for socklen_t)
AC_EGREP_HEADER([socklen_t], sys/socket.h, is_socklen=yes, is_socklen=no)
if test "$is_socklen" = "yes"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AH_TEMPLATE(HAVE_NO_SOCKLEN_T)
  AC_DEFINE(HAVE_NO_SOCKLEN_T)
fi


# --------------------------------------------------------------------------
#
# Checks for library functions.
#
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_ALLOCA
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_FUNC_STRTOD

AC_CHECK_FUNCS([getuid getpid getcwd getpwuid gethostname gethostid gethostbyaddr gethostbyname gethostbyname_r getservbyname_r getipnodebyname getipnodebyaddr freehostent inet_pton inet_ntoa inet_ntop isascii gettimeofday memset mkdir pow sqrt strcasecmp strrchr strstr utime fcntl floor select poll socket setrlimit waitpid strchr strncasecmp strdup strerror memcpy readdir getopt getopt_long uname rand random drand48 lrand48 usleep fpsetmask nice getaddrinfo setlocale alarm ftime strtol sysinfo memmove strpbrk ])

AH_TEMPLATE(_REENTRANT)
if test "$ac_cv_func_gethostbyname_r" = "yes" ; then
  AC_DEFINE(_REENTRANT)
fi

if test "$ac_cv_func_strerror" = "no"; then
  AC_CHECK_LIB([iberty], [strerror])
fi


# --------------------------------------------------------------------------
#
# Check Multicast support
#
AH_TEMPLATE(HAVE_MULTICAST)
AC_MSG_CHECKING(for IP Multicast support)
AC_EGREP_HEADER([struct ip_mreq], netinet/in.h, is_mc=yes, is_mc=no)
if test "${is_mc}" = "yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_MULTICAST)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# Check IPv6 support
#
am_ipv6="no"
AC_ARG_ENABLE(ipv6,
              AS_HELP_STRING([--enable-ipv6],
              [allow use of ipv6 if available [[default=no]]]))

AH_TEMPLATE(HAVE_IPV6)
if test "${enable_ipv6:=no}" = "yes"; then
  am_ipv6="yes"
  AC_MSG_CHECKING(IPv6 support)
  AC_EGREP_HEADER(sockaddr_in6, netinet/in.h,
                  [
    	          AC_MSG_RESULT(yes)
    	          AC_DEFINE(HAVE_IPV6)
	          ])
fi


# --------------------------------------------------------------------------
#
# Posix threads configuration
#
AH_TEMPLATE(WITH_PTHREAD)
AH_TEMPLATE(HAVE_LIBPTHREAD)
AC_ARG_ENABLE([pthread],
              AS_HELP_STRING([--enable-pthread],
              [allow use of pthreads [[default=yes]]]))

if test "${enable_pthread:=yes}" = "yes"; then
  #
  # check pthread
  #
  AC_CHECK_LIB([pthread], [pthread_create], 
               [VRELIBS="-lpthread -L/usr/lib -ldl ${VRELIBS}"],
               [AC_MSG_WARN([pthreads library not found, check config.log!])])
  if test "$ac_cv_lib_pthread_pthread_create" = "yes" ; then
    CXXFLAGS="${CXXFLAGS} -D_REENTRANT"
    AC_DEFINE(WITH_PTHREAD)
    AC_DEFINE(HAVE_LIBPTHREAD)
  fi
fi


# --------------------------------------------------------------------------
#
# Check for dmalloc option
#
AC_ARG_ENABLE([dmalloc],
              AS_HELP_STRING([--enable-dmalloc],
              [enable dmalloc debugging [[default=no]]]))

if test "${GCC}" = "yes"; then
  if test "${enable_dmalloc:=no}" = "yes"; then
    CFLAGS="${CFLAGS} -DDMALLOC_FUNC_CHECK"
    CXXFLAGS="${CXXFLAGS} -DDMALLOC_FUNC_CHECK"
    AC_CHECK_LIB([dmallocthcxx], [dmalloc_verify])
    AC_CHECK_HEADERS([dmalloc.h])
  fi
fi


# --------------------------------------------------------------------------
#
# Check for efence option
#
AC_ARG_ENABLE([efence],
              AS_HELP_STRING([--enable-efence],
              [enable efence debugging [[default=no]]]))

if test "${enable_efence:=no}" = "yes"; then
  AC_CHECK_LIB([efence], [malloc])
fi


# --------------------------------------------------------------------------
#
# standalone
#
AC_MSG_CHECKING(if standalone is enabled)
AC_ARG_ENABLE(standalone,
              AS_HELP_STRING([--enable-standalone],
              [allow use without network connectivity [[default=no]]]))

if test "${enable_standalone:=no}" = "yes"; then
  CFLAGS="${CFLAGS} -DSTANDALONE"
  CXXFLAGS="${CXXFLAGS} -DSTANDALONE"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

if test "${enable_standalone}" = "yes"; then
  AH_TEMPLATE(DEF_URL_PFX)
  AC_MSG_CHECKING(if localhost httpd prefix is defined)
  AC_ARG_WITH(httpd_prefix,
              AS_HELP_STRING([--with-httpd-prefix=[DIR]],
                             [prefix relative to the localhost httpd root directory where are located vreng data]))
  if test "${with_httpd_prefix}" != ""; then
    AC_DEFINE_UNQUOTED(DEF_URL_PFX, ["${with_httpd_prefix}"]) 
    AC_MSG_RESULT(yes -> "${with_httpd_prefix}")
  else
    AC_DEFINE_UNQUOTED(DEF_URL_PFX, ["/vreng"]) 
    AC_MSG_RESULT(no -> "/vreng")
  fi
fi


# --------------------------------------------------------------------------
#
# vrl configuration
#
am_vrl="no"
AC_MSG_CHECKING(if vrl is enabled)
AC_ARG_ENABLE(vrl,
              AS_HELP_STRING([--enable-vrl],
              [allow use of vrl [[default=no]]]))

if test "${enable_vrl:=no}" = "yes"; then
  am_vrl="yes"
  CXXFLAGS="${CXXFLAGS} -DVRL"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# vrel configuration
#
am_vrel="no"
AC_MSG_CHECKING(if vrel is enabled)
AC_ARG_ENABLE(vrel,
              AS_HELP_STRING([--enable-vrel],
              [allow use of vrel [[default=no]]]))

if test "${enable_vrel:=no}" = "yes"; then
  am_vrel="yes"
  CXXFLAGS="${CXXFLAGS} -DVREL"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# vred configuration
#
am_vred="no"
AC_MSG_CHECKING(if vred is enabled)
AC_ARG_ENABLE(vred,
              AS_HELP_STRING([--enable-vred],
              [allow use of vred [[default=no]]]))

if test "${enable_vred:=no}" = "yes"; then
  am_vred="yes"
  am_libglui="yes"
  CXXFLAGS="${CXXFLAGS} -DVRED"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# vmm configuration
#
am_vmm="no"
AC_MSG_CHECKING(if vmm is enabled)
AC_ARG_ENABLE(vmm,
              AS_HELP_STRING([--enable-vmm],
              [allow use of vmm [[default=no]]]))

if test "${enable_vmm:=no}" = "yes"; then
  am_vmm="yes"
  CXXFLAGS="${CXXFLAGS} -DVMM"
  #
  # Java configuration
  #
  AC_JAVA_OPTIONS
  AC_PROG_JAVAC
  AC_PROG_JAVA
  AC_PROG_JAR

  AH_TEMPLATE(HAVE_JAVA)
  if test "$ac_cv_prog_java_works" = "yes" ; then
    AC_DEFINE(HAVE_JAVA)
  else
    am_java="no"
  fi
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# vaps server configuration
#
am_vaps="no"
AC_MSG_CHECKING(if vaps is enabled)
AC_ARG_ENABLE(vaps,
              AS_HELP_STRING([--enable-vaps],
              [allow use of vaps server [[default=no]]]))

if test "${enable_vaps:=no}" = "yes"; then
  am_vaps="yes"
  CXXFLAGS="${CXXFLAGS} -DVAPS"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# vjs server configuration
#
am_vjs="no"
am_java="yes"
AC_MSG_CHECKING(if vjs is enabled)
AC_ARG_ENABLE(vjs,
              AS_HELP_STRING([--enable-vjs],
              [allow use of vjs server [[default=no]]]))

if test "${enable_vjs:=no}" = "yes"; then
  am_vjs="yes"
  CXXFLAGS="${CXXFLAGS} -DVJS"
  AC_MSG_RESULT(yes)
  #
  # Java configuration
  #
  AC_JAVA_OPTIONS
  AC_PROG_JAVAC
  AC_PROG_JAVA
  AC_PROG_JAR

  AH_TEMPLATE(HAVE_JAVA)
  if test "$ac_cv_prog_java_works" = "yes" ; then
    AC_DEFINE(HAVE_JAVA)
  else
    am_java="no"
  fi
else
  AC_MSG_RESULT(no)
  am_java="no"
fi


# --------------------------------------------------------------------------
#
# vacs server configuration
#
am_vacs="no"
AC_MSG_CHECKING(if vacs is enabled)
AC_ARG_ENABLE(vacs,
              AS_HELP_STRING([--enable-vacs],
              [allow use of vacs server [[default=no]]]))

if test "${enable_vacs:=no}" = "yes"; then
  am_vacs="yes"
  CXXFLAGS="$CXXFLAGS -DVACS"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# Check for X11 header and library files
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
  case "`$ac_config_sub $build`" in
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


# --------------------------------------------------------------------------
VRELIBS=""
AC_SUBST(VRELIBS)

#
# check X11 first
#
AC_X11_CHECK

saved_ldflags=$LDFLAGS
LDFLAGS="$X_LDFLAGS $X_LIBS"


# --------------------------------------------------------------------------
#
# check Xpm library
#
AC_CHECK_LIB([Xpm], [XpmReadFileToXpmImage],
	     [VRELIBS="-lXpm $VRELIBS"],
             [AC_MSG_WARN([Xpm library not found, check config.log!,
                          Ubit needs Xpm, get http://koala.ilog.fr/ftp/pub/xpm or try apt|dnf|yum|brew install libXpm-devel | apt-get install libxpm-dev])],
             [$X_LIBS $x_libs])
AH_TEMPLATE(HAVE_LIBXPM)
if test "$ac_cv_lib_Xpm_XpmReadFileToXpmImage" = "yes"; then
  AC_DEFINE([HAVE_LIBXPM])
  ubit_features="$ubit_features UBIT_WITH_XPM"
fi

LDFLAGS="$LDFLAGS $OSDEP_LIBS"


# --------------------------------------------------------------------------
#
# check gif library
#
AH_TEMPLATE(HAVE_LIBGIF)
AC_CHECK_LIB([ungif], [DGifOpenFileName],
             [VRELIBS="$OSDEP_LIBS -lungif $VRELIBS"],
             [],
             [$OSDEP_LIBS])
if test "$ac_cv_lib_ungif_DGifOpenFileName" = "yes"; then
  AC_DEFINE(HAVE_LIBGIF)
  ubit_features="$ubit_features UBIT_WITH_GIF"
else
  AC_CHECK_LIB([gif], [DGifOpenFileName],
               [VRELIBS="$OSDEP_LIBS -lgif $VRELIBS"],
               [AC_MSG_WARN([gif library not found, check config.log!,
	                    get ftp://prtr-13.ucsc.edu/pub/libungif/ or try apt|dnf|yum|brew install giflib-devel | apt-get install libgif-dev giflib-tools])],
               [$OSDEP_LIBS])
  if test "$ac_cv_lib_gif_DGifOpenFileName" = "yes"; then
    AC_DEFINE(HAVE_LIBGIF)
    ubit_features="$ubit_features UBIT_WITH_GIF"
  fi
fi


# --------------------------------------------------------------------------
#
# check jpeg library
#
AH_TEMPLATE(HAVE_LIBJPEG)
AC_CHECK_LIB([jpeg], [jpeg_read_scanlines],
             [VRELIBS="$OSDEP_LIBS -ljpeg $VRELIBS"],
             [AC_MSG_WARN([jpeg library not found, check config.log!, or try apt|dnf|yum|brew install libjpeg-turbo-devel | apt-get install libjpeg8-dev])],
             [$OSDEP_LIBS])
if test "$ac_cv_lib_jpeg_jpeg_read_scanlines" = "yes"; then
  AC_DEFINE([HAVE_LIBJPEG])
  ubit_features="$ubit_features UBIT_WITH_JPEG"
fi
AH_TEMPLATE(HAVE_JPEGLIB_H)
AC_CHECK_HEADER([jpeglib.h], [AC_DEFINE([HAVE_JPEGLIB_H])],
	        AC_MSG_WARN([unable to find jpeg header]))

# --------------------------------------------------------------------------
#
# check tiff library
#
AH_TEMPLATE(HAVE_LIBTIFF)
AC_CHECK_LIB([tiff], [TIFFOpen],
             [VRELIBS="$OSDEP_LIBS -ltiff $VRELIBS"],
             [AC_MSG_WARN([tiff library not found, check config.log!, try apt|dnf|yum|brew install libtiff-devel | apt-get install libtiff4-dev])],
             [$OSDEP_LIBS])
if test "$ac_cv_lib_tiff_TIFFOpen" = "yes"; then
  AC_DEFINE([HAVE_LIBTIFF])
  AH_TEMPLATE(HAVE_TIFFIO_H)
  AC_CHECK_HEADER([tiffio.h], [AC_DEFINE([HAVE_TIFFIO_H])],
	          AC_MSG_WARN([unable to find tiff header]))
fi


# --------------------------------------------------------------------------
#
# check Z library
#
#AC_ARG_WITH([libz-builtin],
#    AS_HELP_STRING([--with-libz-builtin],[use builtin libz [[default=no]]]),
#    [ac_cv_use_libz_builtin=$withval], [ac_cv_use_libz_builtin=no])
#
#if test "$ac_cv_use_libz_builtin" = "no"; then
#  am_libz="yes"
#  AH_TEMPLATE(HAVE_LIBZ)
#  AC_CHECK_LIB([z], [inflate],
#     [VRELIBS="-lz $VRELIBS"],
#     [AC_MSG_WARN([z library not found, check config.log!])],
#     [])
#  if test "$ac_cv_lib_z_inflate" = "yes" ; then
#    AC_DEFINE(HAVE_LIBZ)
#    am_libz="no"
#  fi
#else
#  AC_DEFINE(HAVE_LIBZ)
#  am_libz="yes"
#fi


# --------------------------------------------------------------------------
#
# check png library
#
AH_TEMPLATE(HAVE_LIBPNG)
LDFLAGS="$LDFLAGS $OSDEP_LIBS"
AC_CHECK_LIB([png], [png_error],
             [VRELIBS="$OSDEP_LIBS -lpng $VRELIBS"],
             [AC_MSG_WARN([png library not found, check config.log, or try apt|dnf|yum|brew install libpng-devel or apt-get install libpng-dev])],
             [$OSDEP_LIBS -lz])
if test "$ac_cv_lib_png_png_error" = "yes" ; then
  AC_DEFINE(HAVE_LIBPNG)
fi
AH_TEMPLATE(HAVE_PNG_H)
AC_CHECK_HEADER([png.h], [AC_DEFINE([HAVE_PNG_H])],
                AC_MSG_WARN([unable to find png header]))


# --------------------------------------------------------------------------
#
# Mpeg configuration
#
AC_ARG_WITH([mpeg],
            AS_HELP_STRING([--with-mpeg],
            [allow use of libmpeg if available [[default=yes]]]))

am_libmpeg="no"
AH_TEMPLATE(HAVE_LIBMPEG)
if test "${with_mpeg:=yes}" = "yes"; then
  #
  # check mpeg
  #
  AC_CHECK_LIB([mpeg], [OpenMPEG],
               [VRELIBS="${OSDEP_LIBS} -l${libmpeg} ${VRELIBS}"],
               [AC_MSG_RESULT([	use embedded mpeg_lib ])],
               [${OSDEP_LIBS}])
  if test $ac_cv_lib_mpeg_OpenMPEG = "yes" ; then
    AC_DEFINE(HAVE_LIBMPEG)
  else
    am_libmpeg="yes"
    AC_DEFINE(HAVE_LIBMPEG)
  fi
fi


# --------------------------------------------------------------------------
#
# check curl library
#
AH_TEMPLATE(HAVE_LIBCURL)
AC_CHECK_LIB([curl], [curl_easy_init],
             [VRELIBS="$OSDEP_LIBS -lcurl $VRELIBS"],
             [AC_MSG_WARN([curl library not found, check config.log, or try apt|dnf|yum|brew install libcurl-devel or apt-get install libcurl4-openssl-dev])],
             [$OSDEP_LIBS])
if test "$ac_cv_lib_curl_curl_easy_init" = "yes"; then
  AC_DEFINE([HAVE_LIBCURL])
  AH_TEMPLATE(HAVE_CURL_CURL_H)
  AC_CHECK_HEADER([curl/curl.h], [AC_DEFINE([HAVE_CURL_CURL_H])],
	          AC_MSG_WARN([unable to find curl header]))
fi


# --------------------------------------------------------------------------
#
# searches for freetype2 library
#
have_freetype="no"
have_freetype_and_gl="no"
AH_TEMPLATE(WITH_FREETYPE)
AC_CHECK_LIB([freetype],[FT_Library_Version],[have_freetype="yes"
             [VRELIBS="-lfreetype $VRELIBS"]],
             [AC_MSG_WARN([could not find FreeType2!, try apt|dnf|yum|brew install freetype-devel or apt-get install libfreetype6-dev])])
#dax AC_CHECK_FREETYPE([23.0.0],[have_freetype="yes"], [AC_MSG_WARN([could not find FreeType2!, try apt|dnf|yum|brew install freetype-devel or apt-get install libfreetype6-dev])])
AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
freetype_cflags=`$FREETYPE_CONFIG --cflags`
freetype_libs=`$FREETYPE_CONFIG --libs`
CPPFLAGS="$freetype_cflags $CPPFLAGS"
LDFLAGS="$LDFLAGS $freetype_libs"
if test "$have_freetype" = "yes"; then
  ubit_features="$ubit_features UBIT_WITH_FREETYPE"
  if test "$have_gl" = "yes"; then
    have_freetype_and_gl="yes";
  fi
fi
AM_CONDITIONAL(WITH_FREETYPE, test "$have_freetype" = "yes")
AM_CONDITIONAL(WITH_FTGL, test "$have_freetype_and_gl" = "yes")


# --------------------------------------------------------------------------
#
# Ubit configuration
#
AC_ARG_WITH([ubit-builtin],
    AS_HELP_STRING([--with-ubit-builtin],[use builtin UBIT toolkit [[default=yes]]]),
    [ac_cv_use_ubit_builtin=$withval], [ac_cv_use_ubit_builtin=yes])
LDFLAGS="$X_LDFLAGS $X_LIBS -lpthread"

if test "$ac_cv_use_ubit_builtin" = "no"; then
  AC_ARG_WITH([ubit-prefix],
    AS_HELP_STRING([--with-ubit-prefix=DIR],[prefix to look for ubit lib & include]))
  #
  # check ubit
  #
  min_ubit_version=5.5.0
  min_ubit_version_decimal=`echo $min_ubit_version | sed -e 's/\.//g'`
  UBIT_VERSION=none
  if test "$with_ubit_prefix" = "" ; then
    UBIT_L="-L$prefix/lib"
    UBIT_I="-I$prefix/include"
    AC_PATH_PROG(UBITCONFIG, ubit-config)
    if test "$ac_cv_path_UBITCONFIG" != "" ; then
      UBIT_VERSION=`ubit-config --version`
      ubit_version_decimal=`ubit-config --version | sed -e 's/\.//g'`
      AC_MSG_CHECKING(for Ubit - version >= $min_ubit_version)
      if test "$ubit_version_decimal" -ge "$min_ubit_version_decimal"; then
        AC_MSG_RESULT(yes)
      else
        AC_MSG_RESULT(no)
        if test "$with_ubit_prefix" = "" ; then
          AC_MSG_ERROR([VREng requires at minimum ubit-$min_ubit_version])
        fi
      fi
    else
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([$UBITCONFIG not found. VREng requires ubit-$min_ubit_version])
    fi
  else
    UBIT_L="-L$with_ubit_prefix/lib"
    UBIT_I="-I$with_ubit_prefix/include"
  fi

  AC_CHECK_LIB([ubit], [main],
               [VRELIBS="$UBIT_L -lubit $VRELIBS"],
               [AC_MSG_ERROR([ubit library not found, check config.log!,
               get http://www.infres.enst.fr/ftp/pub/unix/gui/ubit/ubit.tar.gz])],
               [$UBIT_L $X_LIBS -lXpm $x_libs])

  UBIT_CFLAGS=$UBIT_I
  am_ubit="no"
  am_libftgl="no"
  toolkit="ubit-$UBIT_VERSION"
else
  # Use builtin ubit
  UBIT_L="-L../../src/ubit"
  UBIT_I="-I../../src"
  UBIT_CFLAGS=$UBIT_I
  VRELIBS="$X_LIBS -lXpm -lICE -lSM -lXext -lXmu -lX11 $VRELIBS"
  am_ubit="yes"
  am_libftgl="yes"
  min_ubit_version=7.0.0
  toolkit=7.0.0
fi
XLIBS=$x_libs
AC_SUBST(XLIBS)
AC_SUBST(UBIT_CFLAGS)
LDFLAGS=$saved_ldflags


# --------------------------------------------------------------------------
#
# TinyGL configuration
#
#AC_ARG_WITH([tinygl],
#            AS_HELP_STRING([--with-tinygl],
#            [allow use of libtinygl if available [[default=no]]]))
#
am_libtinygl="no"
#AH_TEMPLATE(HAVE_LIBTINYGL)
#if test "${with_tinygl:=yes}" = "yes"; then
#  #
#  # check tinygl
#  #
#  AC_CHECK_LIB([tinygl], [glBegin],
#               [VRELIBS="${OSDEP_LIBS} -l${libtinygl} ${VRELIBS}"],
#               [AC_MSG_RESULT([	use embedded tinygl ])],
#               [${OSDEP_LIBS}])
#  if test $ac_cv_lib_tinygl_glBegin = "yes" ; then
#    AC_DEFINE(HAVE_LIBTINYGL)
#  else
#    am_libtinygl="yes"
#    AC_DEFINE(HAVE_LIBTINYGL)
#  fi
#fi


# --------------------------------------------------------------------------
#
# OpenGL configuration
#
have_gl="no"
AC_ARG_WITH([GL-prefix],
            AS_HELP_STRING([--with-GL-prefix=DIR],
            [prefix for OpenGL/Mesa3D lib & include]))

case "`$ac_config_sub $build`" in
*-*-darwin*)
GL_L="-L/usr/X11/lib"
GL_I="-I/usr/X11/include"
;;
*)
GL_L="-L/usr/lib"
GL_I="-I/usr/include"
;;
esac
if test -n "$with_GL_prefix"; then
  GL_L="-L$with_GL_prefix/lib"
  GL_I="-I$with_GL_prefix/include"
fi
saved_cppflags=$CPPFLAGS
saved_ldflags=$LDFLAGS
CPPFLAGS="$GL_I $X_CFLAGS"
LDFLAGS="$X_LDFLAGS $X_LIBS $GL_L"

# check that the libs are here
if test "$LDFLAGS_EXT" = "" ; then
  AC_CHECK_LIB([GL], [glBegin], [have_lib_OpenGL="yes"])
else
  GL_L=""
  AC_CHECK_LIB([GL], [glBegin],
               [have_lib_OpenGL="yes" VRELIBS="-lGL $LDFLAGS_EXT $VRELIBS"],
               ,
               [-lGL $LDFLAGS_EXT])
fi

# now build the path for the OpenGL librairies
if test "$have_lib_OpenGL" = "yes"; then
  GL_lib="GL $LDFLAGS_EXT"
  have_gl="yes"
  glerror="no"
else
  AC_MSG_WARN([Can't find GL library!, try apt|dnf|yum|brew install libGL-devel or apt-get install libgl1-mesa-dev])
  glerror="yes"
fi
AH_TEMPLATE(HAVE_GL_GL_H)
AC_CHECK_HEADER([GL/gl.h], [AC_DEFINE([HAVE_GL_GL_H])],
	        AC_MSG_WARN([unable to find gl header]))

#
# check glu
#
AH_TEMPLATE(HAVE_LIBGLU)
AC_CHECK_LIB([GLU], [gluPerspective],
             [VRELIBS="$GL_L -lGLU -l$GL_lib $VRELIBS"],
             [VRELIBS="$GL_L -l$GL_lib $VRELIBS"],
             [$GL_L -l$GL_lib])
if test "$ac_cv_lib_GLU_gluPerspective" = "yes"; then
  AC_DEFINE(HAVE_LIBGLU)
else
  AC_MSG_WARN([Can't find GLU library!, try apt|dnf|yum|brew install libGLU-devel or apt-get install libglu1-mesa-dev])
  glerror="yes"
fi
AH_TEMPLATE(HAVE_GL_GLU_H)
AC_CHECK_HEADER([GL/glu.h], [AC_DEFINE([HAVE_GL_GLU_H])],
	        AC_MSG_WARN([unable to find glu header]))

if test "$glerror" != no; then
  AC_MSG_ERROR([Some necessary libraries / headers for OpenGL are missing])
  AC_MSG_RESULT([	--> OpenGL: http://www.opengl.org/])
  AC_MSG_RESULT([	--> Mesa3D: http://www.mesa3d.org/])
fi

CPPFLAGS=$saved_cppflags
LDFLAGS=$saved_ldflags
GL_CFLAGS=$GL_I
AC_SUBST(GL_CFLAGS)


# --------------------------------------------------------------------------
AC_ARG_WITH([glut],
    AS_HELP_STRING([--with-glut],[use GLUT toolkit if available [[default=no]]]),
    [ac_cv_use_glut=$withval], [ac_cv_use_glut=no])

# check toolkit system first
if test "$ac_cv_use_glut" = "yes"; then
  AC_GLUT_CHECK
  want_glut="yes"
  want_x11="no"
  ubit_features="$ubit_features UBIT_WITH_GLUT"
  if test "$have_gl" = "yes"; then
    ubit_features="$ubit_features UBIT_WITH_GL"
  fi
else
  #AC_X11_CHECK
  want_x11="yes"
  want_glut="no"
  ubit_features="$ubit_features UBIT_WITH_X11"
  if test "$have_gl" = "yes"; then
    ubit_features="$ubit_features UBIT_WITH_GL"
  fi
fi
VRELIBS="$LIBS $VRELIBS"


# --------------------------------------------------------------------------
#
# Glut configuration
#
AC_DEFUN([AC_GLUT_CHECK],
[ 
am_libglut="no"
AH_TEMPLATE(HAVE_LIBGLUT)

case "`$ac_config_sub $build`" in
*-*-darwin*)
  AC_CHECK_HEADERS([GLUT/glut.h])
  # LIBS="$LIBS -framework GLUT -framework OpenGL"
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
  case "`$ac_config_sub $build`" in
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


# --------------------------------------------------------------------------
#
# ftgl configuration
#
#AH_TEMPLATE(WITH_FTGL)
#AH_TEMPLATE(HAVE_FTGL)
#AH_TEMPLATE(HAVE_FTGL_H)
#AC_ARG_WITH([ftgl],
#      AS_HELP_STRING([--with-ftgl],[allow use of libftgl if available [[default=no]]]),
#     [ac_cv_use_ftgl=$withval], [ac_cv_use_ftgl=no])
#
#if test "${with_ftgl:=no}" = "yes"; then
#  #
#  # check ftgl
#  #
#  AC_CHECK_LIB([ftgl], [main], 
#               [VRELIBS="-lftgl ${VRELIBS}"],
#               [AC_MSG_WARN([ftgl library not found, use embedded ftgl])])
#  if test "$ac_cv_lib_ftgl_main" = "yes" ; then
#    AC_DEFINE(WITH_FTGL)
#    AC_DEFINE(HAVE_FTGL)
#    am_ftgl="no"
#    AC_CHECK_HEADER([FTGL/ftgl.h], [AC_DEFINE([HAVE_FTGL_H])],
#	             AC_MSG_WARN([unable to find ftgl header]))
#  else
#    am_libftgl="yes"
#  fi
#else
#  am_libftgl="yes"
#fi


# --------------------------------------------------------------------------
#
# Check gl2ps support
#
am_libgl2ps="no"
AC_ARG_ENABLE([gl2ps],
              AS_HELP_STRING([--enable-gl2ps],
              [allow use of gl2ps if available [[default=no]]]))

AH_TEMPLATE(HAVE_GL2PS)
if test "${enable_gl2ps:=no}" = "yes"; then
  am_libgl2ps="yes"
  AC_DEFINE(HAVE_GL2PS)
fi


# --------------------------------------------------------------------------
#
# Check glui support
#
am_libglui="no"
AC_ARG_ENABLE([glui],
              AS_HELP_STRING([--enable-glui],
              [allow use of glui if available [[default=no]]]))

if test "${enable_glui:=no}" = "yes"; then
  am_libglui="yes"
fi


# --------------------------------------------------------------------------
#
# SQLlite configuration
#
AC_ARG_WITH([sqlite],
            AS_HELP_STRING([--with-sqlite],
            [allow use of sqlite if available [[default=no]]]),
            [ac_cv_use_sqlite=$withval], [ac_cv_use_sqlite=no])

AH_TEMPLATE(WITH_SQLITE)
if test "${with_sqlite:=no}" = "yes"; then
  AC_DEFINE(WITH_SQLITE)
  AH_TEMPLATE(HAVE_SQLITE)
  AH_TEMPLATE(HAVE_SQLITE_H)

  SQLITE_L="-L${prefix}/lib"
  SQLITE_I="-I${prefix}/include"
  if test -f ${prefix}/include/sqlite3.h ; then
    SQLITE_I="-I${prefix}/include"
    #AC_DEFINE(HAVE_SQLITE_H)
  else
    echo "sqlite3.h not found, check config.log!"
  fi
  saved_cppflags=$CPPFLAGS
  SQLITE_CFLAGS=${SQLITE_I}
  AC_SUBST(SQLITE_CFLAGS)
  CPPFLAGS="${SQLITE_I} ${CPPFLAGS}"
  AC_CHECK_HEADER([sqlite3.h], [AC_DEFINE([HAVE_SQLITE_H])],)
  AC_CHECK_LIB([sqlite3], [sqlite3_open], [VRELIBS="${SQLITE_L} -lsqlite3 ${VRELIBS}"],,)
  if test "$ac_cv_lib_sqlite3_sqlite3_open" = "yes" ; then
    AC_DEFINE(HAVE_SQLITE)
  fi
fi


# --------------------------------------------------------------------------
#
# PostgreSql configuration
#
AC_ARG_WITH([pgsql],
            AS_HELP_STRING([--with-pgsql],
            [allow use of pgsql if available [[default=no]]]),
            [ac_cv_use_pgsql=$withval], [ac_cv_use_pgsql=no])

AH_TEMPLATE(WITH_PGSQL)
if test "${with_pgsql:=no}" = "yes"; then
  AC_DEFINE(WITH_PGSQL)
  AH_TEMPLATE(HAVE_PGSQL)
  AH_TEMPLATE(HAVE_PGSQL_H)

  PGSQL_L="-L${prefix}/lib"
  PGSQL_I="-I${prefix}/include"
  if test -f ${prefix}/include/libpq-fe.h ; then
    PGSQL_I="-I${prefix}/include"
  else
    echo "libpq-fe.h not found, check config.log!"
  fi
  saved_cppflags=$CPPFLAGS
  PGSQL_CFLAGS=${PGSQL_I}
  AC_SUBST(PGSQL_CFLAGS)
  CPPFLAGS="${PGSQL_I} ${CPPFLAGS}"
  AC_CHECK_HEADER([libpq-fe.h], [AC_DEFINE([HAVE_PGSQL_H])],)
  AC_CHECK_LIB([pq], [PQstatus], [VRELIBS="${PGSQL_L} -lpq ${VRELIBS}"],,)
  if test "$ac_cv_lib_pq_PQstatus" = "yes" ; then
    AC_DEFINE(HAVE_PGSQL)
  fi
fi


# --------------------------------------------------------------------------
#
# MySql configuration
#
AC_ARG_WITH([mysql],
            AS_HELP_STRING([--with-mysql],
            [allow use of mysql if available [[default=no]]]),
            [ac_cv_use_mysql=$withval], [ac_cv_use_mysql=no])

if test "${with_mysql:=no}" = "yes"; then
  AH_TEMPLATE(WITH_MYSQL)
  AH_TEMPLATE(HAVE_LIBMYSQLCLIENT)
  AH_TEMPLATE(HAVE_MYSQL_ERROR)
  AH_TEMPLATE(HAVE_MYSQL)
  AH_TEMPLATE(HAVE_MYSQL_H)
  AH_TEMPLATE(HAVE_MYSQL_MYSQL_H)

  # lib path
  MYSQL_L="-L${prefix}/lib"
  if test -f ${prefix}/lib/mysql/libmysqlclient.so ; then
    MYSQL_L="-L${prefix}/lib/mysql -R${prefix}/lib/mysql"
  elif test -f ${prefix}/lib64/mysql/libmysqlclient.so ; then
    MYSQL_L="-L${prefix}/lib/mysql -R${prefix}/lib/mysql"
  elif test -f ${prefix}/lib/mysql/libmysqlclient.a ; then
    MYSQL_L="-L${prefix}/lib/mysql"
  elif test -f ${prefix}/lib/libmysqlclient.a ; then
    MYSQL_L="-L${prefix}/lib"
  elif test -f /usr/lib64/mysql/libmysqlclient.so ; then
    MYSQL_L="-L/usr/lib64/mysql"
  elif test -f /usr/lib64/libmysqlclient.so ; then
    MYSQL_L="-L/usr/lib64"
  elif test -f /usr/lib64/libmysqlclient.a ; then
    MYSQL_L="-L/usr/lib64"
  elif test -f /usr/lib/libmysqlclient.a ; then
    MYSQL_L="-L/usr/lib"
  elif test -f /usr/local/mysql/lib/libmysqlclient.a ; then
    MYSQL_L="-L/usr/local/mysql/lib"
  elif test -f /usr/lib/mysql/libmysqlclient.a ; then
    MYSQL_L="-L/usr/lib/mysql"
  elif test -f /usr/sfw/mysql/lib/libmysqlclient.a ; then
    MYSQL_L="-L/usr/sfw/mysql/lib"
  elif test -f /opt/local/lib/mysql/libmysqlclient.a ; then
    MYSQL_L="-L/opt/local/lib/mysql"
  elif test -f /sw/lib/mysql/libmysqlclient.a ; then
    MYSQL_L="-L/sw/lib/mysql"
  else
    echo "libmysqlclient.a not found, check config.log!"
  fi
  # header path
  MYSQL_I="-I${prefix}/include"
  if test -f ${prefix}/include/mysql/mysql.h ; then
    MYSQL_I="-I${prefix}/include/mysql"
  elif test -f /usr/include/mysql/mysql.h ; then
    MYSQL_I="-I/usr/include/mysql"
  elif test -f /usr/local/mysql/include/mysql.h ; then
    MYSQL_I="-I/usr/local/mysql/include"
  elif test -f /usr/sfw/mysql/include/mysql.h ; then
    MYSQL_I="-I/usr/sfw/mysql/include"
  elif test -f /opt/local/include/mysql/mysql.h ; then
    MYSQL_I="-I/opt/local/include/mysql"
  elif test -f /sw/include/mysql/mysql.h ; then
    MYSQL_I="-I/sw/include/mysql"
  else
    echo "mysql.h not found, check config.log!"
  fi
  saved_cppflags=$CPPFLAGS
  MYSQL_CFLAGS=${MYSQL_I}
  AC_SUBST(MYSQL_CFLAGS)
  CPPFLAGS="${MYSQL_I} ${CPPFLAGS}"
  AC_CHECK_HEADER([mysql.h],
	          [AC_DEFINE([HAVE_MYSQL_H])],
	          [AC_CHECK_HEADER([mysql/mysql.h],
			           [AC_DEFINE([HAVE_MYSQL_MYSQL_H])],
			           AC_MSG_WARN([unable to find mysql header])]
			          )
	         )
  #
  # check libmysqlclient
  #
  if test "$ac_cv_lib_z_inflate" = "yes" ; then
    AC_CHECK_LIB([mysqlclient], [mysql_real_connect],
	         [VRELIBS="${MYSQL_L} -lmysqlclient ${VRELIBS}"],,
	         [${MYSQL_L} -lz])
  else
    AC_CHECK_LIB([mysqlclient], [mysql_real_connect],
	         [VRELIBS="${MYSQL_L} -lmysqlclient ${VRELIBS}"],,
	         [${MYSQL_L}])
  fi
  if test "$ac_cv_lib_mysqlclient_mysql_real_connect" = "yes" ; then
    AC_DEFINE(WITH_MYSQL)
    AC_DEFINE(HAVE_LIBMYSQLCLIENT)
    if test "$ac_cv_lib_z_inflate" = "yes" ; then
      AC_CHECK_LIB([mysqlclient], [mysql_error],
	           [VRELIBS="${VRELIBS}"],,
	           [${MYSQL_L} -lz])
    else
      AC_CHECK_LIB([mysqlclient], [mysql_error],
	           [VRELIBS="${VRELIBS}"],,
	           [${MYSQL_L}])
    fi
    if test "$ac_cv_lib_mysqlclient_mysql_error" = "yes" ; then
      AC_DEFINE(HAVE_MYSQL_ERROR)
    fi
    AC_DEFINE(HAVE_MYSQL)
  else
    AC_MSG_RESULT([	--> libmysqlclient: http://www.mysql.com/])
  fi
fi


# --------------------------------------------------------------------------
#
# Xml2 configuration
#
AC_ARG_WITH([xml2],
            AS_HELP_STRING([--with-xml2],
            [allow use of libxml2 if available [[default=yes]]]))

AH_TEMPLATE(HAVE_LIBXML2)
if test "${with_xml2:=yes}" = "yes"; then
  #
  # check xml2
  #
  AH_TEMPLATE(XML2_CFLAGS)
  XML2_CFLAGS=
  AC_PATH_PROG(XML2CONFIG, xml2-config)
  if test "${ac_cv_path_XML2CONFIG}" != "" ; then
    XML2_CFLAGS=`${XML2CONFIG} --cflags`
    AC_SUBST(XML2_CFLAGS)
  fi
  saved_cppflags=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $XML2_CFLAGS"

  AC_CHECK_HEADERS([libxml/parser.h])
  AC_CHECK_HEADERS([libxml/tree.h])
  AC_CHECK_HEADERS([libxml/xmlreader.h])
  AC_CHECK_HEADERS([libxml/xpath.h])
  AC_CHECK_HEADERS([libxml/xpathInternals.h])

  AC_CHECK_LIB([xml2], [xmlReaderForFile],
               [VRELIBS="${OSDEP_LIBS} -lxml2 ${VRELIBS}"],
               [AC_MSG_RESULT([	--> libxml2: http://xmlsoft.org/downloads.html])],
               [`${XML2CONFIG} --libs`])
  CPPFLAGS=$saved_cppflags
  if test "$ac_cv_lib_xml2_xmlReaderForFile" = "yes" ; then
    AC_DEFINE(HAVE_LIBXML2)
  fi
fi


# --------------------------------------------------------------------------
#
# Python configuration
#
AC_ARG_WITH([python],
            AS_HELP_STRING([--with-python],
            [allow use of python (experimental) [[default=no]]]))
AH_TEMPLATE(WITH_PYTHON)
AH_TEMPLATE(HAVE_LIBPYTHON)
AH_TEMPLATE(HAVE_PYTHON_H)

if test "${with_python:=no}" = "yes"; then
  AC_DEFINE(WITH_PYTHON)
  # lib path
  PYTHON_L="-L${prefix}/lib"
  if test -f ${prefix}/lib/python2.5/config/libpython2.5.so ; then
    PYTHON_L="-L${prefix}/lib/python2.5/config -R${prefix}/lib/python2.5/config"
  elif test -f ${prefix}/lib/python2.5/config/libpython2.5.a ; then
    PYTHON_L="-L${prefix}/lib/python2.5/config"
  elif test -f ${prefix}/lib/config/libpython2.5.a ; then
    PYTHON_L="-L${prefix}/lib/python2.5/config"
  elif test -f /usr/lib/python2.5/config/libpython2.5.a ; then
    PYTHON_L="-L/usr/lib/python2.5/config"
  elif test -f /opt/local/lib/python2.5/config/libpython2.5.a ; then
    PYTHON_L="-L/opt/local/lib/python2.5/config"
  elif test -f /sw/lib/python2.5/config/libpython2.5.a ; then
    PYTHON_L="-L/sw/lib/python2.5/config"
  else
    echo "libpython2.5.a not found, check config.log!"
  fi
  # header path
  PYTHON_I="-I${prefix}/include"
  if test -f ${prefix}/include/python2.5/Python.h ; then
    PYTHON_I="-I${prefix}/include/python2.5"
  elif test -f /usr/include/python2.5/Python.h ; then
    PYTHON_I="-I/usr/include/python2.5"	
  elif test -f /opt/local/include/python2.5/Python.h ; then
    PYTHON_I="-I/opt/local/include/python2.5"
  elif test -f /sw/include/python2.5/Python.h ; then
    PYTHON_I="-I/sw/include/python2.5"
  else
    echo "Python.h not found, check config.log!"
  fi
  saved_cppflags=$CPPFLAGS
  PYTHON_CFLAGS=${PYTHON_I}
  AC_SUBST(PYTHON_CFLAGS)
  CPPFLAGS="${PYTHON_I} ${CPPFLAGS}"
  AC_CHECK_HEADER([Python.h],
	          [AC_DEFINE([HAVE_PYTHON_H])],
	          [AC_CHECK_HEADER([python2.5/Python.h],
			           [AC_DEFINE([HAVE_PYTHON_H])],
			           AC_MSG_WARN([unable to find python header])]
			          )
	         )
  #
  # check libpython2.5
  #
  AC_CHECK_LIB([python2.5], [Py_Initialize],
	       [VRELIBS="${PYTHON_L} -lpython2.5 ${VRELIBS}"],,
	       [${PYTHON_L}])
  if test "$ac_cv_lib_python2_5_Py_Initialize" = "yes" ; then
    AC_DEFINE(HAVE_LIBPYTHON)
  else
    AC_MSG_RESULT([	--> libpython2.5: http://www.python.org/])
  fi
fi

# --------------------------------------------------------------------------
#
# Ocaml configuration
#
AH_TEMPLATE(HAVE_OCAML)
AH_TEMPLATE(OCAML_PATH)
AH_TEMPLATE(HAVE_CAML_CALLBACK_H)

am_ocaml="no"
AC_ARG_WITH([ocaml],
            AS_HELP_STRING([--with-ocaml],
            [allow use of ocaml if available [[default=no]]]))

if test "${with_ocaml:=no}" = "yes"; then
  OCAML_PATH=
  AC_PATH_PROG(OCAMLOPT, ocamlopt)
  if test "${ac_cv_path_OCAMLOPT}" != "" ; then
    OCAML_PATH=`${OCAMLOPT} -where`
  fi
  OCAML_LIBS="-L${OCAML_PATH}"
  OCAML_CFLAGS="-I${OCAML_PATH}"
  saved_cppflags=$CPPFLAGS
  AC_SUBST(OCAML_CFLAGS)
  AC_SUBST(OCAML_LIBS)
  CPPFLAGS="${OCAML_CFLAGS} ${CPPFLAGS}"
  AC_CHECK_HEADER([caml/callback.h],
	          [AC_DEFINE([HAVE_CAML_CALLBACK_H])],
	          AC_MSG_WARN([unable to find ocaml header])
	         )
  #
  # check libasmrun
  #
  AC_LANG_PUSH(C)
  echo <<!EOL >conftestml.ml
  exit(0);;
!EOL
${OCAMLOPT} -output-obj conftestml.ml -o conftest_ml.o
  AC_CHECK_LIB([asmrun], [caml_startup], [VRELIBS="${VRELIBS}"],,
               [conftest_ml.o ${OCAML_LIBS} -lasmrun -ldl])
  if test "$ac_cv_lib_asmrun_caml_startup" = "yes" ; then
    AC_DEFINE(HAVE_OCAML)
    am_ocaml="yes"
  else
    AC_MSG_RESULT([	--> try apt|dnf|yum|brew install ocaml or http://caml.inria.fr/download_en.html])
  fi
  CPPFLAGS=$saved_cppflags
  AC_DEFINE_UNQUOTED(OCAML_PATH, ["$OCAML_PATH"])
  AC_LANG_POP([C])
fi

# --------------------------------------------------------------------------
#
# check openal library
#
am_libopenal="no"
AC_ARG_WITH([openal],
            AS_HELP_STRING([--with-openal],
            [allow use of openal if available  [[default=no]]]),
            [ac_cv_use_openal=$withval], [ac_cv_use_openal=no])

if test "$ac_cv_use_openal" = "yes"; then
  AH_TEMPLATE(HAVE_OPENAL)
  AC_CHECK_LIB([openal], [alGetError],
               [VRELIBS="$OSDEP_LIBS -lopenal $VRELIBS"],
               [AC_MSG_WARN([openal library not found, check config.log!, try apt|dnf|yum|brew install openal-soft])],
               [$OSDEP_LIBS])
  if test "$ac_cv_lib_openal_alGetError" = "yes"; then
    AC_DEFINE([HAVE_OPENAL])
    AH_TEMPLATE(HAVE_AL_H)
    AC_CHECK_HEADER([AL/al.h], [AC_DEFINE([HAVE_AL_H])],
	            AC_MSG_WARN([unable to find openal header AL/al.h]))
    am_libopenal="yes"
    # check alut
    AH_TEMPLATE(HAVE_ALUT)
    AC_CHECK_LIB([freealut], [alutLoadWAVFile],
	         [VRELIBS="$OSDEP_LIBS -lfreealut $VRELIBS"],
	         [AC_MSG_WARN([freealut library not found, check config.log!, try apt|dnf|yum|brew install freealut])],
	         [$OSDEP_LIBS])
  #if test "$ac_cv_lib_freealut_alutLoadWAVFile" = "yes"; then
    VRELIBS="$OSDEP_LIBS -lfreealut $VRELIBS"
    AC_DEFINE([HAVE_ALUT])
    AH_TEMPLATE(HAVE_ALUT_H)
    AC_CHECK_HEADER([AL/alut.h], [AC_DEFINE([HAVE_ALUT_H])],
	            AC_MSG_WARN([unable to find alut header]))
    # check libsndfile
    AH_TEMPLATE(HAVE_SNDFILE)
    AC_CHECK_LIB([sndfile], [sf_open],
	         [VRELIBS="$OSDEP_LIBS -lsndfile $VRELIBS"],
	         [AC_MSG_WARN([sndfile library not found, check config.log!, try apt|dnf|yum|brew  install libsndfile])],
	         [$OSDEP_LIBS])
    if test "$ac_cv_lib_sndfile_sf_open" = "yes"; then
      AC_DEFINE([HAVE_SNDFILE])
      AH_TEMPLATE(HAVE_SNDFILE_H)
      AC_CHECK_HEADER([sndfile.h], [AC_DEFINE([HAVE_SNDFILE_H])],
	              AC_MSG_WARN([unable to find sndfile header]))
    fi
  fi
fi


# --------------------------------------------------------------------------
#
# SDL configuration
#
am_libsdl="no"
AC_ARG_WITH([sdl2],
            AS_HELP_STRING([--with-sdl2],
            [allow use of sdl2 toolkit if available [[default=no]]]),
            [ac_cv_use_sdl2=$withval], [ac_cv_sdl2=no])

if test "$ac_cv_use_SDL" = "yes" ; then
  SDL_VERSION_MIN=1.2.11
  AM_PATH_SDL($SDL_VERSION_MIN,
      [am_libsdl="yes"],
      AC_MSG_WARN([*** SDL version $SDL_VERSION_MIN not found!]))
  AH_TEMPLATE(HAVE_LIBSDL)
  if test "$am_libsdl" = "yes"; then
    AC_DEFINE(HAVE_LIBSDL)
    SDL2LIBS=`sdl2-config --libs`
    #SDL_LIBS=`echo $SDL_LIBS | sed -e 's/-Wl,-framework,Cocoa//'`
    [VRELIBS="${OSDEP_LIBS} $SDL2LIBS ${VRELIBS}"]
    AC_CHECK_HEADERS([SDL/SDL.h])
    SDL2CFLADS=`sdl2-config --cflags`
    CPPFLAGS="$CPPFLAGS $SDL2CFLAGS"
  fi
  #
  # check SDL_mixer library
  #
  AH_TEMPLATE(HAVE_LIBSDLMIXER)
  AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio],
               [VRELIBS="${OSDEP_LIBS} -lSDL_mixer ${VRELIBS}"],
               [AC_MSG_RESULT([	--> libSDL_mixer: http://www.libsdl.org/projects/SDL_Mixer:])],
               [${OSDEP_LIBS}])
  if test "$ac_cv_lib_SDL_mixer_Mix_OpenAudio" = "yes" ; then
    AC_DEFINE(HAVE_LIBSDLMIXER)
    AC_CHECK_HEADERS([SDL/SDL_mixer.h])
  fi
fi


# --------------------------------------------------------------------------
#
# Checks for external tools
#
# check for wget
AH_TEMPLATE(HAVE_WGET)
AH_TEMPLATE(WGETPATH)
AC_PATH_PROG(WGETPATH, wget, no)
if test x$WGETPATH != xno; then
  AC_DEFINE(HAVE_WGET)
  AC_DEFINE_UNQUOTED(WGETPATH, ["$WGETPATH"])
fi
# check for curl
AH_TEMPLATE(HAVE_CURL)
AH_TEMPLATE(CURLPATH)
AC_PATH_PROG(CURLPATH, curl, no)
if test x$CURLPATH != xno; then
  AC_DEFINE(HAVE_CURL)
  AC_DEFINE_UNQUOTED(CURLPATH, ["$CURLPATH"])
fi
# check for perl
AH_TEMPLATE(PERLPATH)
AC_PATH_PROG(PERLPATH, perl, no)
if test x$PERLPATH != xno; then
  AC_DEFINE_UNQUOTED(PERLPATH, ["$PERLPATH"])
fi


# --------------------------------------------------------------------------
# 
# Automake conditions (used in Makefile.am)
# 
AM_CONDITIONAL(ENABLE_UBIT,      test "$am_ubit" = "yes")
AM_CONDITIONAL(ENABLE_JAVA,      test "$am_java" = "yes")
AM_CONDITIONAL(ENABLE_OCAML,     test "$am_ocaml" = "yes")
AM_CONDITIONAL(ENABLE_VRL,       test "$am_vrl" = "yes")
AM_CONDITIONAL(ENABLE_VREL,      test "$am_vrel" = "yes")
AM_CONDITIONAL(ENABLE_VRED,      test "$am_vred" = "yes")
AM_CONDITIONAL(ENABLE_VMM,       test "$am_vmm" = "yes")
AM_CONDITIONAL(ENABLE_VACS,      test "$am_vacs" = "yes")
AM_CONDITIONAL(ENABLE_VAPS,      test "$am_vaps" = "yes")
AM_CONDITIONAL(ENABLE_VJS,       test "$am_vjs" = "yes")
AM_CONDITIONAL(ENABLE_GLUT,      test "$am_libglut" = "yes")
AM_CONDITIONAL(ENABLE_GL2PS,     test "$am_libgl2ps" = "yes")
AM_CONDITIONAL(ENABLE_SDL,       test "$am_libsdl" = "yes")
AM_CONDITIONAL(ENABLE_LIBMPEG,   test "$am_libmpeg" = "yes")
AM_CONDITIONAL(ENABLE_FTGL,      test "$am_libftgl" = "yes")
AM_CONDITIONAL(ENABLE_GLUI,      test "$am_libglui" = "yes")
AM_CONDITIONAL(ENABLE_OPENAL,    test "$am_libopenal" = "yes")
AM_CONDITIONAL(ENABLE_LIBTINYGL, test "$am_libtinygl" = "yes")


# --------------------------------------------------------------------------
#
# Make Makefiles
#
AC_CONFIG_FILES([
        Makefile 
        bin/Makefile
        bin/vre
        etc/Makefile
        man/Makefile
        src/Makefile
        src/base/Makefile
        src/gui/Makefile
        src/gui/xpm/Makefile
        src/world/Makefile
        src/net/Makefile
        src/2d/Makefile
        src/3d/Makefile
        src/model/Makefile
        src/tool/Makefile
        src/vnc/Makefile
        src/ai/Makefile
        src/ubit/Makefile
        src/ext/Makefile
        src/ext/mpeg_lib/Makefile
        src/ext/glui/Makefile
        src/ext/ftgl/Makefile
        src/ext/xmlParser/Makefile
        src/ext/gl2ps/Makefile
        src/ext/TinyGL/Makefile
        src/utils/Makefile
        src/utils/vrl/Makefile
        src/utils/vrel/Makefile
        src/utils/vred/Makefile
        src/utils/misc/Makefile
        src/servers/Makefile
        src/servers/vacs/Makefile
        src/servers/vaps/Makefile
        src/servers/vrum/Makefile
        src/servers/vjs/Makefile
        src/servers/vjs/vjs
        src/servers/vjs/base/Makefile
        src/servers/vjs/base/fr/Makefile
        src/servers/vjs/base/fr/enst/Makefile
        src/servers/vjs/base/fr/enst/vreng/Makefile
        src/servers/vjs/base/fr/enst/vreng/server/Makefile
        src/servers/vjs/base/fr/enst/vreng/net/Makefile
        src/servers/vjs/base/fr/enst/vreng/data/Makefile
        src/servers/vjs/base/fr/enst/vreng/data/types/Makefile
        src/servers/vjs/base/fr/enst/vreng/data/messages/Makefile
        src/servers/vjs/app/Makefile
        src/servers/vjs/app/fr/Makefile
        src/servers/vjs/app/fr/enst/Makefile
        src/servers/vjs/app/fr/enst/vreng/Makefile
        src/servers/vjs/app/fr/enst/vreng/tester/Makefile
        src/servers/vmm/Makefile
        ])

AC_CONFIG_COMMANDS([src/ubit/ubit_features.h],
        [
	chmod +x bin/vre src/servers/vjs/vjs;

	# Ubit
        featuresfile=src/ubit/ubit_features.h
        outfile=${featuresfile}.tmp
        AC_MSG_NOTICE([creating ${featuresfile}])
        cat > ${outfile} <<_EOF
/* Ubit features generated by configure.  Do not edit */
#ifndef UBIT_FEATURES_H
#define UBIT_FEATURES_H

#ifndef ubit_public
#define ubit_public
#endif

#define UBIT_VERSION_MAJOR 7
#define UBIT_VERSION_MINOR 0
#define UBIT_VERSION_MICRO 0
#define UBIT_VERSION "$ubit_version_major.$ubit_version_minor.$ubit_version_micro"

#include "config.h"	// vreng heritage

/* Features */
_EOF

#echo "Ubit features :"
for feature in $ubit_features
do
        echo "#define   $feature 1" >> ${outfile}
done
if test "$TTFPATH" != ""; then
  echo "#define	TTFPATH $TTFPATH" >> ${outfile}
fi
echo '' >> ${outfile}
echo '#endif // UBIT_FEATURES_H' >> ${outfile}

if cmp -s ${outfile} ${featuresfile}; then
  AC_MSG_NOTICE([${featuresfile} is unchanged])
  rm -f ${outfile}
else
  mv ${outfile} ${featuresfile}
fi
],
[
  ubit_features='${ubit_features}'
  ubit_version_major=7
  ubit_version_minor=0
  ubit_version_micro=0
])

# config.status
AC_OUTPUT

AC_MSG_RESULT([
----------------------------------------------------
VREng-$VERSION has been configured successfully!

Now type 'make' to build ./bin/vreng

'make install' to install it in ${prefix}/bin/vreng.
----------------------------------------------------
])
