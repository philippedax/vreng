- Admin rights
--------------

Your account needs to have Admin privileges to do sudo or make install invocations.

- MACOS
-------

First, you need to install "brew" to downdoad other dependencies by the command:

  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

then do (if you have not) :

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
  brew install libXICE
  brew install libXpm
  brew install libjpeg
  brew install libungif
  brew install freetype

  # optional
  brew install ocaml
  brew install java
  brew install openal

- LINUX or Unix-like
--------------------

Use "apt-get" or "yum" or "dnf" (according to your distribution) to download dependencies
and compile/install them.

