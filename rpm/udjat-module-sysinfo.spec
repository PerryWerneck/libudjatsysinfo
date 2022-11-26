#
# spec file for package udjat-module-sysinfo
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Summary:		System information module for udjat
Name:			udjat-module-sysinfo
Version:		1.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/udjat-module-sysinfo

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	gcc-c++

BuildRequires:	pkgconfig(libudjat)

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

Requires:		libudjatsysinfo%{_libvrs} = %{version}

%description
System information module for udjat

#---[ Library ]-------------------------------------------------------------------------------------------------------

%package -n libudjatsysinfo%{_libvrs}
Summary:	UDJat core library

%description -n libudjatsysinfo%{_libvrs}
UDJat user/session library

System information library for udjat

#---[ Development ]---------------------------------------------------------------------------------------------------

%package -n udjat-sysinfo-devel
Summary: Development files for %{name}
Requires: libudjatsysinfo%{_libvrs} = %{version}

%description -n udjat-sysinfo-devel

Development files for Udjat system information library.

%lang_package

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup

NOCONFIGURE=1 \
	./autogen.sh

%configure 

%build
make all

%install
%makeinstall
%find_lang libudjatsysinfo-%{MAJOR_VERSION}.%{MINOR_VERSION} langfiles

%files
%{_libdir}/udjat-modules/*/*.so

%files -n libudjatsysinfo%{_libvrs}
%defattr(-,root,root)
%{_libdir}/libudjatsysinfo.so.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files lang -f langfiles

%files -n udjat-sysinfo-devel
%defattr(-,root,root)
%dir %{_includedir}/udjat/tools/system
%{_includedir}/udjat/tools/system/*.h
%dir %{_includedir}/udjat/tools/disk
%{_includedir}/udjat/tools/disk/*.h
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%pre -n libudjatsysinfo%{_libvrs} -p /sbin/ldconfig

%post -n libudjatsysinfo%{_libvrs} -p /sbin/ldconfig

%postun -n libudjatsysinfo%{_libvrs} -p /sbin/ldconfig

%changelog

