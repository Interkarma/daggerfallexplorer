/*************************************************************************************************\
*
* Filename:		MainFrm.h
* Purpose:		Define CMainFrame class
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


#if !defined(AFX_MAINFRM_H__F5322053_8002_41AD_832C_A9594FD1745D__INCLUDED_)
#define AFX_MAINFRM_H__F5322053_8002_41AD_832C_A9594FD1745D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "ArenaBar.h"
#include "PropertiesBar.h"
#include "LogBar.h"
#include "resource.h"


// View IDs
#define VIEWID_ARENA		0x80
#define VIEWID_PROPERTIES	0x81
#define VIEWID_LOG			0x82


class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CChildView		m_wndMediaView;

	CArenaBar		m_wndArenaBar;
	CPropertiesBar	m_wndPropertiesBar;
	CLogBar			m_wndLogBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:		// methods
	CWnd*	GetMediaView() {return (CWnd*)&m_wndMediaView;}
	CWnd*	GetArenaBar() {return (CWnd*)&m_wndArenaBar;}
	CWnd*	GetLogBar() {return (CWnd*)&m_wndLogBar;}
	CWnd*	GetPropertiesBar() {return (CWnd*)&m_wndPropertiesBar;}
	CWnd*	GetLogDlg() {return (CWnd*)m_wndLogBar.GetWindow(GW_CHILD);}

private:	// methods
	int VerifyBarState( LPCTSTR lpszProfileName );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F5322053_8002_41AD_832C_A9594FD1745D__INCLUDED_)
