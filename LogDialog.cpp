/*************************************************************************************************\
*
* Filename:		LogDialog.cpp
* Purpose:		Implement CLogDialog.cpp class
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
#include "LogDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogDialog dialog


CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogDialog)
	m_strLog = _T("");
	//}}AFX_DATA_INIT
}


void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogDialog)
	DDX_Control(pDX, IDC_LOG_RICHEDIT, m_ctrlLogEdit);
	DDX_Text(pDX, IDC_LOG_RICHEDIT, m_strLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	//{{AFX_MSG_MAP(CLogDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogDialog message handlers
