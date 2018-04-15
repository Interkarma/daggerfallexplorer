/*************************************************************************************************\
*
* Filename:		ArenaBar.cpp
* Purpose:		Implement CArenaBar class
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
#include "ArenaBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArenaBar

CArenaBar::CArenaBar()
{
	// Initialise
	m_bIsCreated = FALSE;
	m_oaNodeProperties.SetLength( 100, 100 );
}

CArenaBar::~CArenaBar()
{
}


BEGIN_MESSAGE_MAP(CArenaBar, baseCViewBar)
	//{{AFX_MSG_MAP(CArenaBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//
// void Purge()
// Delete all nodes from the Arena tree
//
void CArenaBar::Purge()
{
	// Purge tree control items
	m_ctrlArenaTree.DeleteAllItems();

	// Purge node properties database
	m_oaNodeProperties.DeleteAll();
}/* Purge */


//
// HTREEITEM InsertNode( HTREEITEM hParent, LPCSTR pszText, LPARAM nt, int iImage = NI_FOLDER, int iSelectedImage = NI_FOLDEROPEN, LPCSTR pszFile = NULL )
// Adds a node to the Arena tree
// Return: Handle to new node
//
HTREEITEM CArenaBar::InsertNode( HTREEITEM hParent, LPCSTR pszText, LPARAM nt, int iImage/*=NI_FOLDER*/, int iSelectedImage/*=NI_FOLDEROPEN*/, LPCSTR pszFile/*=NULL*/ )
{
	HPOLYOBJ hNode;
	HTREEITEM hItem = NULL;
	TVINSERTSTRUCT tvi;

	// Create a properties object
	hNode = m_oaNodeProperties.New();
	if ( hNode == HSTOPERR )
		return NULL;

	// Create tree node
	tvi.hParent = hParent;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.item.pszText = (char*)pszText;
	tvi.item.lParam = hNode;
	tvi.item.iImage = iImage;
	tvi.item.iSelectedImage = iSelectedImage;
	hItem = m_ctrlArenaTree.InsertItem( &tvi );
	if ( !hItem )
	{
		m_oaNodeProperties.Delete( hNode );
		return NULL;
	}

	// Set properties
	m_oaNodeProperties[hNode].nt = nt;
	m_oaNodeProperties[hNode].hThis = hItem;
	m_oaNodeProperties[hNode].hParent = hParent;
	m_oaNodeProperties[hNode].strText = pszText;
	if ( pszFile ) m_oaNodeProperties[hNode].strFile = pszFile;

	return hItem;
}/* InsertNode */


/////////////////////////////////////////////////////////////////////////////
// CArenaBar message handlers

int CArenaBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCViewBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create list control
	if ( !m_ctrlArenaTree.Create( WS_VISIBLE|WS_CHILD|WS_EX_CLIENTEDGE|
								  TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS,
								  CRect(0,0,0,0), this, IDC_ARENA_TREE ) )
		return -1;

	// Create image list
	if ( !m_ilArenaTree.Create( IDB_ARENA_TREE_BITMAP, 16, 1, RGB(0,0,255) ) ) return FALSE;
	m_ctrlArenaTree.SetImageList( &m_ilArenaTree, TVSIL_NORMAL );

	// Set created flag
	m_bIsCreated = TRUE;

	return 0;
}

void CArenaBar::OnSize(UINT nType, int cx, int cy) 
{
	baseCViewBar::OnSize(nType, cx, cy);
	
	// Size child control
	if ( m_bIsCreated ) {
		CRect rct;
		GetClientRect( rct );
		m_ctrlArenaTree.MoveWindow( rct );
	}
}

BOOL CArenaBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// Route messages to parent application
	int idCtrl = (int)wParam;
	LPNMHDR pnmh = (LPNMHDR)lParam;
	CDaggerfallExplorerApp *pApp = (CDaggerfallExplorerApp*)::AfxGetApp();
	if ( pApp ) {
		pApp->HandleChildMessage( pnmh );
	}

	return baseCViewBar::OnNotify(wParam, lParam, pResult);
}
