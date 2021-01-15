
Name:           torrenttools
Version:        0.1.0
Release:        1

License:        MIT
Vendor:         fbdtemme
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Summary:        A commandline tool for creating, inspecting and modifying bittorrent metafiles.

Source0:        https://www.github.com/fbdtemme/torrenttools/releases/tag/%{version}
BuildRequires:  openssl-devel, tbb-devel, libstdc++-devel >= 10.0.0, cmake, re2-devel
Requires:       openssl >= 1.0.0, tbb, re2, libstdc++ >= 10.0.0

%description
A commandline tool for creating, inspecting and modifying bittorrent metafiles.

%prep
%setup -q

%build
%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
%cmake_build --target torrenttools

%install
%cmake_install --component torrenttools

%clean
%cmake_clean

%files
%defattr(-,root,root,-)
%doc

%changelog
