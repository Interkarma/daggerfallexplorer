/*************************************************************************************************\
*
* Filename:		DE_Tools.cpp
* Purpose:		Implement tool-related CDaggerfallExplorerApp functions
* Version:		0.90
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
#include "Daggerfall Explorer.h"
#include "BitmapAnimationDialog.h"
#include "AcStatDialog.h"
#include "GotoDialog.h"
#include "UVEditorDialog.h"
#include "OpenArena2Dialog.h"


void CDaggerfallExplorerApp::OnFileOpenArena2() 
{
	// Set profile string
	CString strSection = "Startup Values";
	CString strPath = "PreviousArena2Path";
	CString strReadOnly = "PreviousReadOnlyState";

	// Get previous settings from registry
	CString strNewPath = GetProfileString( strSection, strPath );
	BOOL bReadOnly = GetProfileInt( strSection, strReadOnly, TRUE );

	// Init open dialog
	COpenArena2Dialog dlg;
	dlg.m_strArena2Path = strNewPath;
	dlg.m_bOpenReadOnly = bReadOnly;

	// Start open dialog
	if ( dlg.DoModal() == IDOK ) {	
		// Open Arena2
		if ( !m_DaggerTool.OpenArena2( dlg.m_strArena2Path, dlg.m_bOpenReadOnly ) ) {
			MessageBox( NULL, "Failed to open Arena2 folder.\nContents may be incomplete or damaged.", "Error!", MB_ICONSTOP|MB_OK );
			return;
		}
		
		// Initialise views
		if ( !InitialiseViews() ) {
			m_DaggerTool.CloseArena2();
			InitialiseViews();
		}

		// Save data
		WriteProfileString( strSection, strPath, dlg.m_strArena2Path );
		WriteProfileInt( strSection, strReadOnly, dlg.m_bOpenReadOnly );
	}
}


void CDaggerfallExplorerApp::OnToolsAlchemyStatistics()
{
	CAcStatDialog stat;
	stat.SetAlchemy( &m_Alchemy );
	stat.DoModal();
}


void CDaggerfallExplorerApp::OnPlayUVEditor()
{
	// Initialise editor
	OBJVERTEX vert;
	CUVEditorDialog dlg;
	LPARCH3D_FACE pFace = m_DaggerTool.m_Arch3D.GetFace( m_nCurFace );
	int nPointCount = m_DaggerTool.m_Arch3D.GetPointCount( pFace );
	if ( nPointCount > 4 ) nPointCount = 4;
	for ( int nPoint = 0; nPoint < nPointCount; nPoint++ ) {
		m_DaggerTool.m_Arch3D.GetPoint( pFace, nPoint, &vert );
		dlg.m_oaUV.New(nPoint);
		dlg.m_oaUV[nPoint].u = (short)vert.tu;
		dlg.m_oaUV[nPoint].v = (short)vert.tv;
	}

	// Run editor
	short u, v;
	if ( dlg.DoModal() == IDOK ) {
		// Write points back to memory and save to disk
		for ( nPoint = 0; nPoint < nPointCount; nPoint++ ) {
			u = dlg.m_oaUV[nPoint].u;
			v = dlg.m_oaUV[nPoint].v;
			m_DaggerTool.m_Arch3D.SetPointUV( pFace, nPoint, u, v );
		}

		// Save changes
		m_DaggerTool.m_Arch3D.SaveObject();
		DrawMediaView();
		SetPropertyView();
	}
}


void CDaggerfallExplorerApp::OnBitmapAnimation() 
{
	// Determine settings for bitmap animation
	CBitmapAnimationDialog bmpDlg;
	bmpDlg.m_bAnimateBitmaps = m_bAnimateBitmaps;
	bmpDlg.m_dwBitmapFPS = m_dwBitmapFPS;
	if ( IDOK == bmpDlg.DoModal() ) {
		// Kill timer if already installed and changed
		if ( m_nAnimTimer && ((bmpDlg.m_dwBitmapFPS != m_dwBitmapFPS) || !bmpDlg.m_bAnimateBitmaps) ) {
			m_pMediaView->KillTimer( DETIMER_STEPBITMAPS );
			m_nAnimTimer = 0;
		}

		m_bAnimateBitmaps = bmpDlg.m_bAnimateBitmaps;
		m_dwBitmapFPS = bmpDlg.m_dwBitmapFPS;
		OnSelectNode( m_nCurNode );
		DrawMediaView();
	}

	// Install new timer if specified
	if ( !m_nAnimTimer && m_dwBitmapFPS && m_bAnimateBitmaps ) {
		m_nAnimTimer = m_pMediaView->SetTimer( DETIMER_STEPBITMAPS, (1000 / m_dwBitmapFPS), NULL );
	}

	// Clean up selected bitmap
	m_nSelectedSprite = NULL;
}


void CDaggerfallExplorerApp::OnObjectFaceGotoFace() 
{
	CGotoDialog dlg;
	if ( dlg.DoModal() == IDOK ) {
		if ( dlg.m_nGoto < 0 || dlg.m_nGoto > m_DaggerTool.m_Arch3D.GetFaceCount() - 1 ) {
			MessageBox( NULL, "Specified face is invalid.", "Invalid Face", MB_ICONSTOP | MB_OK );
			return;
		}
		m_nCurFace = dlg.m_nGoto;
		DrawMediaView();
		SetPropertyView();
	}
}


void CDaggerfallExplorerApp::OnObjectGoto() 
{
	CGotoDialog dlg;
	switch ( m_dwCurNodeType )
	{
	case NT_ARCH3D:
		if ( dlg.DoModal() == IDOK ) {
			if ( dlg.m_nGoto < 0 || dlg.m_nGoto > NUM_UPPER_ARCH3D_RECORD ) {
				MessageBox( NULL, "Specified object is invalid.", "Invalid Object", MB_ICONSTOP | MB_OK );
				return;
			}
			
			ResetObject();
			ResetCamera();
			m_nCurArch3DObject = dlg.m_nGoto;
			m_nCurFace = 0;
			if ( !BuildObjectView() ) {
				m_Alchemy.EnableObject( m_nViewActor, FALSE );
			}
			DrawMediaView();
			SetPropertyView();
		}
		break;

	case NT_BLOCKS:
		if ( dlg.DoModal() == IDOK ) {
			if ( dlg.m_nGoto < 0 || dlg.m_nGoto > NUM_UPPER_BLOCK_RECORD ) {
				MessageBox( NULL, "Specified object is invalid.", "Invalid Object", MB_ICONSTOP | MB_OK );
				return;
			}
			
			ResetObject();
			ResetCamera();
			m_nCurBlockObject = dlg.m_nGoto;
			BuildBlockView();
			DrawMediaView();
			BuildPropertyView();
		}
		break;
	}
}


//
// void AutoBlockTests()
// This method runs the block viewer through its paces to find errors
//
void CDaggerfallExplorerApp::AutoBlockTests()
{
	// Enforce initial test conditions
	m_bDrawGround = m_bDrawAllBlocks = TRUE;
	m_nCurBlockObject = m_nCurSubBlockObject = 0;

	// Interate through all blocks
	for ( long nBlock = 69; nBlock < NUM_UPPER_BLOCK_RECORD; nBlock++ ) {
		// Set the current block
		m_bViewInterior = FALSE;
		m_nCurBlockObject = nBlock;

		// Draw this block once for each and weather type
		m_dwTerrain = TERRAIN_DESERT; m_dwWeather = WEATHER_NORMAL; BuildMediaView();
		m_dwTerrain = TERRAIN_DESERT; m_dwWeather = WEATHER_WINTER; BuildMediaView();
		m_dwTerrain = TERRAIN_DESERT; m_dwWeather = WEATHER_RAIN; BuildMediaView();

		m_dwTerrain = TERRAIN_MOUNTAIN; m_dwWeather = WEATHER_NORMAL; BuildMediaView();
		m_dwTerrain = TERRAIN_MOUNTAIN; m_dwWeather = WEATHER_WINTER; BuildMediaView();
		m_dwTerrain = TERRAIN_MOUNTAIN; m_dwWeather = WEATHER_RAIN; BuildMediaView();

		m_dwTerrain = TERRAIN_TEMPERATE; m_dwWeather = WEATHER_NORMAL; BuildMediaView();
		m_dwTerrain = TERRAIN_TEMPERATE; m_dwWeather = WEATHER_WINTER; BuildMediaView();
		m_dwTerrain = TERRAIN_TEMPERATE; m_dwWeather = WEATHER_RAIN; BuildMediaView();

		m_dwTerrain = TERRAIN_SWAMP; m_dwWeather = WEATHER_NORMAL; BuildMediaView();
		m_dwTerrain = TERRAIN_SWAMP; m_dwWeather = WEATHER_WINTER; BuildMediaView();
		m_dwTerrain = TERRAIN_SWAMP; m_dwWeather = WEATHER_RAIN; BuildMediaView();

		// Iterate through all sub-blocks with interiors enabled
		m_bViewInterior = TRUE;
		m_dwTerrain = TERRAIN_TEMPERATE; m_dwWeather = WEATHER_NORMAL;
		for ( long nSubBlock = 0; nSubBlock < m_nSubBlockObjectCount; nSubBlock++ ) {
			// Set the current sub-block
			m_nCurSubBlockObject = nSubBlock;

			// Build the view
			BuildMediaView();
		}
	}
}/* AutoBlockTests */