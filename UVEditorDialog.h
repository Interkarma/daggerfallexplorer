/*************************************************************************************************\
*
* Filename:		UVEditorDialog.h
* Purpose:		Define CUVEditorDialog class
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


#if !defined(AFX_UVEDITORDIALOG_H__7BAAAA7F_A119_4CF7_8EFA_8C5E4E32F912__INCLUDED_)
#define AFX_UVEDITORDIALOG_H__7BAAAA7F_A119_4CF7_8EFA_8C5E4E32F912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UVEditorDialog.h : header file
//

#include "TStaticArray.h"


// Define a UV pair
typedef struct _tagUVEDIT_UVPAIR
{
	short u, v;
} UVEDIT_UVPAIR, *LPUVEDIT_UVPAIR;


/////////////////////////////////////////////////////////////////////////////
// CUVEditorDialog dialog

class CUVEditorDialog : public CDialog
{
// Construction
public:
	CUVEditorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUVEditorDialog)
	enum { IDD = IDD_UV_EDITOR_DIALOG };
	CListCtrl	m_ctrlUVList;
	short	m_nCurU;
	short	m_nCurV;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUVEditorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUVEditorDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnApplyButton();
	afx_msg void OnItemChangedUVList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ResolveUV();

public:
	TStaticArray<UVEDIT_UVPAIR> m_oaUV;

private:
	int m_nItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UVEDITORDIALOG_H__7BAAAA7F_A119_4CF7_8EFA_8C5E4E32F912__INCLUDED_)
