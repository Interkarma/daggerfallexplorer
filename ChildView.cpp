/*************************************************************************************************\
*
* Filename:		ChildView.cpp
* Purpose:		Implement CChildView class
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
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	// Initialise
	m_bLBtnDown = m_bRBtnDown = FALSE;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
//	ON_WM_KEYDOWN()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), 0, NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->OnDrawMediaView();
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}


void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->OnSizeMediaView( cx, cy );
	}
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp && !pScrollBar ) {
		pApp->OnScrollMediaView( SB_HORZ, nSBCode, nPos );
	}
	
	CWnd ::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp && !pScrollBar ) {
		pApp->OnScrollMediaView( SB_VERT, nSBCode, nPos );
	}
	
	CWnd ::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnTimer(UINT nIDEvent) 
{
	CDaggerfallExplorerApp *pApp = NULL;
	switch ( nIDEvent )
	{
	case DETIMER_STEPBITMAPS:
		pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
		if ( pApp ) {
			pApp->OnStepBitmaps();
		}
		break;

	case DETIMER_POLLKEYBOARD:
		pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
		if ( pApp ) {
			pApp->OnPollKeyboard();
		}
		break;
	}
	CWnd ::OnTimer(nIDEvent);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Set focus to this window
	SetFocus();

	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleMouseMessages( GetCurrentMessage(), point.x, point.y );
	}

	m_bLBtnDown = TRUE;
	SetCapture();
	
	CWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleMouseMessages( GetCurrentMessage(), point.x, point.y );
	}

	m_bLBtnDown = FALSE;
	ReleaseCapture();
	
	CWnd ::OnLButtonUp(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Set focus to this window
	SetFocus();

	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleMouseMessages( GetCurrentMessage(), point.x, point.y );
	}

	m_bRBtnDown = TRUE;
	SetCapture();
	
	CWnd ::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleMouseMessages( GetCurrentMessage(), point.x, point.y );
	}

	m_bRBtnDown = FALSE;
	ReleaseCapture();
	
	CWnd ::OnRButtonUp(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_ptLastMouse = m_ptCurMouse;
	m_ptCurMouse = point;

	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleMouseMessages( GetCurrentMessage(), point.x, point.y );
		if ( m_bLBtnDown ) {
			pApp->OnLMouseDrag( m_ptLastMouse.x - point.x, m_ptLastMouse.y - point.y );
		}
		else if ( m_bRBtnDown ) {
			pApp->OnRMouseDrag( m_ptLastMouse.x - point.x, m_ptLastMouse.y - point.y );
		}
	}// end if ( pApp )
	
	CWnd ::OnMouseMove(nFlags, point);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		//pApp->OnKeyDown( nChar, nRepCnt, nFlags );
	}// end if ( pApp )

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
