/*************************************************************************************************\
*
* Filename:		UVEditorDialog.cpp
* Purpose:		Implement CUVEditorDialog class
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
#include "UVEditorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUVEditorDialog dialog


CUVEditorDialog::CUVEditorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUVEditorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUVEditorDialog)
	m_nCurU = 0;
	m_nCurV = 0;
	//}}AFX_DATA_INIT
}


void CUVEditorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUVEditorDialog)
	DDX_Control(pDX, IDC_UV_LIST, m_ctrlUVList);
	DDX_Text(pDX, IDC_U_EDIT, m_nCurU);
	DDX_Text(pDX, IDC_V_EDIT, m_nCurV);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUVEditorDialog, CDialog)
	//{{AFX_MSG_MAP(CUVEditorDialog)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UV_LIST, OnItemChangedUVList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUVEditorDialog message handlers

BOOL CUVEditorDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set extended styles
	m_nItem = -1;
	m_ctrlUVList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP );
	
	// Setup the list view columns
	m_ctrlUVList.InsertColumn( 0, "Pair", LVCFMT_LEFT, 50 );
	m_ctrlUVList.InsertColumn( 1, "U", LVCFMT_LEFT, 80 );
	m_ctrlUVList.InsertColumn( 2, "V", LVCFMT_LEFT, 80 );
	m_ctrlUVList.InsertColumn( 3, "Result", LVCFMT_LEFT, 150 );

	// Add the UV pairs
	CString str;
	for ( UINT nPoint = 0; nPoint < m_oaUV.GetCount(); nPoint++ ) {
		// Set point text
		str.Format( "%d", nPoint );
		m_ctrlUVList.InsertItem( nPoint, str );

		// Set U text
		str.Format( "%d", m_oaUV[nPoint].u );
		m_ctrlUVList.SetItemText( nPoint, 1, str );

		// Set V text
		str.Format( "%d", m_oaUV[nPoint].v );
		m_ctrlUVList.SetItemText( nPoint, 2, str );
	}

	// Solve the UV points
	ResolveUV();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUVEditorDialog::OnItemChangedUVList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Load UV coordinates
	m_nItem = pNMListView->iItem;
	m_nCurU = m_oaUV[m_nItem].u;
	m_nCurV = m_oaUV[m_nItem].v;
	UpdateData( FALSE );

	*pResult = 0;
}

void CUVEditorDialog::OnApplyButton() 
{
	// Update UV coordinates
	UpdateData();
	m_oaUV[m_nItem].u = m_nCurU;
	m_oaUV[m_nItem].v = m_nCurV;

	// Show changes
	CString str;
	str.Format( "%d", m_nCurU );
	m_ctrlUVList.SetItemText( m_nItem, 1, str );
	str.Format( "%d", m_nCurV );
	m_ctrlUVList.SetItemText( m_nItem, 2, str );

	// Solve the UV points
	ResolveUV();
}

void CUVEditorDialog::ResolveUV() {
	float u0, v0, u1, v1, u2, v2, u3, v3;

	// Combine values
	u0 = (float)m_oaUV[0].u;
	v0 = (float)m_oaUV[0].v;
	u1 = (float)m_oaUV[1].u + u0;
	v1 = (float)m_oaUV[1].v + v0;
	u2 = (float)m_oaUV[2].u + u1;
	v2 = (float)m_oaUV[2].v + v1;
	if ( m_oaUV.GetCount() > 3 ) {
		u3 = (float)m_oaUV[3].u;
		v3 = (float)m_oaUV[3].v;
	}

	// Set values
	CString str;
	str.Format( "%.0f, %.0f", u0, v0 );
	m_ctrlUVList.SetItemText( 0, 3, str );
	str.Format( "%.0f, %.0f", u1, v1 );
	m_ctrlUVList.SetItemText( 1, 3, str );
	str.Format( "%.0f, %.0f", u2, v2 );
	m_ctrlUVList.SetItemText( 2, 3, str );
	if ( m_oaUV.GetCount() > 3 ) {
		str.Format( "%.0f, %.0f", u3, v3 );
		m_ctrlUVList.SetItemText( 3, 3, str );
	}
}