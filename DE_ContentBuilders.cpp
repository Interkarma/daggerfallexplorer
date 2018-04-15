/*************************************************************************************************\
*
* Filename:		DE_ContentBuilders.cpp
* Purpose:		Implement content-related CDaggerfallExplorerApp functions
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
// BOOL CacheTextureArchive( LPCSTR pszName )
// Cache the currently selected texture archive into memory
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CacheTextureArchive( LPCSTR pszName )
{
	// Attempt to open texture archive
	if ( !m_DaggerTool.OpenTextureArchive( pszName ) )
		return FALSE;

	// Get pointer to current archive
	DTXArchive* pArchive = &m_DaggerTool.m_dtxArchive;

	// Clean up sprites
	m_Alchemy.PurgeSpriteStore();

	// Reset scroll information
	m_nScrollX = m_nScrollY = 0;
	m_nPageWidth = m_nPageHeight = 0;

	// Process all records in archive
	m_bSupressAnimateBitmaps = TRUE;
	for ( UINT record = 0; record < pArchive->GetRecordCount(); record++ )
	{
		// Get image rect for this record - skip if no dimension
		RECT rctImage;
		pArchive->GetImageRect( record, 0, &rctImage );
		if ( rctImage.right == 0 || rctImage.bottom == 0 )
			continue;

		// Process all images in record
		UINT nExtracted = 0;
		UINT nTexture = ACNULL;
		ACSETTINGS_SPRITE acss;
		UINT nImageCount = pArchive->GetImageCount( record );
		if ( nImageCount > 1 ) m_bSupressAnimateBitmaps = FALSE;
		for ( UINT image = 0; image < nImageCount; image++ )
		{
			// Get texture bits
			DTX_IMAGEDESC img;
			if ( !pArchive->GetImage( record, image, &img ) ) {
				return FALSE;
			}

			// Install texture based on animation
			if ( m_bAnimateBitmaps ) {
				// Append texture
				nTexture = m_Alchemy.InstallTextureFromMemory( nTexture, &rctImage, NUM_TEXTURE_PITCH, img.pBuffer, ACBD_32 );
			}
			else {
				// Create new texture
				nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &rctImage, NUM_TEXTURE_PITCH, img.pBuffer, ACBD_32 );
			}

			// Handle create failure
			if ( ACNULL == nTexture ) {
				return FALSE;
			}

			// Create an individual sprite for this texture if animation is disabled
			if ( !m_bAnimateBitmaps ) {
				ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
				acss.dwSize = sizeof(ACSETTINGS_SPRITE);
				acss.dwFlags = 0;
				acss.nTexture = nTexture;
				acss.vPos.x = acss.vPos.y = 0.0f;
				acss.vScaling.x = acss.vScaling.y = 1.0f;
				acss.colour = DECOLOUR_SPRITE;
				UINT nSprite = m_Alchemy.InstallSprite( &acss, NULL, TRUE );
				if ( ACNULL == nSprite ) {
					m_Alchemy.DecResRef( nTexture );
					return FALSE;
				}

				// Sprite will manage texture now, so we can decrement create reference count
				m_Alchemy.DecResRef( nTexture );
			}
		}// end for ( UINT image = 0; image < pArchive->GetImageCount( record ); image++ )

		// If animations are enabled only one sprite is required for each record
		if ( m_bAnimateBitmaps ) {
			ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
			acss.dwSize = sizeof(ACSETTINGS_SPRITE);
			acss.dwFlags = 0;
			acss.nTexture = nTexture;
			acss.vPos.x = acss.vPos.y = 0.0f;
			acss.vScaling.x = acss.vScaling.y = 1.0f;
			acss.colour = DECOLOUR_SPRITE;
			UINT nSprite = m_Alchemy.InstallSprite( &acss, NULL, TRUE );
			if ( ACNULL == nSprite ) {
				m_Alchemy.DecResRef( nTexture );
				return FALSE;
			}
			
			// Sprite will manage texture now, so we can decrement create reference count
			m_Alchemy.DecResRef( nTexture );
		}
	}// end for ( UINT record = 0; record < pArchive->GetRecordCount(); record++ )

	// Store current archive
	m_strCurTextureArchive = pszName;

	return TRUE;
}/* CacheTextureArchive */


//
// BOOL CacheIMGImage( LPCSTR pszName )
// Cache the currently selected IMG image into memory
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CacheIMGImage( LPCSTR pszName )
{
	// Acquire description of IMG file
	if ( !m_DaggerTool.GetIMGDesc( pszName, &m_idCurIMG ) )
		return FALSE;

	// Fail if no dimension
	if ( !m_idCurIMG.dwWidth || !m_idCurIMG.dwHeight )
		return FALSE;

	// Create a buffer to store image
	char *pBuffer = new char[m_idCurIMG.dwBufferSize];
	if ( !pBuffer )
		return FALSE;

	// Read image into buffer
	if ( !m_DaggerTool.GetIMGImage( pszName, pBuffer, &m_idCurIMG ) ) {
		delete[] pBuffer;
		return FALSE;
	}

	// Compose source rect
	RECT rctImage = {0, 0, m_idCurIMG.dwWidth, m_idCurIMG.dwHeight};
	
	// Install texture
	UINT nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &rctImage, rctImage.right, pBuffer, ACBD_32, NULL, NULL );
	if ( ACNULL == nTexture ) {
		delete[] pBuffer;
		return FALSE;
	}
	
	// Install sprite
	ACSETTINGS_SPRITE acss;
	ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
	acss.dwSize = sizeof(ACSETTINGS_SPRITE);
	acss.dwFlags = 0;
	acss.nTexture = nTexture;
	acss.vPos.x = acss.vPos.y = 0.0f;
	acss.vScaling.x = acss.vScaling.y = m_fBitmapScale;
	acss.colour = DECOLOUR_SPRITE;
	UINT nSprite = m_Alchemy.InstallSprite( &acss, NULL, TRUE );
	if ( ACNULL == nSprite ) {
		delete[] pBuffer;
		m_Alchemy.DecResRef( nTexture );
		return FALSE;
	}

	// Sprite will manage texture now, so we can decrement create reference count
	m_Alchemy.DecResRef( nTexture );

	// Clean up
	delete[] pBuffer;

	return TRUE;
}/* CacheIMGImage */


//
// BOOL CacheCIFArchive( LPCSTR pszName )
// Cache the currently selected CIF archive into memory
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CacheCIFArchive( LPCSTR pszName )
{
	// Attempt to open the archive
	if ( !m_DaggerTool.OpenCIFArchive( pszName ) )
		return FALSE;

	// Get pointer to current archive
	CIFArchive* pArchive = &m_DaggerTool.m_cifArchive;

	// Prepare structure to scan archive
	CIF_RECORDDESC cd;
	ZeroMemory( &cd, sizeof(CIF_RECORDDESC) );

	// Get first image
	if ( !pArchive->GetImage( &cd ) ) {
		return FALSE;
	}

	// Scan through all archives
	m_bSupressAnimateBitmaps = TRUE;
	BOOL bAppending = FALSE;
	UINT nTexture = ACNULL;
	while ( !cd.bFinished ) {
		// Skip if no dimension
		if ( !cd.cx || !cd.cy ) {
			// Get next image
			if ( !pArchive->GetImage( &cd ) ) {
				return FALSE;
			}
			
			continue;
		}

		// Compose source rect
		RECT rctImage = {0, 0, cd.cx, cd.cy};

		// Determine when to append texture
		if ( !cd.bWeaponGroup ) {
			// Always start a new texture if not a weapon group
			nTexture = ACNULL;
			bAppending = FALSE;
		}
		else {
			if ( cd.bFirstInGroup || !m_bAnimateBitmaps ) {
				// First image in weapon group gets a new texture
				nTexture = ACNULL;
				bAppending = FALSE;
			}
			else {
				// Every subsequent image in weapon group is appended to current texture
				bAppending = TRUE;
			}
		}

		// Install texture based on animation
		if ( m_bAnimateBitmaps && bAppending ) {
			// Append texture
			nTexture = m_Alchemy.InstallTextureFromMemory( nTexture, &rctImage, NUM_CIF_PITCH, cd.pBuffer, ACBD_32 );
			m_bSupressAnimateBitmaps = FALSE;
		}
		else {
			// Create new texture
			nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &rctImage, NUM_CIF_PITCH, cd.pBuffer, ACBD_32 );
		}
		
		// Handle create failure
		if ( ACNULL == nTexture ) {
			return FALSE;
		}
		
		// Install sprite
		if ( !bAppending ) {
			ACSETTINGS_SPRITE acss;
			ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
			acss.dwSize = sizeof(ACSETTINGS_SPRITE);
			acss.dwFlags = 0;
			acss.nTexture = nTexture;
			acss.vPos.x = acss.vPos.y = 0.0f;
			acss.vScaling.x = acss.vScaling.y = 1.0f;
			acss.colour = DECOLOUR_SPRITE;
			UINT nSprite = m_Alchemy.InstallSprite( &acss, NULL, TRUE );
			if ( ACNULL == nSprite ) {
				m_Alchemy.DecResRef( nTexture );
				return FALSE;
			}
			
			// Sprite will manage texture now, so we can decrement create reference count
			m_Alchemy.DecResRef( nTexture );
		}

		// Get next image
		if ( !pArchive->GetImage( &cd ) ) {
			return FALSE;
		}
	}// end while ( !cd.bFinished )

	return TRUE;
}/* CacheCIFArchive */


//
// BOOL CacheSkyArchive( LPCSTR pszName )
// Cache the currently selected sky archive into memory
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CacheSkyArchive( LPCSTR pszName )
{
	// Open palette
	CFile palFile;
	CString strPalette;
	palette_t pal;
	strPalette.Format( "%s\\%s", m_DaggerTool.m_strArena2Path, "PAL.PAL" );
	if ( !palFile.Open( strPalette, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) ) {
		return FALSE;
	}
	
	// Read the palette
	if ( strPalette.Right( 3 ) == "PAL" ) {
		palFile.Read( (void*)&pal, 768 );
	}
	else if ( strPalette.Right( 3 ) == "COL" )
	{
		palFile.Seek( 8, CFile::begin );
		palFile.Read( (void*)&pal, 768 );
	}
	else {
		palFile.Close();
		return FALSE;
	}
	palFile.Close();

	// Open the sky file
	CFile file;
	CString filename;
	filename.Format( "%s\\%s", m_DaggerTool.m_strArena2Path, pszName );
	if ( !file.Open( filename, CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary ) )
		return FALSE;

	// Create a buffer and read into memory
	char *pImageRaw = new char[(DWORD)file.GetLength()];
	if ( !pImageRaw ) {
		file.Close();
		return FALSE;
	}
	else {
		file.Read( pImageRaw, (DWORD)file.GetLength() );
	}

	// Set variables
	const DWORD dwSrcPitch = 512;
	const DWORD dwDstWidth = 800;
	const DWORD dwDstHeight = 600;
	const DWORD dwDstPitch = dwDstWidth*3;
	const DWORD dwDstLength = dwDstWidth*dwDstHeight*3;
	char *pImageSource = pImageRaw + (15800 * 256/*dwSrcPitch*/);

	// KLUDGE: Create an image buffer
	char *pData = new char[dwDstLength];
	ZeroMemory( pData, dwDstLength );

	// KLUDGE: Read parts of the file into the buffer
	for ( int y = 0; y < dwDstHeight; y++ ) {
		for ( int x = 0; x < dwDstWidth; x++ ) {
			// Determine palette index
			unsigned char nIndex = pImageSource[ (y * dwSrcPitch) + x ];

			// Extract the RGB colours for this pixel
			char r = pal.colour[nIndex].r;
			char g = pal.colour[nIndex].g;
			char b = pal.colour[nIndex].b;

			// Set the pixel colour
			unsigned char *pOut = (unsigned char*)&pData[(y*dwDstPitch)+(x*3)];
			pOut[0] = b;
			pOut[1] = g;
			pOut[2] = r;
		}
	}

	// KLUDGE: Create a texture from this image
	RECT rct = {0, 0, dwDstWidth, dwDstHeight};
	UINT nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &rct, dwDstWidth, pData, ACBD_32, "TestSkyImage", 0 );
	if ( ACNULL == nTexture ) {
		delete[] pData;
		delete[] pImageRaw;
		file.Close();
		return FALSE;
	}

	// KLUDGE: Install sprite for this texture
	ACSETTINGS_SPRITE acss;
	ZeroMemory( &acss, sizeof(ACSETTINGS_SPRITE) );
	acss.dwSize = sizeof(ACSETTINGS_SPRITE);
	acss.dwFlags = 0;
	acss.nTexture = nTexture;
	acss.vPos.x = acss.vPos.y = 0.0f;
	acss.vScaling.x = acss.vScaling.y = 1.0f;
	acss.colour = DECOLOUR_SPRITE;
	UINT nSprite = m_Alchemy.InstallSprite( &acss, NULL, TRUE );
	if ( ACNULL == nSprite ) {
		m_Alchemy.DecResRef( nTexture );
		delete[] pData;
		delete[] pImageRaw;
		file.Close();
		return FALSE;
	}

	// Sprite will manage texture now, so we can decrement create reference count
	m_Alchemy.DecResRef( nTexture );

	// Clean up
	delete[] pData;
	delete[] pImageRaw;
	file.Close();

	return TRUE;
}/* CacheSkyArchive */


//
// BOOL InitialiseMeshObject( TFlexibleMesh* pMesh, DWORD dwVertCount = 0, DWORD dwFaceCount = 0 )
// Correctly initialise a mesh object based on viewing properties
// Return: TRUE if successful, otherwise FALSE
//
inline BOOL CDaggerfallExplorerApp::InitialiseMeshObject( TFlexibleMesh* pMesh, DWORD dwVertCount/*=0*/, DWORD dwFaceCount/*=0*/ )
{
	_ASSERT( pMesh );

	// Set mesh flags
	DWORD dwFlags = 0;
	DWORD dwPrimType = D3DPT_TRIANGLELIST;
	switch ( m_dwDrawMode )
	{
	case OBJDRAW_PUREFACES:
		dwPrimType = D3DPT_LINELIST;
		break;

	case OBJDRAW_WIREFRAME:
		dwFlags = AFMFFLAG_DRAWWIREFRAME;
		break;

	case OBJDRAW_SOLID:
	case OBJDRAW_TEXTURED:
		break;

	default:
		return FALSE;
	}

	// Create new mesh object
	BOOL bResult = FALSE;
	if ( OBJDRAW_TEXTURED == m_dwDrawMode ) {
		bResult = pMesh->Create( dwVertCount, dwFaceCount, "store://", dwFlags, dwPrimType );
	}
	else {
		bResult = pMesh->Create( dwVertCount, dwFaceCount, NULL, dwFlags, dwPrimType );
	}

	return bResult;
}/* InitialiseMeshObject */


//
// BOOL	CacheDFTextureCombine( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut, TString& strCacheNameOut )
// Combines unique textures that will be installed into Alchemy
// Return: TRUE if successful, otherwise FALSE
//
inline BOOL CDaggerfallExplorerApp::CacheDFTextureCombine( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut, TString& strCacheNameOut )
{
	// Validate
	_ASSERT( pRectOut );

	// Check this texture has not already been cached
	LPDFTEXTURECACHE pCache = NULL;
	LPDFTEXTUREREF pIndex = m_haDFTextureCacheIndexCombine.GetObject( pszName );
	if ( pIndex ) {
		// Return cache RECT and name only
		pCache = m_oaDFTextureCache.GetObject( pIndex->nTexture );
		*pRectOut = pCache->oaSubset[pIndex->nSubset];
		strCacheNameOut = pCache->strName;
		return TRUE;
	}

	// Query current cache texture
	CString strCacheName;
	UINT nTexture = m_nLastDFTextureCache;
	if ( !nTexture ) {
		// Create first cache texture
		nTexture = m_oaDFTextureCache.NewTail();
		if ( !nTexture ) {
			return FALSE;
		}
		m_nLastDFTextureCache = nTexture;

		// Generate name for this texture cache
		pCache = m_oaDFTextureCache.GetObject( nTexture );
		strCacheName.Format( "DE%d", m_oaDFTextureCache.GetCount() );
		pCache->strName = strCacheName;
	}
	else {
		strCacheName = m_oaDFTextureCache.GetObject( m_nLastDFTextureCache )->strName;
	}

	// Get current cache texture
	pCache = m_oaDFTextureCache.GetObject( m_nLastDFTextureCache );

	// Open archive
	CString strFile;
	strFile.Format( "TEXTURE.%.3d", nArchive );
	if ( !m_DaggerTool.OpenTextureArchive( strFile ) ) {
		return FALSE;
	}

	// Get image dimensions
	int	cx, cy;
	RECT rctImage;
	m_DaggerTool.m_dtxArchive.GetImageRect( nRecord, 0, &rctImage );
	cx = rctImage.right - rctImage.left;
	cy = rctImage.bottom - rctImage.top;

	// This assertion is tripped if the texture is too large
	// No Daggerfall texture is beyond these dimensions
	_ASSERT( cx < 256 );
	_ASSERT( cy < 256 );

	// Determine if a new column must be started
	if ( pCache->cy + cy > DE_CACHETEXTURE_HEIGHT ) {
		// Start new column
		pCache->cy = 0;
		pCache->cx += pCache->maxWidth;
		pCache->maxWidth = 0;
	}

	// Determine if this texture will fit into the current cache object
	if ( pCache->cx + cx > DE_CACHETEXTURE_WIDTH ) {
		// Start a new cache object
		nTexture = m_oaDFTextureCache.NewTail();
		if ( !nTexture ) {
			return FALSE;
		}
		m_nLastDFTextureCache = nTexture;

		// Generate name for this texture cache
		strCacheName.Format( "DE%d", m_oaDFTextureCache.GetCount() );

		// Get current cache object
		pCache = m_oaDFTextureCache.GetObject( nTexture );
		if ( !pCache ) {
			return FALSE;
		}
		pCache->strName = strCacheName;
	}

	// Store image rectangle
	UINT nSubset = pCache->oaSubset.NewTail();
	if ( !nSubset ) {
		return FALSE;
	}
	LPRECT pRect = pCache->oaSubset.GetObject( nSubset );
	pRect->left = pCache->cx;
	pRect->top = pCache->cy;
	pRect->right = pCache->cx + cx;
	pRect->bottom = pCache->cy + cy;

	// Store image bits
	DTX_IMAGEDESC img;
	char *pBits = pCache->pBits + (pCache->cy * DE_CACHETEXTURE_PITCH) + (pCache->cx * 3);
	if ( !m_DaggerTool.m_dtxArchive.GetImage( nRecord, 0, &img, pBits, DE_CACHETEXTURE_PITCH ) ) {
		return FALSE;
	}

	// Update position in cache
	if ( cx > pCache->maxWidth ) {
		pCache->maxWidth = cx;
	}
	pCache->cy += cy;

	// Hash name to index and set details
	pIndex = m_haDFTextureCacheIndexCombine.New( pszName );
	if ( !pIndex ) {
		return FALSE;
	}
	pIndex->nTexture = nTexture;
	pIndex->nSubset = nSubset;

	// Store names and dimensions
	pCache->rctTexture.left = 0;
	pCache->rctTexture.top = 0;
	pCache->rctTexture.right = DE_CACHETEXTURE_WIDTH;
	pCache->rctTexture.bottom = DE_CACHETEXTURE_HEIGHT;
	strCacheNameOut = pCache->strName;

	// Set output RECT
	*pRectOut = *pRect;

	return TRUE;
}/* CacheDFTextureCombine */


//
// BOOL	CacheDFTextureVerbose( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut )
// Build a list of unique textures that will be added to the store
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CacheDFTextureVerbose( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut )
{
	// Check this texture has not already been cached
	LPDFTEXTURECACHE pCache = NULL;
	LPDFTEXTUREREF pIndex = m_haDFTextureCacheIndexVerbose.GetObject( pszName );
	if ( pIndex ) {
		// Return cache RECT and name only
		pCache = m_oaDFTextureCache.GetObject( pIndex->nTexture );
		pRectOut->left = pRectOut->top = 0;
		pRectOut->right = pCache->cx;
		pRectOut->bottom = pCache->cy;
		return TRUE;
	}

	// Create new cache texture
	UINT nTexture = m_oaDFTextureCache.NewTail();
	if ( !nTexture ) {
		return FALSE;
	}

	// Hash name to index and set details
	pIndex = m_haDFTextureCacheIndexVerbose.New( pszName );
	if ( !pIndex ) {
		m_oaDFTextureCache.Delete( nTexture );
		return FALSE;
	}
	pIndex->nTexture = nTexture;
	pIndex->nSubset = ACNULL;

	// Open archive
	CString strFile;
	strFile.Format( "TEXTURE.%.3d", nArchive );
	m_DaggerTool.OpenTextureArchive( strFile );

	// Get image rect
	RECT rctImage;
	m_DaggerTool.m_dtxArchive.GetImageRect( nRecord, 0, &rctImage );

	// Store image bits
	DTX_IMAGEDESC img;
	pCache = m_oaDFTextureCache.GetObject( nTexture );
	char *pBits = pCache->pBits + (pCache->cy * DE_CACHETEXTURE_PITCH) + (pCache->cx * 3);
	if ( !m_DaggerTool.m_dtxArchive.GetImage( nRecord, 0, &img, pBits, DE_CACHETEXTURE_PITCH ) ) {
		m_oaDFTextureCache.Delete( nTexture );
		m_haDFTextureCacheIndexVerbose.Delete( pszName );
		return FALSE;
	}

	// Set properties
	pCache->strName = pszName;
	pCache->cx = rctImage.right;
	pCache->cy = rctImage.bottom;

	// Set outgoing properties
	pRectOut->left = pRectOut->top = 0;
	pRectOut->right = pCache->cx;
	pRectOut->bottom = pCache->cy;
	pCache->rctTexture = *pRectOut;

	return TRUE;
}/* CacheDFTextureVerbose */


//
// BOOL CommitDFTextures()
// Commit all cached textures to the Alchemy store
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::CommitDFTextures()
{
	// Process all items in list
	UINT nRef = m_oaDFTextureCache.GetRoot();
	while ( nRef ) {
		// Install texture
		LPDFTEXTURECACHE pCache = m_oaDFTextureCache.GetObject( nRef );
		UINT nTexture = m_Alchemy.InstallTextureFromMemory( ACNULL, &pCache->rctTexture, DE_CACHETEXTURE_WIDTH, pCache->pBits, ACBD_32, pCache->strName );
		if ( ACNULL == nTexture ) {
			ReleaseDFTextures();
			return FALSE;
		}

		// Get next item in list
		nRef = m_oaDFTextureCache.GetNext( nRef );
	}

	return TRUE;
}/* CommitDFTextures */


//
// void ReleaseDFTextures()
// Dereference textures from the Alchemy store and clear list
// This puts the texture in the hands of Alchemy
//
void CDaggerfallExplorerApp::ReleaseDFTextures()
{
	// Process all items in list
	UINT nRef = m_oaDFTextureCache.GetRoot();
	while ( nRef ) {
		// Get reference to texture
		UINT nTexture = m_Alchemy.FindResource( m_oaDFTextureCache[nRef].strName );

		// Remove texture if present
		if ( ACNULL != nTexture ) {
			m_Alchemy.DecResRef( nTexture );
		}

		// Get next item in list
		nRef = m_oaDFTextureCache.GetNext( nRef );
	}

	m_oaDFTextureCache.Destroy();
	m_haDFTextureCacheIndexCombine.Destroy();
	m_haDFTextureCacheIndexVerbose.Destroy();
	m_nLastDFTextureCache = NULL;
}/* ReleaseDFTextures */


//
// BOOL ApplyRegion( long *pArchive )
// Change texture archive based on supported regions
// Return: TRUE if successful, otherwise FALSE
//
inline BOOL CDaggerfallExplorerApp::ApplyRegion( long *pArchive )
{
	_ASSERT( pArchive );

	// Exit if not using processor
	if ( TERRAIN_NONE == m_dwTerrain ) {
		return TRUE;
	}

	// Determine whether to apply climate offset
	long nClimate = 0;
	if ( WEATHER_WINTER == m_dwWeather && TERRAIN_DESERT != m_dwTerrain ) {
		nClimate = 1;
	}

	// Find base index index of this texture
	long nBase = *pArchive / 100;

	// Process region against texture archive
	long nIndex = *pArchive - (nBase * 100);
	switch ( nIndex )
	{
		case STO_TERRAIN:
		case STO_RUINS:
		case STO_CASTLE:
		case STO_CITYA:
		case STO_CITYB:
		case STO_CITYWALLS:
		case STO_FARM:
		case STO_FENCES:
		case STO_MAGESGUILD:
		case STO_MANOR:
		case STO_MERCHANTHOMES:
		case STO_TAVERNEXTERIORS:
		case STO_TEMPLEEXTERIORS:
		case STO_VILLAGE:
		case STO_ROOFS:
			*pArchive = (nIndex += m_dwTerrain + nClimate);
			break;

		default:
			break;
	};

	return TRUE;
}/* ApplyRegion */


//
// BuildMesh( long nObject, TFlexibleMesh* pMeshOut )
// Construct a mesh object
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerfallExplorerApp::BuildMesh( long nObject, TFlexibleMesh* pMeshOut )
{
	_ASSERT( pMeshOut );

	// Open Arch3D object
	BSAArch3D* pArch3D = &m_DaggerTool.m_Arch3D;
	if ( !pArch3D->OpenObject( nObject ) )
		return FALSE;

	// Add face information to mesh
	int nFace = 0;
	DWORD dwTotalVertCount = 0;
	LPARCH3D_FACE pFace = NULL;
	if ( OBJDRAW_PUREFACES == m_dwDrawMode )
	{
		// Iterate once through faces to get total vertex count
		int nPureFaces = pArch3D->GetFaceCount();
		for ( nFace = 0; nFace < nPureFaces; nFace++ ) {
			pFace = pArch3D->GetFace( nFace );
			if ( !pFace ) {
				return FALSE;
			}
			dwTotalVertCount += pArch3D->GetPointCount( pFace );
		}

		// Buffer must be double size to correctly support line segments
		dwTotalVertCount *= 2;

		// Create temporary vertex buffer
		LPOBJVERTEX pVertBuffer = new OBJVERTEX[dwTotalVertCount];
		if ( !pVertBuffer )
			return FALSE;
		
		// Create new mesh object
		InitialiseMeshObject( pMeshOut, dwTotalVertCount );

		// Load pure face vertices into temporary buffer as line segments
		DWORD dwCurVertex = 0;
		int nCount = 0;
		for ( nFace = 0; nFace < nPureFaces; nFace++ ) {
			pFace = pArch3D->GetFace( nFace );
			if ( !pArch3D->ExportPureFace( pFace, pVertBuffer + dwCurVertex, &nCount ) ) {
				delete[] pVertBuffer;
				return FALSE;
			}
			
			dwCurVertex += nCount;
		}
		
		// Transfer temporary vertices to mesh object
		AFMF_VERTEXDESC avd;
		for ( DWORD dwVertex = 0; dwVertex < dwTotalVertCount; dwVertex++ ) {
			avd.x = pVertBuffer[dwVertex].pos.x;
			avd.y = pVertBuffer[dwVertex].pos.y;
			avd.z = pVertBuffer[dwVertex].pos.z;
			avd.nx = pVertBuffer[dwVertex].normal.x;
			avd.ny = pVertBuffer[dwVertex].normal.y;
			avd.nz = pVertBuffer[dwVertex].normal.z;
			avd.tu = pVertBuffer[dwVertex].tu;
			avd.tv = pVertBuffer[dwVertex].tv;
			pMeshOut->SetVertex( dwVertex, &avd );
		}

		// Free temporary vertex buffer
		delete[] pVertBuffer;
	}
	else {
		// Create a list of triangulated faces and cache textures
		UINT fl;
		CString str;
		int nCount, nPureFaces = pArch3D->GetFaceCount();
		long nArchive, nRecord;
		TLinkedList<WIREFACE> oaFace;
		for ( nFace = 0; nFace < nPureFaces; nFace++ ) {
			// Extend list by one face
			if ( NULL == (fl = oaFace.NewTail() ) ) {
				return FALSE;
			}

			// Get this face
			pFace = pArch3D->GetFace( nFace );
			if ( !pFace ) {
				return FALSE;
			}

			// Choose face export method based on texture mode
			if ( OBJDRAW_TEXTURED == m_dwDrawMode ) {				
				// Get texture assigned to this face
				m_DaggerTool.m_Arch3D.GetFaceTexture( pFace, &nArchive, &nRecord );

				// Adjust texture for weather and region
				if ( NT_BLOCKS == m_dwCurNodeType ) {
					ApplyRegion( &nArchive );
				}

				// TEXTURE.036 records do not match TEXTURE.035
				// Work around by setting back to 035
				if ( nArchive == 36 ) {
					nArchive = 35;
				}

				// TEXTURE.409 does not have a matching winter record 4 in TEXTURE.410
				// Work around by setting back to 409.4
				if ( nArchive == 410 && nRecord == 4 ) {
					nArchive = 409;
				}

				// TEXTURE.435 does not have a matching winter archive at all
				// ie. There is no TEXTURE.436, it's missing
				// Work around by setting back to 435
				if ( nArchive == 436 ) {
					nArchive = 435;
				}

				// Compose texture name
				str.Format( "%d.%d", nArchive, nRecord );

				// Cache texture
				RECT rct;
				if ( !CacheDFTextureVerbose( str, nArchive, nRecord, &rct ) ) {
					return FALSE;
				}

				// Store cache texture name in face list
				oaFace[fl].strTexture = str;

				// Export triangulated face
				SIZE sz = {rct.right, rct.bottom};
				if ( !pArch3D->ExportTriangulatedFace( pFace, oaFace[fl].verts, &nCount, &sz, &rct ) ) {
					return FALSE;
				}
			}
			else {
				// Export triangulated face
				if ( !pArch3D->ExportTriangulatedFace( pFace, oaFace[fl].verts, &nCount ) ) {
					return FALSE;
				}
			}// end if ( OBJDRAW_TEXTURED == m_dwDrawMode )

			// Update vertex count
			oaFace[fl].dwVertCount = nCount;
			dwTotalVertCount += nCount;
		}// end for ( nFace = 0; nFace < nPureFaces; nFace++ )

		// Set mesh vertex and face counts
		InitialiseMeshObject( pMeshOut, dwTotalVertCount, (dwTotalVertCount/3) );

		// Transfer faces to mesh object
		nFace = 0;
		LPOBJVERTEX pv;
		AFMF_VERTEXDESC avd;
		AFMF_FACEDESC afd;
		unsigned short nCurVertex = 0, nBaseVertex = 0;
		fl = oaFace.GetRoot();
		while ( fl ) {
			pv = oaFace[fl].verts;
			while ( nCurVertex < oaFace[fl].dwVertCount ) {
				// Compose vertex
				for ( int nVertex = 0; nVertex < 3; nVertex++ ) {
					avd.x = pv[nVertex].pos.x;
					avd.y = pv[nVertex].pos.y;
					avd.z = pv[nVertex].pos.z;
					avd.nx = pv[nVertex].normal.x;
					avd.ny = pv[nVertex].normal.y;
					avd.nz = pv[nVertex].normal.z;
					avd.tu = pv[nVertex].tu;
					avd.tv = pv[nVertex].tv;
					pMeshOut->SetVertex( nBaseVertex + nVertex, &avd );
				}// end for ( int nVertex = 0; nVertex < 3; nVertex++ )
				
				// Compose face
				afd.v0 = nBaseVertex;
				afd.v1 = nBaseVertex + 1;
				afd.v2 = nBaseVertex + 2;
				nCurVertex += 3;
				nBaseVertex += 3;
				pv += 3;

				// Store face in mesh
				if ( OBJDRAW_TEXTURED == m_dwDrawMode )
					pMeshOut->SetFace( nFace++, &afd, oaFace[fl].strTexture );
				else
					pMeshOut->SetFace( nFace++, &afd );
			}// end while ( nCurVertex < oaFace[fl].dwVertCount )

			nCurVertex = 0;
			fl = oaFace.GetNext( fl );
		}// end while ( fl )
	}// end if ( OBJDRAW_PUREFACES == m_dwDrawMode )

	return TRUE;
}/* BuildMesh */


//
// BOOL BuildBlockGroundMesh( LPRMBFLD_SMALLMAPDATA pMap, TFlexibleMesh* pMeshOut )
// Builds a mesh for the land under blocks
// Return: TRUE if successful, otherwise FALSE
// Note: The top-left corner of the mesh is at 0,0,0 - the scale is 1:1 to DF
//
BOOL CDaggerfallExplorerApp::BuildBlockGroundMesh( LPRMBFLD_SMALLMAPDATA pMap, TFlexibleMesh* pMeshOut )
{
	_ASSERT( pMap );
	_ASSERT( pMeshOut );

	// Construct mesh
	int x, y, v;
	DWORD dwVertex = 0;
	float fSide = 256.0f;
	if ( OBJDRAW_PUREFACES == m_dwDrawMode ) {
		// Describe pure face ground segment
		float pfSegment[] = {
			0.0f,	0.0f,
			0.0f,	fSide,
			0.0f,	fSide,
			fSide,	fSide,
			fSide,	fSide,
			fSide,	0.0f,
			fSide,	0.0f,
			0.0f,	0.0f,
		};

		// Construct vertices for the ground mesh
		AFMF_VERTEXDESC avd;
		ZeroMemory( &avd, sizeof(AFMF_VERTEXDESC) );
		InitialiseMeshObject( pMeshOut, 2048 );
		for ( y = 0; y < 16; y++ ) {
			for ( x = 0; x < 16; x++ ) {
				for ( v = 0; v < 16; v += 4 ) {
					avd.x = fSide * x + pfSegment[v];
					avd.z = fSide * y + pfSegment[v+1];
					pMeshOut->SetVertex( dwVertex++, &avd );
					avd.x = fSide * x + pfSegment[v+2];
					avd.z = fSide * y + pfSegment[v+3];
					pMeshOut->SetVertex( dwVertex++, &avd );
				}// end for ( v = 0; v < 16; v += 4 )
			}// end for ( x = 0; x < 16; x++ )
		}// end for ( y = 0; y < 16; y++ )
	}
	else {
		// Construct vertices, faces, and textures for the ground mesh
		CString str;
		UINT nFace = 0;
		AFMF_VERTEXDESC avd[4];
		AFMF_FACEDESC afd;
		ZeroMemory( &avd, sizeof(AFMF_VERTEXDESC) * 4 );
		ZeroMemory( &afd, sizeof(AFMF_FACEDESC) );
		InitialiseMeshObject( pMeshOut, 1024, 512 );
		for ( y = 0; y < 16; y++ ) {
			for ( x = 0; x < 16; x++ ) {
				// Add vertex 0
				avd[0].x = fSide * x;
				avd[0].z = fSide * y;
				avd[0].tu = 0.0f;
				avd[0].tv = 0.0f;

				// Add vertex 1
				avd[1].x = fSide * x;
				avd[1].z = fSide * (y+1);
				avd[1].tu = 0.0f;
				avd[1].tv = 0.0f;

				// Add vertex 2
				avd[2].x = fSide * (x+1);
				avd[2].z = fSide * y;
				avd[2].tu = 0.0f;
				avd[2].tv = 0.0f;

				// Add vertex 3
				avd[3].x = fSide * (x+1);
				avd[3].z = fSide * (y+1);
				avd[3].tu = 0.0f;
				avd[3].tv = 0.0f;

				// Cache and align texture
				TString strCacheName = str;
				if ( OBJDRAW_TEXTURED == m_dwDrawMode ) {
					long nArchive = 302;	// Temperate is the most generic texture set, so say I
					if ( TERRAIN_NONE != m_dwTerrain ) {
						// Choose archive based on terrain and weather
						nArchive = m_dwTerrain + 2 + m_dwWeather;
					}

					// Get record details
					long nRecord = pMap->TextureInfo[y][15-x] & 0x3f;
					bool bRotate = ((pMap->TextureInfo[y][15-x] & 0x40) == 0x40) ? true : false;
					bool bFlip = ((pMap->TextureInfo[y][15-x] & 0x80) == 0x80) ? true : false;

					// Compose texture name
					str.Format( "%d.%d", nArchive, nRecord );

					// Cache texture
					//_ASSERT( nRecord < 56 );
					RECT subRect = {0,0,0,0};
					if ( nRecord > 55 ) {
						// Some records are above the 0-55 range?
						// For now just set them to "water"
						nRecord = 0;
					}
					if ( !CacheDFTextureCombine( str, nArchive, nRecord, &subRect, strCacheName ) ) {
						return FALSE;
					}

					// Align texture UV based on properties
					float fm = 0.01f;
					float ft = 0.99f;
					if ( !bFlip && !bRotate ) {
						// Standard
						avd[2].tu = fm;
						avd[2].tv = fm;
						avd[3].tu = fm;
						avd[3].tv = ft;
						avd[0].tu = ft;
						avd[0].tv = fm;
						avd[1].tu = ft;
						avd[1].tv = ft;
					}
					else if ( bFlip && !bRotate ) {
						// Flip
						avd[1].tu = fm;
						avd[1].tv = fm;
						avd[0].tu = fm;
						avd[0].tv = ft;
						avd[3].tu = ft;
						avd[3].tv = fm;
						avd[2].tu = ft;
						avd[2].tv = ft;
					}
					else if ( !bFlip && bRotate ) {
						// Rotate
						avd[3].tu = fm;
						avd[3].tv = fm;
						avd[1].tu = fm;
						avd[1].tv = ft;
						avd[2].tu = ft;
						avd[2].tv = fm;
						avd[0].tu = ft;
						avd[0].tv = ft;
					}
					else if ( bFlip && bRotate ){
						// Flip and Rotate
						avd[0].tu = fm;
						avd[0].tv = fm;
						avd[2].tu = fm;
						avd[2].tv = ft;
						avd[1].tu = ft;
						avd[1].tv = fm;
						avd[3].tu = ft;
						avd[3].tv = ft;
					}
					else {
						// Unknown
						avd[0].tu = fm;
						avd[0].tv = fm;
						avd[1].tu = fm;
						avd[1].tv = fm;
						avd[2].tu = fm;
						avd[2].tv = fm;
						avd[3].tu = fm;
						avd[3].tv = fm;
					}

					// Adjust UV coordinates to match correct subset of source texture
					float stu, stv;
					for ( v = 0; v < 4; v++ ) {
						stu = (float)subRect.left / DE_CACHETEXTURE_WIDTH;
						stv = (float)subRect.top / DE_CACHETEXTURE_HEIGHT;
						avd[v].tu = (avd[v].tu * 0.25f) + stu;
						avd[v].tv = (avd[v].tv * 0.25f) + stv;
					}
				}// end if ( OBJDRAW_TEXTURED == m_dwDrawMode )

				// Add vertices
				pMeshOut->SetVertex( dwVertex, &avd[0] );
				pMeshOut->SetVertex( dwVertex+1, &avd[1] );
				pMeshOut->SetVertex( dwVertex+2, &avd[2] );
				pMeshOut->SetVertex( dwVertex+3, &avd[3] );

				// Add triangle 0
				afd.v0 = (unsigned short)dwVertex;
				afd.v1 = (unsigned short)dwVertex+1;
				afd.v2 = (unsigned short)dwVertex+2;
				pMeshOut->SetFace( nFace++, &afd, strCacheName );

				// Add triangle 1
				afd.v0 = (unsigned short)dwVertex+2;
				afd.v1 = (unsigned short)dwVertex+1;
				afd.v2 = (unsigned short)dwVertex+3;
				pMeshOut->SetFace( nFace++, &afd, strCacheName );

				dwVertex += 4;
			}
		}
	}// end if ( OBJDRAW_PUREFACES == m_dwDrawMode )

	return TRUE;
}/* BuildBlockGroundMesh */