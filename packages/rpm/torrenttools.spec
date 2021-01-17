%global forgeurl https://www.github.com/fbdtemme/torrenttools
%global tag      0.1.1
Version:         %{tag}

# Verbose processing. Remove -i -v before commit
%forgemeta -i -v

Name:            %{repo}
URL:             %{forgeurl}
Source:          %{forgesource}
License:         MIT
Release:         1%{dist}
Summary:         A commandline tool for creating, inspecting and modifying bittorrent metafiles.

BuildRequires:   openssl-devel
BuildRequires:   tbb-devel
BuildRequires:   gcc-c++ >= 10.0.0
BuildRequires:   libstdc++-devel >= 10.0.0
BuildRequires:   cmake
BuildRequires:   make
BuildRequires:   git

Requires:        openssl >= 1.0.0
Requires:        tbb
Requires:        libstdc++ >= 10.0.0

%description
A commandline tool for creating, inspecting and modifying bittorrent metafiles.

%prep
# Verbose processing. Remove -v before commit
%forgesetup -v

%build
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
%cmake_build --target torrenttools

%install
%cmake_install --component torrenttools

%files
%defattr(-,root,root,-)
%dir %{_sysconfdir}/torrenttools
%{_sysconfdir}/torrenttools/config.yaml
%{_sysconfdir}/torrenttools/trackers.json
%{_bindir}/torrenttools

%changelog
