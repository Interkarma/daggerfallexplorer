/*************************************************************************************************\
*
* Filename:		AcStatDialog.h
* Purpose:		Define CAcStatDialog class
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
#include "afxcmn.h"
#include "TAlchemy.h"


// Statistic tabs
enum
{
	STATTAB_GENERAL,
	STATTAB_TEXTURES,
	STATTAB_MESHES,
	STATTAB_OBJECTS,
	STATTAB_SPRITES,
	STATTAB_FONTS,
};

// General properties
enum
{
	STATCOL_GENERAL_ITEM,
	STATCOL_GENERAL_VALUE,
};

// Texture properties
enum
{
	STATCOL_TEXTURE_ALIAS,
	STATCOL_TEXTURE_REFCOUNT,
	STATCOL_TEXTURE_SOURCE,
	STATCOL_TEXTURE_WIDTH,
	STATCOL_TEXTURE_HEIGHT,
	STATCOL_TEXTURE_DEPTH,
	STATCOL_TEXTURE_TILEX,
	STATCOL_TEXTURE_TILEY,
	STATCOL_TEXTURE_FRAMECOUNT,
	STATCOL_TEXTURE_D3DTEXTURES,
};

// Mesh properties
enum
{
	STATCOL_MESH_ALIAS,
	STATCOL_MESH_REFCOUNT,
	STATCOL_MESH_VERTCOUNT,
	STATCOL_MESH_FACECOUNT,
	STATCOL_MESH_TEXTURECOUNT,
	STATCOL_MESH_TEXTUREALIAS,
};


// CAcStatDialog dialog

class CAcStatDialog : public CDialog
{
	DECLARE_DYNAMIC(CAcStatDialog)

public:
	CAcStatDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAcStatDialog();

	void SetAlchemy( TAlchemy *pAlchemy );

// Dialog Data
	enum { IDD = IDD_ACSTAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTabCtrl m_ctrlAcStatTab;
	CListCtrl m_ctrlAcStatList;
	afx_msg void OnTcnSelchangeAcstatTab(NMHDR *pNMHDR, LRESULT *pResult);

private:
	void BuildList( int nTab );

private:
	TAlchemy		*m_pAlchemy;
	ACSTAT_GENERAL	m_acGeneral;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
