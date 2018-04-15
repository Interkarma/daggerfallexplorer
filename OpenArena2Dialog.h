/*************************************************************************************************\
*
* Filename:		OpenArena2Dialog.h
* Purpose:		Define COpenArena2Dialog class
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


#if !defined(AFX_OPENARENA2DIALOG_H__6D5F7DCE_169D_4E63_B076_D239344A68BB__INCLUDED_)
#define AFX_OPENARENA2DIALOG_H__6D5F7DCE_169D_4E63_B076_D239344A68BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenArena2Dialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenArena2Dialog dialog

class COpenArena2Dialog : public CDialog
{
// Construction
public:
	COpenArena2Dialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenArena2Dialog)
	enum { IDD = IDD_OPEN_ARENA2_DIALOG };
	CString	m_strArena2Path;
	BOOL	m_bOpenReadOnly;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenArena2Dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenArena2Dialog)
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENARENA2DIALOG_H__6D5F7DCE_169D_4E63_B076_D239344A68BB__INCLUDED_)
