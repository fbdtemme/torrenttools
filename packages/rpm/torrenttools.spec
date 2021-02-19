%global forgeurl https://www.github.com/fbdtemme/torrenttools
%global tag      0.3.0
Version:         %{tag}

# Verbose processing. Remove -i -v before commit
%forgemeta

Name:            torrenttools
URL:             %{forgeurl}
Source:          %{forgesource}
License:         MIT
Release:         2%{?dist}
Summary:         A commandline tool for creating, inspecting and modifying BitTorrent metafiles.

BuildRequires:   openssl-devel
BuildRequires:   cmake
BuildRequires:   make
BuildRequires:   git
BuildRequires:   autoconf
BuildRequires:   automake

%{?fedora:BuildRequires:   gcc-c++ >= 10.0.0}
%{?fedora:BuildRequires:   libstdc++-devel >= 10.0.0}
%{?fedora:Requires:        libstdc++ >= 10.0.0}

%{?el8:BuildRequires:      gcc-toolset-10}

Requires:            openssl >= 1.0.0

%description
A commandline tool for creating, inspecting and modifying BitTorrent metafiles.

%prep
# Verbose processing. Remove -v before commit
%forgesetup

%build

%if 0%{?el8}
source /opt/rh/gcc-toolset-10/enable
%endif

%if 0%{?el8}
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOTTORRENT_CRYPTO_MULTIBUFFER=ON . .
%else
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDOTTORRENT_CRYPTO_MULTIBUFFER=ON
%endif

%if 0%{?el8}
%__cmake --build . --target torrenttools
%else
%cmake_build --target torrenttools
%endif

%install

%if 0%{?el8}
DESTDIR="%{buildroot}" %__cmake  --install . --component torrenttools
%else
%cmake_install --component torrenttools
%endif

%files
%defattr(-,root,root,-)
%dir %{_sysconfdir}/torrenttools
%{_sysconfdir}/torrenttools/config.yml
%{_sysconfdir}/torrenttools/trackers.json
%{_bindir}/torrenttools

%changelog
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
