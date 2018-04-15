/*************************************************************************************************\
*
* Filename:		BitmapAnimationDialog.cpp
* Purpose:		Implement CBitmapAnimationDialog class
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
#include "BitmapAnimationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapAnimationDialog dialog


CBitmapAnimationDialog::CBitmapAnimationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBitmapAnimationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBitmapAnimationDialog)
	m_bAnimateBitmaps = FALSE;
	m_dwBitmapFPS = 0;
	//}}AFX_DATA_INIT
}


void CBitmapAnimationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBitmapAnimationDialog)
	DDX_Check(pDX, IDC_ENABLE_ANIMATION_CHECK, m_bAnimateBitmaps);
	DDX_Text(pDX, IDC_FPS_EDIT, m_dwBitmapFPS);
	DDV_MinMaxDWord(pDX, m_dwBitmapFPS, 0, 30);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBitmapAnimationDialog, CDialog)
	//{{AFX_MSG_MAP(CBitmapAnimationDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapAnimationDialog message handlers
