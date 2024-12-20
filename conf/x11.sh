#! /bin/sh
#
# x11.sh
#
# To have a X11 server running
#
PATH=/opt/X11/bin:/usr/bin:/usr/local/bin:/usr/X11/bin:$PATH export PATH

p=$(basename $0)

case $(uname -s) in
Darwin)	# MacOS
  if [ ! $(which xquartz) ]; then
    brew install --cask xquartz
    # or
    # - download XQuartz.2.x.y.pkg at www.xquartz.org
    # - unpackage it
    # and
    # - execute it
  fi
  ps ax | grep " :0 " >/dev/null 2>&1
  if [ $? = 0 ]; then
    echo "$p: Xquartz already launched"
  else
    echo "$p: launch Xquartz"
    /opt/X11/libexec/launchd_startx /opt/X11/bin/startx -- /opt/X11/bin/Xquartz &
  fi
  ;;

*)	# Linux and others
  ps ax | grep " :0 " >/dev/null 2>&1
  if [ $? = 0 ]; then
    echo "$p: Xserver already launched"
  else
    startx
  fi
  ;;

esac

