Name: kompas-qt
URL: http://mosra.cz/blog/kompas.php
Version: 0.1.1
Release: 1
License: GNU LGPLv3
Group: Productivity/Graphics/Viewers
Source: https://github.com/mosra/%{name}/tarball/v%{version}/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: cmake >= 2.6.0
BuildRequires: libqt4-devel >= 4.6.0
BuildRequires: update-desktop-files
BuildRequires: kompas-core-devel = %{version}

Summary: Qt GUI for Kompas navigation software
Requires: libqt4 >= 4.6.0
Requires: libqt4-x11 >= 4.6.0

%description
Online and offline map viewer and tools for map conversion and download.

%package devel
Group: Development/Libraries/X11
Summary: Kompas Qt GUI development files
Requires: %{name} = %{version}
Requires: kompas-core-devel = %{version}
Requires: libqt4-devel >= 4.6.0

%description devel
Qt GUI headers needed for developing for Kompas Qt GUI.

%prep
%setup -q -n mosra-kompas-qt-16ef819

%build
export CFLAGS="$RPM_OPT_FLAGS"
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release
make %{?_smp_mflags}

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install
%suse_update_desktop_file Kompas

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_prefix}
%exclude %{_prefix}/include
%exclude %{_prefix}/share/*/Modules

%files devel
%defattr(-,root,root,-)
%{_prefix}/include/Kompas
%{_prefix}/share/*/Modules
