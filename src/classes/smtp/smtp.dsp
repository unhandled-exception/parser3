# Microsoft Developer Studio Project File - Name="smtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=smtp - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "smtp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "smtp.mak" CFG="smtp - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "smtp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "smtp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "smtp - Win32 Profile" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "smtp"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "smtp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../include" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /TP /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "smtp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "smtp___Win32_Debug"
# PROP BASE Intermediate_Dir "smtp___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "smtp___Win32_Debug"
# PROP Intermediate_Dir "smtp___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "../../include" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FD /GZ /TP /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "smtp - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "smtp___Win32_Profile"
# PROP BASE Intermediate_Dir "smtp___Win32_Profile"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "../../include" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /TP /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I "../../include" /I "\parser3project\win32\xml\glib\include" /I "\parser3project\win32\xml\glib\include\glib" /I "\parser3project\win32\xml\gdome2-x.x.x\libgdome" /I "\parser3project\win32\xml\libxml2-x.x.x\include" /I "\parser3project\win32\xml\libxslt-x.x.x" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /TP /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "smtp - Win32 Release"
# Name "smtp - Win32 Debug"
# Name "smtp - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\comms.C
# End Source File
# Begin Source File

SOURCE=.\smtp.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\smtp.h
# End Source File
# End Group
# Begin Group "ident"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ident.awk

!IF  "$(CFG)" == "smtp - Win32 Release"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32\tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=smtp >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "smtp - Win32 Debug"

# Begin Custom Build - Up-to-dating ident.C...
InputPath=..\..\ident.awk

BuildCmds= \
	SET tools=\parser3project\win32\tools \
	ECHO // do not edit. autogenerated by .dsp, see ident.awk settings > ident.C.new \
	REN ident.C ident.C.old  >nul 2>&1 \
	%tools%\cat *.h *.C | %tools%\gawk -f \parser3project\parser3\src\ident.awk -v project=smtp >> ident.C.new \
	REN ident.C.old ident.C  >nul 2>&1 \
	%tools%\diff ident.C ident.C.new  >nul 2>&1 \
	IF ERRORLEVEL 1 COPY ident.C.new ident.C  >nul 2>&1 \
	%tools%\touch 0101000001 ident.C.new \
	

"ident.C.new" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"ident.C" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "smtp - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C

!IF  "$(CFG)" == "smtp - Win32 Release"

!ELSEIF  "$(CFG)" == "smtp - Win32 Debug"

!ELSEIF  "$(CFG)" == "smtp - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ident.C.new

!IF  "$(CFG)" == "smtp - Win32 Release"

!ELSEIF  "$(CFG)" == "smtp - Win32 Debug"

!ELSEIF  "$(CFG)" == "smtp - Win32 Profile"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
