# Microsoft Developer Studio Project File - Name="DoubleCross Client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DoubleCross Client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DoubleCross Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DoubleCross Client.mak"\
 CFG="DoubleCross Client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DoubleCross Client - Win32 Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "DoubleCross Client - Win32 Debug" (based on\
 "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Client-Server/games/DoubleCross Client", GDMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\all room clients"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /W3 /O2 /I "..\..\Connection Manager" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCMT"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Connection Manager" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NOMORESIGS" /D "NOCHARACTERS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DoubleCross Client - Win32 Release"
# Name "DoubleCross Client - Win32 Debug"
# Begin Group "Connection Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\Connection Manager\cmBase.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmConMgr.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmConMgr.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmConn.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmConn.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmMsg.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmObject.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmObject.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmRingbuf.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmRingbuf.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmSocket.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmSocket.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\cmUtil.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\icmpAPI.h"
# End Source File
# Begin Source File

SOURCE="..\..\Connection Manager\ipexport.h"
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\chat.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirtyWord.cpp
# End Source File
# Begin Source File

SOURCE=".\DoubleCross Client.cpp"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\DoubleCross Client.rc"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\DoubleCross Dialog.cpp"
# End Source File
# Begin Source File

SOURCE=.\DoubleCrossClientTable.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DoubleCrossEngine.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\facemaker.cpp"
# End Source File
# Begin Source File

SOURCE=.\GIFView.cpp
# End Source File
# Begin Source File

SOURCE=".\Room Client.cpp"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\Room Dialog.cpp"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\Room Display.cpp"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SNMachineID.cpp
# End Source File
# Begin Source File

SOURCE=".\Table Display.cpp"

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tickertext.cpp
# End Source File
# End Group
# Begin Group "sciHeaders"

# PROP Default_Filter ""
# End Group
# Begin Group "sciSource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\app.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\editbox.cpp
# End Source File
# Begin Source File

SOURCE=.\framebuf.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\inifile.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\neuquant.cpp
# End Source File
# Begin Source File

SOURCE=.\object.cpp
# End Source File
# Begin Source File

SOURCE=.\Os.cpp
# End Source File
# Begin Source File

SOURCE=.\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\prf.cpp
# End Source File
# Begin Source File

SOURCE=.\quantize.cpp
# End Source File
# Begin Source File

SOURCE=.\random.cpp
# End Source File
# Begin Source File

SOURCE=.\resman.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "..\..\all room clients"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "..\..\all room clients"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\room0.cpp

!IF  "$(CFG)" == "DoubleCross Client - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "DoubleCross Client - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sciactor.cpp
# End Source File
# Begin Source File

SOURCE=.\scibtngp.cpp
# End Source File
# Begin Source File

SOURCE=.\Scibuttn.cpp
# End Source File
# Begin Source File

SOURCE=.\Scichkbx.cpp
# End Source File
# Begin Source File

SOURCE=.\scicycle.cpp
# End Source File
# Begin Source File

SOURCE=.\scidlgbx.cpp
# End Source File
# Begin Source File

SOURCE=.\scidplst.cpp
# End Source File
# Begin Source File

SOURCE=.\scidrpbx.cpp
# End Source File
# Begin Source File

SOURCE=.\scievent.cpp
# End Source File
# Begin Source File

SOURCE=.\scifeatr.cpp
# End Source File
# Begin Source File

SOURCE=.\scigame.cpp
# End Source File
# Begin Source File

SOURCE=.\scigbox.cpp
# End Source File
# Begin Source File

SOURCE=.\scilist.cpp
# End Source File
# Begin Source File

SOURCE=.\Scilstbx.cpp
# End Source File
# Begin Source File

SOURCE=.\scimotn.cpp
# End Source File
# Begin Source File

SOURCE=.\sciobj.cpp
# End Source File
# Begin Source File

SOURCE=.\sciplane.cpp
# End Source File
# Begin Source File

SOURCE=.\scipopup.cpp
# End Source File
# Begin Source File

SOURCE=.\sciprop.cpp
# End Source File
# Begin Source File

SOURCE=.\scirail.cpp
# End Source File
# Begin Source File

SOURCE=.\scirect.cpp
# End Source File
# Begin Source File

SOURCE=.\sciroom.cpp
# End Source File
# Begin Source File

SOURCE=.\sciscrip.cpp
# End Source File
# Begin Source File

SOURCE=.\sciscrol.cpp
# End Source File
# Begin Source File

SOURCE=.\scislidr.cpp
# End Source File
# Begin Source File

SOURCE=.\scisong.cpp
# End Source File
# Begin Source File

SOURCE=.\scisound.cpp
# End Source File
# Begin Source File

SOURCE=.\scitext.cpp
# End Source File
# Begin Source File

SOURCE=.\sciview.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\sounder.cpp
# End Source File
# Begin Source File

SOURCE=.\symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\tex.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\tsprite.cpp
# End Source File
# Begin Source File

SOURCE=.\unihand.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\vapp.cpp
# End Source File
# Begin Source File

SOURCE=.\vinifile.cpp
# End Source File
# Begin Source File

SOURCE=.\vpalette.cpp
# End Source File
# Begin Source File

SOURCE=.\vsound.cpp
# End Source File
# Begin Source File

SOURCE=.\vsounder.cpp
# End Source File
# Begin Source File

SOURCE=.\vtimer.cpp
# End Source File
# Begin Source File

SOURCE=.\vwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\window.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\1000.cur
# End Source File
# Begin Source File

SOURCE=.\wordgames.ico
# End Source File
# End Target
# End Project
