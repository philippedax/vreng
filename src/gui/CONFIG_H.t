const char CONFIG_H[] = "\
/* config.h.  Generated from config.h.in by configure.  */\n\
/* config.h.in.  Generated from configure.ac by autoheader.  */\n\
\n\
/* Define if building universal (internal helper macro) */\n\
/* #undef AC_APPLE_UNIVERSAL_BUILD */\n\
\n\
/* */\n\
/* #undef AIX */\n\
\n\
/* */\n\
/* #undef CENTOS */\n\
\n\
/* Define to 1 if the `closedir' function returns void instead of `int'. */\n\
/* #undef CLOSEDIR_VOID */\n\
\n\
/* */\n\
#define CURLPATH \"/usr/bin/curl\"\n\
\n\
/* */\n\
/* #undef CYGWIN32 */\n\
\n\
/* */\n\
/* #undef DEBIAN */\n\
\n\
/* */\n\
/* #undef DEF_URL_PFX */\n\
\n\
/* */\n\
/* #undef FEDORA */\n\
\n\
/* */\n\
/* #undef FREEBSD */\n\
\n\
/* gcc major version */\n\
#define GCC_MAJOR_VERSION version\n\
\n\
/* gcc version */\n\
#define GCC_VERSION \"version\"\n\
\n\
/* */\n\
/* #undef HAVE_ALUT */\n\
\n\
/* */\n\
/* #undef HAVE_ALUT_H */\n\
\n\
/* */\n\
/* #undef HAVE_AL_H */\n\
\n\
/* Define to 1 if you have the <arpa/inet.h> header file. */\n\
#define HAVE_ARPA_INET_H 1\n\
\n\
/* */\n\
/* #undef HAVE_CAML_CALLBACK_H */\n\
\n\
/* Define to 1 if you have the <ctype.h> header file. */\n\
#define HAVE_CTYPE_H 1\n\
\n\
/* */\n\
#define HAVE_CURL 1\n\
\n\
/* */\n\
#define HAVE_CURL_CURL_H 1\n\
\n\
/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.\n\
   */\n\
#define HAVE_DIRENT_H 1\n\
\n\
/* Define to 1 if you have the <dmalloc.h> header file. */\n\
/* #undef HAVE_DMALLOC_H */\n\
\n\
/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */\n\
/* #undef HAVE_DOPRNT */\n\
\n\
/* Define to 1 if you have the `drand48' function. */\n\
#define HAVE_DRAND48 1\n\
\n\
/* Define to 1 if you have the <errno.h> header file. */\n\
#define HAVE_ERRNO_H 1\n\
\n\
/* Define to 1 if you have the `fcntl' function. */\n\
#define HAVE_FCNTL 1\n\
\n\
/* Define to 1 if you have the <fcntl.h> header file. */\n\
#define HAVE_FCNTL_H 1\n\
\n\
/* Define to 1 if you have the <floatingpoint.h> header file. */\n\
/* #undef HAVE_FLOATINGPOINT_H */\n\
\n\
/* Define to 1 if you have the <float.h> header file. */\n\
#define HAVE_FLOAT_H 1\n\
\n\
/* Define to 1 if you have the `floor' function. */\n\
#define HAVE_FLOOR 1\n\
\n\
/* Define to 1 if you have the `fork' function. */\n\
#define HAVE_FORK 1\n\
\n\
/* Define to 1 if you have the `fpsetmask' function. */\n\
/* #undef HAVE_FPSETMASK */\n\
\n\
/* Define to 1 if you have the `freehostent' function. */\n\
#define HAVE_FREEHOSTENT 1\n\
\n\
/* */\n\
#define HAVE_FREETYPE 1\n\
\n\
/* Define to 1 if you have the `getaddrinfo' function. */\n\
#define HAVE_GETADDRINFO 1\n\
\n\
/* Define to 1 if you have the `getcwd' function. */\n\
#define HAVE_GETCWD 1\n\
\n\
/* Define to 1 if you have the `gethostbyaddr' function. */\n\
#define HAVE_GETHOSTBYADDR 1\n\
\n\
/* Define to 1 if you have the `gethostbyname' function. */\n\
#define HAVE_GETHOSTBYNAME 1\n\
\n\
/* Define to 1 if you have the `gethostbyname_r' function. */\n\
/* #undef HAVE_GETHOSTBYNAME_R */\n\
\n\
/* Define to 1 if you have the `gethostid' function. */\n\
#define HAVE_GETHOSTID 1\n\
\n\
/* Define to 1 if you have the `gethostname' function. */\n\
#define HAVE_GETHOSTNAME 1\n\
\n\
/* Define to 1 if you have the `getipnodebyaddr' function. */\n\
#define HAVE_GETIPNODEBYADDR 1\n\
\n\
/* Define to 1 if you have the `getipnodebyname' function. */\n\
#define HAVE_GETIPNODEBYNAME 1\n\
\n\
/* Define to 1 if you have the `getopt' function. */\n\
#define HAVE_GETOPT 1\n\
\n\
/* Define to 1 if you have the <getopt.h> header file. */\n\
#define HAVE_GETOPT_H 1\n\
\n\
/* Define to 1 if you have the `getopt_long' function. */\n\
#define HAVE_GETOPT_LONG 1\n\
\n\
/* Define to 1 if you have the `getpid' function. */\n\
#define HAVE_GETPID 1\n\
\n\
/* Define to 1 if you have the `getpwuid' function. */\n\
#define HAVE_GETPWUID 1\n\
\n\
/* Define to 1 if you have the `getservbyname_r' function. */\n\
/* #undef HAVE_GETSERVBYNAME_R */\n\
\n\
/* Define to 1 if you have the `gettimeofday' function. */\n\
#define HAVE_GETTIMEOFDAY 1\n\
\n\
/* Define to 1 if you have the `getuid' function. */\n\
#define HAVE_GETUID 1\n\
\n\
/* Define to 1 if you have the <GLUT/glut.h> header file. */\n\
/* #undef HAVE_GLUT_GLUT_H */\n\
\n\
/* Define to 1 if you have the <GL/glut.h> header file. */\n\
/* #undef HAVE_GL_GLUT_H */\n\
\n\
/* */\n\
#define HAVE_GL_GLU_H 1\n\
\n\
/* */\n\
#define HAVE_GL_GL_H 1\n\
\n\
/* Define to 1 if you have the <ieeefp.h> header file. */\n\
/* #undef HAVE_IEEEFP_H */\n\
\n\
/* Define to 1 if you have the `inet_ntoa' function. */\n\
#define HAVE_INET_NTOA 1\n\
\n\
/* Define to 1 if you have the `inet_ntop' function. */\n\
#define HAVE_INET_NTOP 1\n\
\n\
/* Define to 1 if you have the `inet_pton' function. */\n\
#define HAVE_INET_PTON 1\n\
\n\
/* Define to 1 if you have the <inttypes.h> header file. */\n\
#define HAVE_INTTYPES_H 1\n\
\n\
/* */\n\
/* #undef HAVE_IPV6 */\n\
\n\
/* Define to 1 if you have the `isascii' function. */\n\
#define HAVE_ISASCII 1\n\
\n\
/* */\n\
#define HAVE_JAVA 1\n\
\n\
/* */\n\
#define HAVE_JPEGLIB_H 1\n\
\n\
/* */\n\
#define HAVE_LIBCURL 1\n\
\n\
/* Define to 1 if you have the `dmallocthcxx' library (-ldmallocthcxx). */\n\
/* #undef HAVE_LIBDMALLOCTHCXX */\n\
\n\
/* Define to 1 if you have the `efence' library (-lefence). */\n\
/* #undef HAVE_LIBEFENCE */\n\
\n\
/* */\n\
#define HAVE_LIBFREETYPE 1\n\
\n\
/* */\n\
#define HAVE_LIBGIF 1\n\
\n\
/* */\n\
#define HAVE_LIBGLU 1\n\
\n\
/* Define to 1 if you have the `glut' library (-lglut). */\n\
/* #undef HAVE_LIBGLUT */\n\
\n\
/* Define to 1 if you have the `iberty' library (-liberty). */\n\
/* #undef HAVE_LIBIBERTY */\n\
\n\
/* */\n\
#define HAVE_LIBJPEG 1\n\
\n\
/* Define to 1 if you have the `m' library (-lm). */\n\
#define HAVE_LIBM 1\n\
\n\
/* */\n\
#define HAVE_LIBMPEG 1\n\
\n\
/* */\n\
/* #undef HAVE_LIBMYSQLCLIENT */\n\
\n\
/* Define to 1 if you have the `nsl' library (-lnsl). */\n\
/* #undef HAVE_LIBNSL */\n\
\n\
/* */\n\
/* #undef HAVE_LIBPNG */\n\
\n\
/* */\n\
#define HAVE_LIBPTHREAD 1\n\
\n\
/* */\n\
/* #undef HAVE_LIBPYTHON */\n\
\n\
/* */\n\
/* #undef HAVE_LIBSDL */\n\
\n\
/* */\n\
/* #undef HAVE_LIBSDLMIXER */\n\
\n\
/* Define to 1 if you have the `socket' library (-lsocket). */\n\
/* #undef HAVE_LIBSOCKET */\n\
\n\
/* */\n\
/* #undef HAVE_LIBTIFF */\n\
\n\
/* */\n\
#define HAVE_LIBX11 1\n\
\n\
/* */\n\
#define HAVE_LIBXML2 1\n\
\n\
/* Define to 1 if you have the <libxml/parser.h> header file. */\n\
/* #undef HAVE_LIBXML_PARSER_H */\n\
\n\
/* Define to 1 if you have the <libxml/tree.h> header file. */\n\
/* #undef HAVE_LIBXML_TREE_H */\n\
\n\
/* Define to 1 if you have the <libxml/xmlreader.h> header file. */\n\
/* #undef HAVE_LIBXML_XMLREADER_H */\n\
\n\
/* Define to 1 if you have the <libxml/xpathInternals.h> header file. */\n\
/* #undef HAVE_LIBXML_XPATHINTERNALS_H */\n\
\n\
/* Define to 1 if you have the <libxml/xpath.h> header file. */\n\
/* #undef HAVE_LIBXML_XPATH_H */\n\
\n\
/* */\n\
#define HAVE_LIBXMU 1\n\
\n\
/* */\n\
#define HAVE_LIBXPM 1\n\
\n\
/* */\n\
#define HAVE_LIBZ 1\n\
\n\
/* Define to 1 if you have the <locale.h> header file. */\n\
#define HAVE_LOCALE_H 1\n\
\n\
/* Define to 1 if you have the `lrand48' function. */\n\
#define HAVE_LRAND48 1\n\
\n\
/* Define to 1 if your system has a GNU libc compatible `malloc' function, and\n\
   to 0 otherwise. */\n\
#define HAVE_MALLOC 1\n\
\n\
/* Define to 1 if you have the <malloc.h> header file. */\n\
/* #undef HAVE_MALLOC_H */\n\
\n\
/* Define to 1 if you have the <math.h> header file. */\n\
#define HAVE_MATH_H 1\n\
\n\
/* Define to 1 if you have the `memcpy' function. */\n\
#define HAVE_MEMCPY 1\n\
\n\
/* Define to 1 if you have the <memory.h> header file. */\n\
#define HAVE_MEMORY_H 1\n\
\n\
/* Define to 1 if you have the `memset' function. */\n\
#define HAVE_MEMSET 1\n\
\n\
/* Define to 1 if you have the `mkdir' function. */\n\
#define HAVE_MKDIR 1\n\
\n\
/* */\n\
#define HAVE_MULTICAST 1\n\
\n\
/* */\n\
/* #undef HAVE_MYSQL */\n\
\n\
/* */\n\
/* #undef HAVE_MYSQL_ERROR */\n\
\n\
/* */\n\
/* #undef HAVE_MYSQL_H */\n\
\n\
/* */\n\
/* #undef HAVE_MYSQL_MYSQL_H */\n\
\n\
/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */\n\
/* #undef HAVE_NDIR_H */\n\
\n\
/* Define to 1 if you have the <netdb.h> header file. */\n\
#define HAVE_NETDB_H 1\n\
\n\
/* Define to 1 if you have the <netinet/in.h> header file. */\n\
#define HAVE_NETINET_IN_H 1\n\
\n\
/* Define to 1 if you have the <netinet/tcp.h> header file. */\n\
#define HAVE_NETINET_TCP_H 1\n\
\n\
/* Define to 1 if you have the `nice' function. */\n\
#define HAVE_NICE 1\n\
\n\
/* */\n\
/* #undef HAVE_NO_SOCKLEN_T */\n\
\n\
/* */\n\
/* #undef HAVE_OCAML */\n\
\n\
/* */\n\
/* #undef HAVE_OPENAL */\n\
\n\
/* */\n\
/* #undef HAVE_PNG_H */\n\
\n\
/* Define to 1 if you have the `pow' function. */\n\
#define HAVE_POW 1\n\
\n\
/* Define to 1 if you have the <pthread.h> header file. */\n\
#define HAVE_PTHREAD_H 1\n\
\n\
/* Define to 1 if you have the <pwd.h> header file. */\n\
#define HAVE_PWD_H 1\n\
\n\
/* */\n\
/* #undef HAVE_PYTHON_H */\n\
\n\
/* Define to 1 if you have the `rand' function. */\n\
#define HAVE_RAND 1\n\
\n\
/* Define to 1 if you have the `random' function. */\n\
#define HAVE_RANDOM 1\n\
\n\
/* Define to 1 if you have the `readdir' function. */\n\
#define HAVE_READDIR 1\n\
\n\
/* Define to 1 if your system has a GNU libc compatible `realloc' function,\n\
   and to 0 otherwise. */\n\
#define HAVE_REALLOC 1\n\
\n\
/* Define to 1 if you have the <SDL/SDL.h> header file. */\n\
/* #undef HAVE_SDL_SDL_H */\n\
\n\
/* Define to 1 if you have the <SDL/SDL_mixer.h> header file. */\n\
/* #undef HAVE_SDL_SDL_MIXER_H */\n\
\n\
/* Define to 1 if you have the `select' function. */\n\
#define HAVE_SELECT 1\n\
\n\
/* Define to 1 if you have the `setlocale' function. */\n\
#define HAVE_SETLOCALE 1\n\
\n\
/* Define to 1 if you have the `setrlimit' function. */\n\
#define HAVE_SETRLIMIT 1\n\
\n\
/* Define to 1 if you have the <signal.h> header file. */\n\
#define HAVE_SIGNAL_H 1\n\
\n\
/* */\n\
/* #undef HAVE_SNDFILE */\n\
\n\
/* */\n\
/* #undef HAVE_SNDFILE_H */\n\
\n\
/* Define to 1 if you have the `socket' function. */\n\
#define HAVE_SOCKET 1\n\
\n\
/* Define to 1 if you have the `sqrt' function. */\n\
#define HAVE_SQRT 1\n\
\n\
/* Define to 1 if `stat' has the bug that it succeeds when given the\n\
   zero-length file name argument. */\n\
/* #undef HAVE_STAT_EMPTY_STRING_BUG */\n\
\n\
/* Define to 1 if you have the <stdarg.h> header file. */\n\
#define HAVE_STDARG_H 1\n\
\n\
/* Define to 1 if you have the <stddef.h> header file. */\n\
#define HAVE_STDDEF_H 1\n\
\n\
/* Define to 1 if you have the <stdint.h> header file. */\n\
#define HAVE_STDINT_H 1\n\
\n\
/* Define to 1 if you have the <stdlib.h> header file. */\n\
#define HAVE_STDLIB_H 1\n\
\n\
/* Define to 1 if you have the `strcasecmp' function. */\n\
#define HAVE_STRCASECMP 1\n\
\n\
/* Define to 1 if you have the `strchr' function. */\n\
#define HAVE_STRCHR 1\n\
\n\
/* Define to 1 if you have the `strdup' function. */\n\
#define HAVE_STRDUP 1\n\
\n\
/* Define to 1 if you have the `strerror' function. */\n\
#define HAVE_STRERROR 1\n\
\n\
/* Define to 1 if you have the <strings.h> header file. */\n\
#define HAVE_STRINGS_H 1\n\
\n\
/* Define to 1 if you have the <string.h> header file. */\n\
#define HAVE_STRING_H 1\n\
\n\
/* Define to 1 if you have the `strncasecmp' function. */\n\
#define HAVE_STRNCASECMP 1\n\
\n\
/* Define to 1 if you have the `strrchr' function. */\n\
#define HAVE_STRRCHR 1\n\
\n\
/* Define to 1 if you have the `strstr' function. */\n\
#define HAVE_STRSTR 1\n\
\n\
/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.\n\
   */\n\
/* #undef HAVE_SYS_DIR_H */\n\
\n\
/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.\n\
   */\n\
/* #undef HAVE_SYS_NDIR_H */\n\
\n\
/* Define to 1 if you have the <sys/param.h> header file. */\n\
#define HAVE_SYS_PARAM_H 1\n\
\n\
/* Define to 1 if you have the <sys/resource.h> header file. */\n\
#define HAVE_SYS_RESOURCE_H 1\n\
\n\
/* Define to 1 if you have the <sys/select.h> header file. */\n\
#define HAVE_SYS_SELECT_H 1\n\
\n\
/* Define to 1 if you have the <sys/socket.h> header file. */\n\
#define HAVE_SYS_SOCKET_H 1\n\
\n\
/* Define to 1 if you have the <sys/stat.h> header file. */\n\
#define HAVE_SYS_STAT_H 1\n\
\n\
/* Define to 1 if you have the <sys/time.h> header file. */\n\
#define HAVE_SYS_TIME_H 1\n\
\n\
/* Define to 1 if you have the <sys/types.h> header file. */\n\
#define HAVE_SYS_TYPES_H 1\n\
\n\
/* Define to 1 if you have the <sys/utsname.h> header file. */\n\
#define HAVE_SYS_UTSNAME_H 1\n\
\n\
/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */\n\
#define HAVE_SYS_WAIT_H 1\n\
\n\
/* */\n\
/* #undef HAVE_TIFFIO_H */\n\
\n\
/* */\n\
/* #undef HAVE_TTF_PFA */\n\
\n\
/* Define to 1 if you have the `uname' function. */\n\
#define HAVE_UNAME 1\n\
\n\
/* Define to 1 if you have the <unistd.h> header file. */\n\
#define HAVE_UNISTD_H 1\n\
\n\
/* Define to 1 if you have the `usleep' function. */\n\
#define HAVE_USLEEP 1\n\
\n\
/* Define to 1 if you have the `utime' function. */\n\
#define HAVE_UTIME 1\n\
\n\
/* Define to 1 if you have the <utime.h> header file. */\n\
#define HAVE_UTIME_H 1\n\
\n\
/* Define to 1 if `utime(file, NULL)' sets file's timestamp to the present. */\n\
#define HAVE_UTIME_NULL 1\n\
\n\
/* Define to 1 if you have the `vfork' function. */\n\
#define HAVE_VFORK 1\n\
\n\
/* Define to 1 if you have the <vfork.h> header file. */\n\
/* #undef HAVE_VFORK_H */\n\
\n\
/* Define to 1 if you have the `vprintf' function. */\n\
#define HAVE_VPRINTF 1\n\
\n\
/* Define to 1 if you have the `waitpid' function. */\n\
#define HAVE_WAITPID 1\n\
\n\
/* */\n\
#define HAVE_WGET 1\n\
\n\
/* Define to 1 if `fork' works. */\n\
#define HAVE_WORKING_FORK 1\n\
\n\
/* Define to 1 if `vfork' works. */\n\
#define HAVE_WORKING_VFORK 1\n\
\n\
/* */\n\
#define HAVE_XMU_WINUTIL_H 1\n\
\n\
/* */\n\
#define HAVE_XPM_H 1\n\
\n\
/* */\n\
/* #undef HPUX */\n\
\n\
/* */\n\
/* #undef IRIX */\n\
\n\
/* */\n\
/* #undef LINUX */\n\
\n\
/* */\n\
/* #undef LINUX_ALPHA */\n\
\n\
/* */\n\
/* #undef LINUX_IPAQ */\n\
\n\
/* */\n\
/* #undef LINUX_SPARC */\n\
\n\
/* Define to 1 if `lstat' dereferences a symlink specified with a trailing\n\
   slash. */\n\
/* #undef LSTAT_FOLLOWS_SLASHED_SYMLINK */\n\
\n\
/* */\n\
#define MACOSX 1\n\
\n\
/* */\n\
/* #undef NETBSD */\n\
\n\
/* */\n\
/* #undef NEXTSTEP */\n\
\n\
/* */\n\
/* #undef OCAML_PATH */\n\
\n\
/* */\n\
/* #undef OPENBSD */\n\
\n\
/* */\n\
/* #undef OSF1 */\n\
\n\
/* Name of package */\n\
#define PACKAGE \"vreng\"\n\
\n\
/* Define to the address where bug reports for this package should be sent. */\n\
#define PACKAGE_BUGREPORT \"philippe.dax@gmail.com\"\n\
\n\
/* Define to the full name of this package. */\n\
#define PACKAGE_NAME \"vreng\"\n\
\n\
/* Define to the full name and version of this package. */\n\
#define PACKAGE_STRING \"vreng 9.1.5\"\n\
\n\
/* Define to the one symbol short name of this package. */\n\
#define PACKAGE_TARNAME \"vreng\"\n\
\n\
/* Define to the home page for this package. */\n\
#define PACKAGE_URL \"\"\n\
\n\
/* Define to the version of this package. */\n\
#define PACKAGE_VERSION \"9.1.5\"\n\
\n\
/* */\n\
#define PERLPATH \"/opt/local/bin/perl\"\n\
\n\
/* Define to the type of arg 1 for `select'. */\n\
#define SELECT_TYPE_ARG1 int\n\
\n\
/* Define to the type of args 2, 3 and 4 for `select'. */\n\
#define SELECT_TYPE_ARG234 (fd_set *)\n\
\n\
/* Define to the type of arg 5 for `select'. */\n\
#define SELECT_TYPE_ARG5 (struct timeval *)\n\
\n\
/* */\n\
/* #undef SOLARIS */\n\
\n\
/* Define to 1 if you have the ANSI C header files. */\n\
#define STDC_HEADERS 1\n\
\n\
/* */\n\
/* #undef SUNOS */\n\
\n\
/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */\n\
#define TIME_WITH_SYS_TIME 1\n\
\n\
/* */\n\
/* #undef TTFPATH */\n\
\n\
/* */\n\
/* #undef UBUNTU */\n\
\n\
/* Version number of package */\n\
#define VERSION \"9.1.5\"\n\
\n\
/* VReng major version */\n\
#define VRENG_MAJOR_VERSION 9\n\
\n\
/* VReng micro version */\n\
#define VRENG_MICRO_VERSION 5\n\
\n\
/* VReng minor version */\n\
#define VRENG_MINOR_VERSION 1\n\
\n\
/* */\n\
/* #undef WANT_GL2PS */\n\
\n\
/* */\n\
#define WANT_GLUI 1\n\
\n\
/* */\n\
/* #undef WANT_IPV6 */\n\
\n\
/* */\n\
/* #undef WANT_OCAML */\n\
\n\
/* */\n\
#define WANT_UBIT 1\n\
\n\
/* */\n\
#define WGETPATH \"/usr/local/bin/wget\"\n\
\n\
/* */\n\
/* #undef WIN32 */\n\
\n\
/* */\n\
/* #undef WITH_FREETYPE */\n\
\n\
/* */\n\
/* #undef WITH_MYSQL */\n\
\n\
/* */\n\
#define WITH_PTHREAD 1\n\
\n\
/* */\n\
/* #undef WITH_PYTHON */\n\
\n\
/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most\n\
   significant byte first (like Motorola and SPARC, unlike Intel). */\n\
#if defined AC_APPLE_UNIVERSAL_BUILD\n\
# if defined __BIG_ENDIAN__\n\
#  define WORDS_BIGENDIAN 1\n\
# endif\n\
#else\n\
# ifndef WORDS_BIGENDIAN\n\
/* #  undef WORDS_BIGENDIAN */\n\
# endif\n\
#endif\n\
\n\
/* */\n\
/* #undef XML2_CFLAGS */\n\
\n\
/* Define to 1 if the X Window System is missing or not being used. */\n\
/* #undef X_DISPLAY_MISSING */\n\
\n\
/* */\n\
/* #undef _REENTRANT */\n\
\n\
/* Define to empty if `const' does not conform to ANSI C. */\n\
/* #undef const */\n\
\n\
/* Define to `int' if <sys/types.h> doesn't define. */\n\
/* #undef gid_t */\n\
\n\
/* Define to `__inline__' or `__inline' if that's what the C compiler\n\
   calls it, or to nothing if 'inline' is not supported under any name.  */\n\
#ifndef __cplusplus\n\
/* #undef inline */\n\
#endif\n\
\n\
/* Define to rpl_malloc if the replacement function should be used. */\n\
/* #undef malloc */\n\
\n\
/* Define to `int' if <sys/types.h> does not define. */\n\
/* #undef mode_t */\n\
\n\
/* Define to `long int' if <sys/types.h> does not define. */\n\
/* #undef off_t */\n\
\n\
/* Define to `int' if <sys/types.h> does not define. */\n\
/* #undef pid_t */\n\
\n\
/* Define to rpl_realloc if the replacement function should be used. */\n\
/* #undef realloc */\n\
\n\
/* Define to `unsigned int' if <sys/types.h> does not define. */\n\
/* #undef size_t */\n\
\n\
/* Define to `int' if <sys/types.h> doesn't define. */\n\
/* #undef uid_t */\n\
\n\
/* Define as `fork' if `vfork' does not work. */\n\
/* #undef vfork */\n\
\n\
/* Define to empty if the keyword `volatile' does not work. Warning: valid\n\
   code using `volatile' can become incorrect without. Disable with care. */\n\
/* #undef volatile */\n\
\n\
";
