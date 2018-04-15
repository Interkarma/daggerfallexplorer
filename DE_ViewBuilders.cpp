/*************************************************************************************************\
*
* Filename:		DE_ViewBuilders.cpp
* Purpose:		Implement view-related CDaggerfallExplorerApp functions
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
// BOOL InitialiseViews()
// Reset view state and rebuild if Arena2 folder is open
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::InitialiseViews()
{
	HTREEITEM hContentRootNode, hTexturesRootNode, hLocaleRootNode;
	HTREEITEM hIMGRootNode, hCIFRootNode, hRCIRootNode, h3DObjectsRootNode, hBlocksRootNode, hSkyRootNode;

	// Clean up sprites
	m_Alchemy.PurgeSpriteStore();

	// Purge Arena tree
	m_pArenaBar->m_ctrlArenaTree.Select( NULL, TVGN_CARET );
	m_pArenaBar->Purge();

	// Purge Properties view
	m_pPropertiesBar->m_PropertyDlg.m_Tree.DeleteAllItems();

	// Reset node handles
	hContentRootNode = NULL;
	hTexturesRootNode = m_hTexturesRootNode = hSkyRootNode = NULL;
	hIMGRootNode = m_hIMGRootNode = NULL;
	hCIFRootNode = m_hCIFRootNode = NULL;
	hRCIRootNode = m_hRCIRootNode = NULL;
	h3DObjectsRootNode = m_h3DObjectsRootNode = NULL;
	hBlocksRootNode = m_hBlocksRootNode = NULL;
	m_hLocaleRootNode = hLocaleRootNode = NULL;

	// Initialise view related variables
	m_fBitmapScale = 1.0f;
	m_nCurArch3DObject = DE_START_OBJECT;
	m_nCurBlockObject = DE_START_BLOCK;
	m_nCurSubBlockObject = 0;
	m_nSubBlockObjectCount = 0;
	m_bTextureArchiveNodeBuilt = FALSE;
	m_bLockView = FALSE;
	m_bIMGRootNodeBuilt = FALSE;
	m_bCIFRootNodeBuilt = FALSE;
	m_bLocaleRootNodeBuilt = FALSE;
	m_dwMouseMode = MOUSEMODE_OBJECT;
	m_dwDrawMode = DE_START_DRAW;
	m_dwCurNodeType = m_dwCurNodeGeneric = NT_NONE;
	m_nCurNode = HSTOPERR;
	m_nCurFace = 0;
	m_nScrollX = m_nScrollY = 0;
	m_nPageWidth = m_nPageHeight = 0;
	m_strCurTextureArchive = "";
	m_nSelectedSprite = NULL;
	m_nAutomapSprite = NULL;
	m_nLastDFTextureCache = NULL;
	m_dwTerrain = DE_START_TERRAIN;
	m_dwWeather = WEATHER_NORMAL;
	ResetObject();
	ResetCamera();

	// Clear media view
	m_Alchemy.Clear();
	m_Alchemy.Present();

	// Complete if Arena2 not open
	if ( !m_DaggerTool.IsArena2Open() )
		return FALSE;

	// Add root items to Arena tree
	if ( NULL == (hContentRootNode = m_pArenaBar->InsertNode( TVI_ROOT, "Bitmaps", NT_BASICROOT ) ) ) return FALSE;
	//if ( NULL == (hLocaleRootNode = m_pArenaBar->InsertNode( TVI_ROOT, "Locales", NT_LOCALEROOT ) ) ) return FALSE;

	// Add first-level child nodes
	if ( NULL == (hTexturesRootNode = m_pArenaBar->InsertNode( hContentRootNode, "Texture Files", NT_BASICROOT ) ) ) return FALSE;
	if ( NULL == (hIMGRootNode = m_pArenaBar->InsertNode( hContentRootNode, "IMG Files", NT_BASICROOT ) ) ) return FALSE;
	if ( NULL == (hCIFRootNode = m_pArenaBar->InsertNode( hContentRootNode, "CIF Files", NT_BASICROOT ) ) ) return FALSE;
	if ( NULL == (hRCIRootNode = m_pArenaBar->InsertNode( hContentRootNode, "RCI Files", NT_BASICROOT ) ) ) return FALSE;
	if ( NULL == (h3DObjectsRootNode = m_pArenaBar->InsertNode( TVI_ROOT/*hContentRootNode*/, "3D Objects", NT_ARCH3D, NI_CUBE, NI_SELECTEDCUBE ) ) ) return FALSE;
	if ( NULL == (hBlocksRootNode = m_pArenaBar->InsertNode( TVI_ROOT/*hContentRootNode*/, "3D Blocks", NT_BLOCKS, NI_BLOCKS, NI_SELECTEDBLOCKS ) ) ) return FALSE;

	// Expand content node by default
	m_pArenaBar->m_ctrlArenaTree.Expand( hContentRootNode, TVE_EXPAND );

	// Store root nodes
	m_hTexturesRootNode = hTexturesRootNode;
	m_hIMGRootNode = hIMGRootNode;
	m_hCIFRootNode = hCIFRootNode;
	m_hRCIRootNode = hRCIRootNode;
	m_hSkyRootNode = hSkyRootNode;
	m_h3DObjectsRootNode = h3DObjectsRootNode;
	m_hBlocksRootNode = hBlocksRootNode;
	m_hLocaleRootNode = hLocaleRootNode;

	// Build bitmap nodes
	BuildTextureArchiveNode();
	BuildIMGNode();
	BuildCIFNode();
	BuildRCINode();

	// Build initial property view
	BuildPropertyView();

	return TRUE;
}/* InitialiseViews */


//
// BOOL BuildTextureArchiveNode()
// Build the texture archive node
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildTextureArchiveNode()
{
	// Add all enumerated texture archives to the media view
	CString strNode;
	char *pFilename, *pDescription;
	for ( int nArchive = 0; nArchive < NUM_TEXTURE_ARCHIVES; nArchive++ )
	{
		// Get filename and description of this archive
		pFilename = TextureArchives[nArchive].Filename;
		pDescription = TextureArchives[nArchive].Description;

		// Compose description of archive
		if ( "" == CString( pDescription ) ) {
			strNode.Format( "%s", pFilename );
		}
		else {
			strNode.Format( "%s [%s]", pDescription, pFilename );
		}

		// Add this archive to the tree
		if ( !m_pArenaBar->InsertNode( m_hTexturesRootNode, strNode, NT_TEXTUREARCHIVE, NI_BITMAP, NI_BITMAP, pFilename ) )
			return FALSE;
	}

	return TRUE;
}/* BuildTextureArchiveNode */


//
// BOOL BuildIMGNode()
// Build the IMG node
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildIMGNode()
{
	// Add all enumerated IMG images to the media view
	for ( int nImage = 0; nImage < NUM_IMG_FILES; nImage++ )
	{
		// Get filename and description of this archive
		char *pFilename = IMGFiles[nImage];

		// Add this image to the tree
		CString strName = pFilename;
		if ( !m_pArenaBar->InsertNode( m_hIMGRootNode, strName.Left(strName.GetLength()-4), NT_IMGFILE, NI_BITMAP, NI_BITMAP, pFilename ) )
			return FALSE;
	}

	return TRUE;
}/* BuildIMGNode */


//
// BOOL BuildCIFNode()
// Build the CIF node
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildCIFNode()
{
	// Add all enumerated CIF images to the media view
	for ( int nImage = 0; nImage < NUM_CIF_FILES; nImage++ )
	{
		// Get filename and description of this archive
		char *pFilename = CIFFiles[nImage];

		// Add this image to the tree
		CString strName = pFilename;
		if ( !m_pArenaBar->InsertNode( m_hCIFRootNode, strName.Left(strName.GetLength()-4), NT_CIFFILE, NI_BITMAP, NI_BITMAP, pFilename ) )
			return FALSE;
	}

	return TRUE;
}/* BuildCIFNode */


//
// BOOL BuildRCINode()
// Build the RCI node
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildRCINode()
{
	// Add all enumerated CIF images to the media view
	for ( int nImage = 0; nImage < NUM_RCI_FILES; nImage++ )
	{
		// Get filename and description of this archive
		char *pFilename = RCIFiles[nImage];

		// Add this image to the tree
		CString strName = pFilename;
		if ( !m_pArenaBar->InsertNode( m_hRCIRootNode, strName.Left(strName.GetLength()-4), NT_CIFFILE, NI_BITMAP, NI_BITMAP, pFilename ) )
			return FALSE;
	}

	return TRUE;
}/* BuildRCINode */


//
// BOOL BuildLocaleNode()
// Build the locales
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildLocaleNode()
{
	// Build local nodes
	HTREEITEM hNode;
	for ( int l = 0; l < NUM_LOCALE_COUNT; l++ ) {
		// Add root locale name
		if ( NULL == (hNode = m_pArenaBar->InsertNode( m_hLocaleRootNode, LocaleNames[l], NT_BASICROOT ) ) ) return FALSE;

		// Add place and dungeon roots
		if ( NULL == m_pArenaBar->InsertNode( hNode, "Places", NT_PLACEROOT ) ) return FALSE;
		if ( NULL == m_pArenaBar->InsertNode( hNode, "Dungeons", NT_DUNGEONROOT ) ) return FALSE;
	}

	// Set node as buile
	m_bLocaleRootNodeBuilt = TRUE;

	return TRUE;
}/* BuildLocaleNode */


//
// BOOL BuildMediaView( BOOL bReset = FALSE )
// Used to build the contents of the media view
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildMediaView( BOOL bReset/*=FALSE*/ )
{
	BOOL bResult = FALSE;

	switch ( m_dwCurNodeType )
	{
	case NT_ARCH3D:
		bResult = BuildObjectView( bReset );
		break;

	case NT_BLOCKS:
		bResult = BuildBlockView( bReset );
		break;
	}

	DrawMediaView();

	return bResult;
}/* BuildMediaView */


//
// BOOL BuildObjectView( BOOL bResetObject = FALSE )
// Construct an object view
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildObjectView( BOOL bResetObject/*=FALSE*/ )
{
	// Build view mesh
	if ( !BuildMesh( m_nCurArch3DObject, &m_meshView  ) ) {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Scale the object for easier viewing
	D3DXVECTOR3 vSize;
	m_meshView.GetSize( &vSize );
	float fScale = (float)750.0 / ((vSize.x + vSize.y + vSize.z) / 3);
	m_meshView.Scale( fScale, fScale, fScale );

	// Transform into position
	m_meshView.RotateDegrees( 180.0f, 0.0f, 0.0f );
	m_meshView.CentreAxis();

	// Commit texture cache to store
	if ( !CommitDFTextures() ) {
		return FALSE;
	}

	// Install view mesh
	UINT nViewMesh;
	if ( ACNULL == (nViewMesh = m_Alchemy.InstallMeshFromMemory( &m_meshView )) )
		return FALSE;

	// Release texture list to remove create-reference from each texture
	ReleaseDFTextures();

	// Reset object if required
	if ( bResetObject ) {
		switch( m_dwMouseMode )
		{
		case MOUSEMODE_OBJECT:
			ResetObject();
			break;

		case MOUSEMODE_CAMERA:
			ResetCamera();
			break;
		}
	}

	// Set new actor properties
	ACSETTINGS_ACTOR acsa;
	acsa.Object.dwSize = sizeof( ACSETTINGS_ACTOR );
	m_Alchemy.GetObject( m_nViewActor, &acsa );
	acsa.nMesh = nViewMesh;
	m_Alchemy.SetObject( m_nViewActor, &acsa );

	// Remove create-reference from mesh as it will now managed by actor
	m_Alchemy.DecResRef( nViewMesh );

	return TRUE;
}/* BuildObjectView */


//
// BOOL BuildBlockView( BOOL bResetObject = FALSE )
// Construct a group of objects to form a single block view
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildBlockView( BOOL bResetObject/*=FALSE*/ )
{
	// Mesh Building Rules:
	// 1. Build mesh object
	// 2. CommitDFTextures()
	// 3. Install mesh object into store
	// 4. Set actor to use mesh
	// 5. ReleaseDFTextures()
	// 6. DecResRef( mesh )

	// Clear current actor settings
	ACSETTINGS_ACTOR acsa;
	acsa.Object.dwSize = sizeof(ACSETTINGS_ACTOR);
	m_Alchemy.GetObject( m_nViewActor, &acsa );
	acsa.nMesh = ACNULL;
	m_Alchemy.SetObject( m_nViewActor, &acsa, FALSE );

	// Delete existing automap sprite if present
	if ( ACNULL != m_nAutomapSprite ) {
		m_Alchemy.RemoveSprite( m_nAutomapSprite );
		m_nAutomapSprite = NULL;
	}

	// Open this block
	if ( !m_DaggerTool.m_ArchBlocks.OpenBlock( m_nCurBlockObject ) ) {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Exit if no subrecords
	if ( !m_DaggerTool.m_ArchBlocks.GetBlockSubRecordCount() ) {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Reference start of information
	LPRMBFLD pFLD = m_DaggerTool.m_ArchBlocks.GetFLD();

	// Install minimap texture
	RECT rct = {0,0,64,64};
	UINT nTexture = ACNULL;
	BLKIMG_AUTOMAP imgAutomap;
	m_DaggerTool.m_ArchBlocks.GetBlockAutomap( &imgAutomap );
	nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &rct, 64, imgAutomap.pBuffer, ACBD_32, "MiniMap" );
	if ( ACNULL == nTexture ) {
		m_DaggerTool.m_ArchBlocks.CloseBlock();
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Initialise main mesh object
	InitialiseMeshObject( &m_meshView );

	// Build block objects
	if ( !BuildBlockObjects( pFLD ) ) {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Recentre combine mesh so rotation pivots on central axis
	m_meshView.Translate( 2048.0f, 0.0f, -2048.0f );

	// Commit texture cache to store
	CommitDFTextures();

	// Install mesh resource into engine
	UINT nViewMesh = m_Alchemy.InstallMeshFromMemory( &m_meshView );
	if ( ACNULL == nViewMesh ) {
		ReleaseDFTextures();
		return FALSE;
	}

	// Create a sprite for automap
	ACSETTINGS_SPRITE acss;
	ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
	acss.dwSize = sizeof(ACSETTINGS_SPRITE);
	acss.dwFlags = 0;
	acss.nTexture = nTexture;
	acss.vPos.x = 0.0f;
	acss.vPos.y = 0.0f;
	acss.vScaling.x = acss.vScaling.y = 2.0f;
	acss.colour = DECOLOUR_SPRITE;
	m_nAutomapSprite = m_Alchemy.InstallSprite( &acss );
	if ( ACNULL == m_nAutomapSprite ) {
		m_Alchemy.DecResRef( nTexture );
		m_DaggerTool.m_ArchBlocks.CloseBlock();
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Decrement automap texture reference count so it dies with the sprite
	m_Alchemy.DecResRef( nTexture );

	// Reset object if required
	if ( bResetObject ) {
		switch( m_dwMouseMode )
		{
		case MOUSEMODE_OBJECT:
			ResetObject();
			break;

		case MOUSEMODE_CAMERA:
			ResetCamera();
			break;
		}
	}

	// Set actor to use new mesh
	acsa.Object.dwSize = sizeof(ACSETTINGS_ACTOR);
	m_Alchemy.GetObject( m_nViewActor, &acsa );
	acsa.nMesh = nViewMesh;
	m_Alchemy.SetObject( m_nViewActor, &acsa );

	// Release texture list to remove create-reference from each texture
	ReleaseDFTextures();

	// Remove create-reference from mesh as it will now managed by actor
	m_Alchemy.DecResRef( nViewMesh );

	// Set number of interior objects
	m_nSubBlockObjectCount = m_DaggerTool.m_ArchBlocks.GetBlockSubRecordCount();

	return TRUE;
}/* BuildBlockView */


//
// BOOL BuildBlockObjects( LPRMBFLD pFLD )
// Construct the exterior block object
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildBlockObjects( LPRMBFLD pFLD )
{
	_ASSERT( pFLD );

	// Build all subrecords
	float fRotate;
	long nObjectRecord;
	TFlexibleMesh MeshTemp, MeshSub;
	LPRMB_BLOCKHEADER pBlockHeader;
	LPRMB_BLOCK3DOBJECTS p3DObjects;
	for ( long r = 0; r < m_DaggerTool.m_ArchBlocks.GetBlockSubRecordCount(); r++ ) {
		// Skip if drawing single objects and this is not the current object
		if ( !m_bDrawAllBlocks && r != m_nCurSubBlockObject ) {
			continue;
		}

		// Draw interior if specified
		if ( r == m_nCurSubBlockObject && m_bViewInterior ) {
			BuildInsideBlockObject( pFLD );
			continue;
		}

		// Get subrecord header
		pBlockHeader = m_DaggerTool.m_ArchBlocks.GetOutsideBlockSubRecord( r );
		if ( !pBlockHeader ) {
			m_Alchemy.EnableObject( m_nViewActor, FALSE );
			return FALSE;
		}

		// Get list of subobjects
		p3DObjects = m_DaggerTool.m_ArchBlocks.GetBlockSubRecord3DObjects( pBlockHeader );

		// Initialise temporary mesh object
		InitialiseMeshObject( &MeshTemp );

		// Build a combine mesh for all 3D subobjects in this block subrecord
		for ( int nObject = 0; nObject < pBlockHeader->n3DObjectRecords; nObject++ ) {
			// Build the mesh
			nObjectRecord = m_DaggerTool.m_Arch3D.ResolveObjectID( p3DObjects[nObject].nObjectID1 * 100 + p3DObjects[nObject].nObjectID2 );
			if ( !BuildMesh( nObjectRecord, &MeshSub ) )
				return FALSE;

			// Get rotation in degrees (512 is 90 degrees of rotation, so 1024 is 180 degrees, etc.)
			fRotate = (float)(90.0f * p3DObjects[nObject].Rotation / 512.0f );
			MeshSub.RotateDegrees( -fRotate, 0.0f, 0.0f );

			// Adjust position2 of this mesh
			MeshSub.Translate( (float)-(p3DObjects[nObject].XPos2), (float)-(p3DObjects[nObject].YPos2), (float)-(p3DObjects[nObject].ZPos2) );

			// Merge the submesh into the larger working mesh
			MeshTemp += MeshSub;
		}

		// Adjust Rotation of this working mesh
		fRotate = (float)(90.0f * pFLD->BlockPos[r].BlockRotation / 512.0f);
		MeshTemp.RotateDegrees( -fRotate, 0.0f, 0.0f );

		// Adjust position1 of this mesh
		MeshTemp.Translate( (float)-(pFLD->BlockPos[r].XPos2), 0.0f, (float)(pFLD->BlockPos[r].ZPos2) );

		// Merge working mesh into main mesh object
		m_meshView += MeshTemp;
	}

	// Build additional outside features
	if ( m_bDrawAllBlocks ) {
		InitialiseMeshObject( &MeshTemp );
		p3DObjects = m_DaggerTool.m_ArchBlocks.GetBlock3DObjects();
		for ( r = 0; r < m_DaggerTool.m_ArchBlocks.GetFLD()->nSubRecords2; r++ ) {
			// Build the mesh
			nObjectRecord = m_DaggerTool.m_Arch3D.ResolveObjectID( p3DObjects[r].nObjectID1 * 100 + p3DObjects[r].nObjectID2 );
			if ( !BuildMesh( nObjectRecord, &MeshSub ) )
				return FALSE;

			// Get rotation in degrees (512 is 90 degrees of rotation, so 1024 is 180 degrees, etc.)
			fRotate = (float)(90.0f * p3DObjects[r].Rotation / 512.0f);
			MeshSub.RotateDegrees( -fRotate, 0.0f, 0.0f );

			// Adjust position2 of this mesh
			MeshSub.Translate( (float)-(p3DObjects[r].XPos2), (float)-(p3DObjects[r].YPos2), (float)-(p3DObjects[r].ZPos2) );

			MeshTemp += MeshSub;
		}

		// Merge auxillary 3D objects into outside block
		m_meshView += MeshTemp;
	}

	// Build block ground plane
	if ( m_bDrawGround ) {
		BuildBlockGroundMesh( &pFLD->SmallMaps, &MeshTemp );
		MeshTemp.Translate( -4096.0, -3.0f, 0.0f );
		m_meshView += MeshTemp;
	}

	return TRUE;
}/* BuildBlockObjects */


//
// BOOL BuildInsideBlockObject( LPRMBFLD pFLD )
// Construct the interior block object
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildInsideBlockObject( LPRMBFLD pFLD )
{
	_ASSERT( pFLD );

	float fRotate = 0;
	long nObjectRecord;
	TFlexibleMesh MeshTemp, MeshSub;
	LPRMB_BLOCKHEADER pBlockHeader;
	LPRMB_BLOCK3DOBJECTS p3DObjects;

	// Get subrecord header
	pBlockHeader = m_DaggerTool.m_ArchBlocks.GetInsideBlockSubRecord( m_nCurSubBlockObject );
	if ( !pBlockHeader ) {
		m_Alchemy.EnableObject( m_nViewActor, FALSE );
		return FALSE;
	}

	// Get list of subobjects
	p3DObjects = m_DaggerTool.m_ArchBlocks.GetBlockSubRecord3DObjects( pBlockHeader );

	// Initialise temporary mesh object
	InitialiseMeshObject( &MeshTemp );

	// Build a combine mesh for all 3D subobjects in this block subrecord
	for ( int nObject = 0; nObject < pBlockHeader->n3DObjectRecords; nObject++ ) {
		// Build the mesh
		nObjectRecord = m_DaggerTool.m_Arch3D.ResolveObjectID( p3DObjects[nObject].nObjectID1 * 100 + p3DObjects[nObject].nObjectID2 );
		if ( !BuildMesh( nObjectRecord, &MeshSub ) )
			return FALSE;

		// Get rotation in degrees (512 is 90 degrees of rotation, so 1024 is 180 degrees, etc.)
		fRotate = (float)(90.0f * p3DObjects[nObject].Rotation / 512.0f );
		MeshSub.RotateDegrees( -fRotate, 0.0f, 0.0f );

		// Adjust position2 of this mesh
		if ( p3DObjects[nObject].nObjectType == 3 ) {
			// Kibble (bookcases, chairs, etc.) requires special handling
			MeshSub.DropAxis();
			MeshSub.Translate( (float)-(p3DObjects[nObject].XPos2), (float)(p3DObjects[nObject].YPos2), (float)-(p3DObjects[nObject].ZPos2) );
		}
		else {
			MeshSub.Translate( (float)-(p3DObjects[nObject].XPos2), (float)-(p3DObjects[nObject].YPos2), (float)-(p3DObjects[nObject].ZPos2) );
		}

		// Merge the submesh into the larger working mesh
		MeshTemp += MeshSub;
	}

	// Adjust Rotation of this working mesh
	fRotate = (float)(90.0f * pFLD->BlockPos[m_nCurSubBlockObject].BlockRotation / 512.0f);
	MeshTemp.RotateDegrees( -fRotate, 0.0f, 0.0f );

	// Adjust position1 of this mesh
	MeshTemp.Translate( (float)-(pFLD->BlockPos[m_nCurSubBlockObject].XPos2) + 4096, 0.0f, (float)(pFLD->BlockPos[m_nCurSubBlockObject].ZPos2) );

	// Merge working mesh into main mesh object
	MeshTemp.Translate( -4096.0, 0.0f, 0.0f );
	m_meshView += MeshTemp;

	return TRUE;
}/* BuildInsideBlockObject */


//
// BOOL BuildPropertyView()
// Construct the items within the property view for the current node type
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildPropertyView()
{
	// Exit if not in use
	if ( !m_pPropertiesBar->IsVisible() ) {
		return FALSE;
	}

	// Get pointers
	CPropertyDialog* pDialog = &m_pPropertiesBar->m_PropertyDlg;
	CPropTree* pTree = &m_pPropertiesBar->m_PropertyDlg.m_Tree;

	// Clear tree contents
	pTree->DeleteAllItems();

	// Create object root
	CPropTreeItem *pRoot, *pChild;
	pRoot = pTree->InsertItem(new CPropTreeItem());
	pRoot->SetInfoText(_T("Contains current properties"));
	pRoot->Expand();
	switch ( m_dwCurNodeType )
	{
	case NT_TEXTUREARCHIVE:
	case NT_IMGFILE:
	case NT_CIFFILE:
		pRoot->SetLabelText(_T("Bitmaps"));
		m_oaPropCache[BMPPROP_FILENAME] = pDialog->InsertStatic( "File Name", "File name of the bitmap container", pRoot );
		m_oaPropCache[BMPPROP_DESCRIPTION] = pDialog->InsertStatic( "Description", "Description of the bitmap container", pRoot );
		m_oaPropCache[BMPPROP_RECORDCOUNT] = pDialog->InsertStatic( "Record Count", "Number of bitmap records in the bitmap container", pRoot );
		SetPropertyView();
		break;

	case NT_ARCH3D:
		pRoot->SetLabelText(_T("3D Object"));
		m_oaPropCache[OBJPROP_OBJECT] = pDialog->InsertStatic( "Object", "Current Object / Total Objects", pRoot );
		m_oaPropCache[OBJPROP_OBJECTID] = pDialog->InsertStatic( "Object ID", "Unique ID of current object", pRoot );
		m_oaPropCache[OBJPROP_OBJECTVERSION] = pDialog->InsertStatic( "Version", "Version of current object (2.5, 2.6, or 2.7)", pRoot );
		m_oaPropCache[OBJPROP_FACECOUNT] = pDialog->InsertStatic( "Face Count", "Number of primitives (triangles or DF faces) this object is built from", pRoot );
		if ( OBJDRAW_PUREFACES == m_dwDrawMode ) {
			pChild = pTree->InsertItem( new CPropTreeItem() );
			pChild->SetLabelText( "DF Face" );
			pChild->SetInfoText( "Contains properties of current DF face" );
			pChild->Expand();
			m_oaPropCache[OBJPROP_SELFACE] = pDialog->InsertStatic( "Face", "Current DF face", pChild );
			m_oaPropCache[OBJPROP_POINTCOUNT] = pDialog->InsertStatic( "Point Count", "Number of points in this face", pChild );
			m_oaPropCache[OBJPROP_CORNERCOUNT] = pDialog->InsertStatic( "Corner Count", "Number of corners in this face", pChild );
			m_oaPropCache[OBJPROP_CORNERPOINTS] = pDialog->InsertStatic( "Corner Points", "Enumeration of corner points", pChild );
			m_oaPropCache[OBJPROP_ARCHIVE] = pDialog->InsertStatic( "Texture", "Face texture in the format Archive.Record", pChild );
			m_oaPropCache[OBJPROP_TEXTUREDIM] = pDialog->InsertStatic( "Dimensions", "Width x Height of this texture in pixels", pChild );
			m_oaPropCache[OBJPROP_UVPAIRS] = pDialog->InsertStatic( "UV Pairs", "The four UV values that map the texture to the face", pChild );
		}
		SetPropertyView();
		break;

	case NT_BLOCKS:
		pRoot->SetLabelText(_T("3D Blocks"));
		m_oaPropCache[BLKPROP_OBJECT] = pDialog->InsertStatic( "Object", "Current Object / Total Objects", pRoot );
		m_oaPropCache[BLKPROP_FILENAME] = pDialog->InsertStatic( "File Name", "File name of this record", pRoot );
		m_oaPropCache[BLKPROP_NUMRECORDS1] = pDialog->InsertStatic( "Block", "Current Block / Total Blocks", pRoot );

		if ( m_DaggerTool.m_ArchBlocks.IsBlockOpen() && m_DaggerTool.m_ArchBlocks.GetBlockSubRecordCount() > 0 ) {
			pChild = pTree->InsertItem( new CPropTreeItem() );
			pChild->SetLabelText( "Outside" );
			pChild->SetInfoText( "Contains properties of exterior sub-records" );
			pChild->Expand();
			m_oaPropCache[BLKPROP_OUTSIDE_NUM3DOBJECTS] = pDialog->InsertStatic( "3D Objects", "Number of 3D objects in this sub-record", pChild );
			m_oaPropCache[BLKPROP_OUTSIDE_NUMFLATOBJECTS] = pDialog->InsertStatic( "Flat Objects", "Number of billboard sprites in this sub-record", pChild );
			m_oaPropCache[BLKPROP_OUTSIDE_NUMPEOPLE] = pDialog->InsertStatic( "People", "Number of NPC objects in this sub-record", pChild );
			m_oaPropCache[BLKPROP_OUTSIDE_NUMDOORS] = pDialog->InsertStatic( "Doors", "Number of door objects in this sub-record", pChild );

			pChild = pTree->InsertItem( new CPropTreeItem() );
			pChild->SetLabelText( "Inside" );
			pChild->SetInfoText( "Contains properties of interior sub-records" );
			pChild->Expand();
			m_oaPropCache[BLKPROP_INSIDE_NUM3DOBJECTS] = pDialog->InsertStatic( "3D Objects", "Number of 3D objects in this sub-record", pChild );
			m_oaPropCache[BLKPROP_INSIDE_NUMFLATOBJECTS] = pDialog->InsertStatic( "Flat Objects", "Number of billboard sprites in this sub-record", pChild );
			m_oaPropCache[BLKPROP_INSIDE_NUMPEOPLE] = pDialog->InsertStatic( "People", "Number of NPC objects in this sub-record", pChild );
			m_oaPropCache[BLKPROP_INSIDE_NUMDOORS] = pDialog->InsertStatic( "Doors", "Number of door objects in this sub-record", pChild );
		}
		SetPropertyView();
		break;
	};

	return TRUE;
}/* BuildPropertyView() */


//
// BOOL SetPropertyView()
// Set the properties of the current property view
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::SetPropertyView()
{
	// Exit if not in use
	if ( !m_pPropertiesBar->IsVisible() ) {
		return FALSE;
	}

	// Get pointers
	CPropertyDialog* pDialog = &m_pPropertiesBar->m_PropertyDlg;
	CPropTree* pTree = &m_pPropertiesBar->m_PropertyDlg.m_Tree;

	char sz[128], szi[64];
	LPARCH3D_FACE pFace = NULL;
	switch ( m_dwCurNodeType )
	{
	case NT_TEXTUREARCHIVE:
		pDialog->SetStatic( m_oaPropCache[BMPPROP_FILENAME], m_DaggerTool.m_dtxArchive.GetArchiveName() );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_DESCRIPTION], m_DaggerTool.m_dtxArchive.GetArchiveDesc() );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_RECORDCOUNT], m_DaggerTool.m_dtxArchive.GetRecordCount() );
		break;

	case NT_IMGFILE:
		pDialog->SetStatic( m_oaPropCache[BMPPROP_FILENAME], m_DaggerTool.GetLastIMGName() );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_DESCRIPTION], "N/A" );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_RECORDCOUNT], "1" );
		break;

	case NT_CIFFILE:
		pDialog->SetStatic( m_oaPropCache[BMPPROP_FILENAME], m_DaggerTool.m_cifArchive.GetArchiveName() );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_DESCRIPTION], "N/A" );
		pDialog->SetStatic( m_oaPropCache[BMPPROP_RECORDCOUNT], m_DaggerTool.m_cifArchive.GetLastRecordCount() );
		break;

	case NT_ARCH3D:
		sprintf( sz, "%d / %d", m_nCurArch3DObject, NUM_UPPER_ARCH3D_RECORD );
		pDialog->SetStatic( m_oaPropCache[OBJPROP_OBJECT], sz );

		m_DaggerTool.m_Arch3D.GetRecordDesc( m_nCurArch3DObject, sz, 64 );
		pDialog->SetStatic( m_oaPropCache[OBJPROP_OBJECTID], sz );

		sprintf( sz, "%.1f", m_DaggerTool.m_Arch3D.GetVersionAsFloat() );
		pDialog->SetStatic( m_oaPropCache[OBJPROP_OBJECTVERSION], sz );

		if ( OBJDRAW_PUREFACES == m_dwDrawMode ) { 
			pDialog->SetStatic( m_oaPropCache[OBJPROP_FACECOUNT], m_DaggerTool.m_Arch3D.GetFaceCount() );

			// Current face
			pFace = m_DaggerTool.m_Arch3D.GetFace( m_nCurFace );
			if ( pFace ) {
				pDialog->SetStatic( m_oaPropCache[OBJPROP_SELFACE], m_nCurFace );
			}

			// Point count
			pDialog->SetStatic( m_oaPropCache[OBJPROP_POINTCOUNT], m_DaggerTool.m_Arch3D.GetPointCount( pFace ) );

			// Get Corners
			int CornerBuffer[NUM_MAX_CORNER_POINTS];
			int nCorners = m_DaggerTool.m_Arch3D.GetCornerPoints( pFace, CornerBuffer );
			pDialog->SetStatic( m_oaPropCache[OBJPROP_CORNERCOUNT], nCorners );

			// Corner points
			*sz = 0;
			for ( int c = 0; c < nCorners; c++ ) {
				if ( c != nCorners - 1 )
					sprintf( szi, "%d,", CornerBuffer[c] );
				else
					sprintf( szi, "%d", CornerBuffer[c] );
				strcat( sz, szi );
			}
			pDialog->SetStatic( m_oaPropCache[OBJPROP_CORNERPOINTS], sz );

			// Texture archive
			long nArchive, nRecord;
			m_DaggerTool.m_Arch3D.GetFaceTexture( pFace, &nArchive, &nRecord );
			sprintf( sz, "%d.%d", nArchive, nRecord );
			pDialog->SetStatic( m_oaPropCache[OBJPROP_ARCHIVE], sz );

			// Show texture width and height
			RECT rct;
			sprintf( sz, "%s%.3d", "TEXTURE.", nArchive );
			m_DaggerTool.OpenTextureArchive( sz );
			m_DaggerTool.m_dtxArchive.GetImageRect( nRecord, 0, &rct );
			sprintf( sz, "%dx%d", rct.right, rct.bottom );
			pDialog->SetStatic( m_oaPropCache[OBJPROP_TEXTUREDIM], sz );

			// Show point UV pairs
			*sz = 0;
			OBJVERTEX vert;
			int nPoints = m_DaggerTool.m_Arch3D.GetPointCount( pFace );
			if ( nPoints > 4 ) nPoints = 4;
			for ( int point = 0; point < nPoints; point++ ) {
				m_DaggerTool.m_Arch3D.GetPoint( pFace, point, &vert );
				if ( point != nPoints - 1 )
					sprintf( szi, "[%.1f %.1f],", vert.tu, vert.tv );
				else
					sprintf( szi, "[%.1f %.1f]", vert.tu, vert.tv );
				strcat( sz, szi );
			}
			pDialog->SetStatic( m_oaPropCache[OBJPROP_UVPAIRS], sz );
		}
		else {
			pDialog->SetStatic( m_oaPropCache[OBJPROP_FACECOUNT], m_meshView.GetFaceCount() );
		}

		pTree->Invalidate();
		break;

	case NT_BLOCKS:
		sprintf( sz, "%d / %d", m_nCurBlockObject, NUM_UPPER_BLOCK_RECORD );
		pDialog->SetStatic( m_oaPropCache[BLKPROP_OBJECT], sz );

		m_DaggerTool.m_ArchBlocks.GetRecordDesc( m_nCurBlockObject, sz, 64 );
		pDialog->SetStatic( m_oaPropCache[BLKPROP_FILENAME], sz );

		if ( m_DaggerTool.m_ArchBlocks.IsBlockOpen() ) {
			sprintf( sz, "%d / %d", m_nCurSubBlockObject, m_DaggerTool.m_ArchBlocks.GetBlockSubRecordCount() );
			pDialog->SetStatic( m_oaPropCache[BLKPROP_NUMRECORDS1], sz );

			// Outside data
			LPRMB_BLOCKHEADER pHeader = m_DaggerTool.m_ArchBlocks.GetOutsideBlockSubRecord( m_nCurSubBlockObject );
			if ( pHeader ) {
				pDialog->SetStatic( m_oaPropCache[BLKPROP_OUTSIDE_NUM3DOBJECTS], pHeader->n3DObjectRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_OUTSIDE_NUMFLATOBJECTS], pHeader->nFlatObjectsRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_OUTSIDE_NUMPEOPLE], pHeader->nPeopleRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_OUTSIDE_NUMDOORS], pHeader->nDoorRecords );
			}

			// Inside data
			pHeader = m_DaggerTool.m_ArchBlocks.GetInsideBlockSubRecord( m_nCurSubBlockObject );
			if ( pHeader ) {
				pDialog->SetStatic( m_oaPropCache[BLKPROP_INSIDE_NUM3DOBJECTS], pHeader->n3DObjectRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_INSIDE_NUMFLATOBJECTS], pHeader->nFlatObjectsRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_INSIDE_NUMPEOPLE], pHeader->nPeopleRecords );
				pDialog->SetStatic( m_oaPropCache[BLKPROP_INSIDE_NUMDOORS], pHeader->nDoorRecords );
			}
		}
		else {
			pDialog->SetStatic( m_oaPropCache[BLKPROP_NUMRECORDS1], "Unsupported Format" );
		}

		pTree->Invalidate();
		break;
	}

	return TRUE;
}/* SetPropertyView */


//
// BOOL FormatSpritePage()
// Format the sprites into a single page
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::FormatSpritePage()
{
	// Get display size
	SIZE szDisplay;
	m_Alchemy.GetDisplaySize( &szDisplay );

	// Format all sprites on the page
	ACSETTINGS_SPRITE acss;
	BOOL bEnabled;
	float cx = 0, cy = 0, w = 0, h = 0;
	int maxY = 0, offX = 2, offY = 2, nImageCount = 0;
	int nPageWidth = 0, nPageHeight = 0;
	ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
	UINT nSprite = m_Alchemy.FindFirstSprite();
	while ( ACNULL != nSprite ) {
		// Get sprite settings
		m_Alchemy.GetSprite( nSprite, &acss );

		// Get sprite dimensions
		w = m_Alchemy.GetSpriteWidth( nSprite ) * m_fBitmapScale;
		h = m_Alchemy.GetSpriteHeight( nSprite ) * m_fBitmapScale;

		// Calculate wrap position if sprite exceeds right edge
		if ( (cx+w) > szDisplay.cx && nImageCount > 0 ) {
			cx = 0;
			cy += (maxY + offY);
			nPageHeight += (maxY + offY);
			maxY = 0;
		}

		// Set sprite position and scale
		acss.vPos.x = cx - m_nScrollX;
		acss.vPos.y = cy - m_nScrollY;
		acss.vScaling.x = acss.vScaling.y = m_fBitmapScale;

		// Hide sprite if not visible
		RECT rctSprite;
		rctSprite.left = (long)acss.vPos.x;
		rctSprite.top = (long)acss.vPos.y;
		rctSprite.right = (long)(acss.vPos.x + w);
		rctSprite.bottom = (long)(acss.vPos.y + h);
		( rctSprite.top > szDisplay.cy || rctSprite.bottom < 0 ) ? bEnabled = FALSE : bEnabled = TRUE;

		// Calc next sprite position and determine max width of page
		cx += (w + offX);
		if ( cx > nPageWidth ) {
			nPageWidth = (DWORD)cx;
		}

		// Set the sprite
		m_Alchemy.SetSprite( nSprite, &acss, bEnabled );

		// Determine max height for row
		if ( h > maxY ) {
			maxY = (int)h;
		}

		// Get next sprite
		nImageCount++;
		if ( ACNULL == (nSprite = m_Alchemy.FindNextSprite( nSprite ) ) ) {
			// Add final row to page height
			nPageHeight += (DWORD)maxY;
		}
	}
	m_nPageWidth = nPageWidth;
	m_nPageHeight = nPageHeight;
	SetPageScrollers();

	return TRUE;
}/* FormatSpritePage */


//
// BOOL SetPageScrollers()
// Setup the scroll bars for this page
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::SetPageScrollers()
{
	// Get page size
	SIZE sz;
	m_Alchemy.GetDisplaySize( &sz );

	// Setup horizontal scroll bar details
	SCROLLINFO sih;
	ZeroMemory( &sih, sizeof(SCROLLINFO) );
	sih.cbSize = sizeof(SCROLLINFO);
	sih.fMask = SIF_DISABLENOSCROLL|SIF_RANGE|SIF_PAGE|SIF_POS;
	sih.nMin = 0;
	sih.nMax = m_nPageWidth;
	sih.nPage = sz.cx;
	sih.nPos = m_nScrollX;
	m_pMediaView->SetScrollInfo( SB_HORZ, &sih );

	// Setup vertical scroll bar details
	SCROLLINFO siv;
	ZeroMemory( &siv, sizeof(SCROLLINFO) );
	siv.cbSize = sizeof(SCROLLINFO);
	siv.fMask = SIF_DISABLENOSCROLL|SIF_RANGE|SIF_PAGE|SIF_POS;
	siv.nMin = 0;
	siv.nMax = m_nPageHeight;
	siv.nPage = sz.cy;
	siv.nPos = m_nScrollY;
	m_pMediaView->SetScrollInfo( SB_VERT, &siv );

	return TRUE;
}/* SetPageScrollers */


//
// void SelectSprite( int x, int y )
// Attempt to find a sprite beneath the specified coordinates and select it
//
void CDaggerfallExplorerApp::SelectSprite( int x, int y )
{
	// Not used for now
	return;

	// Process all sprites to see if one has been selected
	POINT pt = {x,y};
	RECT rctSprite;
	ACSETTINGS_SPRITE acss;
	UINT nSelected = NULL;
	UINT nSprite = m_Alchemy.FindFirstSprite();
	while ( nSprite ) {
		m_Alchemy.GetSpriteRect( nSprite, &rctSprite );
		if ( PtInRect( &rctSprite, pt ) ) {
			nSelected = nSprite;
		}
		nSprite = m_Alchemy.FindNextSprite( nSprite );
	}

	// Deselect current sprite
	if ( m_nSelectedSprite && m_nSelectedSprite != nSelected ) {
		m_Alchemy.GetSprite( m_nSelectedSprite, &acss );
		acss.colour = DECOLOUR_SPRITE;
		m_Alchemy.SetSprite( m_nSelectedSprite, &acss );
	}
	
	// Set selected sprite to selected colour
	if ( nSelected ) {
		m_Alchemy.GetSprite( nSelected, &acss );
		acss.colour = DECOLOUR_SELECTEDSPRITE;
		m_Alchemy.SetSprite( nSelected, &acss );
		
		// Store this selection
		m_nSelectedSprite = nSelected;
	}

	// Refresh the view
	DrawMediaView();
}/* SelectSprite */


//
// BOOL DrawMediaView()
// Render the media view
// Note: This will redraw the entire scene
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::DrawMediaView()
{
	if ( m_bLockView )
		return TRUE;

	// Format sprite page if required
	switch ( m_dwCurNodeType )
	{
	case NT_TEXTUREARCHIVE:
	case NT_IMGFILE:
	case NT_CIFFILE:
		FormatSpritePage();
		break;
	case NT_NONE:
		m_Alchemy.Clear();
		m_Alchemy.Present();
		return TRUE;
	}

	/*
	if ( NT_ARCH3D == m_dwCurNodeType ) {
		// Reopen 3D object if somehow still closed
		if ( !m_DaggerTool.m_Arch3D.IsObjectOpen() ) {
			m_DaggerTool.m_Arch3D.OpenObject( m_nCurArch3DObject );
		}
		_ASSERT( m_DaggerTool.m_Arch3D.IsObjectOpen() );
	}
	*/

	// Render to back buffer
	m_Alchemy.DoWork( ACDOWORK_CLEARDRAW );

	// Highlight selected pure face
	LPARCH3D_FACE pFace = NULL;
	if ( NT_ARCH3D == m_dwCurNodeType && OBJDRAW_PUREFACES == m_dwDrawMode  ) {
		LPDIRECT3DDEVICE8 pd3dDevice = NULL;
		BSAArch3D* pArch3D = &m_DaggerTool.m_Arch3D;

		// Count vertices up to but not including highlighted face
		DWORD dwVertCount = 0;
		for ( int nFace = 0; nFace < m_nCurFace; nFace++ ) {
			// Count vertices for this face
			pFace = pArch3D->GetFace( nFace );
			if ( !pFace ) {
				m_Alchemy.Present();
				return FALSE;
			}
			dwVertCount += pFace->nPointCount;
		}

		// Get details of face to highlight
		pFace = pArch3D->GetFace( m_nCurFace );
		if ( !pFace ) {
			m_Alchemy.Present();
			return FALSE;
		}

		// Acquire device
		m_Alchemy.GraphicsGetDevice( &pd3dDevice );

		// Get transform from actor
		D3DXMATRIX matTransform;
		m_Alchemy.GetTransformFromObject( m_nViewActor, &matTransform );

		// Set material for operation
		D3DMATERIAL8 mtrl;
		ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
		mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
		pd3dDevice->SetMaterial( &mtrl );

		// Set render state to ignore ZBuffer
		pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

		// Render face information
		int nPoints = pFace->nPointCount;
		LPAFMF_VERTEXDESC pVertBuffer = &m_meshView.m_pVerts[dwVertCount*2];
		if ( SUCCEEDED( pd3dDevice->BeginScene() ) ) {
			// Set transform and draw face
			pd3dDevice->SetVertexShader( D3DFVF_AFMF );
			pd3dDevice->SetTransform( D3DTS_WORLD, &matTransform );
			pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, nPoints, (LPVOID)pVertBuffer, sizeof(AFMF_VERTEXDESC) );

			// Acquire viewport
			D3DVIEWPORT8 vp;
			pd3dDevice->GetViewport( &vp );

			// Acquire transformation matricies
			D3DXMATRIX matProj, matView;
			m_Alchemy.GetTransform( D3DTS_PROJECTION, &matProj );
			m_Alchemy.GetTransform( D3DTS_VIEW, &matView );

			// Set new material
			ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
			mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
			mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
			mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
			mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
			pd3dDevice->SetMaterial( &mtrl );

			// Draw point numbers to show winding order
			char buffer[16];
			OBJVERTEX vIn, vOut;
			AFMF_VERTEXDESC vSrc;
			int nCurrentPoint = 0;
			for ( int point = 0; point < nPoints * 2; point += 2 ) {
				// Project into view space
				vSrc = pVertBuffer[point];
				vIn.pos.x = vSrc.x;
				vIn.pos.y = vSrc.y;
				vIn.pos.z = vSrc.z;
				D3DXVec3Project( &vOut.pos, &vIn.pos, &vp, &matProj, &matView, &matTransform );

				// Draw point number
				sprintf( buffer, "%d", nCurrentPoint++ );
				m_Alchemy.DrawText( m_nDefaultFont, vOut.pos.x, vOut.pos.y, D3DCOLOR_RGBA(0,0,0,255), buffer );
			}

			pd3dDevice->EndScene();

			// Reset render state
			pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		}

		RELEASE( pd3dDevice );
	}

	// Present the scene
	m_Alchemy.Present();

	return TRUE;
}/* DrawMediaView */


//
// void ResetObject()
// Reset object position to view default
//
void CDaggerfallExplorerApp::ResetObject()
{
	// Exit if no actor in use
	if ( ACNULL == m_nViewActor )
		return;

	// Get object enable state
	BOOL bEnabled = m_Alchemy.IsObjectEnabled( m_nViewActor );

	// Acquire object
	ACSETTINGS_ACTOR acsa;
	ZeroMemory( &acsa, sizeof(ACSETTINGS_ACTOR) );
	acsa.Object.dwSize = sizeof( ACSETTINGS_ACTOR );
	m_Alchemy.GetObject( m_nViewActor, &acsa );

	// Reset matrices to identity
	D3DXMatrixIdentity( &acsa.Object.matTranslate );
	D3DXMatrixIdentity( &acsa.Object.matRotate );

	// Set object position
	switch ( m_dwCurNodeType )
	{
	case NT_ARCH3D:
		// Set position
		acsa.Object.vPos = DEOBJECT_STARTPOS;
		acsa.Object.vSpinPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		break;

	case NT_BLOCKS:
		switch ( m_dwMouseMode )
		{
		case MOUSEMODE_OBJECT:
			// Set position and rotation
			acsa.Object.vPos = DEBLOCK_STARTPOSOBJECT;
			acsa.Object.vSpinPos = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(160.0f), D3DXToRadian(-3.0f));
			break;

		case MOUSEMODE_CAMERA:
			// Set position and rotation
			acsa.Object.vPos = DEBLOCK_STARTPOSCAMERA;
			acsa.Object.vSpinPos = D3DXVECTOR3(0.0f, D3DXToRadian(180.0f), 0.0f);
			break;
		}
		break;
	}

	// Store object
	m_Alchemy.SetObject( m_nViewActor, &acsa, bEnabled );
}/* ResetObject */


//
// void ResetCamera()
// Reset camera position and orientation to view default
//
void CDaggerfallExplorerApp::ResetCamera()
{
	// Reset camera position and orientation
	ACSETTINGS_CAMERA acsc;
	ZeroMemory( &acsc, sizeof(ACSETTINGS_CAMERA) );
	acsc.Object.dwSize = sizeof(ACSETTINGS_CAMERA);
	acsc.Object.vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_Alchemy.SetCamera( ACNULL, &acsc );
}/* ResetCamera */