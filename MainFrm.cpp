/*************************************************************************************************\
*
* Filename:		MainFrm.cpp
* Purpose:		Implement CMainFrame class
* Version:		1.0
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
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rect;

	// Create frame window
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create a view to occupy the client area of the frame
	if (!m_wndMediaView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	// Create toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// Create status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Enable docking
	EnableDocking(CBRS_ALIGN_ANY);
#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

	// Dock toolbar
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Create the Arena bar
	if ( !m_wndArenaBar.Create( "Arena View", this, VIEWID_ARENA ) ) {
		TRACE0( "Failed to create Arena View.\n" );
		return -1;
	}

	// Dock Arena bar
	m_wndArenaBar.SetBarStyle( m_wndArenaBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC );
	m_wndArenaBar.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &m_wndArenaBar, AFX_IDW_DOCKBAR_LEFT );

	// Create the properties bar
	if ( !m_wndPropertiesBar.Create( "Properties View", this, VIEWID_PROPERTIES ) ) {
		TRACE0( "Failed to create Properties View.\n" );
		return -1;
	}

	// Dock the properties bar
	RecalcLayout();
	m_wndArenaBar.GetWindowRect( rect );
	rect.OffsetRect( 0, 1 );
	m_wndPropertiesBar.SetBarStyle( m_wndPropertiesBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC );
	m_wndPropertiesBar.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &m_wndPropertiesBar, AFX_IDW_DOCKBAR_LEFT, rect );

	// Create the log view
	if ( !m_wndLogBar.Create( "Log View", this, VIEWID_LOG ) ) {
		TRACE0( "Failed to create Log View.\n" );
		return -1;
	}

	// Dock log view
	m_wndLogBar.SetBarStyle( m_wndLogBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC );
	m_wndLogBar.SetSCBStyle( m_wndLogBar.GetSCBStyle()|SCBS_SIZECHILD );
	m_wndLogBar.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &m_wndLogBar, AFX_IDW_DOCKBAR_BOTTOM );

	CString strProfile = _T("BarState");
	if ( VerifyBarState( strProfile ) )
	{
		CSizingControlBar::GlobalLoadState( this, strProfile );
		LoadBarState( strProfile );
	}

	// Set focus to media view
	m_wndMediaView.SetFocus();

	return 0;
}


// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndMediaView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndMediaView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::DestroyWindow() 
{
	CString strProfile = _T("BarState");
	CSizingControlBar::GlobalSaveState( this, strProfile );
	SaveBarState( strProfile );
	
	return CFrameWnd::DestroyWindow();
}
