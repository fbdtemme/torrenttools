%global tag      0.5.1

Name:            torrenttools
Version:         %{tag}
URL:             https://www.github.com/fbdtemme/torrenttools
Source:          https://www.github.com/fbdtemme/torrenttools/releases/download/v%{tag}/torrenttools-%{tag}.tar.gz
License:         MIT
Release:         1%{?dist}
Summary:         A commandline tool for creating, inspecting and modifying BitTorrent metafiles.

BuildRequires:   openssl-devel
BuildRequires:   cmake
BuildRequires:   make
BuildRequires:   git
BuildRequires:   autoconf
BuildRequires:   automake
BuildRequires:   libtool
BuildRequires:   nasm
BuildRequires:   tbb-devel

%{?fedora:BuildRequires:      gcc-c++ >= 10.2.1}
%{?fedora:BuildRequires:      libstdc++-devel >= 10.2.1}
%{?fedora:Requires:           libstdc++ >= 10.2.1}
%{?el8:BuildRequires:         gcc-toolset-10}
%{?is_opensuse:BuildRequires: gcc10-c++}
%{?is_opensuse:Requires:      libstdc++6 >= 10.2.1}

Requires:            openssl >= 1.0.0
Requires:            tbb

%description
A commandline tool for creating, inspecting and modifying BitTorrent metafiles.

%prep
%setup

%build

%if 0%{?el8}
source /opt/rh/gcc-toolset-10/enable
%endif

%if 0%{?el8}
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOTTORRENT_MB_CRYPTO_LIB=isal . .
%else
%if 0%{?is_opensuse}
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOTTORRENT_MB_CRYPTO_LIB=isal -DCMAKE_CXX_COMPILER=g++-10
%else
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOTTORRENT_MB_CRYPTO_LIB=isal
%endif
%endif


%if 0%{?fedora} >= 33
%cmake_build --target torrenttools
%else
%__cmake --build . --target torrenttools
%endif


%install


%if 0%{?fedora} >= 33
%cmake_install --component torrenttools
%else
%if 0%{?is_opensuse}
DESTDIR="%{buildroot}" %__cmake --install "build" --component torrenttools
%else
DESTDIR="%{buildroot}" %__cmake --install . --component torrenttools
%endif
%endif

%files
%defattr(-,root,root,-)
%dir %{_sysconfdir}/torrenttools
%{_sysconfdir}/torrenttools/config.yml
%{_sysconfdir}/torrenttools/trackers.json
%{_bindir}/torrenttools

%changelog
* Mon Jul 26 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.5.1-1
- Update to 0.5.1

* Fri Jul 23 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.5.0-2
- Fix build on openSUSE

* Fri Jul 23 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.5.0-1
- Update to 0.5.0

* Thu Apr 15 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.4.1-1
- Update to 0.4.1

* Sat Apr 10 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.4.0-2
- Fix ISA-L compile switch

* Sat Apr 10 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.4.0-1
- Update to 0.4.0

* Sat Feb 20 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.3.2-1
- Update to 0.3.2

* Fri Feb 19 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.3.1-1
- Update to 0.3.1

* Fri Feb 19 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.3.0-2
- Fix missing autotools dependencies

* Fri Feb 19 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.3.0-1
- Update to 0.3.0

* Sat Jan 30 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.2.2-1
- Update to 0.2.2

* Thu Jan 28 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.2.1-1
- Update to 0.2.1

* Mon Jan 25 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.2.0-2
- Fix wrong yml extensions

* Mon Jan 25 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.2.0-1
- Update to 0.2.0
- Fix wrong changelog date

* Wed Jan 20 2021 Florian De Temmerman <floriandetemmerman@gmail.com> 0.1.4-1
- Update to 0.1.4
- Fix missing changelog entry
- Remove epel-release dependency for el8 builds
