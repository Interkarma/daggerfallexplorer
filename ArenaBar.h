/*************************************************************************************************\
*
* Filename:		ArenaBar.h
* Purpose:		Define CArenaBar class
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


#if !defined(AFX_ARENABAR_H__DF7841C4_FE45_49D8_A5FF_DD58F50F14B4__INCLUDED_)
#define AFX_ARENABAR_H__DF7841C4_FE45_49D8_A5FF_DD58F50F14B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArenaBar.h : header file
//

#include "TStaticArray.h"


// Node Images
enum _tagNodeImages
{
	NI_FOLDER,
	NI_FOLDEROPEN,
	NI_BITMAP,
	NI_CUBE,
	NI_SELECTEDCUBE,
	NI_BLOCKS,
	NI_SELECTEDBLOCKS,
};


// Node Properties - Properties associated with every node in the tree view
typedef struct _tagNodeProperties
{
	LPARAM		nt;			// Node type
	HTREEITEM	hThis;		// Handle to this node in the tree view
	HTREEITEM	hParent;	// Handle to parent of this node in the tree view
	CString		strText;	// Text of this node
	CString		strFile;	// The associated file (if any) with this node
} NODEPROP;


/////////////////////////////////////////////////////////////////////////////
// CArenaBar window

#ifndef baseCViewBar
#define baseCViewBar CSizingControlBarG
#endif

class CArenaBar : public baseCViewBar
{
// Construction
public:
	CArenaBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArenaBar)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArenaBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CArenaBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:		// methods
	void Purge();
	HTREEITEM InsertNode( HTREEITEM hParent, LPCSTR pszText, LPARAM nt, int iImage = NI_FOLDER, int iSelectedImage = NI_FOLDEROPEN, LPCSTR pszFile = NULL );

public:		// objects
	CTreeCtrl	m_ctrlArenaTree;
	TStaticArray<NODEPROP> m_oaNodeProperties;

private:	// objects
	CImageList	m_ilArenaTree;

private:	// data
	BOOL		m_bIsCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARENABAR_H__DF7841C4_FE45_49D8_A5FF_DD58F50F14B4__INCLUDED_)
