/*************************************************************************************************\
*
* Filename:		AcStatDialog.cpp
* Purpose:		Implement CAcStatDialog class
* Version:		1.0
* Author:		Gavin Clayton
*
* Last Updated:	29/08/2002
*
* Copyright 2002. Gavin Clayton. All Rights Reserved.
*
* If changes are made by you to this code, please log them below:
*
*
\*************************************************************************************************/


#include "stdafx.h"
#include "Daggerfall Explorer.h"
#include "AcStatDialog.h"


// CAcStatDialog dialog

IMPLEMENT_DYNAMIC(CAcStatDialog, CDialog)
CAcStatDialog::CAcStatDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAcStatDialog::IDD, pParent)
{
	m_pAlchemy = NULL;
}

CAcStatDialog::~CAcStatDialog()
{
	m_pAlchemy = NULL;
}

void CAcStatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACSTAT_TAB, m_ctrlAcStatTab);
	DDX_Control(pDX, IDC_ACSTAT_LIST, m_ctrlAcStatList);
}

void CAcStatDialog::SetAlchemy( TAlchemy *pAlchemy )
{
	// Validate
	_ASSERT( pAlchemy );

	m_pAlchemy = pAlchemy;
}

void CAcStatDialog::BuildList( int nTab )
{
	// Clear all current information
	m_ctrlAcStatList.DeleteAllItems();

	// Clear all current columns
	int nColumnCount = m_ctrlAcStatList.GetHeaderCtrl()->GetItemCount();
	for (int i=0; i < nColumnCount; i++) {
		m_ctrlAcStatList.DeleteColumn( 0 );
	}

	// Add category information
	CString str;
	int iItem = 0;
	UINT nRes = NULL;
	ACSTAT_TEXTURE acTexture;
	ACSTAT_MESH acMesh;
	switch ( nTab )
	{
	case STATTAB_GENERAL:
		// Add columns
		m_ctrlAcStatList.InsertColumn( STATCOL_GENERAL_ITEM, "Item", LVCFMT_LEFT, 145 );
		m_ctrlAcStatList.InsertColumn( STATCOL_GENERAL_VALUE, "Value", LVCFMT_LEFT, 150 );

		// Add property names
		m_ctrlAcStatList.InsertItem( iItem++, "Build Number" );
		m_ctrlAcStatList.InsertItem( iItem++, "Engine Status" );
		m_ctrlAcStatList.InsertItem( iItem++, "Vertex Pipeline" );
		m_ctrlAcStatList.InsertItem( iItem++, "Engine Time" );
		m_ctrlAcStatList.InsertItem( iItem++, "Time Integrator" );
		m_ctrlAcStatList.InsertItem( iItem++, "Average FPS" );
		m_ctrlAcStatList.InsertItem( iItem++, "Texture Count" );
		m_ctrlAcStatList.InsertItem( iItem++, "Mesh Count" );
		m_ctrlAcStatList.InsertItem( iItem++, "Object Count" );
		m_ctrlAcStatList.InsertItem( iItem++, "Sprite Count" );
		m_ctrlAcStatList.InsertItem( iItem++, "Font Count" );

		// Set "Build Number"
		iItem = 0;
		m_ctrlAcStatList.SetItemText( iItem++, 1, m_acGeneral.strBuildNumber );

		// Set "Engine Status"
		switch ( m_acGeneral.dwStatus )
		{
		case ACSTATUS_NOTCREATED:
			str = "Not Created";
			break;

		case ACSTATUS_OPERATIONAL:
			str = "Operational";
			break;

		case ACSTATUS_DEVICELOST:
			str = "Device Lost";
			break;
		}
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Vertex Pipeline"
		switch ( m_acGeneral.dwVertexPipeline )
		{
		case ACVPIPE_NONE:
			str = "None";
			break;

		case ACVPIPE_PURE:
			str = "Pure Device";
			break;

		case ACVPIPE_MIXED:
			str = "Mixed";
			break;

		case ACVPIPE_SOFTWARE:
			str = "Software";
			break;
		}
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Engine Time"
		str.Format( "%u", m_acGeneral.dwEngineTime );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Last Time Integrator"
		str.Format( "%.2f", m_acGeneral.fLastTI );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Average FPS"
		str.Format( "%.2f", m_acGeneral.fAverageFPS );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Texture Count"
		str.Format( "%u", m_acGeneral.dwTextureCount );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Mesh Count"
		str.Format( "%u", m_acGeneral.dwMeshCount );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Object Count"
		str.Format( "%u", m_acGeneral.dwObjectCount );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Sprite Count"
		str.Format( "%u", m_acGeneral.dwSpriteCount );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// Set "Font Count"
		str.Format( "%u", m_acGeneral.dwFontCount );
		m_ctrlAcStatList.SetItemText( iItem++, 1, str );

		// End General section
		break;

	case STATTAB_TEXTURES:
		// Add columns
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_ALIAS, "Alias", LVCFMT_LEFT, 140 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_REFCOUNT, "Refs", LVCFMT_LEFT, 40 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_SOURCE, "Source Alias", LVCFMT_LEFT, 140 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_WIDTH, "Width", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_HEIGHT, "Height", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_DEPTH, "Bits", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_TILEX, "TileX", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_TILEY, "TileY", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_FRAMECOUNT, "Frames", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_TEXTURE_D3DTEXTURES, "D3D Textures", LVCFMT_LEFT, 80 );

		// Enumerate all textures
		iItem = 0;
		nRes = NULL;
		m_pAlchemy->GetTextureStatistics( &nRes, &acTexture );
		while ( nRes ) {
			// Insert item "Alias"
			if ( acTexture.strAlias != "" )
				str = acTexture.strAlias;
			else
				str.Format( "%u", acTexture.nHandle );
			m_ctrlAcStatList.InsertItem( iItem, str );

			// Set item "Refs"
			str.Format( "%u", acTexture.dwRefCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_REFCOUNT, str );

			// Set item "Source Alias"
			if ( !acTexture.nSource )
				str = "N/A";
			else {
				str.Format( "%u", acTexture.nSource );
			}
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_SOURCE, str );

			// Set item "Width"
			str.Format( "%u", acTexture.dwWidth );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_WIDTH, str );

			// Set item "Height"
			str.Format( "%u", acTexture.dwHeight );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_HEIGHT, str );

			// Set item "Bits"
			str.Format( "%u", acTexture.dwBitDepth );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_DEPTH, str );

			// Set item "TileX"
			str.Format( "%u", acTexture.dwTileX );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_TILEX, str );

			// Set item "TileY"
			str.Format( "%u", acTexture.dwTileY );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_TILEY, str );

			// Set item "Frames"
			str.Format( "%u", acTexture.dwFrameCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_FRAMECOUNT, str );

			// Set item "D3D Texture"
			str.Format( "%u", acTexture.dwD3DTextures );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_TEXTURE_D3DTEXTURES, str );

			// Get next texture
			iItem++;
			m_pAlchemy->GetTextureStatistics( &nRes, &acTexture );
		}

		// End Textures section
		break;

	case STATTAB_MESHES:
		// Add columns
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_ALIAS, "Alias", LVCFMT_LEFT, 140 );
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_REFCOUNT, "Refs", LVCFMT_LEFT, 40 );
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_VERTCOUNT, "Vertices", LVCFMT_LEFT, 60 );
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_FACECOUNT, "Faces", LVCFMT_LEFT, 50 );
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_TEXTURECOUNT, "Textures", LVCFMT_LEFT, 60 );
		m_ctrlAcStatList.InsertColumn( STATCOL_MESH_TEXTUREALIAS, "Texture Alias List", LVCFMT_LEFT, 250 );

		// Enumerate all meshes
		iItem = 0;
		nRes = NULL;
		m_pAlchemy->GetMeshStatistics( &nRes, &acMesh );
		while ( nRes ) {
			// Insert item "Alias"
			if ( acMesh.strAlias != "" )
				str = acMesh.strAlias;
			else
				str.Format( "%u", acMesh.nHandle );
			m_ctrlAcStatList.InsertItem( iItem, str );

			// Set item "Refs"
			str.Format( "%u", acMesh.dwRefCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_MESH_REFCOUNT, str );

			// Set item "Vertices"
			str.Format( "%u", acMesh.dwVertCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_MESH_VERTCOUNT, str );

			// Set item "Faces"
			str.Format( "%u", acMesh.dwFaceCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_MESH_FACECOUNT, str );

			// Set item "Textures"
			str.Format( "%u", acMesh.dwTextureCount );
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_MESH_TEXTURECOUNT, str );

			// Set item "Texture Alias List"
			m_ctrlAcStatList.SetItemText( iItem, STATCOL_MESH_TEXTUREALIAS, acMesh.strTextureAlias );

			// Get next mesh
			iItem++;
			m_pAlchemy->GetMeshStatistics( &nRes, &acMesh );
		}

		// End Meshes section
		break;

	case STATTAB_OBJECTS:
		break;

	case STATTAB_SPRITES:
		break;

	case STATTAB_FONTS:
		break;
	}
}


BEGIN_MESSAGE_MAP(CAcStatDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ACSTAT_TAB, OnTcnSelchangeAcstatTab)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CAcStatDialog message handlers

BOOL CAcStatDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set extended styles
	m_ctrlAcStatList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP );

	// Assert if Alchemy not set
	_ASSERT( m_pAlchemy );

	// Get general properties
	m_pAlchemy->GetStatistics( &m_acGeneral );

	// Prepare tabs
	m_ctrlAcStatTab.InsertItem( STATTAB_GENERAL, "General" );
	m_ctrlAcStatTab.InsertItem( STATTAB_TEXTURES, "Textures" );
	m_ctrlAcStatTab.InsertItem( STATTAB_MESHES, "Meshes" );
	//m_ctrlAcStatTab.InsertItem( STATTAB_OBJECTS, "Objects" );
	//m_ctrlAcStatTab.InsertItem( STATTAB_SPRITES, "Sprites" );
	//m_ctrlAcStatTab.InsertItem( STATTAB_FONTS, "Fonts" );
	
	// Set default selection
	m_ctrlAcStatTab.SetCurSel( STATTAB_GENERAL );
	BuildList( STATTAB_GENERAL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAcStatDialog::OnTcnSelchangeAcstatTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Build list based on selection
	BuildList( m_ctrlAcStatTab.GetCurSel() );

	*pResult = 0;
}

int CAcStatDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
