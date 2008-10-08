# =========================================================================
#     This makefile was generated by
#     Bakefile 0.2.2 (http://bakefile.sourceforge.net)
#     Do not modify, all changes will be overwritten!
# =========================================================================



# -------------------------------------------------------------------------
# These are configurable options:
# -------------------------------------------------------------------------

# C++ compiler 
CXX = wpp386

# Standard flags for C++ 
CXXFLAGS = 

# Standard preprocessor flags (common for CC and CXX) 
CPPFLAGS = 

# Standard linker flags 
LDFLAGS = 

# Use DLL build of wx library? [0,1]
#   0 - Static
#   1 - DLL
WX_SHARED = 0

# Use Unicode build of wxWidgets? [0,1]
#   0 - ANSI
#   1 - Unicode
WX_UNICODE = 0

# Use debug build of wxWidgets (define __WXDEBUG__)? [0,1]
#   0 - Release
#   1 - Debug
WX_DEBUG = 1

# Use monolithic build of wxWidgets? [0,1]
#   0 - Multilib
#   1 - Monolithic
WX_MONOLITHIC = 0

# The directory where wxWidgets library is installed 
WX_DIR = $(%WXWIN)



# -------------------------------------------------------------------------
# Do not modify the rest of this file!
# -------------------------------------------------------------------------

# Speed up compilation a bit:
!ifdef __LOADDLL__
!  loaddll wcc      wccd
!  loaddll wccaxp   wccdaxp
!  loaddll wcc386   wccd386
!  loaddll wpp      wppdi86
!  loaddll wppaxp   wppdaxp
!  loaddll wpp386   wppd386
!  loaddll wlink    wlink
!  loaddll wlib     wlibd
!endif

# We need these variables in some bakefile-made rules:
WATCOM_CWD = $+ $(%cdrive):$(%cwd) $-

### Conditionally set variables: ###

WX3RDPARTYLIBPOSTFIX =
!ifeq WX_DEBUG 1
WX3RDPARTYLIBPOSTFIX = d
!endif
_BUILDDIR_SHARED_SUFFIX =
!ifeq WX_SHARED 0
_BUILDDIR_SHARED_SUFFIX = 
!endif
!ifeq WX_SHARED 1
_BUILDDIR_SHARED_SUFFIX = _dll
!endif
__propgrid_lib___depname =
!ifeq WX_SHARED 0
__propgrid_lib___depname = &
	..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib
!endif
__propgrid_dll___depname =
!ifeq WX_SHARED 1
__propgrid_dll___depname = &
	..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.dll
!endif
__WXLIB_XML_NAME_p =
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_XML_NAME_p = wxbase26_xml.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_XML_NAME_p = wxbase26u_xml.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_XML_NAME_p = wxbase26d_xml.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_XML_NAME_p = wxbase26ud_xml.lib
!endif
!endif
!endif
__WXLIB_XRC_NAME_p =
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_XRC_NAME_p = wxmsw26_xrc.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_XRC_NAME_p = wxmsw26u_xrc.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_XRC_NAME_p = wxmsw26d_xrc.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_XRC_NAME_p = wxmsw26ud_xrc.lib
!endif
!endif
!endif
____propgrid =
!ifeq WX_DEBUG 0
____propgrid = -ot -ox
!endif
!ifeq WX_DEBUG 1
____propgrid = -od
!endif
____propgrid_2 =
!ifeq WX_DEBUG 0
____propgrid_2 = -d0
!endif
!ifeq WX_DEBUG 1
____propgrid_2 = -d2
!endif
____propgrid_3 =
!ifeq WX_DEBUG 0
____propgrid_3 = 
!endif
!ifeq WX_DEBUG 1
____propgrid_3 = debug all
!endif
____propgrid_5 =
!ifeq WX_SHARED 0
____propgrid_5 = lib
!endif
!ifeq WX_SHARED 1
____propgrid_5 = dll
!endif
__WXLIB_ADV_NAME_p =
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_ADV_NAME_p = wxmsw26_adv.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_ADV_NAME_p = wxmsw26u_adv.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_ADV_NAME_p = wxmsw26d_adv.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_ADV_NAME_p = wxmsw26ud_adv.lib
!endif
!endif
!endif
__WXLIB_CORE_NAME_p =
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_CORE_NAME_p = wxmsw26_core.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_CORE_NAME_p = wxmsw26u_core.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_CORE_NAME_p = wxmsw26d_core.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_CORE_NAME_p = wxmsw26ud_core.lib
!endif
!endif
!endif
__WXLIB_BASE_NAME_p =
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_BASE_NAME_p = wxbase26.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_BASE_NAME_p = wxbase26u.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 1
!ifeq WX_UNICODE 0
__WXLIB_BASE_NAME_p = wxmsw26.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 0
!ifeq WX_MONOLITHIC 1
!ifeq WX_UNICODE 1
__WXLIB_BASE_NAME_p = wxmsw26u.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 0
__WXLIB_BASE_NAME_p = wxbase26d.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 0
!ifeq WX_UNICODE 1
__WXLIB_BASE_NAME_p = wxbase26ud.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 1
!ifeq WX_UNICODE 0
__WXLIB_BASE_NAME_p = wxmsw26d.lib
!endif
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_MONOLITHIC 1
!ifeq WX_UNICODE 1
__WXLIB_BASE_NAME_p = wxmsw26ud.lib
!endif
!endif
!endif
____WX_SHARED =
!ifeq WX_SHARED 0
____WX_SHARED = 
!endif
!ifeq WX_SHARED 1
____WX_SHARED = -dWXUSINGDLL
!endif
__WXUNICODE_DEFINE_p =
!ifeq WX_UNICODE 1
__WXUNICODE_DEFINE_p = -d_UNICODE
!endif
__WXDEBUG_DEFINE_p =
!ifeq WX_DEBUG 1
__WXDEBUG_DEFINE_p = -d__WXDEBUG__
!endif
WXLIBPOSTFIX =
!ifeq WX_DEBUG 0
!ifeq WX_UNICODE 1
WXLIBPOSTFIX = u
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_UNICODE 0
WXLIBPOSTFIX = d
!endif
!endif
!ifeq WX_DEBUG 1
!ifeq WX_UNICODE 1
WXLIBPOSTFIX = ud
!endif
!endif
WXLIBPATH =
!ifeq WX_SHARED 0
WXLIBPATH = \lib\wat_lib
!endif
!ifeq WX_SHARED 1
WXLIBPATH = \lib\wat_dll
!endif

### Variables: ###

PROPGRID_LIB_CXXFLAGS = $(____WX_SHARED) $(__WXUNICODE_DEFINE_p) &
	$(__WXDEBUG_DEFINE_p) -d__WXMSW__ &
	-i=$(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX) -i=$(WX_DIR)\include &
	$(____propgrid) $(____propgrid_2) -wx -i=..\include $(CPPFLAGS) $(CXXFLAGS)
PROPGRID_LIB_OBJECTS =  &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_advprops.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_editors.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_extras.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_manager.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_odcombo.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_propgrid.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_props.obj
PROPGRID_DLL_CXXFLAGS = -bd $(____WX_SHARED) $(__WXUNICODE_DEFINE_p) &
	$(__WXDEBUG_DEFINE_p) -d__WXMSW__ &
	-i=$(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX) -i=$(WX_DIR)\include &
	$(____propgrid) $(____propgrid_2) -wx -i=..\include -dWXMAKINGDLL_PROPGRID &
	$(CPPFLAGS) $(CXXFLAGS)
PROPGRID_DLL_OBJECTS =  &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_advprops.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_editors.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_extras.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_manager.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_odcombo.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_propgrid.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_props.obj
PROPGRIDSAMPLE_CXXFLAGS = $(____WX_SHARED) $(__WXUNICODE_DEFINE_p) &
	$(__WXDEBUG_DEFINE_p) -d__WXMSW__ &
	-i=$(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX) -i=$(WX_DIR)\include &
	$(____propgrid) $(____propgrid_2) -wx -i=..\include $(CPPFLAGS) $(CXXFLAGS)
PROPGRIDSAMPLE_OBJECTS =  &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_propgridsample.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_sampleprops.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_tests.obj &
	watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_xh_propgrid.obj


all : watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)
watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX) :
	-if not exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX) mkdir watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)

### Targets: ###

all : .SYMBOLIC test_for_selected_wxbuild $(__propgrid_lib___depname) $(__propgrid_dll___depname) ..\samples\propgridsample.exe

clean : .SYMBOLIC 
	-if exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.obj del watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.obj
	-if exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.res del watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.res
	-if exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.lbc del watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.lbc
	-if exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.ilk del watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.ilk
	-if exist watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.pch del watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\*.pch
	-if exist ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib del ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib
	-if exist ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.dll del ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.dll
	-if exist ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib del ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib
	-if exist ..\samples\propgridsample.exe del ..\samples\propgridsample.exe

test_for_selected_wxbuild :  
	@if not exist $(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX)\wx\setup.h \
	echo ----------------------------------------------------------------------------
	@if not exist $(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX)\wx\setup.h \
	echo The selected wxWidgets build is not available!
	@if not exist $(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX)\wx\setup.h \
	echo Please use the options prefixed with WX_ to select another wxWidgets build.
	@if not exist $(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX)\wx\setup.h \
	echo ----------------------------------------------------------------------------
	@if not exist $(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX)\wx\setup.h \
	exit 1

!ifeq WX_SHARED 0
..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib :  make_dir_propgrid_lib  $(PROPGRID_LIB_OBJECTS)
	@%create watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib.lbc
	@for %i in ($(PROPGRID_LIB_OBJECTS)) do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib.lbc +%i
	wlib -q -p4096 -n -b $^@ @watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib.lbc
!endif

make_dir_propgrid_lib :  
	if not exist ..\lib\wat_$(____propgrid_5) mkdir ..\lib\wat_$(____propgrid_5)

!ifeq WX_SHARED 1
..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.dll :  make_dir_propgrid_dll  $(PROPGRID_DLL_OBJECTS)
	@%create watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc option quiet
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc name $^@
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc option caseexact
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc $(LDFLAGS) libpath $(WX_DIR)$(WXLIBPATH) $(____propgrid_3) libpath ..$(WXLIBPATH)
	@for %i in ($(PROPGRID_DLL_OBJECTS)) do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc file %i
	@for %i in ( $(__WXLIB_ADV_NAME_p) $(__WXLIB_CORE_NAME_p) $(__WXLIB_BASE_NAME_p) wxtiff$(WX3RDPARTYLIBPOSTFIX).lib wxjpeg$(WX3RDPARTYLIBPOSTFIX).lib wxpng$(WX3RDPARTYLIBPOSTFIX).lib wxzlib$(WX3RDPARTYLIBPOSTFIX).lib wxregex$(WXLIBPOSTFIX).lib wxexpat$(WX3RDPARTYLIBPOSTFIX).lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib) do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc library %i
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc system nt_dll
	wlink @watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll.lbc
	wlib -q -n -b ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib +$^@
!endif

make_dir_propgrid_dll :  
	if not exist ..\lib\wat_$(____propgrid_5) mkdir ..\lib\wat_$(____propgrid_5)

..\samples\propgridsample.exe :  $(PROPGRIDSAMPLE_OBJECTS) make_sample_dir_propgridsample watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_sample.res $(__propgrid_lib___depname)
	@%create watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc option quiet
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc name $^@
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc option caseexact
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc $(LDFLAGS) libpath $(WX_DIR)$(WXLIBPATH) $(____propgrid_3) libpath ..$(WXLIBPATH) system nt_win ref '_WinMain@16'
	@for %i in ($(PROPGRIDSAMPLE_OBJECTS)) do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc file %i
	@for %i in ( ..\lib\wat_$(____propgrid_5)\wxcode_msw26$(WXLIBPOSTFIX)_propgrid.lib $(__WXLIB_XML_NAME_p) $(__WXLIB_XRC_NAME_p) $(__WXLIB_ADV_NAME_p) $(__WXLIB_CORE_NAME_p) $(__WXLIB_BASE_NAME_p) wxtiff$(WX3RDPARTYLIBPOSTFIX).lib wxjpeg$(WX3RDPARTYLIBPOSTFIX).lib wxpng$(WX3RDPARTYLIBPOSTFIX).lib wxzlib$(WX3RDPARTYLIBPOSTFIX).lib wxregex$(WXLIBPOSTFIX).lib wxexpat$(WX3RDPARTYLIBPOSTFIX).lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib) do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc library %i
	@%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc option resource=watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_sample.res
	@for %i in () do @%append watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc option stack=%i
	wlink @watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample.lbc

make_sample_dir_propgridsample :  
	if not exist ..\samples mkdir ..\samples

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_advprops.obj :  .AUTODEPEND ..\src\advprops.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_editors.obj :  .AUTODEPEND ..\src\editors.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_extras.obj :  .AUTODEPEND ..\src\extras.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_manager.obj :  .AUTODEPEND ..\src\manager.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_odcombo.obj :  .AUTODEPEND ..\src\odcombo.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_propgrid.obj :  .AUTODEPEND ..\src\propgrid.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_lib_props.obj :  .AUTODEPEND ..\src\props.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_LIB_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_advprops.obj :  .AUTODEPEND ..\src\advprops.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_editors.obj :  .AUTODEPEND ..\src\editors.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_extras.obj :  .AUTODEPEND ..\src\extras.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_manager.obj :  .AUTODEPEND ..\src\manager.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_odcombo.obj :  .AUTODEPEND ..\src\odcombo.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_propgrid.obj :  .AUTODEPEND ..\src\propgrid.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgrid_dll_props.obj :  .AUTODEPEND ..\src\props.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRID_DLL_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_propgridsample.obj :  .AUTODEPEND ..\samples\propgridsample.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRIDSAMPLE_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_sampleprops.obj :  .AUTODEPEND ..\samples\sampleprops.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRIDSAMPLE_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_tests.obj :  .AUTODEPEND ..\samples\tests.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRIDSAMPLE_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_xh_propgrid.obj :  .AUTODEPEND ..\src\xh_propgrid.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(PROPGRIDSAMPLE_CXXFLAGS) $<

watmsw$(WXLIBPOSTFIX)$(_BUILDDIR_SHARED_SUFFIX)\propgridsample_sample.res :  .AUTODEPEND ..\samples\sample.rc
	wrc -q -ad -bt=nt -r -fo=$^@  $(____WX_SHARED) $(__WXUNICODE_DEFINE_p) $(__WXDEBUG_DEFINE_p) -d__WXMSW__ -i=$(WX_DIR)$(WXLIBPATH)\msw$(WXLIBPOSTFIX) -i=$(WX_DIR)\include -i=..\include -i=..\samples $<
