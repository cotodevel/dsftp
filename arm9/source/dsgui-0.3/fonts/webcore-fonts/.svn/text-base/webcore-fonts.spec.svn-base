Name: webcore-fonts
Summary: Collection of minimum popular high quality TrueType fonts
Version: 1.3
Release: 1
License: Microsoft
Group: User Interface/X
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/build-root-%{name}
BuildArch: noarch
Packager: Avi Alkalay <avi at unix dot sh>
#Distribution:
Prefix: /usr/share/fonts
Url: http://microsoft.com/typography
Obsoletes: msfonts

%description
Collection of high quality TrueType fonts, default in any MS Windows installation. These are also the main webfonts as specified in microsoft.com/typography
The fonts:
Andale Mono, Arial, Arial Black, Comic, Courier New, Georgia, Impact, Lucida Sans, Lucida Console, Microsoft Sans Serif, Symbol, Tahoma, Times New Roman, Trebuchet, Verdana, Webdings, Wingdings


%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n webcore-fonts

%build

%install
mkdir -p $RPM_BUILD_ROOT/%{prefix}/webcore
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/webcore-fonts
mv fonts/* $RPM_BUILD_ROOT/%{prefix}/webcore
mv doc/* $RPM_BUILD_ROOT/usr/share/doc/webcore-fonts

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%{prefix}/webcore
%doc /usr/share/doc/webcore-fonts


%post
{
	ttmkfdir -d %{prefix}/webcore -o %{prefix}/webcore/fonts.scale
	umask 133
	/usr/X11R6/bin/mkfontdir %{prefix}/webcore
	/usr/sbin/chkfontpath -q -a %{prefix}/webcore
	[ -x /usr/bin/fc-cache ] && /usr/bin/fc-cache
} &> /dev/null || :
echo "See file:/usr/share/doc/webcore-fonts/index.html to get the most from this fonts"

%preun
{
	if [ "$1" = "0" ]; then
		cd %{prefix}/webcore
		rm -f fonts.dir fonts.scale fonts.cache*
	fi
} &> /dev/null || :

%postun
if [ "$1" = "0" ]; then
  /usr/sbin/chkfontpath -q -r %{prefix}/webcore
fi
[ -x /usr/bin/fc-cache ] && /usr/bin/fc-cache


%changelog
* Mon May 31 2005 Avi Alkalay <avi@unix.sh> 1.3
- Renamed to webcore-fonts
- Completely disassociated with the -style package
* Thu Dec 14 2002 Avi Alkalay <avi@unix.sh> 1.2.1
- Included screenshots for international text
- Small fixes in the documentation
* Thu Dec 10 2002 Avi Alkalay <avi@unix.sh> 1.2
- Included documentation for public release
* Thu Oct 27 2002 Avi Alkalay <avi@unix.sh> 1.1-5
- Better support for upgrades
- Support for Red Hat 8.0 with Xft
* Thu Apr 21 2002 Avi Alkalay <avi@unix.sh> 1.1
- Added screenshots
* Thu Mar 28 2002 Avi Alkalay <avi@unix.sh> 0.6
- First packaging

