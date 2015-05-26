# Microsoft Developer Studio Project File - Name="Krnlnex_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Krnlnex_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Krnlnex_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Krnlnex_static.mak" CFG="Krnlnex_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Krnlnex_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Krnlnex_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Krnlnex_static - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "__E_STATIC_LIB" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"krnlnex_static.lib"
# Begin Special Build Tool
TargetPath=.\krnlnex_static.lib
SOURCE="$(InputPath)"
PostBuild_Cmds="C:\Program Files\e\sdk\tools\resym.exe" all infile="$(TargetPath)" outfile="$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Krnlnex_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Krnlnex_static___Win32_Debug"
# PROP BASE Intermediate_Dir "Krnlnex_static___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Krnlnex_static___Win32_Debug"
# PROP Intermediate_Dir "Krnlnex_static___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "__E_STATIC_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"krnlnex_static.lib"
# Begin Special Build Tool
TargetPath=.\krnlnex_static.lib
SOURCE="$(InputPath)"
PostBuild_Cmds="C:\Program Files\e\sdk\tools\resym.exe" all infile="$(TargetPath)" outfile="$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Krnlnex_static - Win32 Release"
# Name "Krnlnex_static - Win32 Debug"
# Begin Group "elib_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ELIB_H\CMEM.CPP
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\cmem.h
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\ELIB.CPP
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\elib.h
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\FNSHARE.CPP
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\fnshare.h
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\krnllib.h
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\lang.h
# End Source File
# Begin Source File

SOURCE=.\ELIB_H\lib2.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Global_functions.hpp
# End Source File
# Begin Source File

SOURCE=.\share.hpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Krnlnex.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Krnlnex.def
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# End Target
# End Project
