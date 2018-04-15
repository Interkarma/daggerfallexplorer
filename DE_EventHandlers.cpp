/*************************************************************************************************\
*
* Filename:		DE_EventHandlers.h
* Purpose:		Implement event-related CDaggerfallExplorerApp functions
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


//
// void	HandleChildMessage( LPNMHDR pnmh )
// Handle messages coming from child windows or controls
//
void CDaggerfallExplorerApp::HandleChildMessage( LPNMHDR pnmh )
{
	NM_TREEVIEW* pNMTreeView = NULL;

	// pnmh cannot be NULL
	ASSERT( pnmh );

	UINT idFrom = pnmh->idFrom;
	UINT code = pnmh->code;
	HWND hwndFrom = pnmh->hwndFrom;

	// Find source of message
	switch ( idFrom )
	{
	case IDC_ARENA_TREE:
		pNMTreeView = (NM_TREEVIEW*)pnmh;
		switch ( code )
		{
		case TVN_SELCHANGED:
			OnSelectNode( pNMTreeView->itemNew.lParam );
			break;
		}// end switch ( code )
		break;
	}// end switch ( idFrom )
}/* HandleChildMessage */


//
// void HandleMouseMessages( MSG* pMsg )
// Called when the user is left-click dragging the mouse in the child view
//
void CDaggerfallExplorerApp::HandleMouseMessages( const MSG* pMsg, int x, int y )
{
	// Handle various mouse messages
	POINTS pt;
	switch ( pMsg->message )
	{
	case WM_LBUTTONDOWN:
		// Take action based on view type
		pt = MAKEPOINTS(pMsg->lParam);
		switch ( m_dwCurNodeType )
		{
		case NT_TEXTUREARCHIVE:
		case NT_IMGFILE:
		case NT_CIFFILE:
			SelectSprite( pt.x, pt.y );
			break;
		}
		break;

	case WM_RBUTTONDOWN:
		//OnRButtonDown();
		break;
	}
}/* HandleMouseMessages */


//
// void OnLMouseDrag( int x, int y )
// Called when the user is left-click dragging the mouse in the child view
//
void CDaggerfallExplorerApp::OnLMouseDrag( int x, int y )
{
	// Base action on view mode and mouse mode
	D3DXMATRIX matX, matY;
	switch( m_dwCurNodeGeneric )
	{
	case NTGENERIC_OBJECT:
		switch ( m_dwMouseMode )
		{
		case MOUSEMODE_OBJECT:
			// Left dragging in object mode will rotate the object
			D3DXMatrixRotationX( &matX, (float)y / 256 );
			D3DXMatrixRotationY( &matY, (float)x / 256 );
			matX *= matY;
			m_Alchemy.ObjectRotateMatrix( m_nViewActor, &matX );
			break;

		case MOUSEMODE_CAMERA:
			// Left dragging in camera mode will mouse look
			m_Alchemy.CameraRotate( ACNULL, -(float)y / 256, -(float)x / 256, 0.0f );

			// Poll keyboard - as Windows messages will steal from timers
			// Without this, mouselook stalls during camera pans
			// A little hacky, and the view vibrates, but it works
			//OnPollKeyboard();
			break;
		}
		DrawMediaView();
		break;
	}
}/* OnLMouseDrag */


//
// void OnRMouseDrag( int x, int y )
// Called when the user is right-click dragging the mouse in the child view
//
void CDaggerfallExplorerApp::OnRMouseDrag( int x, int y )
{
	// Get keyboard state
	m_Alchemy.GetKeyboardState( m_kbState );

	// Get shift state
	bool bShiftDown = false;
	if ( KEYDOWN(m_kbState,DIK_LSHIFT) || KEYDOWN(m_kbState,DIK_RSHIFT) )
		bShiftDown = true;

	// Base action on view mode and mouse mode
	D3DXMATRIX mat;
	D3DXQUATERNION quat;
	switch( m_dwCurNodeGeneric )
	{
	case NTGENERIC_OBJECT:
		switch ( m_dwMouseMode )
		{
		case MOUSEMODE_OBJECT:
			if ( !bShiftDown ) {
				// Right dragging in object mode will pan object up, down, left, and right
				m_Alchemy.ObjectTranslate( m_nViewActor, -(float)x/2, (float)y/2, 0.0 );
			}
			else {
				// Right dragging in object mode with shift down will pan object forward and back
				m_Alchemy.ObjectTranslate( m_nViewActor, 0.0f, 0.0f, -(float)y );
			}
			break;

		case MOUSEMODE_CAMERA:
			if ( !bShiftDown ) {
				// Right dragging in camera mode will pan camera up, down, left, and right
				m_Alchemy.CameraThrust( ACNULL, -(float)x, (float)y, 0.0f );
			}
			else {
				// Right dragging in camera mode will pan camera forward and back
				m_Alchemy.CameraThrust( ACNULL, 0.0f, 0.0f, (float)y );
			}
			break;
		}
		DrawMediaView();
		break;
	}
}/* OnRMouseDrag */


//
// void OnDrawMediaView()
// Called when the media view must be redrawn
// Note: The scene has not changed, only a present is required
//
void CDaggerfallExplorerApp::OnDrawMediaView()
{
	if ( !m_Alchemy.IsCreated() )
		return;

	m_Alchemy.Present();
}/* OnDrawMediaView */


//
// void OnSizeMediaView( int cx, int cy )
// Called when the media view must be resized
// Note: The scene must be redrawn
//
void CDaggerfallExplorerApp::OnSizeMediaView( int cx, int cy )
{
	if ( m_Alchemy.IsCreated() ) {
		if ( m_Alchemy.SetDisplaySize( cx, cy ) ) {
			DrawMediaView();
		}
	}
}/* OnSizeMediaView */


//
// void OnScrollMediaView( int nBar, UINT nSBCode, UINT nPos )
// Handle a scroll event from the media view
//
void CDaggerfallExplorerApp::OnScrollMediaView( int nBar, UINT nSBCode, UINT nPos )
{
	const int nScrollStep = 32, nScrollPage = 128;

	SIZE szDisplay;
	m_Alchemy.GetDisplaySize( &szDisplay );
	if ( SB_HORZ == nBar ) {
		switch ( nSBCode )
		{
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				m_nScrollX = (int)nPos;
				break;

			case SB_LINERIGHT:
				m_nScrollX += nScrollStep;
				break;

			case SB_LINELEFT:
				m_nScrollX -= nScrollStep;
				break;

			case SB_PAGERIGHT:
				m_nScrollX += nScrollPage;
				break;

			case SB_PAGELEFT:
				m_nScrollX -= nScrollPage;
				break;
		}

		// Snap to limits
		if ( m_nScrollX < 0 ) m_nScrollX = 0;
		if ( m_nScrollX + szDisplay.cx > m_nPageWidth ) m_nScrollX = m_nPageWidth - szDisplay.cx;
	}
	else {
		switch ( nSBCode )
		{
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				m_nScrollY = (int)nPos;
				break;

			case SB_LINEDOWN:
				m_nScrollY += 32;
				break;

			case SB_LINEUP:
				m_nScrollY -= 32;
				break;

			case SB_PAGEDOWN:
				m_nScrollY += nScrollPage;
				break;

			case SB_PAGEUP:
				m_nScrollY -= nScrollPage;
				break;
		}

		// Snap to limits
		if ( m_nScrollY < 0 ) m_nScrollY = 0;
		if ( m_nScrollY + szDisplay.cy > m_nPageHeight ) m_nScrollY = m_nPageHeight - szDisplay.cy;
	}

	DrawMediaView();
}/* OnScrollMediaView */


//
// void OnSelectNode( UINT hNode )
// Called when a node is selected
//
void CDaggerfallExplorerApp::OnSelectNode( UINT hNode )
{
	// Lock view so refresh events do not get called during update
	m_bLockView = TRUE;

	// Store current node
	m_nCurNode = hNode;

	// Disable view actor and strip it of any mesh details
	ACSETTINGS_ACTOR acsa;
	acsa.Object.dwSize = sizeof(ACSETTINGS_ACTOR);
	m_Alchemy.GetObject( m_nViewActor, &acsa );
	acsa.nMesh = ACNULL;
	m_Alchemy.SetObject( m_nViewActor, &acsa, FALSE );

	// Clean up all sprites and objects
	m_Alchemy.PurgeSpriteStore();

	// Clear scroll data
	m_nScrollX = m_nScrollY = 0;
	m_nPageWidth = m_nPageHeight = 0;

	// Initialise stored values
	m_nSelectedSprite = NULL;
	m_nAutomapSprite = NULL;

	// Display node based on type
	BOOL bBuildSuccess = TRUE;
	NODEPROP* pnp = m_pArenaBar->m_oaNodeProperties.GetObject( hNode );
	switch ( pnp->nt )
	{
	case NT_TEXTUREARCHIVE:
		m_dwCurNodeType = NT_TEXTUREARCHIVE;
		m_dwCurNodeGeneric = NTGENERIC_BITMAP;
		m_pMediaView->ShowScrollBar( SB_BOTH, TRUE );
		CacheTextureArchive( pnp->strFile );
		break;

	case NT_IMGFILE:
		m_dwCurNodeType = NT_IMGFILE;
		m_dwCurNodeGeneric = NTGENERIC_BITMAP;
		m_pMediaView->ShowScrollBar( SB_BOTH, TRUE );
		CacheIMGImage( pnp->strFile );
		break;

	case NT_CIFFILE:
		m_dwCurNodeType = NT_CIFFILE;
		m_dwCurNodeGeneric = NTGENERIC_BITMAP;
		m_pMediaView->ShowScrollBar( SB_BOTH, TRUE );
		CacheCIFArchive( pnp->strFile );
		break;

	case NT_ARCH3D:
		m_dwCurNodeType = NT_ARCH3D;
		m_dwCurNodeGeneric = NTGENERIC_OBJECT;
		m_pMediaView->ShowScrollBar( SB_BOTH, FALSE );
		ResetObject();
		ResetCamera();
		bBuildSuccess = BuildMediaView( TRUE );
		break;

	case NT_BLOCKS:
		m_dwCurNodeType = NT_BLOCKS;
		m_dwCurNodeGeneric = NTGENERIC_OBJECT;
		m_pMediaView->ShowScrollBar( SB_BOTH, FALSE );
		ResetObject();
		ResetCamera();
		//NOTE: Testing only. Do not uncomment.// m_bLockView = FALSE; AutoBlockTests();
		bBuildSuccess = BuildMediaView( TRUE );
		break;

	default:
		m_dwCurNodeType = m_dwCurNodeGeneric = NT_NONE;
		m_pMediaView->ShowScrollBar( SB_BOTH, FALSE );
		break;
	}

	// Build properties
	BuildPropertyView();

	// Unlock the view for updates
	m_bLockView = FALSE;

	// Draw media view - must be done as view has been locked
	if ( bBuildSuccess ) {
		DrawMediaView();
	}
	else {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		m_Alchemy.Clear( FALSE );
		m_Alchemy.Present();
	}
}/* OnSelectNode */


//
// void OnStepBitmaps()
// Step the animation of bitmaps in the media view
//
void CDaggerfallExplorerApp::OnStepBitmaps()
{
	if ( NTGENERIC_BITMAP != m_dwCurNodeGeneric || m_bSupressAnimateBitmaps )
		return;

	UINT nSprite = m_Alchemy.FindFirstSprite();
	while ( ACNULL != nSprite ) {
		m_Alchemy.StepSpriteAnim( nSprite );
		nSprite = m_Alchemy.FindNextSprite( nSprite );
	}

	// Redraw media view
	DrawMediaView();
}/* OnStepBitmaps */


//
// void OnPollKeyboard()
// Poll the keyboard for input when in camera mode
//
void CDaggerfallExplorerApp::OnPollKeyboard()
{
	// Poll keyboard
	char buffer[256];
	m_Alchemy.GetKeyboardState( buffer );

	// Exit if CTRL is depressed (cheer up, CTRL)
	if ( KEYDOWN(buffer,DIK_LCONTROL) || KEYDOWN(buffer,DIK_RCONTROL) )
		return;

	// Get shift state
	bool bShiftDown = false;
	if ( KEYDOWN(buffer,DIK_LSHIFT) || KEYDOWN(buffer,DIK_RSHIFT) )
		bShiftDown = true;

	// Set movement speed
	float fMovementRate = 55.0f;
	if ( bShiftDown ) fMovementRate = 10.0f;

	// Move camera
	if ( KEYDOWN(buffer,DIK_UP) || KEYDOWN(buffer,DIK_W) ) {
		m_Alchemy.CameraThrust( ACNULL, 0.0f, 0.0f, fMovementRate );
		DrawMediaView();
	}
	if ( KEYDOWN(buffer,DIK_DOWN) || KEYDOWN(buffer,DIK_S) ) {
		m_Alchemy.CameraThrust( ACNULL, 0.0f, 0.0f, -fMovementRate );
		DrawMediaView();
	}
	if ( KEYDOWN(buffer,DIK_LEFT) || KEYDOWN(buffer,DIK_A) ) {
		m_Alchemy.CameraThrust( ACNULL, -fMovementRate, 0.0f, 0.0f );
		DrawMediaView();
	}
	if ( KEYDOWN(buffer,DIK_RIGHT) || KEYDOWN(buffer,DIK_D) ) {
		m_Alchemy.CameraThrust( ACNULL, fMovementRate, 0.0f, 0.0f );
		DrawMediaView();
	}
	if ( KEYDOWN(buffer,DIK_Q) || KEYDOWN(buffer,DIK_Z) || KEYDOWN(buffer, DIK_DELETE) ) {
		m_Alchemy.CameraThrust( ACNULL, 0.0f, -fMovementRate, 0.0f );
		DrawMediaView();
	}
	if ( KEYDOWN(buffer,DIK_E) || KEYDOWN(buffer,DIK_C) || KEYDOWN(buffer, DIK_PGDN) ) {
		m_Alchemy.CameraThrust( ACNULL, 0.0f, fMovementRate, 0.0f );
		DrawMediaView();
	}
}/* OnPollKeyboard */


void CDaggerfallExplorerApp::OnFileCloseArena2() 
{
	// Close Arena2
	m_DaggerTool.CloseArena2();

	// Initialise views
	InitialiseViews();
}

void CDaggerfallExplorerApp::OnUpdateFileCloseArena2(CCmdUI* pCmdUI) 
{
	if ( m_DaggerTool.IsArena2Open() )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnUpdateObjectCamera(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwMouseMode == MOUSEMODE_CAMERA )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnViewReset()
{
	ResetObject();
	ResetCamera();

	// Redraw view
	DrawMediaView();
}

void CDaggerfallExplorerApp::OnUpdateViewReset(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectPrevious() 
{
	switch ( m_dwCurNodeType )
	{
	case NT_ARCH3D:
		// Check for lower bounds
		if ( --m_nCurArch3DObject < NUM_LOWER_ARCH3D_RECORD )
			m_nCurArch3DObject = NUM_UPPER_ARCH3D_RECORD;
		
		// Draw view
		m_nCurFace = 0;
		if ( !BuildObjectView() ) {
			m_Alchemy.EnableObject( m_nViewActor, FALSE );
		}
		BuildMediaView( TRUE );
		SetPropertyView();
		break;

	case NT_BLOCKS:
		// Check for lower bounds
		m_nCurSubBlockObject = 0;
		m_nSubBlockObjectCount = 0;
		if ( --m_nCurBlockObject < NUM_LOWER_BLOCK_RECORD )
			m_nCurBlockObject = NUM_UPPER_BLOCK_RECORD;

		BuildMediaView( TRUE );
		BuildPropertyView();
		break;
	}
}

void CDaggerfallExplorerApp::OnUpdateObjectPrevious(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectNext() 
{
	switch ( m_dwCurNodeType )
	{
	case NT_ARCH3D:
		// Check for upper bounds
		if ( ++m_nCurArch3DObject > NUM_UPPER_ARCH3D_RECORD )
			m_nCurArch3DObject = NUM_LOWER_ARCH3D_RECORD;
		
		// Draw view
		m_nCurFace = 0;
		if ( !BuildObjectView() ) {
			m_Alchemy.EnableObject( m_nViewActor, FALSE );
		}
		BuildMediaView( TRUE );
		SetPropertyView();
		break;

	case NT_BLOCKS:
		// Check for upper bounds
		m_nCurSubBlockObject = 0;
		m_nSubBlockObjectCount = 0;
		if ( ++m_nCurBlockObject > NUM_UPPER_BLOCK_RECORD )
			m_nCurBlockObject = NUM_LOWER_BLOCK_RECORD;

		BuildMediaView( TRUE );
		BuildPropertyView();
		break;
	}
}

void CDaggerfallExplorerApp::OnUpdateObjectNext(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnUpdateObjectGoto(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectPureFaces() 
{
	m_dwDrawMode = OBJDRAW_PUREFACES;
	BuildMediaView();
	BuildPropertyView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectPureFaces(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwDrawMode == OBJDRAW_PUREFACES )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnObjectWireframe()
{
	m_dwDrawMode = OBJDRAW_WIREFRAME;
	BuildMediaView();
	BuildPropertyView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectWireframe(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwDrawMode == OBJDRAW_WIREFRAME )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnObjectSolid() 
{
	m_dwDrawMode = OBJDRAW_SOLID;
	BuildMediaView();
	BuildPropertyView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectSolid(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable( TRUE );
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwDrawMode == OBJDRAW_SOLID )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnObjectTextured() 
{
	m_dwDrawMode = OBJDRAW_TEXTURED;
	BuildMediaView();
	BuildPropertyView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectTextured(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwDrawMode == OBJDRAW_TEXTURED )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnUpdateBitmapAnimation(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_BITMAP )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectFaceNextFace() 
{
	if ( ++m_nCurFace > m_DaggerTool.m_Arch3D.GetFaceCount() - 1 )
		m_nCurFace = 0;

	DrawMediaView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectFaceNextFace(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwDrawMode == OBJDRAW_PUREFACES && m_dwCurNodeType == NT_ARCH3D )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectFacePreviousFace() 
{
	if ( --m_nCurFace < 0 )
		m_nCurFace = m_DaggerTool.m_Arch3D.GetFaceCount() - 1;

	DrawMediaView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectFacePreviousFace(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwDrawMode == OBJDRAW_PUREFACES && m_dwCurNodeType == NT_ARCH3D )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnUpdateObjectFaceGotoFace(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwDrawMode == OBJDRAW_PUREFACES && m_dwCurNodeType == NT_ARCH3D )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnBitmapZoomIn() 
{
	// Increment zoom value
	if ( m_fBitmapScale < 8.0f )
		m_fBitmapScale *= 2;

	// Format view
	switch ( m_dwCurNodeType )
	{
	case NT_TEXTUREARCHIVE:
	case NT_IMGFILE:
	case NT_CIFFILE:
		FormatSpritePage();
		break;
	}

	m_nScrollX = m_nScrollY = 0;
	DrawMediaView();
}

void CDaggerfallExplorerApp::OnUpdateBitmapZoomIn(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_BITMAP && m_fBitmapScale < 8.0f )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnBitmapZoomOut() 
{
	// Decrement zoom value
	if ( m_fBitmapScale > 1.0f )
		m_fBitmapScale /= 2;

	// Format view
	switch ( m_dwCurNodeType )
	{
	case NT_TEXTUREARCHIVE:
	case NT_IMGFILE:
	case NT_CIFFILE:
		FormatSpritePage();
		break;
	}

	m_nScrollX = m_nScrollY = 0;
	DrawMediaView();
}

void CDaggerfallExplorerApp::OnUpdateBitmapZoomOut(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_BITMAP && m_fBitmapScale > 1.0f )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnUpdatePlayUVEditor(CCmdUI* pCmdUI) 
{
	// Set enable state
	if ( m_dwDrawMode == OBJDRAW_PUREFACES && m_dwCurNodeType == NT_ARCH3D )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnViewArenaView() 
{
	if ( m_pArenaBar->IsVisible() ) {
		m_pFrame->ShowControlBar( m_pArenaBar, FALSE, FALSE );
	}
	else {
		m_pFrame->ShowControlBar( m_pArenaBar, TRUE, FALSE );
	}
}

void CDaggerfallExplorerApp::OnUpdateViewArenaView(CCmdUI* pCmdUI) 
{
	if ( m_pArenaBar->IsVisible() ) {
		pCmdUI->SetCheck();
	}
	else {
		pCmdUI->SetCheck( FALSE );
	}
}

void CDaggerfallExplorerApp::OnViewPropertiesView() 
{
	if ( m_pPropertiesBar->IsVisible() ) {
		m_pFrame->ShowControlBar( m_pPropertiesBar, FALSE, FALSE );
	}
	else {
		m_pFrame->ShowControlBar( m_pPropertiesBar, TRUE, FALSE );
	}
}

void CDaggerfallExplorerApp::OnUpdateViewPropertiesView(CCmdUI* pCmdUI) 
{
	if ( m_pPropertiesBar->IsVisible() ) {
		pCmdUI->SetCheck();
	}
	else {
		pCmdUI->SetCheck( FALSE );
	}
}

void CDaggerfallExplorerApp::OnViewLogView() 
{
	if ( m_pLogBar->IsVisible() ) {
		m_pFrame->ShowControlBar( m_pLogBar, FALSE, FALSE );
	}
	else {
		m_pFrame->ShowControlBar( m_pLogBar, TRUE, FALSE );
	}
}

void CDaggerfallExplorerApp::OnUpdateViewLogView(CCmdUI* pCmdUI) 
{
	if ( m_pLogBar->IsVisible() ) {
		pCmdUI->SetCheck();
	}
	else {
		pCmdUI->SetCheck( FALSE );
	}
}

void CDaggerfallExplorerApp::OnViewObjectmode()
{
	// Prevent view from being reset if mode already active
	if ( MOUSEMODE_OBJECT == m_dwMouseMode )
		return;

	m_dwMouseMode = MOUSEMODE_OBJECT;
	OnViewReset();

	// Kill keyboard poll timer if present
	if ( m_nKeyTimer ) {
		if ( m_pMediaView->KillTimer( m_nKeyTimer ) )
			m_nKeyTimer = 0;
	}
}

void CDaggerfallExplorerApp::OnUpdateViewObjectmode(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwMouseMode == MOUSEMODE_OBJECT )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnViewCameramode()
{
	// Prevent view from being reset if mode already active
	if ( MOUSEMODE_CAMERA == m_dwMouseMode )
		return;

	m_dwMouseMode = MOUSEMODE_CAMERA;
	OnViewReset();

	// Install keyboard poll timer
	if ( !m_nKeyTimer ) {
		m_nKeyTimer = m_pMediaView->SetTimer( DETIMER_POLLKEYBOARD, 25, NULL );
	}
}

void CDaggerfallExplorerApp::OnUpdateViewCameramode(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeGeneric == NTGENERIC_OBJECT )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set button state
	if ( m_dwMouseMode == MOUSEMODE_CAMERA )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnBitmapToggleAnimation() 
{
	// Clean up selected bitmap
	m_nSelectedSprite = NULL;

	// Toggle animation flag
	m_bAnimateBitmaps = !m_bAnimateBitmaps;

	// Kill timer if already installed and no longer animating
	if ( m_nAnimTimer && !m_bAnimateBitmaps) {
		m_pMediaView->KillTimer( DETIMER_STEPBITMAPS );
		m_nAnimTimer = 0;
	}
	else {
		m_nAnimTimer = m_pMediaView->SetTimer( DETIMER_STEPBITMAPS, (1000 / m_dwBitmapFPS), NULL );
	}
	
	OnSelectNode( m_nCurNode );
	DrawMediaView();
}

void CDaggerfallExplorerApp::OnUpdateBitmapToggleAnimation(CCmdUI* pCmdUI) 
{
	if ( NTGENERIC_BITMAP == m_dwCurNodeGeneric )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	if ( m_bAnimateBitmaps )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}


void CDaggerfallExplorerApp::OnTerrainNone()
{
	m_dwTerrain = TERRAIN_NONE;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainNone(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwTerrain == TERRAIN_NONE )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}


void CDaggerfallExplorerApp::OnTerrainDesert()
{
	m_dwTerrain = TERRAIN_DESERT;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainDesert(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwTerrain == TERRAIN_DESERT )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainMountain()
{
	m_dwTerrain = TERRAIN_MOUNTAIN;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainMountain(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwTerrain == TERRAIN_MOUNTAIN )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainTemperate()
{
	m_dwTerrain = TERRAIN_TEMPERATE;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainTemperate(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwTerrain == TERRAIN_TEMPERATE )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainSwamp()
{
	m_dwTerrain = TERRAIN_SWAMP;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainSwamp(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwTerrain == TERRAIN_SWAMP )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainNormal()
{
	m_dwWeather = WEATHER_NORMAL;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainNormal(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS && m_dwTerrain != TERRAIN_NONE )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwWeather == WEATHER_NORMAL )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainWinter()
{
	m_dwWeather = WEATHER_WINTER;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainWinter(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS && m_dwTerrain != TERRAIN_NONE )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwWeather == WEATHER_WINTER )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnTerrainRain()
{
	m_dwWeather = WEATHER_RAIN;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateTerrainRain(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS && m_dwTerrain != TERRAIN_NONE )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_dwWeather == WEATHER_RAIN )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnObjectPreviousBlock()
{
	if ( --m_nCurSubBlockObject < 0 ) {
		m_nCurSubBlockObject = m_nSubBlockObjectCount - 1;
	}
	BuildMediaView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectPreviousBlock(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnObjectNextBlock()
{
	if ( ++m_nCurSubBlockObject >= m_nSubBlockObjectCount ) {
		m_nCurSubBlockObject = 0;
	}
	BuildMediaView();
	SetPropertyView();
}

void CDaggerfallExplorerApp::OnUpdateObjectNextBlock(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );
}

void CDaggerfallExplorerApp::OnViewAllblocks()
{
	m_bDrawAllBlocks = !m_bDrawAllBlocks;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateViewAllblocks(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_bDrawAllBlocks )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnViewGroundplane()
{
	m_bDrawGround = !m_bDrawGround;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateViewGroundplane(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_bDrawGround )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}

void CDaggerfallExplorerApp::OnViewInterior()
{
	m_bViewInterior = !m_bViewInterior;
	BuildMediaView();
}

void CDaggerfallExplorerApp::OnUpdateViewInterior(CCmdUI *pCmdUI)
{
	// Set enable state
	if ( m_dwCurNodeType == NT_BLOCKS )
		pCmdUI->Enable();
	else
		pCmdUI->Enable( FALSE );

	// Set check state
	if ( m_bViewInterior )
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck( FALSE );
}