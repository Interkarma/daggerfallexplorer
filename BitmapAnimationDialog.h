/*************************************************************************************************\
*
* Filename:		BitmapAnimationDialog.h
* Purpose:		Define CBitmapAnimationDialog class
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


#if !defined(AFX_BITMAPANIMATIONDIALOG_H__95E00382_959D_45DA_8986_653FBDA41410__INCLUDED_)
#define AFX_BITMAPANIMATIONDIALOG_H__95E00382_959D_45DA_8986_653FBDA41410__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapAnimationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapAnimationDialog dialog

class CBitmapAnimationDialog : public CDialog
{
// Construction
public:
	CBitmapAnimationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBitmapAnimationDialog)
	enum { IDD = IDD_BITMAP_ANIMATION_DIALOG };
	BOOL	m_bAnimateBitmaps;
	DWORD	m_dwBitmapFPS;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapAnimationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBitmapAnimationDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPANIMATIONDIALOG_H__95E00382_959D_45DA_8986_653FBDA41410__INCLUDED_)
