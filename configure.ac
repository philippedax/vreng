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
# configure.ac for VREng	https://github.com/philippedax/vreng
#
# Written by Philippe Dax, report bugs to philippe.dax@gmail.com


# --------------------------------------------------------------------------
#
# Check name, version, config
#
AC_INIT([vreng],[9.4.7],[philippe.dax@gmail.com])

# Make sure we are using a recent autoconf version
#AC_PREREQ([2.71])

AC_CONFIG_SRCDIR([src/base/vreng.cpp])

# where to find macros m4
AC_CONFIG_MACRO_DIR(conf)
# where to find install.sh, config.sub
AC_CONFIG_AUX_DIR(conf)

# canonicalizing cpu:vendor:os
AC_CANONICAL_HOST([])

AM_INIT_AUTOMAKE([subdir-objects])

# version
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


# --------------------------------------------------------------------------
#
# Check for compiler
#
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP


# --------------------------------------------------------------------------
#
# Programs needed by the build and install process
#
AC_PROG_YACC
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_PROG(AR, ar, ar)
AC_PROG_RANLIB
AC_SEARCH_LIBS([strerror],[cposix])


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


# --------------------------------------------------------------------------
#
# Compile flags
#
CFLAGS="${CFLAGS}"
CXXFLAGS="${CXXFLAGS}"
CPPFLAGS="${CXXFLAGS}"
LDFLAGS="${LDFLAGS}"

if test "${GCC}" = "yes"; then
  CFLAGS="${CFLAGS} -Wall"
  CXXFLAGS="${CXXFLAGS} -Wall"
fi


# --------------------------------------------------------------------------
#
# Check for optimization
#
AC_ARG_ENABLE(optimize,
              AS_HELP_STRING([--enable-optimize],
              [allow to optimize and make code faster [[default=no]]]))
if test "${enable_optimize:=no}" = "yes"; then
  CFLAGS=`echo ${CFLAGS}     | sed -e 's/-O2/-O3/'`
  CXXFLAGS=`echo ${CXXFLAGS} | sed -e 's/-O2/-O3/'`
  LDFLAGS=`echo ${LDFLAGS}   | sed -e 's/-O2/-O3/'`
  if test "${GCC}" = "yes"; then
    optim_flags="-ffast-math -fomit-frame-pointer -funroll-loops -fpermissive"
    CFLAGS="${CFLAGS} ${optim_flags}"
    CXXFLAGS="${CXXFLAGS} ${optim_flags}"
  fi
fi


# --------------------------------------------------------------------------
#
# Check for debugging
#
AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [allow to debug [[default=no]]]))
if test "${enable_debug:=no}" = "yes"; then
  CFLAGS=`echo ${CFLAGS}     | sed -e 's/-O2//'`
  CXXFLAGS=`echo ${CXXFLAGS} | sed -e 's/-O2//'`
  LDFLAGS="`echo ${LDFLAGS}  | sed -e 's/-O2//'` -g"
fi


# --------------------------------------------------------------------------
#
# Check for profiling compiler option
#
AC_ARG_ENABLE(profiling, AS_HELP_STRING([--enable-profiling], [add -pg gcc option [[default=no]]]))
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
AC_ARG_ENABLE(permissive, AS_HELP_STRING([--enable-permissive], [add --permissive gcc option [[default=no]]]))
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
AC_ARG_ENABLE(64bit, AS_HELP_STRING([--enable-64bit], [enable 64 bits compilation [[default=no]]]))
if test "${enable_64bit:=no}" = "yes"; then
  AC_MSG_RESULT([--> Using 64 bits])
  if test "${GCC}" = "yes"; then
    CFLAGS="${CFLAGS} -m64"
    CXXFLAGS="${CXXFLAGS} -m64"
  fi
fi


# --------------------------------------------------------------------------
#
# Check for OS types
#
AH_TEMPLATE(LINUX)
AH_TEMPLATE(MACOSX)
AH_TEMPLATE(WIN32)
AH_TEMPLATE(SOLARIS)
AH_TEMPLATE(SUNOS)
AH_TEMPLATE(LINUX_SPARC)
AH_TEMPLATE(LINUX_ALPHA)
AH_TEMPLATE(LINUX_IPAQ)
AH_TEMPLATE(FREEBSD)
AH_TEMPLATE(NETBSD)
AH_TEMPLATE(OPENBSD)
AH_TEMPLATE(HPUX)
AH_TEMPLATE(AIX)
AH_TEMPLATE(OSF1)
AH_TEMPLATE(IRIX)
AH_TEMPLATE(NEXTSTEP)
AH_TEMPLATE(CYGWIN32)
AH_TEMPLATE(FEDORA)
AH_TEMPLATE(UBUNTU)
AH_TEMPLATE(CENTOS)
AH_TEMPLATE(DEBIAN)

# OS dependent paths
OSDEP_LIBS=""
OSDEP_CFLAGS=""

# X11 paths
case "$build" in
*-*-darwin*)
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

echo "canonical: $build"
case "$build" in
*-*-solaris*)
  AC_DEFINE(SOLARIS)
  X_CFLAGS="-I/usr/X11R6/include -I/usr/openwin/include"
  X_LIBS="-L/usr/openwin/lib"
  if test -d /usr/local/lib ; then
    OSDEP_LIBS="-L/usr/local/lib"
  fi
  if test -d /usr/local/include ; then
    OSDEP_CFLAGS="-I/usr/local/include"
  fi
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
  AC_DEFINE(LINUX)
  X_CFLAGS="-I/usr/include"
  if test -d /usr/lib64 ; then
    X_LIBS="-L/usr/lib64"
    OSDEP_LIBS="-L/usr/lib64"
  else
    X_LIBS="-L/usr/lib"
    OSDEP_LIBS="-L/usr/lib"
  fi
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
*-*-freebsd*)
  AC_DEFINE(FREEBSD)
  ;;
*-*-netbsd*)
  AC_DEFINE(NETBSD)
  OSDEP_LIBS="-L/usr/pkg/lib"
  if test -d /usr/pkg/include ; then
    OSDEP_CFLAGS="-I/usr/pkg/include"
  fi
  CPPFLAGS="${OSDEP_CFLAGS} $CPPFLAGS"
  # special patch for pkgsrc -lmpeg become -lmpeg_lib, because of kde conflict
  libmpeg=mpeg_lib
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
  AC_DEFINE(MACOSX)
  # needed by ocaml linking
  LDFLAGS="-Wl,-no_compact_unwind"
  # search in MacPorts and Fink installs
  if test -d /usr/X11/include ; then
    OSDEP_CFLAGS="-I/usr/X11/include"
    X_CFLAGS="-I/usr/X11/include"
    CPPFLAGS="${OSDEP_CFLAGS} $CPPFLAGS"
  elif test -d /opt/X11/include ; then
    OSDEP_CFLAGS="-I/opt/X11/include"
    X_CFLAGS="-I/opt/X11/include"
    CPPFLAGS="${OSDEP_CFLAGS} $CPPFLAGS"
  elif test -d /opt/local/include ; then
    OSDEP_CFLAGS="-I/opt/local/include"
    CPPFLAGS="${OSDEP_CFLAGS} $CPPFLAGS"
  fi
  if test -d /usr/X11/lib ; then
    OSDEP_LIBS="-L/usr/X11/lib"
    X_LIBS="-L/usr/X11/lib"
  elif test -d /opt/X11/lib ; then
    OSDEP_LIBS="-L/opt/X11/lib"
    X_LIBS="-L/opt/X11/lib"
  elif test -d /opt/local/lib ; then
    OSDEP_LIBS="-L/opt/local/lib"
  fi
  case "$build" in
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
AC_HEADER_DIRENT
AC_HEADER_RESOLV
AC_HEADER_STAT
AC_HEADER_ASSERT
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS([ unistd.h stddef.h float.h limits.h fcntl.h stdarg.h ctype.h utime.h malloc.h signal.h math.h errno.h setjmp.h getopt.h pwd.h pthread.h locale.h wchar.h libintl.h floatingpoint.h sys/param.h sys/time.h sys/timeb.h sys/socket.h sys/select.h sys/poll.h sys/resource.h sys/utsname.h netinet/tcp.h arpa/inet.h ])


# --------------------------------------------------------------------------
#
# Check for sizeofs and typedefs.
#
AC_C_BIGENDIAN
AC_C_INLINE

AC_TYPE_OFF_T
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
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
# Check for library functions.
#
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_FUNC_STRTOD
##AC_FUNC_MALLOC
##AC_FUNC_REALLOC

AC_CHECK_FUNCS([ getuid getpid getcwd getpwuid gethostname gethostid gethostbyaddr gethostbyname gethostbyname_r getservbyname getipnodebyname getipnodebyaddr getaddrinfo freehostent inet_pton inet_ntoa inet_ntop isascii gettimeofday memcpy memmove memset mkdir pow sqrt strcasecmp strchr strrchr strstr strcasecmp strerror strdup strtol strpbrk utime fcntl floor select poll socket setrlimit waitpid readdir getopt getopt_long uname rand random drand48 lrand48 usleep fpsetmask nice setlocale alarm ftime sysinfo ])

AH_TEMPLATE(_REENTRANT)
if test "$ac_cv_func_gethostbyname_r" = "yes" ; then
  AC_DEFINE(_REENTRANT)
fi

if test "$ac_cv_func_strerror" = "no"; then
  AC_CHECK_LIB([iberty], [strerror])
fi


# --------------------------------------------------------------------------
#
# Check for IP-Multicast support
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
LIBS=""
AC_SUBST(LIBS)


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
               [LIBS="-lpthread -ldl $LIBS"],
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
  CXXFLAGS="{$CXXFLAGS} -DVACS"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


# --------------------------------------------------------------------------
#
# check X11 first (see AC_X11_CHECK conf/acinclude.m4)
#
AC_X11_CHECK

saved_ldflags=$LDFLAGS
LDFLAGS="$X_LDFLAGS $X_LIBS"


# --------------------------------------------------------------------------
#
# check Xpm library
#
AC_CHECK_LIB([Xpm], [XpmReadFileToXpmImage],
	     [LIBS="-lXpm $LIBS"],
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
             [LIBS="-lungif $LIBS"],
             [],
             [-L/usr/local/lib])
if test "$ac_cv_lib_ungif_DGifOpenFileName" = "yes"; then
  AC_DEFINE(HAVE_LIBGIF)
  ubit_features="$ubit_features UBIT_WITH_GIF"
else
  AC_CHECK_LIB([gif], [DGifOpenFileName],
               [LIBS="-lgif $LIBS"],
               [AC_MSG_WARN([gif library not found, check config.log!,
	                    get ftp://prtr-13.ucsc.edu/pub/libungif/ or try apt|dnf|yum|brew install giflib-devel | apt-get install libgif-dev giflib-tools])],
               [-L/usr/local/lib])
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
             [LIBS="-ljpeg $LIBS"],
             [AC_MSG_WARN([jpeg library not found, check config.log!, or try apt|dnf|yum|brew install libjpeg-turbo-devel | apt-get install libjpeg8-dev])],
             [-L/usr/local/lib])
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
             [LIBS="-ltiff $LIBS"],
             [AC_MSG_WARN([tiff library not found, check config.log!, try apt|dnf|yum|brew install libtiff-devel | apt-get install libtiff4-dev])],
             [-L/usr/local/lib])
if test "$ac_cv_lib_tiff_TIFFOpen" = "yes"; then
  AC_DEFINE([HAVE_LIBTIFF])
  AH_TEMPLATE(HAVE_TIFFIO_H)
  AC_CHECK_HEADER([tiffio.h], [AC_DEFINE([HAVE_TIFFIO_H])],
	          AC_MSG_WARN([unable to find tiff header]))
fi


# --------------------------------------------------------------------------
#
# check png library
#
AH_TEMPLATE(HAVE_LIBPNG)
LDFLAGS="$LDFLAGS -L/usr/local/lib"
AC_CHECK_LIB([png], [png_error],
             [LIBS="-lpng $LIBS"],
             [AC_MSG_WARN([png library not found, check config.log, or try apt|dnf|yum|brew install libpng-devel or apt-get install libpng-dev])],
             [-L/usr/local/lib -lz])
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
            [allow use of builtin libmpeg [[default=yes]]]))
am_libmpeg="no"
AH_TEMPLATE(HAVE_LIBMPEG)
if test "${with_mpeg:=yes}" = "yes"; then
  #
  # check mpeg
  #
  AC_CHECK_LIB([mpeg], [OpenMPEG],
               [LIBS="-L/usr/local/lib -l${libmpeg} $LIBS"],
               [AC_MSG_RESULT([	use embedded mpeg_lib ])],
               [-L/usr/local/lib])
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
             [LIBS="-lcurl $LIBS"],
             [AC_MSG_WARN([curl library not found, check config.log, or try apt|dnf|yum|brew install libcurl-devel or apt-get install libcurl4-openssl-dev])],
             [-L/usr/local/lib])
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
             [LIBS="-lfreetype $LIBS"]],
             [AC_MSG_WARN([could not find FreeType2!, try apt|dnf|yum|brew install freetype-devel or apt-get install libfreetype6-dev])])
AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
freetype_cflags=`$FREETYPE_CONFIG --cflags`
FREETYPE_CFLAGS=$freetype_cflags
AC_SUBST(FREETYPE_CFLAGS)
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

# Use builtin ubit
UBIT_L="-L../../src/ubit"
UBIT_I="-I../../src"
UBIT_CFLAGS=$UBIT_I
LIBS="-lXmu -lX11 $LIBS"
am_ubit="yes"
am_libftgl="yes"
min_ubit_version=7.0.0
toolkit=7.0.0
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
#               [LIBS="-L/usr/local/lib -l${libtinygl} $LIBS"],
#               [AC_MSG_RESULT([	use embedded tinygl ])],
#               [-L/usr/local/lib])
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

case "$build" in
*-*-darwin*)
GL_L="-L/usr/lib -L/usr/X11/lib -L/usr/local/lib"
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
               [have_lib_OpenGL="yes" LIBS="-lGL $LDFLAGS_EXT $LIBS"],
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
# Check Glu
#
AH_TEMPLATE(HAVE_LIBGLU)
AC_CHECK_LIB([GLU], [gluPerspective],
             [LIBS="$GL_L -lGLU -l$GL_lib $LIBS"],
             [LIBS="$GL_L -l$GL_lib $LIBS"],
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
# Ubit with Glut
#
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
  want_x11="yes"
  want_glut="no"
  ubit_features="$ubit_features UBIT_WITH_X11"
  if test "$have_gl" = "yes"; then
    ubit_features="$ubit_features UBIT_WITH_GL"
  fi
fi


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
              [allow use of builtin glui [[default=no]]]))
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
  AC_CHECK_LIB([sqlite3], [sqlite3_open], [LIBS="${SQLITE_L} -lsqlite3 $LIBS"],,)
  if test "$ac_cv_lib_sqlite3_sqlite3_open" = "yes" ; then
    AC_DEFINE(HAVE_SQLITE)
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
	         [LIBS="${MYSQL_L} -lmysqlclient $LIBS"],,
	         [${MYSQL_L} -lz])
  else
    AC_CHECK_LIB([mysqlclient], [mysql_real_connect],
	         [LIBS="${MYSQL_L} -lmysqlclient $LIBS"],,
	         [${MYSQL_L}])
  fi
  if test "$ac_cv_lib_mysqlclient_mysql_real_connect" = "yes" ; then
    AC_DEFINE(WITH_MYSQL)
    AC_DEFINE(HAVE_LIBMYSQLCLIENT)
    if test "$ac_cv_lib_z_inflate" = "yes" ; then
      AC_CHECK_LIB([mysqlclient], [mysql_error], [LIBS="$LIBS"],, [${MYSQL_L} -lz])
    else
      AC_CHECK_LIB([mysqlclient], [mysql_error], [LIBS="$LIBS"],, [${MYSQL_L}])
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
               [LIBS="-lxml2 $LIBS"],
               [AC_MSG_RESULT([	--> libxml2: http://xmlsoft.org/downloads.html])],
               [`${XML2CONFIG} --libs`])
  CPPFLAGS=$saved_cppflags
  if test "$ac_cv_lib_xml2_xmlReaderForFile" = "yes" ; then
    AC_DEFINE(HAVE_LIBXML2)
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
  OCAML_LIB="-L${OCAML_PATH}"
  OCAML_CFLAGS="-I${OCAML_PATH}"
  saved_cppflags=$CPPFLAGS
  AC_SUBST(OCAML_CFLAGS)
  AC_SUBST(OCAML_LIB)
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
  AC_CHECK_LIB([asmrun], [caml_startup], [LIBS="$LIBS"],,
               [conftest_ml.o ${OCAML_LIB} -lasmrun])
  if test "$ac_cv_lib_asmrun_caml_startup" = "yes" ; then
    AC_DEFINE(HAVE_OCAML)
    am_ocaml="yes"
  else
    AC_MSG_RESULT([	--> try apt|dnf|yum|brew install ocaml or http://caml.inria.fr/download_en.html])
  fi
  CPPFLAGS=$saved_cppflags
  AC_DEFINE_UNQUOTED(OCAML_PATH, ["$OCAML_PATH"])
  AC_LANG_POP([C])
  LDFLAGS="${LDFLAGS} -Wl,-keep_dwarf_unwind -Wl,-no_compact_unwind"
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
               [LIBS="$OSDEP_LIBS -lopenal $LIBS"],
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
	         [LIBS="$OSDEP_LIBS -lfreealut $LIBS"],
	         [AC_MSG_WARN([freealut library not found, check config.log!, try apt|dnf|yum|brew install freealut])],
	         [$OSDEP_LIBS])
    LIBS="$OSDEP_LIBS -lfreealut $LIBS"
    AC_DEFINE([HAVE_ALUT])
    AH_TEMPLATE(HAVE_ALUT_H)
    AC_CHECK_HEADER([AL/alut.h], [AC_DEFINE([HAVE_ALUT_H])],
	            AC_MSG_WARN([unable to find alut header]))
    # check libsndfile
    AH_TEMPLATE(HAVE_SNDFILE)
    AC_CHECK_LIB([sndfile], [sf_open],
	         [LIBS="$OSDEP_LIBS -lsndfile $LIBS"],
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
  AH_TEMPLATE(HAVE_LIBSDL)
  if test "$am_libsdl" = "yes"; then
    AC_DEFINE(HAVE_LIBSDL)
    SDL2_LIB=`sdl2-config --libs`
    [LIBS="${OSDEP_LIBS} $SDL2_LIB $LIBS"]
    AC_CHECK_HEADERS([SDL/SDL.h])
    SDL2_CFLADS=`sdl2-config --cflags`
    CPPFLAGS="$CPPFLAGS $SDL2_CFLAGS"
  fi
  #
  # check SDL_mixer library
  #
  AH_TEMPLATE(HAVE_LIBSDLMIXER)
  AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio],
               [LIBS="${OSDEP_LIBS} -lSDL_mixer $LIBS"],
               [AC_MSG_RESULT([	--> libSDL_mixer: http://www.libsdl.org/projects/SDL_Mixer:])],
               [${OSDEP_LIBS}])
  if test "$ac_cv_lib_SDL_mixer_Mix_OpenAudio" = "yes" ; then
    AC_DEFINE(HAVE_LIBSDLMIXER)
    AC_CHECK_HEADERS([SDL/SDL_mixer.h])
  fi
fi


# --------------------------------------------------------------------------
#
# Check for external tools
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
# Automake conditions
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
AM_CONDITIONAL(ENABLE_TINYGL,    test "$am_libtinygl" = "yes")


# --------------------------------------------------------------------------
#
# Make Makefiles
#
AC_CONFIG_FILES([
        Makefile 
        src/Makefile
        src/man/Makefile
        src/bin/Makefile
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
	chmod +x src/servers/vjs/vjs;

	# Ubit
        featuresfile=src/ubit/ubit_features.h
        out=${featuresfile}.tmp
        AC_MSG_NOTICE([creating ${featuresfile}])
        cat > ${out} <<_EOFUBIT
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
_EOFUBIT

#echo "Ubit features :"
for feature in $ubit_features
do
  echo "#define   $feature 1" >> ${out}
done
if test "$TTFPATH" != ""; then
  echo "#define	TTFPATH $TTFPATH" >> ${out}
fi
echo '' >> ${out}
echo '#endif // UBIT_FEATURES_H' >> ${out}

if cmp -s ${out} ${featuresfile}; then
  AC_MSG_NOTICE([${featuresfile} is unchanged])
  rm -f ${out}
else
  mv ${out} ${featuresfile}
fi
],
[
  ubit_features='${ubit_features}'
  ubit_version_major=7
  ubit_version_minor=0
  ubit_version_micro=0
])

# clean unused scripts in conf/
echo "cleaning up $ac_aux_dir directory"
rm -f $ac_aux_dir/compile
rm -f $ac_aux_dir/depcomp
rm -f $ac_aux_dir/config.guess
rm -f $ac_aux_dir/config.sub
rm -f $ac_aux_dir/install-sh
rm -f $ac_aux_dir/missing

# config.status
AC_OUTPUT

AC_MSG_RESULT([
----------------------------------------------------
VREng-$VERSION has been configured successfully!

Now type 'make' to build ./vreng
----------------------------------------------------
])
