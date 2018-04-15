/*************************************************************************************************\
*
* Filename:		PropertiesBar.h
* Purpose:		Define CPropertiesBar class
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


#if !defined(AFX_PROPERTIESBAR_H__D4417BAC_11AB_48B8_B358_5255E1E64C22__INCLUDED_)
#define AFX_PROPERTIESBAR_H__D4417BAC_11AB_48B8_B358_5255E1E64C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesBar.h : header file
//

#include "PropertyDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertiesBar window

#ifndef baseCViewBar
#define baseCViewBar CSizingControlBarG
#endif

class CPropertiesBar : public baseCViewBar
{
// Construction
public:
	CPropertiesBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertiesBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:		// objects
	CPropertyDialog m_PropertyDlg;

private:	// data
	BOOL		m_bIsCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESBAR_H__D4417BAC_11AB_48B8_B358_5255E1E64C22__INCLUDED_)
