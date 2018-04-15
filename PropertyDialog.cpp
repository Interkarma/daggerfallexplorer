/*************************************************************************************************\
*
* Filename:		PropertyDialog.cpp
* Purpose:		Implement CPropertyDialog class
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
#include "PropertyDialog.h"

#define IDC_PROPERTYTREE			100


// CPropertyDialog dialog

IMPLEMENT_DYNAMIC(CPropertyDialog, CDialog)
CPropertyDialog::CPropertyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyDialog::IDD, pParent)
{
}

CPropertyDialog::~CPropertyDialog()
{
}

void CPropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyDialog, CDialog)
END_MESSAGE_MAP()


// CPropertyDialog message handlers

BOOL CPropertyDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rc;
	DWORD dwStyle;

	// PTS_NOTIFY - CPropTree will send notification messages to the parent window
	dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

	// Create CPropTree control
	GetClientRect(rc);
	if ( !m_Tree.Create( dwStyle, rc, this, IDC_PROPERTYTREE ) )
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CPropTreeItem* CPropertyDialog::InsertStatic( LPCSTR pszName, LPCSTR pszDescription, CPropTreeItem* pRoot )
{
	_ASSERT( pszName );
	_ASSERT( pszDescription );
	_ASSERT( pRoot );

	// Create a simple item
	CPropTreeItem* pItem;
	pItem = m_Tree.InsertItem( new CPropTreeItemStatic(), pRoot );
	pItem->SetLabelText( _T(pszName) );
	pItem->SetInfoText( _T(pszDescription) );

	return pItem;
}

void CPropertyDialog::SetStatic( CPropTreeItem* pItem, LPCSTR pszText )
{
	_ASSERT( pItem );
	_ASSERT( pszText );

	// Set the item text
	pItem->SetItemValue( (LPARAM)_T(pszText) );
}

void CPropertyDialog::SetStatic( CPropTreeItem* pItem, int nNumber )
{
	_ASSERT( pItem );

	// Set the item number
	char sz[64];
	sprintf( sz, "%d", nNumber );
	pItem->SetItemValue( (LPARAM)_T(sz) );
}