/*************************************************************************************************\
*
* Filename:		DaggerTool.cpp
* Purpose:		Implement all Daggerfall tool classes
* Version:		0.90
* Author:		Gavin Clayton
*
* Last Updated:	29/08/2002
*
* Copyright 2002. Gavin Clayton. All Rights Reserved.
*

*
* NOTE:
* This information is derived from Dave Humphrey's DF hacking articles (http://www.m0use.net/~uesp).
* I have occasionally used different naming conventions and expanded based on my own investigations.
* I am deeply grateful to Dave Humphrey for his work. This would not be possible without him.
*
* If changes are made by you to this code, please log them below:
*
*
\*************************************************************************************************/


#include "stdafx.h"
#include "DaggerTool.h"
#include "Other/DH/DFFaceTex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// BSABlocks Construction / Destruction
//////////////////////////////////////////////////////////////////////


BSABlocks::BSABlocks()
{
	// Initialise
	m_pRecordDirectory = NULL;
	m_pFLD = NULL;
	m_nBlockSubRecordCount = m_nOutsideBlockSubRecordCount = m_nInsideBlockSubRecordCount = 0;
}/* BSABlocks */


BSABlocks::~BSABlocks()
{
	Close();
}/* ~BSABlocks */


//////////////////////////////////////////////////////////////////////
// BSABlocks Member Functions
//////////////////////////////////////////////////////////////////////


bool BSABlocks::Open( LPCSTR pszPath, BOOL bReadOnly/*=TRUE*/ )
{
	// Open the blocm
	if ( !BSAArchive::Open( pszPath, "BLOCKS.BSA", bReadOnly ) )
		return false;

	m_pRecordDirectory = m_pRecordDirectoryLong;

	return true;
}/* Open */


void BSABlocks::Close()
{
	if ( !IsOpen() )
		return;

	if ( IsBlockOpen() )
		CloseBlock();

	BSAArchive::Close();

	// Initialise
	m_pRecordDirectory = NULL;
}/* Close */


bool BSABlocks::OpenBlock( long nBlock )
{
	// Archive must be open
	if ( !IsOpen() )
		return false;

	// nBlock must be within limits
	_ASSERT( nBlock >= 0 && nBlock <= GetRecordCount() );

	// Close any existing object
	if ( IsBlockOpen() )
		CloseBlock();

	// Page the record into RAM
	char *pData;
	long nLength = GetRecordLength( nBlock );
	try {
		pData = new char[nLength];
	}
	catch( ... ) {
		return false;
	}
	if ( !pData ) return false;
	if ( !GetRecord( nBlock, pData, nLength ) ) {
		delete[] pData;
		return false;
	}

	// Check filename, as only RMB objects are known at this time
	char sz[64];
	GetRecordDesc( nBlock, sz, 64 );
	CString strFilename = sz;
	if ( strFilename.Right(4) != ".RMB" ) {
		delete[] pData;
		return false;
	}

	// Set values
	m_bIsBlockOpen = true;
	m_pData = pData;
	m_nDataLength = nLength;
	m_pFLD = (LPRMBFLD)pData;
	m_nCurBlock = nBlock;
	m_nBlockSubRecordCount = m_pFLD->nSubRecords1;

	return true;
}/* OpenBlock */


void BSABlocks::CloseBlock()
{
	if ( !IsBlockOpen() )
		return;

	// Initialise
	if ( m_pData ) {
		delete[] m_pData;
		m_pData = NULL;
	}
	m_nDataLength = 0;
	m_bIsBlockOpen = false;
	m_pFLD = NULL;
	m_nBlockSubRecordCount = m_nOutsideBlockSubRecordCount = m_nInsideBlockSubRecordCount = 0;
}/* CloseObject */


LPRMB_BLOCKHEADER BSABlocks::GetOutsideBlockSubRecord( long nRecord )
{
	_ASSERT( nRecord <= m_nBlockSubRecordCount );

	if ( !IsBlockOpen() )
		return NULL;

	// Exit NULL if this block has no subrecords
	if ( 0 == m_nBlockSubRecordCount ) {
		return NULL;
	}

	// Compute offset to outside header
	DWORD dwOffset = 0;
	for ( long r = 0; r < nRecord; r++ ) {
		dwOffset += m_pFLD->Offsets[r];
	}

	// Find start of specified outside header
	char *pData = m_pData + sizeof(RMBFLD) + dwOffset;

	return (LPRMB_BLOCKHEADER)pData;
}/* GetOutsideBlockSubRecord */


LPRMB_BLOCKHEADER BSABlocks::GetInsideBlockSubRecord( long nRecord )
{
	// Get outside header for this subrecord
	LPRMB_BLOCKHEADER pOutside = GetOutsideBlockSubRecord( nRecord );
	if ( !pOutside )
		return NULL;

	// Compute offset to inside header
	char *pData = (char*)pOutside;
	pData += sizeof(RMB_BLOCKHEADER);
	pData += sizeof(RMB_BLOCK3DOBJECTS) * pOutside->n3DObjectRecords;
	pData += sizeof(RMB_BLOCKFLATOBJECTS) * pOutside->nFlatObjectsRecords;
	pData += sizeof(RMB_BLOCKDATA3) * pOutside->nSection3Records;
	pData += sizeof(RMB_BLOCKPEOPLEOBJECTS) * pOutside->nPeopleRecords;
	pData += sizeof(RMB_BLOCKDOOROBJECTS) * pOutside->nDoorRecords;

	return (LPRMB_BLOCKHEADER)pData;
}/* GetInsideBlockSubRecord */


LPRMB_BLOCK3DOBJECTS BSABlocks::GetBlockSubRecord3DObjects( LPRMB_BLOCKHEADER pBlockHeader )
{
	_ASSERT( pBlockHeader );

	if ( !IsBlockOpen() )
		return NULL;

	// Calculate offset to 3D objects
	char *pData = (char*)pBlockHeader;
	pData += sizeof(RMB_BLOCKHEADER);

	return (LPRMB_BLOCK3DOBJECTS)pData;
}/* GetBlockSubRecord3DObjects */


LPRMB_BLOCK3DOBJECTS BSABlocks::GetBlock3DObjects()
{
	if ( !IsBlockOpen() )
		return NULL;

	// Get start of data
	char* pData = (char*)GetOutsideBlockSubRecord( m_nBlockSubRecordCount );

	return (LPRMB_BLOCK3DOBJECTS)pData;
}/* GetBlock3DObjects */


bool BSABlocks::GetBlockAutomap( LPBLKIMG_AUTOMAP pAutomapOut )
{
	// Validate
	_ASSERT( pAutomapOut );

	if ( !IsBlockOpen() )
		return false;

	// Locate automap pixel data
	char *pImageRaw = m_pFLD->Automap;

	// Set colors of image buffer in R8G8B8 format
	int cx = 64;
	int cy = 64;
	for ( int y = 0; y < cy; y++ )
	{
		for ( int x = 0; x < cx; x++ )
		{
			// Determine palette index
			unsigned char nIndex = pImageRaw[ y * 64 + x ];

			// Extract the RGB colours for this pixel
			unsigned char r = 200, g = 200, b = 200;
			switch ( nIndex )
			{
			case 0x00:
				r = g = b = 0;
				break;

			case 0x03:
				r = 240;
				g = 120;
				b = 0;
				break;

			case 0x10:
				r = 0;
				g = 200;
				b = 0;
				break;

			case 0x12:
			case 0x13:
			case 0x14:
				r = 100;
				g = 100;
				b = 50;
				break;

			case 0x0e:
				r = 1;
				g = 1;
				b = 1;
				break;

			case 0xfa:
			case 0xfb:
				r = 0;
				g = 0;
				b = 200;
				break;
			}

			// Set the pixel colour
			unsigned char *pOut = (unsigned char*)&pAutomapOut->pBuffer[(y*BLKSIZE_IMAGE_PITCH)+(x*3)];
			pOut[0] = b;
			pOut[1] = g;
			pOut[2] = r;
		}// end for ( x = 0; x < cx; x++ )
	}// end for ( y = 0; y < cy; y++ )

	return true;
}/* GetBlockAutomap */


//////////////////////////////////////////////////////////////////////
// BSAMaps Construction / Destruction
//////////////////////////////////////////////////////////////////////


BSAMaps::BSAMaps()
{
	// Initialise
	m_pRecordDirectory = NULL;
}/* BSAMaps */


BSAMaps::~BSAMaps()
{
	Close();
}/* ~BSAMaps */


//////////////////////////////////////////////////////////////////////
// BSAMaps Member Functions
//////////////////////////////////////////////////////////////////////


bool BSAMaps::Open( LPCSTR pszPath, BOOL bReadOnly/*=TRUE*/ )
{
	if ( !BSAArchive::Open( pszPath, "MAPS.BSA", bReadOnly ) )
		return false;

	m_pRecordDirectory = m_pRecordDirectoryLong;

	return true;
}/* Open */


void BSAMaps::Close()
{
	if ( !IsOpen() )
		return;

	BSAArchive::Close();

	// Initialise
	m_pRecordDirectory = NULL;
}/* Close */


//////////////////////////////////////////////////////////////////////
// BSAArch3D Construction / Destruction
//////////////////////////////////////////////////////////////////////


BSAArch3D::BSAArch3D()
{
	// Initialise
	m_bIsObjectOpen = false;
	m_pData = NULL;
	m_nDataLength = 0;
	m_pHeader = NULL;
	m_pPointList = NULL;
	m_pNormalList = NULL;
	m_lVersion = 0;
	m_pRecordDirectory = NULL;
}/* BSAArch3D */


BSAArch3D::~BSAArch3D()
{
	Close();
}/* ~BSAArch3D */


//////////////////////////////////////////////////////////////////////
// BSAArch3D Member Functions
//////////////////////////////////////////////////////////////////////


bool BSAArch3D::Open( LPCSTR pszPath, BOOL bReadOnly/*=TRUE*/ )
{
	if ( !BSAArchive::Open( pszPath, "ARCH3D.BSA", bReadOnly ) )
		return false;

	m_pRecordDirectory = m_pRecordDirectoryShort;

	// Index all objects by ID
	char sz[64];
	UINT* pn;
	char r = 'a';
	for ( long n = 0; n < NUM_UPPER_ARCH3D_RECORD; n++ ) {
		GetRecordDesc( n, sz, 64 );
		pn = m_haObjectID.New( sz );
		if ( !pn ) {
			// Resolve conflict
			strncat( sz, &r, 1 );
			r++;
			pn = m_haObjectID.New( sz );
			_ASSERT( pn );
			if ( pn ) {
				*pn = n;
			}
		}
		else {
			// Set number
			*pn = n;
		}
	}

	return true;
}/* Open */


void BSAArch3D::Close()
{
	if ( !IsOpen() )
		return;

	if ( IsObjectOpen() )
		CloseObject();

	BSAArchive::Close();

	// Initialise
	m_pRecordDirectory = NULL;
	m_haObjectID.Destroy();
}/* Close */


bool BSAArch3D::OpenObject( long nObject )
{
	// Archive must be open
	if ( !IsOpen() )
		return false;

	// nObject must be within limits
	_ASSERT( nObject >= 0 && nObject <= GetRecordCount() );

	// Close any existing object
	if ( IsObjectOpen() )
		CloseObject();

	// Page the record into RAM
	char *pData;
	long nLength = GetRecordLength( nObject );
	try {
		pData = new char[nLength];
	}
	catch( ... ) {
		return false;
	}
	if ( !pData ) return false;
	if ( !GetRecord( nObject, pData, nLength ) ) {
		delete[] pData;
		return false;
	}

	// Set values
	m_nCurObject = nObject;
	m_bIsObjectOpen = true;
	m_pData = pData;
	m_nDataLength = nLength;
	m_pHeader = (LPARCH3D_HEADER)pData;
	m_pPointList = (LPARCH3D_POINT)(pData + m_pHeader->nPointOffset);
	m_pNormalList = (LPARCH3D_POINT)(pData + m_pHeader->nNormalOffset);
	if ( 0 == strncmp( m_pHeader->szVersion, "v2.7", 4 ) )
		m_lVersion = ARCH3DVERSION_27;
	else if ( 0 == strncmp( m_pHeader->szVersion, "v2.6", 4 ) )
		m_lVersion = ARCH3DVERSION_26;
	else if ( 0 == strncmp( m_pHeader->szVersion, "v2.5", 4 ) )
		m_lVersion = ARCH3DVERSION_25;
	else
		m_lVersion = ARCH3DVERSION_UNKNOWN;

	return true;
}/* OpenObject */


void BSAArch3D::CloseObject()
{
	if ( !IsObjectOpen() )
		return;

	// Initialise
	if ( m_pData ) {
		delete[] m_pData;
		m_pData = NULL;
	}
	m_nDataLength = 0;
	m_bIsObjectOpen = false;
	m_pHeader = NULL;
	m_pPointList = NULL;
	m_pNormalList = NULL;
	m_lVersion = 0;
}/* CloseObject */


float BSAArch3D::GetVersionAsFloat()
{
	float fv = 0.0f;

	switch ( m_lVersion )
	{
	case ARCH3DVERSION_25:
		fv = 2.5f;
		break;
	case ARCH3DVERSION_26:
		fv = 2.6f;
		break;
	case ARCH3DVERSION_27:
		fv = 2.7f;
		break;
	default:
		fv = 0.0f;
	}

	return fv;
}/* GetVersionAsFloat */


long BSAArch3D::GetFaceCount()
{
	// Object must be open
	if ( !IsObjectOpen() )
		return 0;

	return m_pHeader->nFaceCount;
}/* GetFaceCount */


LPARCH3D_FACE BSAArch3D::GetFace( int nFace )
{
	// Object must be open
	_ASSERT( IsObjectOpen() );
	if ( !IsObjectOpen() )
		return NULL;

	// nFace must be within limits
	_ASSERT( nFace < m_pHeader->nFaceCount );

	// Locate face
	int nFaceDataOffset = m_pHeader->nPointOffset + (m_pHeader->nPointCount * 12);
	if ( nFaceDataOffset == m_pHeader->nNormalOffset ) {
		return NULL;
	}
	LPARCH3D_FACE pFace = (LPARCH3D_FACE)(m_pData + nFaceDataOffset);
	for ( int i = 0; i < nFace; i++ ) {
		char *pWork = (char*)pFace;
		pWork += (8 + (pFace->nPointCount * 8));
		pFace = (LPARCH3D_FACE)pWork;
	}

	// Some faces on various objects seem to have malformed texture coordinates
	// These faces are patched in memory here until I find out what's going on
	// NOTE: Confirmed the game displays most of these malformed faces incorrectly also
	// Possible it uses an in-memory patch similar to this one.
	switch ( m_nCurObject )
	{
	case 5563:
		if ( nFace == 45 ) PatchPointUV( pFace, 1, 1024, DFTOOL_NOPATCH );
		break;

	case 5571:
		if ( nFace == 18 ) {
			PatchPointUV( pFace, 0, 0, DFTOOL_NOPATCH );
			PatchPointUV( pFace, 1, 0, DFTOOL_NOPATCH );
			PatchPointUV( pFace, 3, 0, DFTOOL_NOPATCH );
		}
		break;
	}

	return pFace;
}/* GetFace */


bool BSAArch3D::GetFaceTexture( LPARCH3D_FACE pFace, long* pnArchiveOut, long* pnRecordOut )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	// Out values cannot be NULL
	_ASSERT( pnArchiveOut );
	_ASSERT( pnRecordOut );

	// pFace cannot be NULL
	_ASSERT( pFace );

	*pnRecordOut = pFace->nTexture & 0x7f;
	*pnArchiveOut = (pFace->nTexture / 0x80);

	return true;
}/* GetFaceTexture */


bool BSAArch3D::GetPureFaceUV( LPARCH3D_FACE pFace, LPSIZE uvOut )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	// Out values cannot be NULL
	_ASSERT( uvOut );

	// pFace cannot be NULL
	_ASSERT( pFace );

	// pFace must have 3 or more points
	_ASSERT( pFace->nPointCount >= 3 );

	// Read UV settings from face
	OBJVERTEX v;
	for ( int p = 0; p < 3; p++ ) {
		GetPoint( pFace, p, &v );
		uvOut[p].cx = (long)v.tu;
		uvOut[p].cy = (long)v.tv;
	}

	// Read point 4 from UV list if present
	if ( pFace->nPointCount > 3 ) {
		GetPoint( pFace, 3, &v );
		uvOut[3].cx = (long)v.tu;
		uvOut[3].cy = (long)v.tv;
	}
	else {
		uvOut[3].cx = 0;
		uvOut[3].cy = 0;
	}

	return true;
}/* GetPureFaceUV */


long BSAArch3D::GetPointCount( LPARCH3D_FACE pFace )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return 0;

	// pFace cannot be NULL
	_ASSERT( pFace );

	return pFace->nPointCount;
}/* GetPointCount */


bool BSAArch3D::GetPoint( LPARCH3D_FACE pFace, int nPoint, LPOBJVERTEX pvOut )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	// pvOut cannot be NULL
	_ASSERT( pvOut );

	// pFace cannot be NULL
	_ASSERT( pFace );

	// nPoint must be within limits
	_ASSERT( nPoint < pFace->nPointCount );

	// Locate other resources
	LPARCH3D_POINTDESC pFacePointData = (LPARCH3D_POINTDESC)&pFace->Data;
	LPARCH3D_POINTDESC pPointDesc = pFacePointData + nPoint;

	// Output point information based on version
	long nDivisor;
	( GetVersion() == ARCH3DVERSION_25 ) ? nDivisor = 4 : nDivisor = 12;
	GetDFFP( pvOut->pos.x, m_pPointList[pPointDesc->nOffset/nDivisor].x );
	GetDFFP( pvOut->pos.y, m_pPointList[pPointDesc->nOffset/nDivisor].y );
	GetDFFP( pvOut->pos.z, m_pPointList[pPointDesc->nOffset/nDivisor].z );
	pvOut->normal.x = 0.0f;
	pvOut->normal.y = 0.0f;
	pvOut->normal.z = 0.0f;

	short tu, tv;
	tu = pPointDesc->tu;
	tv = pPointDesc->tv;

	/*
	if ( tu > 16384 ) tu -= 32768;
	if ( tu < -16384 ) tu += 32768;
	if ( tv > 16384 ) tv -= 32768;
	if ( tv < -16384 ) tv += 32768;

	if ( tu > 8192 ) tu -= 16384;
	if ( tu < -8192 ) tu += 16384;
	if ( tv > 8192 ) tv -= 16384;
	if ( tv < -8192 ) tv += 16384;

	if ( tu > 4096 ) tu -= 8192;
	if ( tu < -4096 ) tu += 8192;
	if ( tv > 4096 ) tv -= 8192;
	if ( tv < -4096 ) tv += 8192;
	*/

	pvOut->tu = (float)tu;
	pvOut->tv = (float)tv;

	return true;
}/* GetPoint */


bool BSAArch3D::PatchPointUV( LPARCH3D_FACE pFace, int nPoint, int U, int V )
{
	// pFace cannot be NULL
	_ASSERT( pFace );

	// nPoint must be within limits
	_ASSERT( nPoint < pFace->nPointCount );

	// Locate other resources
	LPARCH3D_POINTDESC pFacePointData = (LPARCH3D_POINTDESC)&pFace->Data;
	LPARCH3D_POINTDESC pPointDesc = pFacePointData + nPoint;

	// Patch U value
	if ( U != DFTOOL_NOPATCH ) {
		pPointDesc->tu = U;
	}

	// Patch V value
	if ( V != DFTOOL_NOPATCH ) {
		pPointDesc->tv = V;
	}

	return true;
}/* PatchPointUV */


long BSAArch3D::GetCornerPoints( LPARCH3D_FACE pFace, int* pCornerBuffer/*=NULL*/, LPOBJVERTEX pPointBuffer/*=NULL*/ )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return 0;

	// pFace cannot be NULL
	_ASSERT( pFace );

	// Get point count
	int nPoints = pFace->nPointCount;

	// Must have greater than 3 points
	_ASSERT( nPoints >= 3 );

	// Get an export of the face
	OBJVERTEX vArray[NUM_MAX_CORNER_POINTS];
	for ( int p = 0; p < nPoints; p++ ) {
		GetPoint( pFace, p, &vArray[p] );

		// Scale vertex so very small faces (8906) don't get overlooked
		vArray[p].pos.x *= 3;
		vArray[p].pos.y *= 3;
		vArray[p].pos.z *= 3;
	}

	// Step through points to count angles
	int nAngles = 0, nCorner = 0;
	float theta = 0.0f, costheta = 0.0f;
	D3DXVECTOR3 v0, v1, v2, l0, l1;
	for ( p = 0; p < nPoints; p++ ) {
		// Determine angle between current vertex and next two vertices
		if ( p < nPoints - 2 ) {
			v0 = vArray[p].pos;
			v1 = vArray[p+1].pos;
			v2 = vArray[p+2].pos;
			nCorner = p+1;
		}
		else if ( p < nPoints - 1 ) {
			v0 = vArray[p].pos;
			v1 = vArray[p+1].pos;
			v2 = vArray[0].pos;
			nCorner = p+1;
		}
		else {
			v0 = vArray[p].pos;
			v1 = vArray[0].pos;
			v2 = vArray[1].pos;
			nCorner = 0;
		}

		// Construct direction vertex for line0 and line1
		l0 = v1 - v0;
		l1 = v2 - v0;

		// Obtain angle between direction vectors
		costheta = D3DXVec3Dot(&l0,&l1) / (D3DXVec3Length(&l0) * D3DXVec3Length(&l1));

		// Colinear lines have a costheta of 1.0f - threshold is lower to avoid precision errors
		if ( costheta < 1.0f ) {
			// Write this corner to a buffer
			if ( pCornerBuffer ) {
				pCornerBuffer[nAngles] = nCorner;
			}

			// Write this point to a buffer
			if ( pPointBuffer ) {
				GetPoint( pFace, nCorner, &pPointBuffer[nAngles] );
			}

			// Increment corner count
			nAngles++;
		}
	}

	return nAngles;
}/* GetCornerPoints */


bool BSAArch3D::ExportPureFace( LPARCH3D_FACE pFace, LPOBJVERTEX pvOut, int* pPointCountOut )
{
	int nPoint;
	int nFacePointCount = pFace->nPointCount;
	int nCount = 0, nVertex = 0;
	OBJVERTEX vStart, v0, v1;

	// Object must be open
	if ( !IsObjectOpen() )
		return false;
	
	// pvOut cannot be NULL
	_ASSERT( pvOut );
	
	// pFace cannot be NULL
	_ASSERT( pFace );
	
	// Obtain the point count of this face
	GetPoint( pFace, 0, &vStart );
	for ( nPoint = 0; nPoint < nFacePointCount - 1; nPoint++ )
	{
		GetPoint( pFace, nPoint, &v0 );
		pvOut[nVertex].pos = -v0.pos;
		pvOut[nVertex].normal = -v0.normal;
		pvOut[nVertex].tu = 0.0f;
		pvOut[nVertex].tv = 0.0f;
		
		GetPoint( pFace, nPoint+1, &v1 );
		pvOut[nVertex+1].pos = -v1.pos;
		pvOut[nVertex+1].normal = -v1.normal;
		pvOut[nVertex+1].tu = 0.0f;
		pvOut[nVertex+1].tv = 0.0f;
		
		nCount += 2;
		nVertex += 2;
	}
	
	// Close polygon
	pvOut[nVertex].pos = -v1.pos;
	pvOut[nVertex].normal = -v1.normal;
	pvOut[nVertex].tu = 0.0f;
	pvOut[nVertex].tv = 0.0f;
	
	pvOut[nVertex+1].pos = -vStart.pos;
	pvOut[nVertex+1].normal = -vStart.normal;
	pvOut[nVertex+1].tu = 0.0f;
	pvOut[nVertex+1].tv = 0.0f;
	
	nCount += 2;
	
	*pPointCountOut = nCount;

	return true;
}/* ExportPureFace */


bool BSAArch3D::ExportTriangulatedFace( LPARCH3D_FACE pFace, LPOBJVERTEX pvOut, int* pPointCountOut, LPSIZE pSrcSize/*=NULL*/, LPRECT pSubRect/*=NULL*/ )
{
	// Validate
	_ASSERT( pFace );
	_ASSERT( pvOut );
	_ASSERT( pPointCountOut );

	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	// Get corner points
	int CornerBuffer[NUM_MAX_CORNER_POINTS];
	OBJVERTEX PointBuffer[NUM_MAX_CORNER_POINTS];
	int nCornerCount = GetCornerPoints( pFace, CornerBuffer, PointBuffer );

	// Set UV coordinates if texture dimensions are specified
	if ( pSrcSize && pSubRect ) {
		SetFaceUV( pFace, nCornerCount, PointBuffer, pSrcSize, pSubRect );
	}

	// Split corner points into triangles
	int nFaceCount = 0, nCount = 0;
	int s = 0, n = nCornerCount - 1;
	while ( nFaceCount != (nCornerCount - 2) )
	{
		// Write triangle 1
		pvOut[nCount].pos = -PointBuffer[s].pos;
		pvOut[nCount].normal = -PointBuffer[s].normal;
		pvOut[nCount].tu = PointBuffer[s].tu;
		pvOut[nCount].tv = PointBuffer[s].tv;

		pvOut[nCount+1].pos = -PointBuffer[n].pos;
		pvOut[nCount+1].normal = -PointBuffer[n].normal;
		pvOut[nCount+1].tu = PointBuffer[n].tu;
		pvOut[nCount+1].tv = PointBuffer[n].tv;

		pvOut[nCount+2].pos = -PointBuffer[s+1].pos;
		pvOut[nCount+2].normal = -PointBuffer[s+1].normal;
		pvOut[nCount+2].tu = PointBuffer[s+1].tu;
		pvOut[nCount+2].tv = PointBuffer[s+1].tv;

		nFaceCount++;
		nCount += 3;

		if ( nFaceCount == (nCornerCount - 2) )
			break;

		// Write triangle 2
		pvOut[nCount].pos = -PointBuffer[s+1].pos;
		pvOut[nCount].normal = -PointBuffer[s+1].normal;
		pvOut[nCount].tu = PointBuffer[s+1].tu;
		pvOut[nCount].tv = PointBuffer[s+1].tv;

		pvOut[nCount+1].pos = -PointBuffer[n].pos;
		pvOut[nCount+1].normal = -PointBuffer[n].normal;
		pvOut[nCount+1].tu = PointBuffer[n].tu;
		pvOut[nCount+1].tv = PointBuffer[n].tv;

		pvOut[nCount+2].pos = -PointBuffer[n-1].pos;
		pvOut[nCount+2].normal = -PointBuffer[n-1].normal;
		pvOut[nCount+2].tu = PointBuffer[n-1].tu;
		pvOut[nCount+2].tv = PointBuffer[n-1].tv;

		s++;
		n--;
		nFaceCount++;
		nCount += 3;
	}

	// Store values
	*pPointCountOut = nCount;

	return true;
}/* ExportTriangulatedFace */


//
// bool SetFaceUV( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect )
// Performs UV setup for the specified face
// Return: true if successful, otherwise false
//
bool BSAArch3D::SetFaceUV( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect )
{
	// Validate
	_ASSERT( nCornerCount >= 3 );
	_ASSERT( pCornerVertexBuffer );
	_ASSERT( pSrcSize );
	_ASSERT( pSubRect );

	// Get first four points of this face
	OBJVERTEX verts[4];
	GetPoint( pFace, 0, &verts[0] );
	GetPoint( pFace, 1, &verts[1] );
	GetPoint( pFace, 2, &verts[2] );
	if ( pFace->nPointCount > 3 ) {
		GetPoint( pFace, 3, &verts[3] );
	}
	else {
		verts[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		verts[3].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		verts[3].tu = 0.0f;
		verts[3].tv = 0.0f;
	}

	// Calculate absolute UV coordinates for point 1, 2
	verts[1].tu += verts[0].tu;
	verts[1].tv += verts[0].tv;
	verts[2].tu += verts[1].tu;
	verts[2].tv += verts[1].tv;

	// Get texture dimensions and divisors
	int nTextureWidth = pSubRect->right - pSubRect->left;
	int nTextureHeight = pSubRect->bottom - pSubRect->top;
	float fWidthDivisor = 16.0f * nTextureWidth;
	float fHeightDivisor = 16.0f * nTextureHeight;

	// Calculate smallest POW2 texture size and subregion
	// This simulates the POW2 logic in Alchemy so we know how large the end texture will be
	// This is neccessary because not all DF textures are POW2 and will be resized by Alchemy
	int smx = 8, smy = 8;
	while ( smx < (int)nTextureWidth ) smx *= 2;
	while ( smy < (int)nTextureHeight ) smy *= 2;
	float stu = (float)nTextureWidth / smx;
	float stv = (float)nTextureHeight / smy;

	// Triangles can be handled quickly and easily
	long cnr;
	if ( pFace->nPointCount == 3 && nCornerCount == 3 ) {
		pCornerVertexBuffer[0].tu = verts[1].tu / fWidthDivisor;
		pCornerVertexBuffer[0].tv = verts[1].tv / fHeightDivisor;
		pCornerVertexBuffer[1].tu = verts[2].tu / fWidthDivisor;
		pCornerVertexBuffer[1].tv = verts[2].tv / fHeightDivisor;
		pCornerVertexBuffer[2].tu = verts[0].tu / fWidthDivisor;
		pCornerVertexBuffer[2].tv = verts[0].tv / fHeightDivisor;

		// Adjust UV to reference subregion of texture
		for ( cnr = 0; cnr < nCornerCount; cnr++ ) {
			pCornerVertexBuffer[cnr].tu *= stu;
			pCornerVertexBuffer[cnr].tv *= stv;

#ifdef _DEBUG
			// Look for overflow on subregion textures
			if ( smx > nTextureWidth && pCornerVertexBuffer[cnr].tu > 1.0f ) _ASSERT(FALSE);
			if ( smy > nTextureHeight && pCornerVertexBuffer[cnr].tv > 1.0f ) _ASSERT(FALSE);
#endif
		}

		return true;
	}

	// Use Dave Humphrey's matrix generator and solve for remaining corner points
	DFFaceTex dft;
	df3duvmatrix_t mat;
	if ( dft.ComputeDFFaceTextureUVMatrix( mat, verts ) ) {
		// Solve for each point in the corner array
		float x, y, z;
		for ( cnr = 0; cnr < nCornerCount; cnr++ ) {
			// Get UV coordinates
			x = pCornerVertexBuffer[cnr].pos.x;
			y = pCornerVertexBuffer[cnr].pos.y;
			z = pCornerVertexBuffer[cnr].pos.z;
			pCornerVertexBuffer[cnr].tu = ((x*mat.UA) + (y*mat.UB) + (z*mat.UC) + mat.UD) / fWidthDivisor * stu;
			pCornerVertexBuffer[cnr].tv = ((x*mat.VA) + (y*mat.VB) + (z*mat.VC) + mat.VD) / fHeightDivisor * stv;

#ifdef _DEBUG
			// Look for overflow on subregion textures
			if ( smx > nTextureWidth && pCornerVertexBuffer[cnr].tu > 1.0f ) _ASSERT(FALSE);
			if ( smy > nTextureHeight && pCornerVertexBuffer[cnr].tv > 1.0f ) _ASSERT(FALSE);
#endif
		}
	}
	else {
		// Quads can be handled quickly and easily
		if ( pFace->nPointCount == 4 && nCornerCount == 4 ) {
			pCornerVertexBuffer[0].tu = verts[1].tu / fWidthDivisor;
			pCornerVertexBuffer[0].tv = verts[1].tv / fHeightDivisor;
			pCornerVertexBuffer[1].tu = verts[2].tu / fWidthDivisor;
			pCornerVertexBuffer[1].tv = verts[2].tv / fHeightDivisor;
			pCornerVertexBuffer[2].tu = verts[3].tu / fWidthDivisor;
			pCornerVertexBuffer[2].tv = verts[3].tv / fHeightDivisor;
			pCornerVertexBuffer[3].tu = verts[0].tu / fWidthDivisor;
			pCornerVertexBuffer[3].tv = verts[0].tv / fHeightDivisor;

			// Adjust UV to reference subregion of texture
			for ( cnr = 0; cnr < nCornerCount; cnr++ ) {
				pCornerVertexBuffer[cnr].tu *= stu;
				pCornerVertexBuffer[cnr].tv *= stv;

#ifdef _DEBUG
				// Look for overflow on subregion textures
				if ( smx > nTextureWidth && pCornerVertexBuffer[cnr].tu > 1.0f ) _ASSERT(FALSE);
				if ( smy > nTextureHeight && pCornerVertexBuffer[cnr].tv > 1.0f ) _ASSERT(FALSE);
#endif
			}

			return true;
		}

		// Fail back to my basic method for all other faces
		// TODO: Use the techniques Dave emailed me to replace this call
		return SetFaceUVOld( pFace, nCornerCount, pCornerVertexBuffer, pSrcSize, pSubRect );
		//_ASSERT(FALSE);
	}

	return true;
}/* SetFaceUV */


//
// bool SetFaceUVOld( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect )
// Performs UV setup for the specified face
// Return: true if successful, otherwise false
// Note: This method is depreciated and is to be removed in the near future
//
bool BSAArch3D::SetFaceUVOld( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect )
{
	// Validate
	_ASSERT( nCornerCount >= 3 );
	_ASSERT( pCornerVertexBuffer );
	_ASSERT( pSrcSize );
	_ASSERT( pSubRect );

	// Get texture dimensions
	int nTextureWidth = pSubRect->right - pSubRect->left;
	int nTextureHeight = pSubRect->bottom - pSubRect->top;

	// Get first three points of this face and calculate their UV values
	OBJVERTEX p0, p1, p2;
	GetPoint( pFace, 0, &p0 );
	GetPoint( pFace, 1, &p1 );
	GetPoint( pFace, 2, &p2 );
	p0.tu = p0.tu / 16 / nTextureWidth;
	p0.tv = p0.tv / 16 / nTextureHeight;
	p1.tu = p1.tu / 16 / nTextureWidth + p0.tu;
	p1.tv = p1.tv / 16 / nTextureHeight + p0.tv;
	p2.tu = p2.tu / 16 / nTextureWidth + p1.tu;
	p2.tv = p2.tv / 16 / nTextureHeight + p1.tv;

	// Point 1 is always the first corner point (index 0 in the corner vertex buffer)
	pCornerVertexBuffer[0].tu = p1.tu;
	pCornerVertexBuffer[0].tv = p1.tv;

	// Create vectors for difference between p0 and p2
	OBJVERTEX vDif, vCmpDif;
	vDif.tu = p2.tu - p0.tu;
	vDif.tv = p2.tv - p0.tv;
	vDif.pos = p2.pos - p0.pos;

	// Start finding axis of smallest change by starting with Z
	float valu, valv;
	float fMinDif = (float)fabs(vDif.pos.z);
	float *pDifU = &vDif.pos.x;
	float *pDifV = &vDif.pos.y;
	float *pCmpDifU = &vCmpDif.pos.x;
	float *pCmpDifV = &vCmpDif.pos.y;

	// Compare smallest with Y
	if ( fabs(vDif.pos.y) < fMinDif ) {
		fMinDif = (float)fabs(vDif.pos.y);
		pDifU = &vDif.pos.x;
		pDifV = &vDif.pos.z;
		pCmpDifU = &vCmpDif.pos.x;
		pCmpDifV = &vCmpDif.pos.z;
	}

	// Compare smallest with X
	if ( fabs(vDif.pos.x) < fMinDif ) {
		pDifU = &vDif.pos.z;
		pDifV = &vDif.pos.y;
		pCmpDifU = &vCmpDif.pos.z;
		pCmpDifV = &vCmpDif.pos.y;
	}

	// Calculate UV for all remaining corner points
	for ( long cnr = 1; cnr < nCornerCount; cnr++ ) {
		// Create vector for difference between point cnr and p0
		vCmpDif.pos = pCornerVertexBuffer[cnr].pos - p0.pos;

		valu = (*pCmpDifU / *pDifU) * vDif.tu;
		valv = (*pCmpDifV / *pDifV) * vDif.tv;

		pCornerVertexBuffer[cnr].tu = p0.tu + valu;
		pCornerVertexBuffer[cnr].tv = p0.tv + valv;
	}

	return true;
}/* SetFaceUVOld */


//
// bool SetPointUV( LPARCH3D_FACE pFace, int nPoint, short u, short v )
// Set UV values on the specified point
// Return: true if successful, otherwise FALSE
//
bool BSAArch3D::SetPointUV( LPARCH3D_FACE pFace, int nPoint, short u, short v )
{
	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	// pFace cannot be NULL
	_ASSERT( pFace );

	// nPoint must be within limits
	_ASSERT( nPoint < pFace->nPointCount );

	// Locate other resources
	LPARCH3D_POINTDESC pFacePointData = (LPARCH3D_POINTDESC)&pFace->Data;
	LPARCH3D_POINTDESC pPointDesc = pFacePointData + nPoint;

	// Set tu and tv for this point
	pPointDesc->tu = u;
	pPointDesc->tv = v;

	return true;
}/* SetPointUV */


//
// bool SaveObject()
// Warning - this will save any changes made to the object in memory to ARCH3D.BSA
// Return: true if successful, otherwise false
//
bool BSAArch3D::SaveObject()
{
	// Object must be open
	if ( !IsObjectOpen() )
		return false;

	return SetRecord( m_nCurObject, m_pData, m_nDataLength );
}/* SaveObject */


//
// long ResolveObjectID( long ObjectID )
// Resolves an ObjectID to an object index number
// Return: The object ordinal if successful, otherwise -1
//
long BSAArch3D::ResolveObjectID( long nObjectID )
{
	// Prepare the ObjectID string
	CString str;
	str.Format( "%d", nObjectID );

	// Resolve the ID
	UINT *pu = m_haObjectID.GetObject( str );
	if ( !pu )
		return -1;

	return *pu;
}/* ResolveObjectID */


//////////////////////////////////////////////////////////////////////
// BSAArchive Construction / Destruction
//////////////////////////////////////////////////////////////////////


BSAArchive::BSAArchive()
{
	// Initialise
	m_dwLength = 0;
	m_bIsOpen = false;
	memset( &m_bsaHeader, 0, sizeof(BSA_ARCHIVEHEADER) );
	m_nRecordDirectoryEntryLength = 0;
	m_nRecordDirectoryType = 0;
	m_nRecordCount = 0;
	m_pRecordDirectoryLong = NULL;
	m_pRecordDirectoryShort = NULL;
}/* BSAArchive */


BSAArchive::~BSAArchive()
{
	Close();
}/* ~BSAArchive */


//////////////////////////////////////////////////////////////////////
// BSAArchive Member Functions
//////////////////////////////////////////////////////////////////////


bool BSAArchive::Open( LPCSTR pszPath, LPCSTR pszArchive, BOOL bReadOnly/*=TRUE*/ )
{
	if ( IsOpen() )
		Close();

	// Open the archive
	CString str = pszPath;
	if ( str.Right(1) != "\\" ) str += "\\";
	str += pszArchive;

	if ( bReadOnly ) {
		if ( !m_bsaFile.Open( str, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
			return false;
	}
	else {
		if ( !m_bsaFile.Open( str, CFile::modeReadWrite | CFile::shareDenyNone | CFile::typeBinary ) )
			return false;
	}

	// Obtain file length
	DWORD dwLength = (DWORD)m_bsaFile.GetLength();

	// Read the header
	m_bsaFile.SeekToBegin();
	m_bsaFile.Read( &m_bsaHeader, sizeof(BSA_ARCHIVEHEADER) );

	// Set values
	m_strArena2Path = pszPath;
	m_dwLength = dwLength;
	m_bIsOpen = true;
	m_nRecordCount = m_bsaHeader.nRecordCount;
	m_nRecordDirectoryType = m_bsaHeader.nRecordType;
	if ( BSA_RECORDTYPELONG == m_nRecordDirectoryType )
		m_nRecordDirectoryEntryLength = BSARECORD_LENGTH_LONG;
	else
		m_nRecordDirectoryEntryLength = BSARECORD_LENGTH_SHORT;

	// Page the directory into RAM
	long nDirectoryLength = m_nRecordCount * m_nRecordDirectoryEntryLength;
	char* pDirectory = new char[nDirectoryLength];
	if ( !pDirectory ) {
		Close();
		return false;
	}
	m_bsaFile.Seek( -nDirectoryLength, CFile::end );
	m_bsaFile.Read( pDirectory, nDirectoryLength );
	if ( BSA_RECORDTYPELONG == m_nRecordDirectoryType )
		m_pRecordDirectoryLong = (LPBSA_RECORDDIRECTORYLONG)pDirectory;
	else
		m_pRecordDirectoryShort = (LPBSA_RECORDDIRECTORYSHORT)pDirectory;

	return true;
}/* Open */


void BSAArchive::Close()
{
	if ( !IsOpen() )
		return;

	// Initialise
	m_strArena2Path = "";
	m_bsaFile.Close();
	m_dwLength = 0;
	m_bIsOpen = false;
	memset( &m_bsaHeader, 0, sizeof(BSA_ARCHIVEHEADER) );
	m_nRecordDirectoryEntryLength = 0;
	m_nRecordDirectoryType = 0;
	m_nRecordCount = 0;
	if ( m_pRecordDirectoryLong ) {
		delete[] m_pRecordDirectoryLong;
		m_pRecordDirectoryLong = NULL;
	}
	if ( m_pRecordDirectoryShort ) {
		delete[] m_pRecordDirectoryShort;
		m_pRecordDirectoryShort = NULL;
	}
}/* Close */


long BSAArchive::GetRecordLength( long nRecord )
{
	if ( !IsOpen() )
		return 0;

	if ( BSA_RECORDTYPELONG == m_nRecordDirectoryType )
		return m_pRecordDirectoryLong[nRecord].nSize;
	else
		return m_pRecordDirectoryShort[nRecord].nSize;
}/* GetRecordLength */


bool BSAArchive::GetRecord( long nRecord, char* pDataOut, long nLength )
{
	if ( !IsOpen() )
		return false;

	// Validate
	_ASSERT( pDataOut );
	_ASSERT( nRecord < GetRecordCount() );

	// Locate start of record
	long record = 0;
	DWORD dwOffset = sizeof(BSA_ARCHIVEHEADER);
	while ( record < nRecord )
	{
		dwOffset += GetRecordLength( record++ );
	}

	// Read record into buffer
	long nRecordLength = GetRecordLength( nRecord );
	if ( nLength > nRecordLength ) nLength = nRecordLength;
	m_bsaFile.Seek( dwOffset, CFile::begin );
	m_bsaFile.Read( pDataOut, nLength );

	return true;
}/* GetRecord */


bool BSAArchive::SetRecord( long nRecord, char *pDataIn, long nLength )
{
	if ( !IsOpen() )
		return false;

	// Locate start of record
	long record = 0;
	DWORD dwOffset = sizeof(BSA_ARCHIVEHEADER);
	while ( record < nRecord )
	{
		dwOffset += GetRecordLength( record++ );
	}

	// Validate
	_ASSERT( pDataIn );
	_ASSERT( nRecord < GetRecordCount() );

	// Write record from buffer
	long nRecordLength = GetRecordLength( nRecord );
	_ASSERT( nLength == nRecordLength );
	if ( nLength != nRecordLength ) return false;
	m_bsaFile.Seek( dwOffset, CFile::begin );
	m_bsaFile.Write( pDataIn, nLength );

	return true;
}


bool BSAArchive::GetRecordDesc( long nRecord, char* pBufferOut, long nLength )
{
	if ( !IsOpen() )
		return FALSE;

	// Validate
	_ASSERT( pBufferOut );
	_ASSERT( nRecord < GetRecordCount() );

	if ( BSA_RECORDTYPELONG == m_nRecordDirectoryType )
	{
		// Copy record name to output buffer
		strncpy( pBufferOut, m_pRecordDirectoryLong[nRecord].pszName, nLength );
	}
	else
	{
		// Format record ID into output buffer
		char buffer[32];
		memset( buffer, 0, 32 );
		sprintf( buffer, "%d", m_pRecordDirectoryShort[nRecord].nID );
		strncpy( pBufferOut, buffer, nLength );
	}

	return true;
}/* GetRecordDesc */


//////////////////////////////////////////////////////////////////////
// DTXArchive Construction / Destruction
//////////////////////////////////////////////////////////////////////


DTXArchive::DTXArchive()
{
	// Initialise
	m_pArchive = NULL;
	m_dwLength = 0;
	m_dwRecordCount = 0;
	m_bIsOpen = m_bIsCreated = false;
	m_pHeader = NULL;
	m_pRecordDirectory = NULL;
}


DTXArchive::~DTXArchive()
{
	Close();
}


//////////////////////////////////////////////////////////////////////
// DTXArchive Member Functions
//////////////////////////////////////////////////////////////////////


bool DTXArchive::Create( LPCSTR pszArena2Path )
{
	// Open the palette
	CString str;
	CFile file;
	str.Format( "%s\\%s", pszArena2Path, "PAL.PAL" );
	if ( !file.Open( str, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
		return false;

	// Read the palette
	file.Seek( 8, CFile::begin );
	file.Read( (void*)&m_Palette, 768 );
	file.Close();

	m_strArena2Path = pszArena2Path;
	m_bIsCreated = true;

	return true;
}


bool DTXArchive::Open( LPCSTR pszArchive )
{
	// Must be created
	_ASSERT( m_bIsCreated );
	_ASSERT( pszArchive );

	if ( IsOpen() )
		Close();

	// Open the archive
	CFile file;
	CString str;
	str.Format( "%s\\%s", m_strArena2Path, pszArchive );
	if ( !file.Open( str, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
		return false;

	// Read the archive
	DWORD dwLength = (DWORD)file.GetLength();
	char *pArchive = new char[dwLength];
	if ( !pArchive )
	{
		file.Close();
		return false;
	}
	file.Read( pArchive, dwLength );
	file.Close();

	// Set values
	m_dwLength = dwLength;
	m_pArchive = pArchive;
	m_strArchiveName = pszArchive;
	m_bIsOpen = true;
	m_pHeader = (LPDTX_ARCHIVEHEADER)pArchive;
	m_pRecordDirectory = LPDTX_RECORDDIRECTORY(pArchive + sizeof(DTX_ARCHIVEHEADER));
	m_dwRecordCount = m_pHeader->nRecords;

	// Compose archive name
	char pszBuffer[32];
	memset( pszBuffer, 0, 32 );
	strncpy( pszBuffer, m_pHeader->strName, 24 );
	m_strArchiveDesc = pszBuffer;
	m_strArchiveDesc.TrimLeft();
	m_strArchiveDesc.TrimRight();

	// Set solid flag
	str = pszArchive;
	if ( str == "TEXTURE.000" )
		m_nSolidType = DTX_SOLIDA;
	else if ( str == "TEXTURE.001" )
		m_nSolidType = DTX_SOLIDB;
	else
		m_nSolidType = DTX_NOTSOLID;

	return true;
}


void DTXArchive::Close()
{
	if ( !IsOpen() )
		return;

	// Clean up
	if ( m_pArchive ) delete[] m_pArchive;
	m_pArchive = NULL;
	m_dwLength = 0;
	m_dwRecordCount = 0;
	m_bIsOpen = false;
	m_pHeader = NULL;
	m_pRecordDirectory = NULL;
	m_strArchiveName = "";
}


/*
BOOL DTXArchive::GetImageRect( DWORD dwRecord, DWORD dwImage, LPRECT pRectOut )
{
	RECT rct;
	int cx=0, cy=0;

	// Validate
	_ASSERT( IsOpen() );
	_ASSERT( pRectOut );
	_ASSERT( dwRecord < GetRecordCount() );
	_ASSERT( dwImage <= GetImageCount(dwRecord) );

	// Locate record in archive
	DWORD dwOffset = m_pRecordDirectory[dwRecord].nOffset;
	LPDTX_RECORDHEADER pRecordHeader = LPDTX_RECORDHEADER(m_pArchive + dwOffset);

	// Locate record data
	char *pData = (char*)pRecordHeader + pRecordHeader->nDataOffset;

	// Start unpacking image
	if ( pRecordHeader->nImages == 1 || pRecordHeader->Compression == DTX_IMAGERLE )
	{
		// Get image size
		cx = pRecordHeader->cx;
		cy = pRecordHeader->cy;
	}
	else if ( pRecordHeader->nImages > 1 )
	{
		// Locate image
		LPDTX_IMAGEDIRECTORY pImageDirectory = (LPDTX_IMAGEDIRECTORY)pData;
		char *p = (char*)pImageDirectory + pImageDirectory[dwImage].nOffset;
		LPDTX_IMAGEDATA pImage = (LPDTX_IMAGEDATA)p;

		// Get image size
		cx = pImage->cx;
		cy = pImage->cy;
	}

	rct.top = 0;
	rct.left = 0;
	rct.right = cx;
	rct.bottom = cy;
	*pRectOut = rct;

	return TRUE;
}/* GetImageRect */


inline void DTXArchive::DecodeRLERows( char *pImageRaw, LPDTX_RECORDHEADER pRecordHeader, LPDTX_RLEROWDIRECTORY pRowDirectory, int cx, int cy )
{
	unsigned char pixel=0;

	// Read the RLE compressed image from the record data
	for ( int y = 0; y < cy; y++ )
	{
		// Decode this row
		char *p = (char*)pRecordHeader + pRowDirectory[y].nOffset;
		LPDTX_RLEROWDATA pRow = (LPDTX_RLEROWDATA)p;
		char *pRowData = &pRow->Data;
		
		unsigned short nRowPos = 0;
		while ( nRowPos < cx )
		{
			// Read compressed row
			if ( pRowDirectory[y].Encoded == DTX_ROWENCODED )
			{
				LPDTX_RLEROWCHUNK pChunk = (LPDTX_RLEROWCHUNK)pRowData;
				if ( pChunk->nCount > 0 )
				{
					// Stream non-RLE bytes
					char* pPixelStream = (char*)&pChunk->Data;
					short nCount = pChunk->nCount + nRowPos;
					short k = 0;
					for ( ; nRowPos < nCount; nRowPos++ )
					{
						pixel = pPixelStream[k++];
						pImageRaw[y*256+nRowPos] = pixel;
					}// end for ( ; nRowPos < pChunk->nCount; nRowPos++ )
					pRowData += (2 + pChunk->nCount);
					
				}
				else
				{
					// Stream nCount RLE bytes of Data
					pixel = pChunk->Data;
					short nCount = -(pChunk->nCount) + nRowPos;
					while ( nRowPos < nCount )
					{
						pImageRaw[y*256+nRowPos] = pixel;
						nRowPos++;
					}
					pRowData += 3;
				}// end if ( pChunk->nCount >= 0 )
			}// end if ( pRowDirectory[y].Encoded == DTX_ROWENCODED )
			else
			{
				// Offset pRow Data
				pRowData -= 2;
				
				// Read uncompressed row
				for ( int x = 0; x < cx; x++ )
				{
					pixel = pRowData[x];
					pImageRaw[y*256+x] = pixel;
					nRowPos++;
				}// end for ( x = 0; x < nWidth; x++ )
			}// end if ( pRowDirectory[y].Encoded == DTX_ROWENCODED )
		}// end while ( nRowPos < nWidth )
	}// end for ( y = 0; y < cy; y++ )
}/* DecodeRLERows */


BOOL DTXArchive::GetImage( DWORD dwRecord, DWORD dwImage, LPDTX_IMAGEDESC pImageDescOut, char* pBitsOut/*=NULL*/, DWORD dwPitchInBytes/*=0*/ )
{
	char pImageRaw[256*256];
	char *pImageOut = NULL;
	int x=0, y=0, cx=0, cy=0;
	unsigned char pixel=0;

	// Validate
	_ASSERT( IsOpen() );
	_ASSERT( pImageDescOut );
	_ASSERT( dwRecord < GetRecordCount() );
	_ASSERT( dwImage <= GetImageCount(dwRecord) );

	// Prepare output location
	if ( pBitsOut ) {
		// If you specify another destination for bits, the pitch must be set
		_ASSERT( dwPitchInBytes );
		pImageOut = pBitsOut;
	}
	else {
		// Set standard output location and pitch
		pImageOut = pImageDescOut->pBuffer;
		dwPitchInBytes = DTXSIZE_IMAGE_PITCH;
	}

	// Handle solid colour extraction
	if ( m_nSolidType ) {
		// Determine palette index
		unsigned char nIndex = (unsigned char)dwRecord;
		if ( DTX_SOLIDB == m_nSolidType ) {
			// Offset to solid colours B
			nIndex += 128;
		}

		// Extract the RGB colours for this pixel
		char r = m_Palette.colour[nIndex].r;
		char g = m_Palette.colour[nIndex].g;
		char b = m_Palette.colour[nIndex].b;

		unsigned char *pOut = NULL;
		cx = cy = DFTOOL_SOLIDTEXTURESIZE;
		for ( y = 0; y < cy; y++ )
		{
			for ( x = 0; x < cx; x++ )
			{
				// Set the pixel colour
				pOut = (unsigned char*)&pImageOut[(y*dwPitchInBytes)+(x*3)];
				pOut[0] = b;
				pOut[1] = g;
				pOut[2] = r;
			}// end for ( x = 0; x < cx; x++ )
		}// end for ( y = 0; y < cy; y++ )

		return TRUE;
	}

	// Locate record in archive
	DWORD dwOffset = m_pRecordDirectory[dwRecord].nOffset;
	LPDTX_RECORDHEADER pRecordHeader = LPDTX_RECORDHEADER(m_pArchive + dwOffset);

	// Locate record data
	char *pData = (char*)pRecordHeader + pRecordHeader->nDataOffset;

	// Start unpacking image
	if ( pRecordHeader->nImages == 1 )
	{
		// Get image size
		cx = pRecordHeader->cx;
		cy = pRecordHeader->cy;

		if ( pRecordHeader->Compression == DTX_RECORDRLE || pRecordHeader->Compression == DTX_IMAGERLE )
		{
			// Locate the row directory
			LPDTX_RLEROWDIRECTORY pRowDirectory = (LPDTX_RLEROWDIRECTORY)pData;

			// Decode rows
			DecodeRLERows( pImageRaw, pRecordHeader, pRowDirectory, cx, cy );
		}
		else
		{
			// Read a single image from record data
			for ( y = 0; y < cy; y++ )
			{
				for ( x = 0; x < cx; x++ )
				{
					pixel = pData[y*256+x];
					pImageRaw[y*256+x] = pixel;
				}// end for ( int x = 0; x < cx; x++ )
			}// end for ( int y = 0; y < cy; y++ )
		}// end if ( pRecordHeader->Compression == DTX_RECORDRLE )
	}
	else if ( pRecordHeader->nImages > 1 )
	{
		// Locate image
		LPDTX_IMAGEDIRECTORY pImageDirectory = (LPDTX_IMAGEDIRECTORY)pData;
		char *p = (char*)pImageDirectory + pImageDirectory[dwImage].nOffset;
		LPDTX_IMAGEDATA pImage = (LPDTX_IMAGEDATA)p;
		char *pImageData = &pImage->Data;

		// Get image size
		cx = pRecordHeader->cx;
		cy = pRecordHeader->cy;

		if ( pRecordHeader->Compression == DTX_IMAGERLE )
		{
			// Locate the row directory
			char *p = (char*)pData + ((cy * dwImage) * sizeof(DTX_RLEROWDIRECTORY));
			LPDTX_RLEROWDIRECTORY pRowDirectory = (LPDTX_RLEROWDIRECTORY)p;

			// Decode rows
			DecodeRLERows( pImageRaw, pRecordHeader, pRowDirectory, cx, cy );
		}
		else
		{
			// Read image nImage from record data
			int run, pos = 0;
			for ( y = 0; y < cy; y++ )
			{
				x = 0;
				while ( x < cx )
				{
					// Write transparent bytes
					pixel = pImageData[pos++];
					run = x + pixel;
					for ( ; x < run; x++ )
					{
						pImageRaw[y*256+x] = 0;
					}
					
					// Write image bytes
					pixel = pImageData[pos++];
					run = x + pixel;
					for ( ; x < run; x++ )
					{
						pixel = pImageData[pos++];
						pImageRaw[y*256+x] = pixel;
					}
				}// end while ( x < cx )
			}// end for ( y = 0; y < cy; y++ )
		}
	}// end if ( pRecordHeader->nImages == 1 )

	// Set dimensions of image
	pImageDescOut->cx = cx;
	pImageDescOut->cy = cy;

	// Set colors of image buffer in R8G8B8 format
	for ( y = 0; y < cy; y++ )
	{
		for ( x = 0; x < cx; x++ )
		{
			// Determine palette index
			unsigned char nIndex = pImageRaw[ y * 256 + x ];

			// Extract the RGB colours for this pixel
			char r = m_Palette.colour[nIndex].r;
			char g = m_Palette.colour[nIndex].g;
			char b = m_Palette.colour[nIndex].b;

			// Set the pixel colour
			unsigned char *pOut = (unsigned char*)&pImageOut[(y*dwPitchInBytes)+(x*3)];
			pOut[0] = b;
			pOut[1] = g;
			pOut[2] = r;
		}// end for ( x = 0; x < cx; x++ )
	}// end for ( y = 0; y < cy; y++ )

	return TRUE;
}/* GetImage */


//////////////////////////////////////////////////////////////////////
// CIFArchive Construction / Destruction
//////////////////////////////////////////////////////////////////////


CIFArchive::CIFArchive()
{
	// Initialise
	m_pArchive = NULL;
	m_dwLength = 0;
	m_bIsOpen = FALSE;
	m_dwLastRecordCount = 0;
}/* CIFArchive */


CIFArchive::~CIFArchive()
{
	Close();
}/* ~CIFArchive */


//////////////////////////////////////////////////////////////////////
// CIFArchive Member Functions
//////////////////////////////////////////////////////////////////////


//
// BOOL Open( LPCSTR pszPath, LPCSTR pszArchive )
// Open the specified CIF archive
// Return: TRUE if successful, otherwise FALSE
//
BOOL CIFArchive::Open( LPCSTR pszPath, LPCSTR pszArchive )
{
	CFile file;
	CString str;

	if ( IsOpen() )
		Close();

	// Open the archive
	str.Format( "%s\\%s", pszPath, pszArchive );
	if ( !file.Open( str, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
		return FALSE;

	// Read the archive
	DWORD dwLength = (DWORD)file.GetLength();
	char *pArchive = new char[dwLength];
	if ( !pArchive ) {
		file.Close();
		return FALSE;
	}
	file.Read( pArchive, dwLength );
	file.Close();

	// Set palette
	char szPalette[16];
	strcpy( szPalette, "ART_PAL.COL" );

	// Read palette
	CString strPalette;
	strPalette.Format( "%s\\%s", pszPath, szPalette );
	if ( !file.Open( strPalette, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) ) {
		delete[] pArchive;
		return FALSE;
	}
	
	// Read the palette
	if ( strPalette.Right( 3 ) == "PAL" ) {
		file.Read( (void*)&m_Palette, 768 );
	}
	else if ( strPalette.Right( 3 ) == "COL" )
	{
		file.Seek( 8, CFile::begin );
		file.Read( (void*)&m_Palette, 768 );
	}
	else {
		file.Close();
		delete[] pArchive;
		return FALSE;
	}
	file.Close();

	// Set values
	m_strArena2Path = pszPath;
	m_strArchiveName = pszArchive;
	m_strArchiveName.MakeUpper();
	m_dwLength = dwLength;
	m_pArchive = pArchive;
	m_bIsOpen = TRUE;

	return true;
}/* Open */


//
// void Close()
// Close the currently open CIF archive
//
void CIFArchive::Close()
{
	if ( !IsOpen() )
		return;

	// Clean up
	if ( m_pArchive ) {
		delete[] m_pArchive;
		m_pArchive = NULL;
	}

	// Initialise
	m_dwLength = 0;
	m_bIsOpen = FALSE;
	m_strArena2Path.Empty();
	m_strArchiveName.Empty();
	m_dwLastRecordCount = 0;
}/* Close */


//
// BOOL GetImage( LPCIF_RECORDDESC pCIFArchDescInOut )
// Scan for first / next CIF image in specified archive
// Return: TRUE if successful, otherwise FALSE
//
BOOL CIFArchive::GetImage( LPCIF_RECORDDESC pCIFArchDescInOut )
{
	// Validate
	_ASSERT( pCIFArchDescInOut );

	// Must be open
	if ( !IsOpen() ) {
		return FALSE;
	}

	// Get offset
	DWORD dwOffset = pCIFArchDescInOut->dwOffset;

	// Locate header
	char *pb = m_pArchive + dwOffset;
	LPIMG_HEADER pHeader = (LPIMG_HEADER)pb;

	// This assertion will be tripped if the specified offset is larger than memory buffer
	_ASSERT( dwOffset <= m_dwLength );

	// This assertion will be tripped if memory buffer is not greater than IMG_HEADER in length
	_ASSERT( m_dwLength > sizeof(IMG_HEADER) );

	// If less than IMG_HEADER bytes remain in memory buffer then all images have been found
	if ( (m_dwLength - dwOffset) < sizeof(IMG_HEADER) ) {
		pCIFArchDescInOut->dwOffset = 0;
		pCIFArchDescInOut->bFinished = TRUE;
		m_dwLastRecordCount = pCIFArchDescInOut->nCurImage + 1;
		return TRUE;
	}

	// Handle FACES.CIF and TFAC00I0.RCI and CHLD00I0.RCI (These are the known portrait types, all using the same format)
	if ( m_strArchiveName == "FACES.CIF" || m_strArchiveName == "TFAC00I0.RCI" || m_strArchiveName == "CHLD00I0.RCI" ) {
		// Transfer properties to pCIFArchDescInOut
		pCIFArchDescInOut->Compression = 0;
		pCIFArchDescInOut->cx = 64;
		pCIFArchDescInOut->cy = 64;
		pCIFArchDescInOut->bFinished = FALSE;
		(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

		// Get this image
		DecodeImage( pCIFArchDescInOut, m_pArchive + dwOffset );

		// Update dwOffset to next image
		pCIFArchDescInOut->dwOffset += (64 * 64);

		return TRUE;
	}// end if ( m_strArchiveName == "FACES.CIF" || m_strArchiveName == "TFAC00I0.RCI" || m_strArchiveName == "CHLD00I0.RCI"  )

	// Handle BUTTONS.RCI
	if ( m_strArchiveName == "BUTTONS.RCI" ) {
		// Transfer properties to pCIFArchDescInOut
		pCIFArchDescInOut->Compression = 0;
		pCIFArchDescInOut->cx = 32;
		pCIFArchDescInOut->cy = 16;
		pCIFArchDescInOut->bFinished = FALSE;
		(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

		// Get this image
		DecodeImage( pCIFArchDescInOut, m_pArchive + dwOffset );

		// Update dwOffset to next image
		pCIFArchDescInOut->dwOffset += (32 * 16);

		return TRUE;
	}// end if ( m_strArchiveName == "BUTTONS.RCI" )

	// Handle MPOP.RCI
	if ( m_strArchiveName == "MPOP.RCI" ) {
		// Transfer properties to pCIFArchDescInOut
		pCIFArchDescInOut->Compression = 0;
		pCIFArchDescInOut->cx = 17;
		pCIFArchDescInOut->cy = 17;
		pCIFArchDescInOut->bFinished = FALSE;
		(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

		// Get this image
		DecodeImage( pCIFArchDescInOut, m_pArchive + dwOffset );

		// Update dwOffset to next image
		pCIFArchDescInOut->dwOffset += (17 * 17);

		return TRUE;
	}// end if ( m_strArchiveName == "MPOP.RCI" )

	// Handle SPOP.RCI
	if ( m_strArchiveName == "SPOP.RCI" ) {
		// Transfer properties to pCIFArchDescInOut
		pCIFArchDescInOut->Compression = 0;
		pCIFArchDescInOut->cx = 22;
		pCIFArchDescInOut->cy = 22;
		pCIFArchDescInOut->bFinished = FALSE;
		(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

		// Get this image
		DecodeImage( pCIFArchDescInOut, m_pArchive + dwOffset );

		// Update dwOffset to next image
		pCIFArchDescInOut->dwOffset += (22 * 22);

		return TRUE;
	}// end if ( m_strArchiveName == "SPOP.RCI" )

	// Handle NOTE.RCI
	if ( m_strArchiveName == "NOTE.RCI" ) {
		// Transfer properties to pCIFArchDescInOut
		pCIFArchDescInOut->Compression = 0;
		pCIFArchDescInOut->cx = 44;
		pCIFArchDescInOut->cy = 9;
		pCIFArchDescInOut->bFinished = FALSE;
		(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

		// Get this image
		DecodeImage( pCIFArchDescInOut, m_pArchive + dwOffset );

		// Update dwOffset to next image
		pCIFArchDescInOut->dwOffset += (44 * 9);

		return TRUE;
	}// end if ( m_strArchiveName == "NOTE.RCI" )

	// Handle weapon CIF files
	if ( m_strArchiveName.Left(5) == "WEAPO" ) {
		// Handle first image
		if ( 0 == pCIFArchDescInOut->nCurImage ) {
			if ( m_strArchiveName != "WEAPON09.CIF" ) {
				// Transfer properties to pCIFArchDescInOut
				pCIFArchDescInOut->nSubImage = 0;
				pCIFArchDescInOut->cx = pHeader->cx;
				pCIFArchDescInOut->cy = pHeader->cy;
				pCIFArchDescInOut->bFinished = FALSE;
				pCIFArchDescInOut->Compression = 0;
				pCIFArchDescInOut->bWeaponGroup = FALSE;
				pCIFArchDescInOut->bFirstInGroup = FALSE;
				
				// Get this image
				DecodeImage( pCIFArchDescInOut, &pHeader->Data );

				// Update dwOffset to next image, subtract one to account for bogus Data entry in header
				pCIFArchDescInOut->dwOffset += ((sizeof(IMG_HEADER) - 1) + pHeader->nImageSize);
			}

			// Increment image count
			pCIFArchDescInOut->nCurImage++;
		}
		else {
			// Set first in group flag
			if ( 0 == pCIFArchDescInOut->nSubImage )
				pCIFArchDescInOut->bFirstInGroup = TRUE;
			else
				pCIFArchDescInOut->bFirstInGroup = FALSE;

			// Handle subsequent images
			LPCIF_WEAPONHEADER pWeaponHeader = (LPCIF_WEAPONHEADER)pHeader;
			
			// Transfer properties to pCIFArchDescInOut
			pCIFArchDescInOut->cx = pWeaponHeader->cx;
			pCIFArchDescInOut->cy = pWeaponHeader->cy;
			pCIFArchDescInOut->bFinished = FALSE;
			pCIFArchDescInOut->Compression = CIF_WEAPONGROUP;
			pCIFArchDescInOut->bWeaponGroup = TRUE;
			
			pb = (char*)pHeader;
			pb += pWeaponHeader->OffsetList[pCIFArchDescInOut->nSubImage];
			DecodeImage( pCIFArchDescInOut, pb );

			// Increment subimage count
			pCIFArchDescInOut->nSubImage++;

			// Test for group end
			if ( pWeaponHeader->OffsetList[pCIFArchDescInOut->nSubImage] < 1 ) {
				// Test for end of group images
				if ( pWeaponHeader->OffsetList[31] < 1 ) {
					pCIFArchDescInOut->bFinished = TRUE;
					m_dwLastRecordCount = pCIFArchDescInOut->nCurImage + 1;
					return TRUE;
				}

				// Locate next group header
				pCIFArchDescInOut->dwOffset += pWeaponHeader->OffsetList[31];
				pCIFArchDescInOut->nCurImage++;
				pCIFArchDescInOut->nSubImage = 0;
			}
		}// end if ( 0 == pCIFArchDescInOut->nCurImage )

		return TRUE;
	}// end if ( m_strArchiveName.Left(5) == "WEAPO" )

	// Transfer properties to pCIFArchDescInOut
	pCIFArchDescInOut->Compression = pHeader->Compression;
	pCIFArchDescInOut->cx = pHeader->cx;
	pCIFArchDescInOut->cy = pHeader->cy;
	pCIFArchDescInOut->bFinished = FALSE;
	(dwOffset) ? pCIFArchDescInOut->nCurImage++ : pCIFArchDescInOut->nCurImage = 0;

	// Get this image
	DecodeImage( pCIFArchDescInOut, &pHeader->Data );

	// Update dwOffset to next image, subtract one to account for bogus Data entry in header
	pCIFArchDescInOut->dwOffset += ((sizeof(IMG_HEADER) - 1) + pHeader->nImageSize);

	return TRUE;
}/* GetImage */


//
// BOOL DecodeImage( LPCIF_RECORDDESC pCIFArchDesc, char *pData )
// Read the specified image
// Return: TRUE if successful, otherwise FALSE
//
inline BOOL CIFArchive::DecodeImage( LPCIF_RECORDDESC pCIFArchDesc, char *pData )
{
	// Validate
	_ASSERT( pCIFArchDesc );
	_ASSERT( pData );

	// Locate start of raw image data
	char *pImageRaw = NULL;
	char *pImageBuffer = NULL;
	if ( CIF_COMPRESSED == pCIFArchDesc->Compression ) {
		// Locate header
		char *pb = m_pArchive + pCIFArchDesc->dwOffset;
		LPIMG_HEADER pHeader = (LPIMG_HEADER)pb;
		
		// Decode the RLE image into a buffer
		pImageBuffer = new char[pCIFArchDesc->cx * pCIFArchDesc->cy];
		if ( !pImageBuffer ) return FALSE;
		DecodeRLEImage( pImageBuffer, pCIFArchDesc->cx, pCIFArchDesc->cy, &pHeader->Data );
		pImageRaw = pImageBuffer;
	}
	else if ( CIF_WEAPONGROUP == pCIFArchDesc->Compression ) {
		// Decode the RLE image into a buffer
		pImageBuffer = new char[pCIFArchDesc->cx * pCIFArchDesc->cy];
		if ( !pImageBuffer ) return FALSE;
		DecodeRLEImage( pImageBuffer, pCIFArchDesc->cx, pCIFArchDesc->cy, pData );
		pImageRaw = pImageBuffer;
	}
	else {
		// Simply point to the image data
		pImageRaw = pData;
	}

	// Set colors of output image buffer in R8G8B8 format
	for ( int y = 0; y < pCIFArchDesc->cy; y++ )
	{
		for ( int x = 0; x < pCIFArchDesc->cx; x++ )
		{
			// Determine palette index
			unsigned char nIndex = pImageRaw[ (y * pCIFArchDesc->cx) + x ];
			
			// Extract the RGB colours for this pixel
			char r = m_Palette.colour[nIndex].r;
			char g = m_Palette.colour[nIndex].g;
			char b = m_Palette.colour[nIndex].b;
			
			// Set the pixel colour
			unsigned char *pOut = (unsigned char*)&pCIFArchDesc->pBuffer[(y*CIFSIZE_IMAGE_PITCH)+(x*3)];
			pOut[0] = b;
			pOut[1] = g;
			pOut[2] = r;
		}// end for ( x = 0; x < cx; x++ )
	}// end for ( y = 0; y < cy; y++ )

	// Clean up
	if ( pImageBuffer ) {
		delete[] pImageBuffer;
	}

	return TRUE;
}/* DecodeImage */


//
// void DecodeRLEImage( char *pImageRaw, int cx, int cy, char* pDataIn )
// Decode a RLE compressed image
//
inline void CIFArchive::DecodeRLEImage( char *pDataOut, int cx, int cy, char* pDataIn )
{
	// Validate
	_ASSERT( pDataOut );
	_ASSERT( pDataIn );

	// Read RLE compressed image from the raw data
	int nCount = 0;
	unsigned char code = 0;
	char* pData = pDataIn;
	DWORD dwPos = 0, dwLength = cx * cy;
	while ( dwPos < dwLength ) {
		// Decode image pixels into buffer
		code = pData[0];
		pData++;
		if ( code > 127 ) {
			// Stream RLE bytes
			for ( nCount = 0; nCount < code - 127; nCount++ ) {
				pDataOut[dwPos++] = pData[0];
			}
			pData++;
		}
		else {
			// Stream non-RLE bytes
			for ( nCount = 0; nCount < code + 1; nCount++ ) {
				pDataOut[dwPos++] = pData[0];
				pData++;
			}
		}// end if ( pChunk->nCount > 0 )
		_ASSERT( dwPos <= dwLength );
	}// while ( dwPos < dwLength )
}/* DecodeRLEImage */


//////////////////////////////////////////////////////////////////////
// CDaggerTool Construction / Destruction
//////////////////////////////////////////////////////////////////////


CDaggerTool::CDaggerTool()
{
	// Initialise
	m_bArena2Open = FALSE;
}


CDaggerTool::~CDaggerTool()
{
	CloseArena2();
}


//////////////////////////////////////////////////////////////////////
// CDaggerTool Member Functions
//////////////////////////////////////////////////////////////////////


//
// BOOL OpenArena2( LPCSTR pszPath, BOOL bReadOnly = TRUE )
// Open Arena2 media path
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerTool::OpenArena2( LPCSTR pszPath, BOOL bReadOnly/*=TRUE*/ )
{
	// Validate
	_ASSERT( pszPath );

	// Test Arena2 path
	if ( !TestArena2Path( pszPath ) )
		return FALSE;

	// Close existing
	if ( IsArena2Open() )
		CloseArena2();

	// Create DTXArchive
	if ( !m_dtxArchive.Create( pszPath ) )
		return FALSE;

	// Open ARCH3D.BSA for general use
	if ( !m_Arch3D.Open( pszPath, bReadOnly ) ) {
		return FALSE;
	}

	// Open BLOCKS.BSA for general use
	if ( !m_ArchBlocks.Open( pszPath, bReadOnly ) ) {
		m_Arch3D.Close();
		return FALSE;
	}

	// Open MAPS.BSA for general use
	if ( !m_ArchMaps.Open( pszPath, bReadOnly ) ) {
		m_ArchBlocks.Close();
		m_Arch3D.Close();
		return FALSE;
	}

	// Store path
	m_strArena2Path = pszPath;

	// Set flags
	m_bArena2Open = TRUE;

	return TRUE;
}/* OpenArena2 */


//
// void CloseArena2()
// Close an open Arena2 path
//
void CDaggerTool::CloseArena2()
{
	if ( !IsArena2Open() )
		return;

	// Initialise
	m_bArena2Open = FALSE;
	m_strArena2Path = "";
	m_dtxArchive.Close();
	m_Arch3D.Close();
	m_ArchBlocks.Close();
	m_ArchMaps.Close();
}/* CloseArena2 */


//
// BOOL TestArena2Path( LPCSTR pszPath )
// Test Arena2 path for validity
// Return: TRUE if valid, otherwise FALSE
//
BOOL CDaggerTool::TestArena2Path( LPCSTR pszPath )
{
	// KLUDGE: These basic tests must be enhanced

	// Test for existance of WOODS.WLD
	CString strPath = pszPath;
	strPath += "\\woods.wld";
	CFile file;
	if ( !file.Open( strPath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
	{
		return FALSE;
	}
	else
	{
		file.Close();
	}

	return TRUE;
}/* TestArena2Path */


//
// BOOL OpenTextureArchive( LPCSTR pszName )
// Helper method to open a texture archive
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerTool::OpenTextureArchive( LPCSTR pszName )
{
	// Attempt to open the archive
	return m_dtxArchive.Open( pszName );
}/* OpenTextureArchive */


//
// BOOL OpenCIFArchive( LPCSTR pszName )
// Helper method to open a CIF archive
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerTool::OpenCIFArchive( LPCSTR pszName )
{
	// Attempt to open the archive
	return m_cifArchive.Open( m_strArena2Path, pszName );
}/* OpenCIFArchive */


//
// BOOL GetIMGDesc( LPCSTR pszName, LPIMG_IMAGEDESC pIMGDescOut )
// Get description of specified IMG image
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerTool::GetIMGDesc( LPCSTR pszName, LPIMG_IMAGEDESC pIMGDescOut )
{
	// Validate
	_ASSERT( pszName );
	_ASSERT( pIMGDescOut );

	// Open IMG file
	CFile file;
	CString strPath;
	strPath.Format( "%s\\%s", m_strArena2Path, pszName );
	if ( !file.Open( strPath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
		return FALSE;

	// Read the file into memory
	DWORD dwLength = (DWORD)file.GetLength();
	char *pBuffer = new char[dwLength];
	if ( !pBuffer ) {
		file.Close();
		return FALSE;
	}
	else {
		if ( dwLength != file.Read( pBuffer, dwLength ) ) {
			delete[] pBuffer;
			file.Close();
			return FALSE;
		}
	}
	file.Close();

	// Acquire header
	LPIMG_HEADER pHeader = (LPIMG_HEADER)pBuffer;

	// Set image dimensions
	switch ( dwLength )
	{
	case 720:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 9;
		pIMGDescOut->dwHeight = 80;
		break;

	case 990:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 45;
		pIMGDescOut->dwHeight = 22;
		break;

	case 1720:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 43;
		pIMGDescOut->dwHeight = 40;
		break;

	case 2140:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 107;
		pIMGDescOut->dwHeight = 20;
		break;

	case 2916:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 81;
		pIMGDescOut->dwHeight = 36;
		break;

	case 3200:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 40;
		pIMGDescOut->dwHeight = 80;
		break;

	case 3938:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 179;
		pIMGDescOut->dwHeight = 22;
		break;

	case 4280:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 107;
		pIMGDescOut->dwHeight = 40;
		break;

	case 4508:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 322;
		pIMGDescOut->dwHeight = 14;
		break;

	case 20480:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 320;
		pIMGDescOut->dwHeight = 64;
		break;

	case 26496:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 184;
		pIMGDescOut->dwHeight = 144;
		break;

	case 64000:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 320;
		pIMGDescOut->dwHeight = 200;
		break;

	case 64768:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = TRUE;
		pIMGDescOut->dwWidth = 320;
		pIMGDescOut->dwHeight = 200;
		break;

	case 68800:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 320;
		pIMGDescOut->dwHeight = 215;
		break;

	case 112128:
		pIMGDescOut->bHeader = FALSE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = 512;
		pIMGDescOut->dwHeight = 219;
		break;

	default:
		pIMGDescOut->bHeader = TRUE;
		pIMGDescOut->bPalette = FALSE;
		pIMGDescOut->dwWidth = pHeader->cx;
		pIMGDescOut->dwHeight = pHeader->cy;
		break;
	}

	// Set required palette
	CString strName = pszName;
	if ( strName == "DANK02I0.IMG" )
		strcpy( pIMGDescOut->szPalette, "DANKBMAP.COL" );
	else if ( strName.Left(4) == "FMAP" )
		strcpy( pIMGDescOut->szPalette, "FMAP_PAL.COL" );
	else if ( strName.Left(4) == "NITE" )
		strcpy( pIMGDescOut->szPalette, "NIGHTSKY.COL" );
	else
		strcpy( pIMGDescOut->szPalette, "ART_PAL.COL" );

	// Set required image buffer size
	pIMGDescOut->dwBufferSize = (pIMGDescOut->dwWidth * 3) * pIMGDescOut->dwHeight;

	// Store name of this image
	m_strLastIMGName = pszName;

	// Clean up
	delete[] pBuffer;

	return TRUE;
}/* GetIMGDesc */


//
// BOOL GetIMGImage( LPCSTR pszName, char* pBufferOut, LPIMG_IMAGEDESC pIMGDesc = NULL )
// Read image data into the provided buffer in R8G8B8 format
// Note: Buffer must be of correct size
// Return: TRUE if successful, otherwise FALSE
//
BOOL CDaggerTool::GetIMGImage( LPCSTR pszName, char* pBufferOut, LPIMG_IMAGEDESC pIMGDesc/*=NULL*/ )
{
	// Validate
	_ASSERT( pszName );
	_ASSERT( pBufferOut );

	// Acquire description if not provided
	IMG_IMAGEDESC id;
	if ( !pIMGDesc ) {
		if ( !GetIMGDesc( pszName, &id ) )
			return FALSE;

		pIMGDesc = &id;
	}

	// Open IMG file
	CFile file;
	CString strPath;
	strPath.Format( "%s\\%s", m_strArena2Path, pszName );
	if ( !file.Open( strPath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) )
		return FALSE;

	// Read the file into memory
	DWORD dwLength = (DWORD)file.GetLength();
	char *pBuffer = new char[dwLength];
	if ( !pBuffer ) {
		file.Close();
		return FALSE;
	}
	else {
		if ( dwLength != file.Read( pBuffer, dwLength ) ) {
			delete[] pBuffer;
			file.Close();
			return FALSE;
		}
	}
	file.Close();

	// Acquire header
	LPIMG_HEADER pHeader = (LPIMG_HEADER)pBuffer;

	// This assertion is tripped if compression is enabled
	_ASSERT( pHeader->Compression != CIF_COMPRESSED );

	// Read palette
	palette_t pal;
	if ( pIMGDesc->bPalette ) {
		// Open the palette from within the image buffer
		char *pb = pBuffer;
		pb += (dwLength - 768);
		for ( int pe = 0; pe < 256; pe++ ) {
			pal.colour[pe].r = pb[0] << 2;
			pal.colour[pe].g = pb[1] << 2;
			pal.colour[pe].b = pb[2] << 2;
			pb += 3;
		}
	}
	else {
		// Open the palette from a file
		CString strPalette;
		strPalette.Format( "%s\\%s", m_strArena2Path, pIMGDesc->szPalette );
		if ( !file.Open( strPalette, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ) ) {
			delete[] pBuffer;
			return FALSE;
		}
		
		// Read the palette
		if ( strPalette.Right( 3 ) == "PAL" ) {
			file.Read( (void*)&pal, 768 );
		}
		else if ( strPalette.Right( 3 ) == "COL" )
		{
			file.Seek( 8, CFile::begin );
			file.Read( (void*)&pal, 768 );
		}
		else {
			delete[] pBuffer;
			return FALSE;
		}
		file.Close();
	}

	// Locate start of image data
	char *pImageRaw = NULL;
	if ( pIMGDesc->bHeader ) {
		pImageRaw = &pHeader->Data;
	}
	else {
		pImageRaw = pBuffer;
	}

	// Read the image into the temporary buffer
	DWORD dwPitch = pIMGDesc->dwWidth * 3;

	// Set colors of image buffer in R8G8B8 format
	for ( int y = 0; y < (int)pIMGDesc->dwHeight; y++ )
	{
		for ( int x = 0; x < (int)pIMGDesc->dwWidth; x++ )
		{
			// Determine palette index
			unsigned char nIndex = pImageRaw[ y * pIMGDesc->dwWidth + x ];

			// Extract the RGB colours for this pixel
			char r = pal.colour[nIndex].r;
			char g = pal.colour[nIndex].g;
			char b = pal.colour[nIndex].b;

			// Set the pixel colour
			char *pOut = &pBufferOut[(y*dwPitch)+(x*3)];
			pOut[0] = b;
			pOut[1] = g;
			pOut[2] = r;
		}// end for ( x = 0; x < cx; x++ )
	}// end for ( y = 0; y < cy; y++ )

	// Clean up
	delete[] pBuffer;

	return TRUE;
}/* GetIMGImage */
