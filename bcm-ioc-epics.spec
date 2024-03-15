Summary: An Input-Output Controller for K500 Beam Current Detector
Name: bcm-ioc-epics
Version: 0.6
Release: 1
License: GPL
Group: Applications/IOC
Source0: %{name}-%{version}.tar.gz
BuildArch: noarch
URL: https://gitlab.inp.nsk.su/apiminov/BCM-ioc-epics
Distribution: WSS Linux
Vendor: BINP
Packager: Arthur Piminov <A.E.Piminov@inp.nsk.su>

%description
An Input-Output Controller for K500 Beam Current Detector. Written with EPICS.

%prep
%setup -q -n

%build
%configure
make

%install

%clean
rm -rf $RPM_BUILD_ROOT
