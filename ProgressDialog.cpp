/*************************************************************************************************\
*
* Filename:		ProgressDialog.cpp
* Purpose:		Implement CProgressDialog class
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
#include "ProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog


CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
	//}}AFX_DATA_INIT
}


void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
	DDX_Control(pDX, IDC_CURRENT_PROGRESS, m_ctrlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CProgressDialog::Go( LPCSTR pszTitle, int nLower, int nUpper )
{
	// Prepare dialog
	Create( IDD_PROGRESS_DIALOG );
	m_ctrlProgress.SetRange( nLower, nUpper );
	m_ctrlProgress.SetPos( nLower );
	SetWindowText( pszTitle );

	// Centre dialog
	CRect rctDlg, rctParent, rctNew;
	GetParent()->GetWindowRect( rctParent );
	GetWindowRect( rctDlg );
	rctNew.left = (rctParent.Width() - rctDlg.Width()) / 2;
	rctNew.top = (rctParent.Height() - rctDlg.Height()) / 2;
	rctNew.right = rctNew.left + rctDlg.Width();
	rctNew.bottom = rctNew.top + rctDlg.Height();
	MoveWindow( rctNew );
	ShowWindow( SW_SHOW );
}/* Go */


void CProgressDialog::Set( int nPos )
{
	m_ctrlProgress.SetPos( nPos );
}/* Set */


/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers
