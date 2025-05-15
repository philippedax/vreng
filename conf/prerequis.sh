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
dev=""			# suffix for libs
pfx=/usr		# prefix for libs
dyl=so			# suffix for libs
ret=0			# return code
log=log/by_hand.log	# log to install by hand


# name of this script
p=$(basename $0)

####
# determines sudo
#
if [ "do_sudo" == "yes" ]; then
  sudo=sudo
else
  sudo=""
  cp /dev/null $log
fi

####
# determines os
#
case $(uname -s) in

Darwin)		# macos
  dist=macos
  echo "$p: $dist !"

  # brew
  if [ $(which brew) ]; then
    echo "$p: brew !"
    dyl=dylib
    dev=
  else
    echo "$p: get brew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    echo "install brew" >>$log
  fi

  # X11
  if [ $(which xquartz) ]; then
    echo "$p: xquartz !"
  else
    echo "$p: get xquartz..."
    echo "brew install --cask xquartz" >>$log
    brew install --cask xquartz >>$log
  fi
  java=java
  ret=0
  ;;

Linux)		# debian, ubuntu, mint, fedora, centos, arch
  dist=$(cat /etc/*-release | grep ID | head -n1 | cut -d '=' -f2)
  if [ $dist != "" ]; then
    echo "$p: $dist !"
    ret=0
  else
    echo "$p: dist $dist unknown"
    ret=1
  fi

  if [ $(which apt-get) ]; then
    echo "$p: apt-get !"
    cmd="$sudo apt-get install -y"
    dev="-dev"
    dyl=so
    pfx=/usr
    java="default-jdk"
  elif [ $(which dnf) ]; then
    echo "$p: dnf !"
    cmd="$sudo dnf install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"
  elif [ $(which yum) ]; then
    echo "$p: yum !"
    cmd="$sudo yum install"
    dev="-devel"
    dyl=so
    pfx=/usr
    java="java-1.8.0-openjdk.x86_64"	# not sure
  elif [ $(which pacman) ]; then
    dist=arch
    echo "$p: pacman !"
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
  echo "$p: bsd distrib !"
  cmd="$sudo pkg"
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

####
# Autotools : autoconf, automake, aclocal, autoreconf
#

# autoconf
if [ $(which autoconf) ]; then
  echo "$p: autoconf !"
else
  echo "$p: get autoconf..."
  echo "$cmd autoconf" >>$log
  $cmd autoconf
fi

# automake
if [ $(which automake) ]; then
  echo "$p: automake !"
else
  echo "$p: get automake..."
  echo "$cmd automake" >>$log
  $cmd automake
fi

# aclocal
if [ $(which aclocal) ]; then
  echo "$p: aclocal !"
else
  echo "$p: get aclocal..."
  echo "$cmd aclocal" >>$log
  $cmd aclocal
fi

# autoreconf
if [ $(which autoreconf) ]; then
  echo "$p: autoreconf !"
else
  echo "$p: get autoreconf..."
  echo "$cmd autoreconf" >>$log
  $cmd autoreconf
fi

####
# Compilers : g++, gcc
#

# g++
if [ $(which g++) ]; then
  echo "$p: g++ !"
else
  echo "$p: get g++..."
  echo "$cmd g++" >>$log
  $cmd g++
fi

# gcc
if [ $(which gcc) ]; then
  echo "$p: gcc !"
else
  echo "$p: get gcc..."
  echo "$cmd gcc" >>$log
  $cmd gcc
fi

####
# Libs : libGL, libGLU, libX11, libXmu, libXpm, libgif, libpng, libjpeg, libtiff, libfreetype
#

# libGL
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libGL.$dyl ]; then
  echo "$p: libGL !"
else
  echo "$p: get libGL..."
  echo "$cmd libgl$dev" >>$log
  $cmd libgl$dev >>$log
  $cmd libGL$dev >>$log
fi

# libGLU
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libGLU.$dyl ]; then
  echo "$p: libGLU !"
else
  echo "$p: get libGLU..."
  echo "$cmd libglu$dev" >>$log
  $cmd libglu$dev >>$log
  $cmd libGLU$dev >>$log
fi

# libX11
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libX11.$dyl ]; then
  echo "$p: libX11 !"
else
  echo "$p: get libX11..."
  echo "$cmd libX11$dev" >>$log
  $cmd libX11$dev >>$log
fi

# libXmu
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libXmu.$dyl ]; then
  echo "$p: libXmu !"
else
  echo "$p: get libXmu..."
  echo "$cmd libXmu$dev" >>$log
  $cmd libXmu$dev >>$log
fi

# libXpm
pfx=$(which xinit | cut -d '/' -f 1,2,3)	# assume X11 is installed
if [ -f $pfx/lib/libXpm.$dyl ]; then
  echo "$p: libXpm !"
else
  echo "$p: get libXpm..."
  echo "$cmd libXpm$dev" >>$log
  $cmd libXpm$dev >>$log
fi

# libgif
pfx=/usr/local
if [ -f $pfx/lib/libgif.$dyl ]; then
  echo "$p: libgif !"
else
  echo "$p: get libgif..."
  echo "$cmd libgif$dev" >>$log
  $cmd libgif$dev >>$log
fi

# libpng
pfx=/usr/local
if [ -f $pfx/lib/libpng.$dyl ]; then
  echo "$p: libpng !"
else
  echo "$p: get libpng..."
  echo "$cmd libpng$dev" >>$log
  $cmd libpng$dev >>$log
fi

# libjpeg
pfx=/usr/local
if [ -f $pfx/lib/libjpeg.$dyl ]; then
  echo "$p: libjpeg !"
else
  echo "$p: get libjpeg..."
  echo "$cmd libjpeg$dev" >>$log
  $cmd libjpeg$dev >>$log
fi

# libtiff
pfx=/usr/local
if [ -f $pfx/lib/libtiff.$dyl ]; then
  echo "$p: libtiff !"
else
  echo "$p: get libtiff..."
  echo "$cmd libtiff$dev" >>$log
  $cmd libtiff$dev >>$log
fi

# libfreetype
pfx=$(freetype-config --prefix)
if [ -f $pfx/lib/libfreetype.$dyl ]; then
  echo "$p: freetype !"
else
  echo "$p: get freetype..."
  echo "$cmd freetype$dev" >>$log
  $cmd freetype$dev >>$log
fi

#
# Optional : ocaml, sqlite3, java
#

# ocaml
if [ $(which ocaml) ]; then
  echo "$p: ocaml !"
else
  echo "$p: get ocaml..."
  echo "$cmd ocaml" >>$log
  $cmd ocaml >>$log
fi

# sqlite3
if [ $(which sqlite3) ]; then
  echo "$p: sqlite3 !"
else
  echo "$p: get sqlite3..."
  echo "$cmd sqlite3" >>$log
  $cmd sqlite3 >>$log
fi

# java
if [ $(which java) ]; then
  echo "$p: java !"
else
  echo "$p: get java..."
  echo "$cmd java" >>$log
  $cmd $java >>$log
fi

ret=0

if [ $do_sudo != "yes" ]; then
  if [ -s $log ]; then
    echo "$p: the following operations must to do manually"
    cat $log
  fi
fi

exit $ret
