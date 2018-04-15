; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDaggerfallExplorerApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "daggerfall explorer.h"
LastPage=0

ClassCount=13
Class1=CArenaBar
Class2=CBitmapAnimationDialog
Class3=CChildView
Class4=CDaggerfallExplorerApp
Class5=CAboutDlg
Class6=CGotoDialog
Class7=CLogBar
Class8=CLogDialog
Class9=CMainFrame
Class10=CProgressDialog
Class11=CPropertiesBar
Class12=CUVEditorDialog

ResourceCount=8
Resource1=IDR_MAINFRAME
Resource2=IDD_BITMAP_ANIMATION_DIALOG
Resource3=IDD_UV_EDITOR_DIALOG
Resource4=IDD_GOTO_DIALOG
Resource5=IDD_ABOUTBOX
Resource6=IDD_LOG_DIALOG
Resource7=IDD_PROGRESS_DIALOG
Class13=COpenArena2Dialog
Resource8=IDD_OPEN_ARENA2_DIALOG

[CLS:CArenaBar]
Type=0
BaseClass=baseCViewBar
HeaderFile=ArenaBar.h
ImplementationFile=ArenaBar.cpp

[CLS:CBitmapAnimationDialog]
Type=0
BaseClass=CDialog
HeaderFile=BitmapAnimationDialog.h
ImplementationFile=BitmapAnimationDialog.cpp

[CLS:CChildView]
Type=0
BaseClass=CWnd
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp

[CLS:CDaggerfallExplorerApp]
Type=0
BaseClass=CWinApp
HeaderFile=Daggerfall Explorer.h
ImplementationFile=Daggerfall Explorer.cpp
LastObject=ID_BITMAP_ANIMATION
Filter=N
VirtualFilter=AC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Daggerfall Explorer.cpp
ImplementationFile=Daggerfall Explorer.cpp
LastObject=CAboutDlg
Filter=D
VirtualFilter=dWC

[CLS:CGotoDialog]
Type=0
BaseClass=CDialog
HeaderFile=GotoDialog.h
ImplementationFile=GotoDialog.cpp

[CLS:CLogBar]
Type=0
BaseClass=baseCViewBar
HeaderFile=LogBar.h
ImplementationFile=LogBar.cpp

[CLS:CLogDialog]
Type=0
BaseClass=CDialog
HeaderFile=LogDialog.h
ImplementationFile=LogDialog.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CProgressDialog]
Type=0
BaseClass=CDialog
HeaderFile=ProgressDialog.h
ImplementationFile=ProgressDialog.cpp

[CLS:CPropertiesBar]
Type=0
BaseClass=baseCViewBar
HeaderFile=PropertiesBar.h
ImplementationFile=PropertiesBar.cpp

[CLS:CUVEditorDialog]
Type=0
BaseClass=CDialog
HeaderFile=UVEditorDialog.h
ImplementationFile=UVEditorDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_UV_LIST

[DLG:IDD_BITMAP_ANIMATION_DIALOG]
Type=1
Class=CBitmapAnimationDialog
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ENABLE_ANIMATION_CHECK,button,1342242819
Control4=IDC_FPS_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_ABOUT_RICHEDIT,RICHEDIT,1352730692

[DLG:IDD_GOTO_DIALOG]
Type=1
Class=CGotoDialog
ControlCount=3
Control1=IDC_GOTO_EDIT,edit,1350639744
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816

[DLG:IDD_LOG_DIALOG]
Type=1
Class=CLogDialog
ControlCount=1
Control1=IDC_LOG_RICHEDIT,RICHEDIT,1353783492

[DLG:IDD_PROGRESS_DIALOG]
Type=1
Class=CProgressDialog
ControlCount=1
Control1=IDC_CURRENT_PROGRESS,msctls_progress32,1350565889

[DLG:IDD_UV_EDITOR_DIALOG]
Type=1
Class=CUVEditorDialog
ControlCount=9
Control1=IDC_STATIC,static,1342308352
Control2=IDC_UV_LIST,SysListView32,1350664205
Control3=IDC_STATIC,static,1342308352
Control4=IDC_U_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_V_EDIT,edit,1350631552
Control7=IDC_APPLY_BUTTON,button,1342242816
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN_ARENA2
Command2=ID_BITMAP_ZOOM_IN
Command3=ID_BITMAP_ZOOM_OUT
Command4=ID_BITMAP_TOGGLE_ANIMATION
Command5=ID_OBJECT_PREVIOUS
Command6=ID_OBJECT_NEXT
Command7=ID_OBJECT_GOTO
Command8=ID_OBJECT_INSIDE
Command9=ID_OBJECT_OUTSIDE
Command10=ID_OBJECT_ROTATE
Command11=ID_OBJECT_PAN
Command12=ID_OBJECT_ZOOM
Command13=ID_OBJECT_CAMERA
Command14=ID_OBJECT_PURE_FACES
Command15=ID_OBJECT_WIREFRAME
Command16=ID_OBJECT_SOLID
Command17=ID_OBJECT_TEXTURED
Command18=ID_VIEW_LIGHTING
Command19=ID_VIEW_FILTERING
Command20=ID_APP_ABOUT
CommandCount=20

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN_ARENA2
Command2=ID_FILE_CLOSE_ARENA2
Command3=ID_APP_EXIT
Command4=ID_VIEW_TOOLBAR
Command5=ID_VIEW_STATUS_BAR
Command6=ID_VIEW_ARENA_VIEW
Command7=ID_VIEW_PROPERTIES_VIEW
Command8=ID_VIEW_LOG_VIEW
Command9=ID_VIEW_LIGHTING
Command10=ID_VIEW_FILTERING
Command11=ID_VIEW_COLOURS
Command12=ID_BITMAP_ZOOM_IN
Command13=ID_BITMAP_ZOOM_OUT
Command14=ID_BITMAP_ANIMATION
Command15=ID_OBJECT_PREVIOUS
Command16=ID_OBJECT_NEXT
Command17=ID_OBJECT_GOTO
Command18=ID_OBJECT_FACE_NEXT_FACE
Command19=ID_OBJECT_FACE_PREVIOUS_FACE
Command20=ID_OBJECT_FACE_GOTO_FACE
Command21=ID_OBJECT_ROTATE
Command22=ID_OBJECT_PAN
Command23=ID_OBJECT_ZOOM
Command24=ID_OBJECT_CAMERA
Command25=ID_OBJECT_RESET
Command26=ID_OBJECT_PURE_FACES
Command27=ID_OBJECT_WIREFRAME
Command28=ID_OBJECT_SOLID
Command29=ID_OBJECT_TEXTURED
Command30=ID_OBJECT_OUTSIDE
Command31=ID_OBJECT_INSIDE
Command32=ID_PLAY_UVEDITOR
Command33=ID_PLAY_EXPORT_FMF
Command34=ID_APP_ABOUT
CommandCount=34

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_OBJECT_FACE_PREVIOUS_FACE
Command2=ID_OBJECT_FACE_NEXT_FACE
Command3=ID_BITMAP_TOGGLE_ANIMATION
Command4=ID_EDIT_COPY
Command5=ID_PLAY_UVEDITOR
Command6=ID_OBJECT_FACE_GOTO_FACE
Command7=ID_OBJECT_GOTO
Command8=ID_FILE_OPEN_ARENA2
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_OBJECT_PREVIOUS
Command17=ID_OBJECT_NEXT
Command18=ID_EDIT_CUT
Command19=ID_EDIT_UNDO
CommandCount=19

[DLG:IDD_OPEN_ARENA2_DIALOG]
Type=1
Class=COpenArena2Dialog
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_ARENA2_PATH_EDIT,edit,1350631552
Control3=IDC_BROWSE_BUTTON,button,1342242816
Control4=IDC_OPEN_READ_ONLY_CHECK,button,1342242819
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[CLS:COpenArena2Dialog]
Type=0
HeaderFile=OpenArena2Dialog.h
ImplementationFile=OpenArena2Dialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COpenArena2Dialog

