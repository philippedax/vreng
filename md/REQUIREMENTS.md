- Admin rights
--------------

Your account needs to have Admin privileges to do 'sudo' or 'make install' invocations.

- MACOS
-------

First, you need to install "brew" to download other dependencies by the command:

  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

then try :

  conf/prerequis.sh

or manually do (if you have not) :

  # compilers
  brew install g++
  brew install gcc

  # autotools
  brew install autoconf
  brew install automake
  brew install aclocal
  brew install autoreconf

  # graphic
  brew install libgl
  brew install libglu
  brew install libX11
  brew install libXmu
  brew install libXpm
  brew install libgif
  brew install libpng
  brew install libjpeg
  brew install libtiff
  brew install freetype

  # optional
  brew install ocaml
  brew install java
  brew install sqlite3
  brew install openal

- LINUX or Unix-like
--------------------

Try:

  conf/prerequis.sh

or

Do "apt-get" or "yum" or "dnf" (according to your distribution) to download requirements
and compile/install them.

- All systems
-------------

To start you need to download, install, execute XQuartz (MacOS only) to have a X11 server running.
The recommended way to do that is to type:

  conf/x11.sh

You need to do "sudo apachectl start" to have a httpd server running.
The recommended way to do that is to type:

  conf/httpd.sh
