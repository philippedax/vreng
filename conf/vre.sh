#! /bin/sh
#
# vre: wrapper for vreng
#
prefix=/usr/local
exec_prefix=/usr/local

PATH=.:..:../bin:${prefix}:/usr/X11R6/bin:$PATH; export PATH
LD_LIBRARY_PATH=${prefix}/lib:/usr/X11R6/lib:/usr/lib:${prefix}/lib/mysql:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

#VRENG=${exec_prefix}/vreng
VRENG=vreng

os=`uname -s`
case ${os} in
  SunOS*) LD_LIBRARY_PATH=/usr/openwin/lib:$LD_LIBRARY_PATH ;;
esac

#
# gets color depth
#
for i in 32 28 24 16 15 8
do
  depth=`xdpyinfo | grep "$i planes" | tail -1 | awk '{print $2}'`
  if [ "${depth}" != "" ]; then
    break
  fi
done
if [ "${depth}" = "" ]; then
  echo "Your visual has an unsupported color depth"
  exit 1
fi

#
# checks the GLX extension
#
if xdpyinfo | grep GLX >/dev/null ; then
  case ${depth} in
  8)
    if [ -x ${VRENG} ]; then
      exec ${VRENG} "$@"
    fi
    ;;
  16|24|28|32)
    if [ -x ${VRENG} ]; then
      exec ${VRENG} -q "$@"
    fi
    ;;
  15)
    if [ -x ${VRENG} ]; then
      exec ${VRENG} -bpp 15 "$@"
    fi
    ;;
  esac
else
  echo "Your X11 server has not the GLX extension"
  exit 1
fi

echo "no vreng executable found"
