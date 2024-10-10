%define ver	9.4.7
%define prefix	/usr

Summary:	VREng (Virtual Reality Engine)
Name:		vreng
Version:	%ver
Release:	1
License:	GPL
Group:
Source0:	http://www.vreng.enst.fr/ftp/pub/vreng/%{name}-%{version}.tar.gz
URL:		http://www.vreng.enst.fr/
BuildRequires:	autoconf
BuildRequires:	automake
BuildRoot:	%{_tmppath}/%{name}-%{version}-root-%(id -u -n)

%description
VREng is an Interactive and Distributed 3D Application
allowing navigation in Virtual Environments connected over the Internet
using Unicast or Multicast if available.
VREng is a Web3D which allows its users to navigate in Virtual Worlds
like rooms, campus, museums, workshops, landscapes, networks, machines,...
Visitors may interact with each other through their avatars.
They may also communicate by exchanging short textual messages (Chat),
audio and/or video channels, shared white-boards and
interact with objects in the 3D environment like Web panels,
virtual workstations, documentation on-line, MP3/Midi musics,
MPEG audio/video clips, MPEG4 animations,
and remote applications and servers.

%package devel
Summary:	The VREng Application - development files
Group:		Development/Applications
Requires:	%{name} = %{version}

%description devel
Development files for VREng.

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/vre*
%doc README COPYING COPYRIGHT INSTALL AUTHORS VERSION ANNOUNCE TODO ChangeLog

%{prefix}/bin/*

%define date	%(echo `LC_ALL="C" date +"%a %b %d %Y"`)
