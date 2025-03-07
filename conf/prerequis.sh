#! /bin/sh
#
# prerequis.sh
#
# Check and install needed requirements from scratch
#
# Note 1:
#       This script has been tested only on MacOS but not on Linux niether *BSD.
#
# Note 2:
#       If you don't trust this script to do sudo in your place
#       then initialize do_sudo to "no" 
#      
#       do_sudo="no"
#
# Philippe Dax - 2024
#
do_sudo="no"	# change to "yes" if you trust this scipt to do sudo

#if [ -f ../configure ];     then exit 0 ; fi	# assume that vreng is configured

dist=
pack=
dyl=so
dev=
ret=0
pfx=/usr
sudo=
log=../log/by_hand.log

if [ "do_sudo" == "yes" ]; then
  sudo=sudo
else
  sudo=
  cp /dev/null $log
fi

p=$(basename $0)

#
# determines os
#
case $(uname -s) in

Darwin)	# macos
  dist=macos
  echo "$p: have $dist !"

  # brew
  if [ ! $(which brew) ]; then
    echo "$p: get brew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  else
    echo "$p: have brew !"
    pack="brew install"
    dyl=dylib
    dev=
    pfx=/opt/X11
  fi

  # X11
  if [ ! $(which xquartz) ]; then
    echo "$p: get xquartz..."
    echo "brew install --cask xquartz" >>$log
    brew install --cask xquartz
  else
    echo "$p: have xquartz !"
  fi
  java=java
  ret=0
  ;;

Linux)	# debian, ubuntu, mint, fedora, centos, arch
  dist=$(cat /etc/*-release | grep ID | head -n1 | cut -d '=' -f2)
  if [ $dist != "" ]; then
    echo "$p: have $dist !"
    ret=0
  else
    echo "$p: dist $dist unknown"
    ret=1
  fi

  if [ $(which apt-get) ]; then
    echo "$p: have apt-get !"
    pack="$sudo apt-get install -y"
    dev="-dev"
    dyl=so
    pfx=/usr
    java="default-jdk"
  elif [ $(which dnf) ]; then
    echo "$p: have dnf !"
    pack="$sudo dnf install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"
  elif [ $(which yum) ]; then
    echo "$p: have yum !"
    pack="$sudo yum install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"	# not sure
  elif [ $(which pacman) ]; then
    dist=arch
    echo "$p: have pacman !"
    pack="$sudo pacman -S"
    dyl=so
    pfx=/usr
    java=java	# not sure
  else
    echo "$p: unknown linux distrib"
    ret=1
    exit $ret
  fi
  ret=0
  ;;

*bsd|*BSD)	# freeBSD, netBSD, openBSD
  pack="$sudo pkg"
  echo "$p: bsd distrib"
  dyl=so
  pfx=/usr
  java=java	# not sure
  ret=0
  ;;

*)
  echo "$p: unknown system"
  ret=1
  exit $ret
  ;;
esac

#
# Autotools
#

# autoconf
if [ ! $(which autoconf) ]; then
  echo "$p: get autoconf..."
  echo "$pack autoconf" >>$log
  $pack autoconf
else
  echo "$p: have autoconf !"
fi

# automake
if [ ! $(which automake) ]; then
  echo "$p: get automake..."
  echo "$pack automake" >>$log
  $pack automake
else
  echo "$p: have automake !"
fi

# aclocal
if [ ! $(which aclocal) ]; then
  echo "$p: get aclocal..."
  echo "$pack aclocal" >>$log
  $pack aclocal
else
  echo "$p: have aclocal !"
fi

# autoreconf
if [ ! $(which autoreconf) ]; then
  echo "$p: get autoreconf..."
  echo "$pack autoreconf" >>$log
  $pack autoreconf
else
  echo "$p: have autoreconf !"
fi

#
# Compilers
#

# g++
if [ ! $(which g++) ]; then
  echo "$p: get g++..."
  echo "$pack g++" >>$log
  $pack g++
else
  echo "$p: have g++ !"
fi

# gcc
if [ ! $(which gcc) ]; then
  echo "$p: get gcc..."
  echo "$pack gcc" >>$log
  $pack gcc
else
  echo "$p: have gcc !"
fi

#
# Graphic libs
#

# libGL
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ ! -f $pfx/lib/libGL.$dyl ]; then
  echo "$p: get libGL..."
  echo "$pack libgl$dev" >>$log
  $pack libgl$dev
  $pack libGL$dev
else
  echo "$p: have libGL !"
fi

# libGLU
if [ ! -f $pfx/lib/libGLU.$dyl ]; then
  echo "$p: get libGLU..."
  echo "$pack libglu$dev" >>$log
  $pack libglu$dev
  $pack libGLU$dev
else
  echo "$p: have libGLU !"
fi

# libX11
if [ ! -f $pfx/lib/libX11.$dyl ]; then
  echo "$p: get libX11..."
  echo "$pack libX11$dev" >>$log
  $pack libX11$dev
else
  echo "$p: have libX11 !"
fi

# libXmu
if [ ! -f $pfx/lib/libXmu.$dyl ]; then
  echo "$p: get libXmu..."
  echo "$pack libXmu$dev" >>$log
  $pack libXmu$dev
else
  echo "$p: have libXmu !"
fi

# libXpm
if [ ! -f $pfx/lib/libXpm.$dyl ]; then
  echo "$p: get libXpm..."
  echo "$pack libXpm$dev" >>$log
  $pack libXpm$dev
else
  echo "$p: have libXpm !"
fi

# libgif
pfx=/usr/local
if [ ! -f $pfx/lib/libgif.$dyl ]; then
  echo "$p: get libgif..."
  echo "$pack libgif$dev" >>$log
  $pack libgif$dev
else
  echo "$p: have libgif !"
fi

# libpng
pfx=/usr/local
if [ ! -f $pfx/lib/libpng.$dyl ]; then
  echo "$p: get libpng..."
  echo "$pack libpng$dev" >>$log
  $pack libpng$dev
else
  echo "$p: have libpng !"
fi

# libjpeg
pfx=/usr/local
if [ ! -f $pfx/lib/libjpeg.$dyl ]; then
  echo "$p: get libjpeg..."
  echo "$pack libjpeg$dev" >>$log
  $pack libjpeg$dev
else
  echo "$p: have libjpeg !"
fi

# libtiff
pfx=/usr/local
if [ ! -f $pfx/lib/libtiff.$dyl ]; then
  echo "$p: get libtiff..."
  echo "$pack libtiff$dev" >>$log
  $pack libtiff$dev
else
  echo "$p: have libtiff !"
fi

# libfreetype
pfx=$(freetype-config --prefix)
pfx=/usr/local
if [ ! -f $pfx/lib/libfreetype.$dyl ]; then
  echo "$p: get freetype..."
  echo "$pack freetype$dev" >>$log
  $pack freetype$dev
else
  echo "$p: have freetype !"
fi

#
# Optional
#

# ocaml
if [ ! $(which ocaml) ]; then
  echo "$p: get ocaml..."
  echo "$pack ocaml" >>$log
  $pack ocaml
else
  echo "$p: have ocaml !"
fi

# sqlite3
if [ ! $(which sqlite3) ]; then
  echo "$p: get sqlite3..."
  echo "$pack sqlite3" >>$log
  $pack sqlite3
else
  echo "$p: have sqlite3 !"
fi

# java
if [ ! $(which java) ]; then
  echo "$p: get java..."
  echo "$pack java" >>$log
  $pack $java
else
  echo "$p: have java !"
fi

# openal
#if [ ! f /usr/local/lib/libopenal.$dyl ]; then
#  echo "$p: get openal..."
#  echo "$pack openal" >>$log
#else
#  echo "$p: have openal !"
#  $pack openal
#fi
ret=0

if [ $do_sudo != "yes" ]; then
  echo "$p: operations to do manually"
  cat $log
fi

exit $ret
