%global debug_package %{nil}
%global __strip /usr/bin/true


Summary: An Input-Output Controller for K500 Beam Current Detector
Name: bcm-ioc-epics
Version: 0.4
Release: 1
License: GPL
Group: Applications/IOC
Source0: %{name}-%{version}.tar.gz
URL: https://gitlab.inp.nsk.su/apiminov/BCM-ioc-epics
Distribution: WSS Linux
Vendor: BINP
Packager: Arthur Piminov <A.E.Piminov@inp.nsk.su>

%description
An Input-Output Controller for K500 Beam Current Detector. Written with EPICS.

%prep
%setup -q

echo "LINKER_USE_RPATH = NO" >> configure/CONFIG_SITE.local

%build

%install
export QA_SKIP_BUILD_ROOT=1
rm -rf %{buildroot}
 make \
   \
  FINAL_LOCATION=/epics/bcm \
  INSTALL_LOCATION=%{buildroot}/epics/bcm install 

%files
%defattr(-,root,root,-)
/epics/bcm

%clean
rm -rf $RPM_BUILD_ROOT
