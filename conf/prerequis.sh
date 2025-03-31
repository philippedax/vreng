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
do_sudo="no"		# change to "yes" if you trust this scipt to do sudo

#if [ -f ../configure ];     then exit 0 ; fi	# assume that vreng is configured

dist=""			# os distribution
cmd=""			# command to install
sudo=""			# sudo or not
dev=""			# suffix for libs packages
pfx=/usr		# prefix for libs
dyl=so			# suffix for libs
ret=0			# return code
log=../log/by_hand.log	# log to install by hand


# name of this script
p=$(basename $0)

#
# determines sudo
#
if [ "do_sudo" == "yes" ]; then
  sudo=sudo
else
  sudo=""
  cp /dev/null $log
fi

#
# determines os
#
case $(uname -s) in

Darwin)	# macos
  dist=macos
  echo "$p: have $dist !"

  # brew
  if [ $(which brew) ]; then
    echo "$p: have brew !"
    cmd="brew install"
    dyl=dylib
    dev=
    pfx=/opt/X11
  else
    echo "$p: get brew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    echo "install brew" >>$log
  fi

  # X11
  if [ $(which xquartz) ]; then
    echo "$p: have xquartz !"
  else
    echo "$p: get xquartz..."
    echo "brew install --cask xquartz" >>$log
    brew install --cask xquartz
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
    cmd="$sudo apt-get install -y"
    dev="-dev"
    dyl=so
    pfx=/usr
    java="default-jdk"
  elif [ $(which dnf) ]; then
    echo "$p: have dnf !"
    cmd="$sudo dnf install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"
  elif [ $(which yum) ]; then
    echo "$p: have yum !"
    cmd="$sudo yum install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"	# not sure
  elif [ $(which pacman) ]; then
    dist=arch
    echo "$p: have pacman !"
    cmd="$sudo pacman -S"
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
  cmd="$sudo pkg"
  echo "$p: bsd distrib"
  dyl=so
  pfx=/usr
  java=java	# not sure
  ret=0
  ;;

*)		# others to be completed
  echo "$p: unknown system"
  ret=1
  exit $ret
  ;;
esac

#
# Autotools
#

# autoconf
if [ $(which autoconf) ]; then
  echo "$p: have autoconf !"
else
  echo "$p: get autoconf..."
  echo "$cmd autoconf" >>$log
  $cmd autoconf
fi

# automake
if [ $(which automake) ]; then
  echo "$p: have automake !"
else
  echo "$p: get automake..."
  echo "$cmd automake" >>$log
  $cmd automake
fi

# aclocal
if [ $(which aclocal) ]; then
  echo "$p: have aclocal !"
else
  echo "$p: get aclocal..."
  echo "$cmd aclocal" >>$log
  $cmd aclocal
fi

# autoreconf
if [ $(which autoreconf) ]; then
  echo "$p: have autoreconf !"
else
  echo "$p: get autoreconf..."
  echo "$cmd autoreconf" >>$log
  $cmd autoreconf
fi

#
# Compilers
#

# g++
if [ $(which g++) ]; then
  echo "$p: have g++ !"
else
  echo "$p: get g++..."
  echo "$cmd g++" >>$log
  $cmd g++
fi

# gcc
if [ $(which gcc) ]; then
  echo "$p: have gcc !"
else
  echo "$p: get gcc..."
  echo "$cmd gcc" >>$log
  $cmd gcc
fi

#
# Graphic libs
#

# libGL
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libGL.$dyl ]; then
  echo "$p: have libGL !"
else
  echo "$p: get libGL..."
  echo "$cmd libgl$dev" >>$log
  $cmd libgl$dev
  $cmd libGL$dev
fi

# libGLU
if [ -f $pfx/lib/libGLU.$dyl ]; then
  echo "$p: have libGLU !"
else
  echo "$p: get libGLU..."
  echo "$cmd libglu$dev" >>$log
  $cmd libglu$dev
  $cmd libGLU$dev
fi

# libX11
if [ -f $pfx/lib/libX11.$dyl ]; then
  echo "$p: have libX11 !"
else
  echo "$p: get libX11..."
  echo "$cmd libX11$dev" >>$log
  $cmd libX11$dev
fi

# libXmu
if [ -f $pfx/lib/libXmu.$dyl ]; then
  echo "$p: have libXmu !"
else
  echo "$p: get libXmu..."
  echo "$cmd libXmu$dev" >>$log
  $cmd libXmu$dev
fi

# libXpm
if [ -f $pfx/lib/libXpm.$dyl ]; then
  echo "$p: have libXpm !"
else
  echo "$p: get libXpm..."
  echo "$cmd libXpm$dev" >>$log
  $cmd libXpm$dev
fi

# libgif
pfx=/usr/local
if [ -f $pfx/lib/libgif.$dyl ]; then
  echo "$p: have libgif !"
else
  echo "$p: get libgif..."
  echo "$cmd libgif$dev" >>$log
  $cmd libgif$dev
fi

# libpng
pfx=/usr/local
if [ -f $pfx/lib/libpng.$dyl ]; then
  echo "$p: have libpng !"
else
  echo "$p: get libpng..."
  echo "$cmd libpng$dev" >>$log
  $cmd libpng$dev
fi

# libjpeg
pfx=/usr/local
if [ -f $pfx/lib/libjpeg.$dyl ]; then
  echo "$p: have libjpeg !"
else
  echo "$p: get libjpeg..."
  echo "$cmd libjpeg$dev" >>$log
  $cmd libjpeg$dev
fi

# libtiff
pfx=/usr/local
if [ -f $pfx/lib/libtiff.$dyl ]; then
  echo "$p: have libtiff !"
else
  echo "$p: get libtiff..."
  echo "$cmd libtiff$dev" >>$log
  $cmd libtiff$dev
fi

# libfreetype
pfx=$(freetype-config --prefix)
pfx=/usr/local
if [ -f $pfx/lib/libfreetype.$dyl ]; then
  echo "$p: have freetype !"
else
  echo "$p: get freetype..."
  echo "$cmd freetype$dev" >>$log
  $cmd freetype$dev
fi

#
# Optional
#

# ocaml
if [ $(which ocaml) ]; then
  echo "$p: have ocaml !"
else
  echo "$p: get ocaml..."
  echo "$cmd ocaml" >>$log
  $cmd ocaml
fi

# sqlite3
if [ $(which sqlite3) ]; then
  echo "$p: have sqlite3 !"
else
  echo "$p: get sqlite3..."
  echo "$cmd sqlite3" >>$log
  $cmd sqlite3
fi

# java
if [ $(which java) ]; then
  echo "$p: have java !"
else
  echo "$p: get java..."
  echo "$cmd java" >>$log
  $cmd $java
fi

# openal
#if [ f /usr/local/lib/libopenal.$dyl ]; then
#  echo "$p: have openal !"
#else
#  echo "$p: get openal..."
#  echo "$cmd openal" >>$log
#  $cmd openal
#fi
ret=0

if [ $do_sudo != "yes" ]; then
  echo "$p: theses operations must to do manually"
  cat $log
fi

exit $ret
