/*************************************************************************************************\
*
* Filename:		OpenArena2Dialog.cpp
* Purpose:		Implement COpenArena2Dialog class
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
#include "OpenArena2Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenArena2Dialog dialog


COpenArena2Dialog::COpenArena2Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(COpenArena2Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenArena2Dialog)
	m_strArena2Path = _T("");
	m_bOpenReadOnly = FALSE;
	//}}AFX_DATA_INIT
}


void COpenArena2Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenArena2Dialog)
	DDX_Text(pDX, IDC_ARENA2_PATH_EDIT, m_strArena2Path);
	DDX_Check(pDX, IDC_OPEN_READ_ONLY_CHECK, m_bOpenReadOnly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenArena2Dialog, CDialog)
	//{{AFX_MSG_MAP(COpenArena2Dialog)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenArena2Dialog message handlers

void COpenArena2Dialog::OnBrowseButton() 
{
	char buffer[MAX_PATH];
	LPITEMIDLIST pidl;
	BROWSEINFO binfo;
	binfo.hwndOwner = GetSafeHwnd();
	binfo.pidlRoot = NULL;
	binfo.pszDisplayName = buffer;
	binfo.lpszTitle = "Select Arena2 Folder";
	binfo.ulFlags = 0;
	binfo.lpfn = NULL;
	binfo.lParam = NULL;
	binfo.iImage = NULL;
	pidl = SHBrowseForFolder( &binfo );
	if ( !pidl )
		return;

	// Get complete path
	SHGetPathFromIDList( pidl, buffer );

	m_strArena2Path = buffer;
	UpdateData( FALSE );
}
