#Available values for profile: mobile, wearable, tv, ivi, common
%{!?profile:%define profile tv}

Name:           wgt-backend
Summary:        Application installer backend for WGT
Version:        0.1
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz

Source1000:     wgt-backend.manifest
Source1001:     wgt-backend-tests.manifest

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  gtest-devel
BuildRequires:  pkgconfig(app-installers)
BuildRequires:  pkgconfig(manifest-parser)
BuildRequires:  pkgconfig(manifest-handlers)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(libwebappenc)

%description
This is a package that installs the WGT backend of pkgmgr.

%package tests
Summary: Unit tests for wgt-backend
Requires: %{name} = %{version}

%description tests
Unit tests for wgt-backend

%prep
%setup -q

cp %{SOURCE1000} .
cp %{SOURCE1001} .

%build
#Variable for setting symlink to runtime
runtime_path=%{_bindir}/xwalk-launcher
%if "%{profile}" == "mobile" || "%{profile}" == "wearable" || "%{profile}" == "tv"
runtime_path=%{_bindir}/wrt
%endif
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} -DWRT_LAUNCHER=${runtime_path}
make %{?_smp_mflags}

%install
%make_install
mkdir -p %{buildroot}/etc/package-manager/backend
ln -s %{_bindir}/wgt-backend %{buildroot}%{_sysconfdir}/package-manager/backend/wgt

%files
%manifest wgt-backend.manifest
%license LICENSE
%{_sysconfdir}/package-manager/backend/wgt
%{_bindir}/wgt-backend

%files tests
%manifest wgt-backend-tests.manifest
%{_bindir}/wgt-backend-ut/*
%{_datadir}/wgt-backend-ut/*

%changelog
* Thu Dec 18 2015 Pawel Sikorski <p.sikorski@samsung.com> 0.1-1
- initial files creation
