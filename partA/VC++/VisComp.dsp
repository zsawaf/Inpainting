# Microsoft Developer Studio Project File - Name="scissor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=scissor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VisComp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VisComp.mak" CFG="scissor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scissor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "scissor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scissor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../Software/fltk-1.1.6" /I "../../../Software/vxl-1.3.0" /I "../../../Software/vxl-1.3.0/vcl" /I "../../../Software/vxl-1.3.0/vcl/config.win32/vc60" /I "../../../Software/vxl-1.3.0/core" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib wsock32.lib comctl32.lib fltk.lib fltkgl.lib vcl.lib vil.lib vul.lib vnl.lib png.lib tiff.lib jpeg.lib z.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmt" /nodefaultlib:"libcimt.lib" /nodefaultlib:"msvcrt.lib" /libpath:"../../../Software/fltk-1.1.6/lib" /libpath:"../../../Software/vxl-1.3.0/bin-Win32/lib/Release"

!ELSEIF  "$(CFG)" == "scissor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\bin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /GX /Z7 /Ot /Op /Ob2 /I "../../../../../fltk-1.1.6" /I "../../../../../vxl-1.3.0" /I "../../../../../vxl-1.3.0/vcl" /I "../../../../../vxl-1.3.0/vcl/config.win32/vc60" /I "../../../../../vxl-1.3.0/core" /I "../../../../../win32/core" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FAs /FR /YX /FD /o /win32 /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib wsock32.lib comctl32.lib fltkd.lib fltkgld.lib vcl.lib vil.lib vil_algo.lib vul.lib vnl.lib vnl_algo.lib vnl_io.lib netlib.lib png.lib tiff.lib jpeg.lib z.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmtd" /nodefaultlib:"libcimtd.lib" /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:"../../../../../fltk-1.1.6/lib" /libpath:"../../../../../win32/lib/Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "scissor - Win32 Release"
# Name "scissor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\gl\glutils.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_draw.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_handle.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_init.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_object.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_utils.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting_algorithm.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting_debug.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting_eval.cxx
# End Source File
# Begin Source File

SOURCE=..\src\file\load_image.cxx
# End Source File
# Begin Source File

SOURCE=..\src\main.cxx
# End Source File
# Begin Source File

SOURCE=..\src\matting\matting.cxx
# End Source File
# Begin Source File

SOURCE=..\src\matting\matting_algorithm.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\patch_db.cxx
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\psi.cxx
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\read_drawing.cxx
# End Source File
# Begin Source File

SOURCE=..\src\gl\Texture.cxx
# End Source File
# Begin Source File

SOURCE=..\src\VisCompUI.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\file_callbacks\callback_open_drawing.h
# End Source File
# Begin Source File

SOURCE=..\src\file_callbacks\callback_save_drawing.h
# End Source File
# Begin Source File

SOURCE=..\src\file_callbacks\callback_save_image.h
# End Source File
# Begin Source File

SOURCE=..\src\fltk_includes.h
# End Source File
# Begin Source File

SOURCE=..\src\gl\glutils.h
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw.h
# End Source File
# Begin Source File

SOURCE=..\src\imdraw\imdraw_object.h
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting.h
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\inpainting_eval.h
# End Source File
# Begin Source File

SOURCE=..\src\file\load_image.h
# End Source File
# Begin Source File

SOURCE=..\src\matting\matting.h
# End Source File
# Begin Source File

SOURCE=..\src\inpainting\psi.h
# End Source File
# Begin Source File

SOURCE=..\src\student_callbacks\save_alpha.h
# End Source File
# Begin Source File

SOURCE=..\src\student_callbacks\save_composite.h
# End Source File
# Begin Source File

SOURCE=..\src\student_callbacks\save_object.h
# End Source File
# Begin Source File

SOURCE=..\src\gl\Texture.h
# End Source File
# Begin Source File

SOURCE=..\src\VisCompUI.h
# End Source File
# Begin Source File

SOURCE=..\src\vxl_includes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\src\VisCompUI.fl

!IF  "$(CFG)" == "scissor - Win32 Release"

!ELSEIF  "$(CFG)" == "scissor - Win32 Debug"

# Begin Custom Build
InputPath=..\src\VisCompUI.fl

BuildCmds= \
	"..\..\..\..\..\fltk-1.1.6\fluid\fluidd" -c ..\src\VisCompUI.fl \
	move VisCompUI.cxx ..\src \
	move VisCompUI.h ..\src \
	

"..\src/VisCompUI.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\src/VisCompUI.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
