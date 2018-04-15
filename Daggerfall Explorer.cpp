/*************************************************************************************************\
*
* Filename:		CDaggerfallExplorerApp.cpp
* Purpose:		Implement CDaggerfallExplorerApp class
* Version:		0.90
* Author:		Gavin Clayton
*
* Last Updated:	29/08/2002
*
* Copyright 2002. Gavin Clayton. All Rights Reserved.
*

*
* If changes are made by you to this code, please log them below:
*
*
\*************************************************************************************************/


#include "stdafx.h"
#include "Daggerfall Explorer.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp

BEGIN_MESSAGE_MAP(CDaggerfallExplorerApp, CWinApp)
	//{{AFX_MSG_MAP(CDaggerfallExplorerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN_ARENA2, OnFileOpenArena2)
	ON_COMMAND(ID_FILE_CLOSE_ARENA2, OnFileCloseArena2)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_ARENA2, OnUpdateFileCloseArena2)
	ON_COMMAND(ID_OBJECT_PREVIOUS, OnObjectPrevious)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PREVIOUS, OnUpdateObjectPrevious)
	ON_COMMAND(ID_OBJECT_NEXT, OnObjectNext)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEXT, OnUpdateObjectNext)
	ON_COMMAND(ID_OBJECT_GOTO, OnObjectGoto)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_GOTO, OnUpdateObjectGoto)
	ON_COMMAND(ID_OBJECT_PURE_FACES, OnObjectPureFaces)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PURE_FACES, OnUpdateObjectPureFaces)
	ON_COMMAND(ID_OBJECT_WIREFRAME, OnObjectWireframe)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_WIREFRAME, OnUpdateObjectWireframe)
	ON_COMMAND(ID_OBJECT_SOLID, OnObjectSolid)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_SOLID, OnUpdateObjectSolid)
	ON_COMMAND(ID_OBJECT_TEXTURED, OnObjectTextured)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_TEXTURED, OnUpdateObjectTextured)
	ON_COMMAND(ID_BITMAP_ANIMATION, OnBitmapAnimation)
	ON_COMMAND(ID_OBJECT_FACE_NEXT_FACE, OnObjectFaceNextFace)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_FACE_NEXT_FACE, OnUpdateObjectFaceNextFace)
	ON_COMMAND(ID_OBJECT_FACE_PREVIOUS_FACE, OnObjectFacePreviousFace)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_FACE_PREVIOUS_FACE, OnUpdateObjectFacePreviousFace)
	ON_COMMAND(ID_BITMAP_ZOOM_IN, OnBitmapZoomIn)
	ON_UPDATE_COMMAND_UI(ID_BITMAP_ZOOM_IN, OnUpdateBitmapZoomIn)
	ON_COMMAND(ID_BITMAP_ZOOM_OUT, OnBitmapZoomOut)
	ON_UPDATE_COMMAND_UI(ID_BITMAP_ZOOM_OUT, OnUpdateBitmapZoomOut)
	ON_COMMAND(ID_OBJECT_FACE_GOTO_FACE, OnObjectFaceGotoFace)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_FACE_GOTO_FACE, OnUpdateObjectFaceGotoFace)
	ON_COMMAND(ID_PLAY_UVEDITOR, OnPlayUVEditor)
	ON_UPDATE_COMMAND_UI(ID_PLAY_UVEDITOR, OnUpdatePlayUVEditor)
	ON_COMMAND(ID_VIEW_ARENA_VIEW, OnViewArenaView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ARENA_VIEW, OnUpdateViewArenaView)
	ON_COMMAND(ID_VIEW_PROPERTIES_VIEW, OnViewPropertiesView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES_VIEW, OnUpdateViewPropertiesView)
	ON_COMMAND(ID_VIEW_LOG_VIEW, OnViewLogView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOG_VIEW, OnUpdateViewLogView)
	ON_COMMAND(ID_BITMAP_TOGGLE_ANIMATION, OnBitmapToggleAnimation)
	ON_UPDATE_COMMAND_UI(ID_BITMAP_TOGGLE_ANIMATION, OnUpdateBitmapToggleAnimation)
	ON_UPDATE_COMMAND_UI(ID_BITMAP_ANIMATION, OnUpdateBitmapAnimation)
	ON_COMMAND(ID_TOOLS_ALCHEMY_STATISTICS, OnToolsAlchemyStatistics)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_CAMERA, OnUpdateObjectCamera)
	ON_COMMAND(ID_TERRAIN_DESERT, OnTerrainDesert)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_DESERT, OnUpdateTerrainDesert)
	ON_COMMAND(ID_TERRAIN_MOUNTAIN, OnTerrainMountain)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_MOUNTAIN, OnUpdateTerrainMountain)
	ON_COMMAND(ID_TERRAIN_TEMPERATE, OnTerrainTemperate)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_TEMPERATE, OnUpdateTerrainTemperate)
	ON_COMMAND(ID_TERRAIN_SWAMP, OnTerrainSwamp)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_SWAMP, OnUpdateTerrainSwamp)
	ON_COMMAND(ID_TERRAIN_NORMAL, OnTerrainNormal)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_NORMAL, OnUpdateTerrainNormal)
	ON_COMMAND(ID_TERRAIN_WINTER, OnTerrainWinter)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_WINTER, OnUpdateTerrainWinter)
	ON_COMMAND(ID_TERRAIN_RAIN, OnTerrainRain)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_RAIN, OnUpdateTerrainRain)
	ON_COMMAND(ID_VIEW_OBJECTMODE, OnViewObjectmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECTMODE, OnUpdateViewObjectmode)
	ON_COMMAND(ID_VIEW_CAMERAMODE, OnViewCameramode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAMERAMODE, OnUpdateViewCameramode)
	ON_COMMAND(ID_VIEW_RESET, OnViewReset)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESET, OnUpdateViewReset)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_OBJECT_PREVIOUSBLOCK, OnObjectPreviousBlock)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PREVIOUSBLOCK, OnUpdateObjectPreviousBlock)
	ON_COMMAND(ID_OBJECT_NEXTBLOCK, OnObjectNextBlock)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEXTBLOCK, OnUpdateObjectNextBlock)
	ON_COMMAND(ID_VIEW_ALLBLOCKS, OnViewAllblocks)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALLBLOCKS, OnUpdateViewAllblocks)
	ON_COMMAND(ID_VIEW_GROUNDPLANE, OnViewGroundplane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GROUNDPLANE, OnUpdateViewGroundplane)
	ON_COMMAND(ID_VIEW_INTERIOR, OnViewInterior)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INTERIOR, OnUpdateViewInterior)
	ON_COMMAND(ID_TERRAIN_NONE, OnTerrainNone)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_NONE, OnUpdateTerrainNone)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp construction

CDaggerfallExplorerApp::CDaggerfallExplorerApp()
{
	// Initialise
	m_pMediaView = NULL;
	m_pArenaBar = NULL;
	m_pPropertiesBar = NULL;
	m_pLogDlg = NULL;
	m_pFrame = NULL;
	m_nLastLogEntry = NULL;
	m_hTexturesRootNode = NULL;
	m_hIMGRootNode = NULL;
	m_hCIFRootNode = NULL;
	m_hSkyRootNode = NULL;
	m_h3DObjectsRootNode = NULL;
	m_hBlocksRootNode = NULL;
	m_hLocaleRootNode = NULL;
	m_nCurArch3DObject = DE_START_OBJECT;
	m_nCurBlockObject = DE_START_BLOCK;
	m_nCurSubBlockObject = 0;
	m_nSubBlockObjectCount = 0;
	m_bTextureArchiveNodeBuilt = FALSE;
	m_bIMGRootNodeBuilt = FALSE;
	m_bCIFRootNodeBuilt = FALSE;
	m_bLocaleRootNodeBuilt = FALSE;
	m_bLockView = FALSE;
	m_Log.SetPrefix( "Daggerfall Explorer: " );
	m_dwMouseMode = MOUSEMODE_OBJECT;
	m_dwDrawMode = DE_START_DRAW;
	m_dwCurNodeType = m_dwCurNodeGeneric = NT_NONE;
	m_nCurNode = HSTOPERR;
	m_nCurFace = 0;
	m_nScrollX = m_nScrollY = 0;
	m_nPageWidth = m_nPageHeight = 0;
	m_bAnimateBitmaps = TRUE;
	m_bSupressAnimateBitmaps = FALSE;
	m_dwBitmapFPS = 6;
	m_nAnimTimer = 0;
	m_nKeyTimer = 0;
	m_fBitmapScale = 1.0f;
	m_nDefaultFont = ACNULL;
	m_nSelectedSprite = NULL;
	m_nAutomapSprite = NULL;
	m_nLastMouseX = m_nLastMouseY = 0;
	m_nViewActor = ACNULL;
	m_nLastDFTextureCache = NULL;
	m_dwTerrain = DE_START_TERRAIN;
	m_dwWeather = WEATHER_NORMAL;
	m_bViewInterior = FALSE;
	m_bDrawGround = TRUE;
	m_bDrawAllBlocks = TRUE;
	m_oaPropCache.NewPool( 32 );

	// Redirect Log
	m_Alchemy.m_Log.Redirect( &m_Log );
}

CDaggerfallExplorerApp::~CDaggerfallExplorerApp()
{
	// Clean up
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDaggerfallExplorerApp object

CDaggerfallExplorerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp initialization

BOOL CDaggerfallExplorerApp::InitInstance()
{
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	*/

	// Capture current directory
	char pBuffer[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, pBuffer );
	m_strStartDirectory = pBuffer;

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Gavin Clayton Applications"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	m_Log.Log( "Daggerfall Explorer is starting." );

	// Obtain a pointer to the media view
	LPCSTR pszFailAttachView = "Failed to attach view.";
	if ( !(m_pMediaView = (CChildView*)pFrame->GetMediaView()) ) {
		m_Log.Log( pszFailAttachView, E_FAIL );
		return FailInitApp();
	}

	// Obtain a pointer to the Arena2 bar
	if ( !(m_pArenaBar = (CArenaBar*)pFrame->GetArenaBar()) ) {
		m_Log.Log( pszFailAttachView, E_FAIL );
		return FailInitApp();
	}

	// Obtain a pointer to the Properties bar
	if ( !(m_pPropertiesBar = (CPropertiesBar*)pFrame->GetPropertiesBar()) ) {
		m_Log.Log( pszFailAttachView, E_FAIL );
		return FailInitApp();
	}

	// Obtain a pointer to the log bar
	if ( !(m_pLogBar = (CLogBar*)pFrame->GetLogBar()) ) {
		m_Log.Log( pszFailAttachView, E_FAIL );
		return FailInitApp();
	}

	// Obtain a pointer to the log view
	if ( !(m_pLogDlg = (CLogDialog*)pFrame->GetLogDlg()) ) {
		m_Log.Log( pszFailAttachView, E_FAIL );
		return FailInitApp();
	}

	// Store pointer to frame window
	m_pFrame = pFrame;

	// Start Alchemy
	ALCHEMY_CREATESTRUCT accs;
	ZeroMemory( &accs, sizeof(ALCHEMY_CREATESTRUCT) );
	accs.dwFlags = ACCS_ENABLETEXTUREFILTERING;
	accs.hInstance = m_hInstance;
	accs.hViewWnd = m_pMediaView->m_hWnd;
	accs.hInputWnd = m_pFrame->m_hWnd;
	accs.bWindowed = TRUE;
	accs.fNearPlaneDepth = 25.09f;
	accs.fFarPlaneDepth = 9000.0f;
	accs.colAmbientLight = D3DCOLOR_RGBA(0xff,0xff,0xff,0xff);
	accs.colRenderErase = D3DCOLOR_RGBA(0x80,0x80,0x80,0xff);
	//accs.colRenderErase = D3DCOLOR_RGBA(38,54,176,0xff);
	if ( !m_Alchemy.Create( &accs ) ) {
		return FailInitApp();
	}

	// Install all-purpose scene actor
	ACSETTINGS_ACTOR acsa;
	ZeroMemory( &acsa, sizeof(ACSETTINGS_ACTOR) );
	acsa.Object.dwSize = sizeof(ACSETTINGS_ACTOR);
	acsa.Object.Material.Ambient = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	acsa.Object.Material.Diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	acsa.nMesh = ACNULL;
	m_nViewActor = m_Alchemy.InstallObject( ACOBJECT_ACTOR, &acsa );
	if ( ACNULL == m_nViewActor ) {
		return FailInitApp();
	}

	// Create font object
	m_nDefaultFont = m_Alchemy.InstallFont( "Arial", 10, NULL );
	if ( ACNULL == m_nDefaultFont ) {
		m_Log.Log( "Failed to create default font.", E_FAIL );
		return FailInitApp();
	}

	// Draw initial view
	DrawMediaView();

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow( SW_SHOWMAXIMIZED );
	pFrame->UpdateWindow();

#ifdef _DEBUG
	// DEBUG: Open my local Arena2 path for development
	//m_DaggerTool.OpenArena2( "C:\\Data\\Common\\Storage\\Dagger\\Arena2" );
	//InitialiseViews();
#endif

	// Install bitmap timer
	if ( m_bAnimateBitmaps ) {
		m_nAnimTimer = m_pMediaView->SetTimer( DETIMER_STEPBITMAPS, (1000 / m_dwBitmapFPS), NULL );
	}

	return TRUE;
}


BOOL CDaggerfallExplorerApp::FailInitApp()
{
	// Output error to dialog on failure
	CString str;
	TLOGENTRY le;
	m_Log.GetLastErrorEntry( &le );
	str.Format( "Last Error: %s\nCheck \"%s\\Log.txt\" for more information.", le.strMsg.GetBuffer(), m_strStartDirectory.GetBuffer() );
	MessageBox( m_pFrame->m_hWnd, str, "Daggerfall Explorer Error!", MB_ICONWARNING | MB_OK );

	return FALSE;
}


int CDaggerfallExplorerApp::ExitInstance() 
{
	// Close down Alchemy
	m_Alchemy.Destroy();

	m_Log.Log( "Daggerfall Explorer has shut down." );
	TString strPath = m_strStartDirectory;
	strPath += "\\Log.txt";
	m_Log.Save( strPath );

	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp message handlers


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

static DWORD CALLBACK StreamAboutText( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb )
{
	memcpy( pbBuff, (LPVOID)dwCookie, cb );
	*pcb = cb;

	return 0;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CRichEditCtrl	m_ctrlAbout;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strWeb;
	CString m_strEmail;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strWeb(_T(""))
, m_strEmail(_T(""))
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT_RICHEDIT, m_ctrlAbout);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_WWWEDIT, m_strWeb);
	DDX_Text(pDX, IDC_EMAILEDIT, m_strEmail);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDaggerfallExplorerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Get about text
	WPARAM wRes = MAKEWPARAM( IDR_ABOUT_TEXT, 0 );
	WPARAM wType = MAKEWPARAM( RT_RCDATA, 0 );
	HGLOBAL hRes = LoadResource( NULL, FindResource( NULL, (char*)wRes, "RTF" ) );
	if ( hRes ) {
		// Stream resource bits into control
		EDITSTREAM es;
		LPVOID pBits = LockResource( hRes );
		es.dwCookie = (DWORD)pBits;
		es.pfnCallback = StreamAboutText;
		m_ctrlAbout.StreamIn( SF_RTF, es );
	}

	// Set web and email addresses
	m_strWeb = "http://m0use.net/~interkarma";
	m_strEmail = "interkarma@m0use.net";
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp message handlers


BOOL CDaggerfallExplorerApp::OnIdle(LONG lCount) 
{
	// Check for a log update
	TLOGENTRY le;
	UINT nEntry;
	CString str, strMsg, strExt;

	// Get next entry
	if ( m_nLastLogEntry )
		nEntry = m_Log.m_oaLog.GetNext( m_nLastLogEntry );
	else
		nEntry = m_Log.m_oaLog.GetRoot();

	if ( nEntry ) {
		// Push the new entries into the view
		while ( nEntry ) {
			m_nLastLogEntry = nEntry;
			m_Log.GetLogEntry( nEntry, &le );
			strMsg = le.strMsg;
			strExt = le.strExtended;
			(strExt.GetLength()) ? str.Format( "%s [%s]", strMsg, strExt ) : str = strMsg;
			m_pLogDlg->m_strLog += (str += "\r\n");
			m_pLogDlg->UpdateData( FALSE );
			nEntry = m_Log.m_oaLog.GetNext( nEntry );
		}
	}

	return CWinApp::OnIdle(lCount);
}
