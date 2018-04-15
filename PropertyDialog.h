/*************************************************************************************************\
*
* Filename:		PropertyDialog.h
* Purpose:		Define CPropertyDialog class
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


#pragma once


// CPropertyDialog dialog

class CPropertyDialog : public CDialog
{
	DECLARE_DYNAMIC(CPropertyDialog)

public:
	CPropertyDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyDialog();

// Dialog Data
	enum { IDD = IDD_PROPERTY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CPropTree	m_Tree;

public:
	CPropTreeItem* InsertStatic( LPCSTR pszName, LPCSTR pszDescription, CPropTreeItem* pRoot );

	void SetStatic( CPropTreeItem* pItem, LPCSTR pszText );
	void SetStatic( CPropTreeItem* pItem, int nNumber );

	virtual BOOL OnInitDialog();
};
