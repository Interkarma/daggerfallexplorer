/*************************************************************************************************\
*
* Filename:		LogBar.cpp
* Purpose:		Implement CLogBar class
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
#include "daggerfall explorer.h"
#include "LogBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogBar

CLogBar::CLogBar()
{
	// Initialise
	m_bIsCreated = FALSE;
}

CLogBar::~CLogBar()
{
}


BEGIN_MESSAGE_MAP(CLogBar, baseCViewBar)
	//{{AFX_MSG_MAP(CLogBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogBar message handlers

int CLogBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Create base object
	if (baseCViewBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !m_LogDlg.Create( IDD_LOG_DIALOG, this ) )
		return -1;

	// Set created flag
	m_bIsCreated = TRUE;
	
	return 0;
}

void CLogBar::OnSize(UINT nType, int cx, int cy) 
{
	baseCViewBar::OnSize(nType, cx, cy);
	
	// Size child control
	if ( m_bIsCreated ) {
		CRect rct;
		GetClientRect( rct );
		m_LogDlg.m_ctrlLogEdit.SetWindowPos( NULL, rct.left, rct.top, rct.Width(), rct.Height(), SWP_DEFERERASE|SWP_NOZORDER );
	}
}
