Name: kompas-qt
URL: http://mosra.cz/blog/kompas.php
Version: 0.1.2
Release: 1
License: LGPLv3
%if %{defined suse_version}
Group: Productivity/Graphics/Viewers
%else
Group: Applications/Multimedia
%endif
Source: https://github.com/mosra/%{name}/tarball/v%{version}/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: gcc-c++
BuildRequires: cmake >= 2.6.0
BuildRequires: kompas-core-devel = %{version}
Requires: kompas-core = %{version}

%if %{defined suse_version}
BuildRequires: update-desktop-files
BuildRequires: libqt4-devel >= 4.6.0
Requires: libqt4 >= 4.6.0
Requires: libqt4-x11 >= 4.6.0
%endif
%if %{defined fedora}
BuildRequires: desktop-file-utils
BuildRequires: qt-devel >= 4.6.0
Requires: qt >= 4.6.0
Requires: qt-x11 >= 4.6.0
%endif
%if %{defined mandriva_version}
Requires(post): desktop-file-utils
Requires(postun): desktop-file-utils
# TODO: Better 64/32 bit splitting?
%ifarch %{ix86}
BuildRequires: libqt4-devel >= 4.6.0
Requires: libqtcore4 >= 4.6.0
Requires: libqtgui4 >= 4.6.0
Requires: libqtnetwork4 >= 4.6.0
%endif
%ifarch x86_64
BuildRequires: lib64qt4-devel >= 4.6.0
Requires: lib64qtcore4 >= 4.6.0
Requires: lib64qtgui4 >= 4.6.0
Requires: lib64qtnetwork4 >= 4.6.0
%endif
%endif

Summary: Qt GUI for Kompas navigation software

%description
Online and offline map viewer and tools for map conversion and download.

%package devel
%if %{defined suse_version}
Group: Development/Libraries/X11
%else
Group: Development/Libraries
%endif
Summary: Kompas Qt GUI development files
Requires: %{name} = %{version}
Requires: kompas-core-devel = %{version}
%if %{defined suse_version}
Requires: libqt4-devel >= 4.6.0
%endif
%if %{defined fedora}
Requires: qt-devel >= 4.6.0
%endif
%if %{defined mandriva_version}
%ifarch %{ix86}
Requires: libqt4-devel >= 4.6.0
%endif
%ifarch x86_64
Requires: lib64qt4-devel >= 4.6.0
%endif
%endif

%description devel
Qt GUI headers needed for developing for Kompas Qt GUI.

%prep
%setup -q -n mosra-kompas-qt-53a7b35

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
strip $RPM_BUILD_ROOT/%{_prefix}/bin/*

%if %{defined suse_version}
%suse_update_desktop_file Kompas
%endif
%if %{defined fedora}
desktop-file-validate $RPM_BUILD_ROOT/%{_datadir}/applications/Kompas.desktop
%endif

%if %{defined mandriva_version}
%post
#TODO: something similar for other?
%update_desktop_database
%update_icon_cache hicolor

%postun
%clean_desktop_database
%update_icon_cache hicolor
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_prefix}/bin/*
%{_prefix}/share/kompas
%{_prefix}/share/applications/*
%{_prefix}/share/icons/*
%doc COPYING COPYING.LESSER

%files devel
%defattr(-,root,root,-)
%{_prefix}/include/Kompas
%{_prefix}/share/*/Modules
%doc COPYING COPYING.LESSER

%changelog
* Sun Mar 20 2011 Vladimír Vondruš <mosra@centrum.cz> - 0.1.2-1
- New upstream version 0.1.2.

* Sat Feb 26 2011 Vladimír Vondruš <mosra@centrum.cz> - 0.1.1-1
- Initial release.
