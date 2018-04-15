/*************************************************************************************************\
*
* Filename:		PropertiesBar.cpp
* Purpose:		Implement CPropertiesBar class
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
#include "PropertiesBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesbar

CPropertiesBar::CPropertiesBar()
{
	// Initialise
	m_bIsCreated = FALSE;
}

CPropertiesBar::~CPropertiesBar()
{
}


BEGIN_MESSAGE_MAP(CPropertiesBar, baseCViewBar)
	//{{AFX_MSG_MAP(CPropertiesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertiesbar message handlers

int CPropertiesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Create base object
	if (baseCViewBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !m_PropertyDlg.Create( IDD_PROPERTY_DIALOG, this ) )
		return -1;

	// Set created flag
	m_bIsCreated = TRUE;
	
	return 0;
}

void CPropertiesBar::OnSize(UINT nType, int cx, int cy) 
{
	baseCViewBar::OnSize(nType, cx, cy);

	// Size child control
	if ( m_bIsCreated ) {
		CRect rct;
		GetClientRect( rct );
		m_PropertyDlg.MoveWindow( rct );
		m_PropertyDlg.m_Tree.SetWindowPos( NULL, rct.left, rct.top, rct.Width(), rct.Height(), SWP_DEFERERASE|SWP_NOZORDER );
	}
}
