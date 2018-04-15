/*************************************************************************************************\
*
* Filename:		LogBar.h
* Purpose:		Define CLogBar class
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


#if !defined(AFX_LOGBAR_H__C06CEEEE_07C3_494F_B724_13DA28BB685F__INCLUDED_)
#define AFX_LOGBAR_H__C06CEEEE_07C3_494F_B724_13DA28BB685F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogBar.h : header file
//

#include "LogDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CLogBar window

#ifndef baseCViewBar
#define baseCViewBar CSizingControlBarG
#endif

class CLogBar : public baseCViewBar
{
// Construction
public:
	CLogBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:		// objects
	CLogDialog	m_LogDlg;
	BOOL		m_bIsCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGBAR_H__C06CEEEE_07C3_494F_B724_13DA28BB685F__INCLUDED_)
