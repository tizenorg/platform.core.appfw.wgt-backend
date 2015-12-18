#Available values for profile: mobile, wearable, tv, ivi, common
%{!?profile:%define profile tv}

Name:           wgt-backend
Summary:        Application installer backend for WGT
Version:        0.1
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz


%changelog
* Thu Dec 18 2015 Pawel Sikorski <p.sikorski@samsung.com> 0.1-1
- initial files creation


