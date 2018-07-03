# Microsoft Developer Studio Project File - Name="Forsight_BASINT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Forsight_BASINT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Forsight_BASINT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Forsight_BASINT.mak" CFG="Forsight_BASINT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Forsight_BASINT - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Forsight_BASINT - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "INCLUDE INCLUDE\REG-SHMI ..\INCLUDE ..\INCLUDE\REG-SHMI" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Forsight_BASINT - Win32 Release"
# Name "Forsight_BASINT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\forsight_basint.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_cJSON.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_innercmd.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_innerfunc.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_int_main.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_inter_control.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_interpreter_shm.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_io_controller.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /WX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_io_mapping.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_misc_func.cpp
# End Source File
# Begin Source File

SOURCE=".\src\reg-shmi\forsight_peterson.cpp"

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\forsight_program_property.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\reg_manager\forsight_registers_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\forsight_xml_reader.cpp

!IF  "$(CFG)" == "Forsight_BASINT - Win32 Release"

!ELSEIF  "$(CFG)" == "Forsight_BASINT - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\reg_manager\reg_manager_interface_wrapper.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\error_monitor.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_auto_lock.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_basint.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_cJSON.h
# End Source File
# Begin Source File

SOURCE=".\include\reg-shmi\forsight_err_shmi.h"
# End Source File
# Begin Source File

SOURCE=.\include\forsight_eval_type.h
# End Source File
# Begin Source File

SOURCE=".\include\reg-shmi\forsight_fst_datatype.h"
# End Source File
# Begin Source File

SOURCE=.\include\forsight_innercmd.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_innerfunc.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_inter_control.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_interpreter_shm.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_io_controller.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_io_mapping.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_manual_lock.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_misc_func.h
# End Source File
# Begin Source File

SOURCE=".\include\reg-shmi\forsight_peterson.h"
# End Source File
# Begin Source File

SOURCE=.\include\forsight_program_property.h
# End Source File
# Begin Source File

SOURCE=".\include\reg-shmi\forsight_registers.h"
# End Source File
# Begin Source File

SOURCE=.\include\reg_manager\forsight_registers_manager.h
# End Source File
# Begin Source File

SOURCE=.\include\forsight_xml_reader.h
# End Source File
# Begin Source File

SOURCE=.\include\fst_datatype.h
# End Source File
# Begin Source File

SOURCE=.\include\interpreter_common.h
# End Source File
# Begin Source File

SOURCE=.\include\reg_manager\reg_manager_interface_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
