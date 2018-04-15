/*************************************************************************************************\
*
* Filename:		DaggerTool.h
* Purpose:		Define all Daggerfall tool classes
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


#if !defined(AFX_DAGGERTOOL_H__51AEEE08_B1F9_4F58_836D_78D53196E7CC__INCLUDED_)
#define AFX_DAGGERTOOL_H__51AEEE08_B1F9_4F58_836D_78D53196E7CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DFTypes.h"


// Predefine classes
class CDaggerTool;
class DTXArchive;
class BSAArchive;

// Defines
#define DFTOOL_SOLIDTEXTURESIZE 32


// Helper macros
#define GetDFFP( d, s ) {long wp = s >> 8; float dp = (unsigned char)s / 1000.0f; d = float(wp) + dp;}	// Convert a DF 24.8 fixed point number to a float


// Values used throughout CDaggerTool
enum
{
	DTX_RECORDRLE = 0x1108,
	DTX_IMAGERLE = 0x0108,
	DTX_ROWENCODED = 0x8000,
	CIF_COMPRESSED = 0x02,
	CIF_WEAPONGROUP = 0x03,
	BSA_RECORDTYPESHORT = 0x0200,
	BSA_RECORDTYPELONG = 0x0100,
	DFTOOL_NOPATCH = 0x8000,
};


// ARCH3D Version enum
enum {
	ARCH3DVERSION_NONE,
	ARCH3DVERSION_27,
	ARCH3DVERSION_26,
	ARCH3DVERSION_25,
	ARCH3DVERSION_UNKNOWN,
};


// Single untransformed vertex for 3D objects
typedef struct _tagObjectVertex
{
	D3DXVECTOR3		pos;
	D3DXVECTOR3		normal;
	FLOAT			tu, tv;
} OBJVERTEX, *LPOBJVERTEX;
#define D3DFVF_OBJVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


// Single untransformed face
typedef struct _tagObjectFace
{
	OBJVERTEX v0, v1, v2;
} OBJFACE, *LPOBJFACE;


// Define a single entry in the palette
typedef struct
{
	unsigned char r, g, b;
} colour_t;

// Define the palette
typedef struct
{
	colour_t colour[256];
} palette_t;


// Image Description for GetImage() method of DTXArchive
typedef struct _tagImageDescription
{
	int		cx;								// Image width in pixels
	int		cy;								// Image height in pixels
	char	pBuffer[DTXSIZE_IMAGE_BUFFER];	// Image buffer in R8G8B8 format
} DTX_IMAGEDESC, *LPDTX_IMAGEDESC;


// Image description for GetIMGDesc() method of CDaggerTool
typedef struct _tagIMG_IMAGEDESC
{
	BOOL		bHeader;			// TRUE if file has header, otherwise FALSE
	BOOL		bPalette;			// TRUE if file has palette, otherwise FALSE
	DWORD		dwWidth;			// Image width in pixels
	DWORD		dwHeight;			// Image height in pixels
	DWORD		dwBufferSize;		// Size of required buffer in bytes
	char		szPalette[16];		// Name of required .PAL or .COL file
} IMG_IMAGEDESC, *LPIMG_IMAGEDESC;


// Archive description of CIF archive
typedef struct _tagCIF_ARCHIVEDESC
{
	int		nCurImage;						// Current image
	int		nSubImage;						// Subimage for weapon group formats
	DWORD	dwOffset;						// Offset to this image
	short	Compression;					// Specifies if compression is enabled
	int		cx;								// Width of image
	int		cy;								// Height of image
	char	pBuffer[CIFSIZE_IMAGE_BUFFER];	// Size of required buffer in bytes
	BOOL	bFinished;						// TRUE if no more images can be found
	BOOL	bWeaponGroup;					// TRUE if this is a weapon group image
	BOOL	bFirstInGroup;					// TRUE if first image in group
} CIF_RECORDDESC, *LPCIF_RECORDDESC;


// Image description for BLOCKS.BSA automaps
typedef struct _BLKIMG_AUTOMAP
{
	char	pBuffer[BLKSIZE_IMAGE_BUFFER];
} BLKIMG_AUTOMAP, *LPBLKIMG_AUTOMAP;


// Base face UV values
typedef struct _FACEUV
{
	float	u0, v0;
	float	u1, v1;
	float	u2, v2;
	float	u3, v3;
} FACEUV, *LPFACEUV;


// Solid colour reference types
enum
{
	DTX_NOTSOLID,
	DTX_SOLIDA,
	DTX_SOLIDB,
};


// Class to manage texture archives
class DTXArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	DTXArchive();
	virtual ~DTXArchive();

public:		// methods
	bool	Create( LPCSTR pszArena2Path );
	bool	Open( LPCSTR pszArchive );
	void	Close();
	bool	IsOpen() {return m_bIsOpen;}

	DWORD	GetRecordCount() {return m_dwRecordCount;}
	DWORD	GetImageCount( DWORD dwRecord ) {
		if ( m_nSolidType ) return 1;
		return LPDTX_RECORDHEADER(m_pArchive + m_pRecordDirectory[dwRecord].nOffset)->nImages;
	}
	void	GetImageRect( DWORD dwRecord, DWORD dwImage, LPRECT pRectOut ) {
		pRectOut->left = 0;
		pRectOut->top = 0;
		if ( m_nSolidType ) {
			pRectOut->right = DFTOOL_SOLIDTEXTURESIZE;
			pRectOut->bottom = DFTOOL_SOLIDTEXTURESIZE;
		}
		else {
			LPDTX_RECORDHEADER pRecordHeader = LPDTX_RECORDHEADER(m_pArchive + m_pRecordDirectory[dwRecord].nOffset);
			pRectOut->right = pRecordHeader->cx;
			pRectOut->bottom = pRecordHeader->cy;
		}
	}
	BOOL	GetImage( DWORD dwRecord, DWORD dwImage, LPDTX_IMAGEDESC pImageDescOut, char* pBitsOut = NULL, DWORD dwPitchInBytes = 0 );

	const char*	GetArchiveName() {return m_strArchiveName.GetBuffer(1);}
	const char*	GetArchiveDesc() {return m_strArchiveDesc.GetBuffer(1);}

private:	// methods
	void	DecodeRLERows( char *pImageRaw, LPDTX_RECORDHEADER pRecordHeader, LPDTX_RLEROWDIRECTORY pRowDirectory, int cx, int cy );

private:	// data
	CString		m_strArena2Path;
	CString		m_strArchiveName;
	CString		m_strArchiveDesc;
	bool		m_bIsCreated;
	bool		m_bIsOpen;
	long		m_nSolidType;
	DWORD		m_dwLength;
	DWORD		m_dwRecordCount;
	char*		m_pArchive;
	palette_t	m_Palette;

	LPDTX_ARCHIVEHEADER		m_pHeader;
	LPDTX_RECORDDIRECTORY	m_pRecordDirectory;
};
typedef DTXArchive* LPDTX_ARCHIVE;


// Class to manage CIF archives
class CIFArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	CIFArchive();
	virtual ~CIFArchive();

public:		// methods
	BOOL	Open( LPCSTR pszPath, LPCSTR pszArchive );
	void	Close();
	BOOL	IsOpen() {return m_bIsOpen;}

	BOOL	GetImage( LPCIF_RECORDDESC pCIFArchDescInOut );

	const char*	GetArchiveName() {return m_strArchiveName.GetBuffer(1);}

	DWORD	GetLastRecordCount() {return m_dwLastRecordCount;}

private:	// methods
	BOOL	DecodeImage( LPCIF_RECORDDESC pCIFArchDesc, char *pData );
	void	DecodeRLEImage( char *pImageRaw, int cx, int cy, char* pDataIn );

private:	// data
	DWORD		m_dwLastRecordCount;
	CString		m_strArena2Path;
	CString		m_strArchiveName;
	BOOL		m_bIsOpen;
	DWORD		m_dwLength;
	char*		m_pArchive;
	palette_t	m_Palette;
};
typedef CIFArchive* LPCIF_ARCHIVE;


// Class to manage BSA archives
class BSAArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	BSAArchive();
	virtual ~BSAArchive();

public:		// methods
	bool	Open( LPCSTR pszPath, LPCSTR pszArchive, BOOL bReadOnly = TRUE );
	void	Close();
	bool	IsOpen() {return m_bIsOpen;}

	long	GetRecordDirectoryType() {return m_nRecordDirectoryType;}
	long	GetRecordCount() {return m_nRecordCount;}
	long	GetRecordLength( long nRecord );
	bool	GetRecordDesc( long nRecord, char* pBufferOut, long nLength );
	bool	GetRecord( long nRecord, char* pDataOut, long nLength );
	bool	SetRecord( long nRecord, char *pDataIn, long nLength );

protected:	// data
	CString		m_strArena2Path;
	CFile		m_bsaFile;
	bool		m_bIsOpen;
	DWORD		m_dwLength;
	long		m_nRecordCount;
	long		m_nRecordDirectoryType;
	long		m_nRecordDirectoryEntryLength;

	BSA_ARCHIVEHEADER			m_bsaHeader;
	LPBSA_RECORDDIRECTORYLONG	m_pRecordDirectoryLong;
	LPBSA_RECORDDIRECTORYSHORT	m_pRecordDirectoryShort;
};


// Class to manage ARCH3D.BSA
class BSAArch3D : public BSAArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	BSAArch3D();
	virtual ~BSAArch3D();

public:		// methods
	bool	Open( LPCSTR pszPath, BOOL bReadOnly = TRUE );
	void	Close();
	bool	OpenObject( long nObject );
	bool	OpenObject( LPCSTR pszObjectID );
	void	CloseObject();
	bool	IsObjectOpen() {return m_bIsObjectOpen;}

	long	GetVersion() {return m_lVersion;}
	float	GetVersionAsFloat();

	long	GetFaceCount();
	LPARCH3D_FACE	GetFace( int nFace );
	bool	GetFaceTexture( LPARCH3D_FACE pFace, long* pnArchiveOut, long* pnRecordOut );
	bool	GetPureFaceUV( LPARCH3D_FACE pFace, LPSIZE uvOut );
	bool	PatchPointUV( LPARCH3D_FACE pFace, int nPoint, int U, int V );

	long	GetPointCount( LPARCH3D_FACE pFace );
	bool	GetPoint( LPARCH3D_FACE pFace, int nPoint, LPOBJVERTEX pvOut );
	long	GetCornerPoints( LPARCH3D_FACE pFace, int* pCornerBuffer = NULL, LPOBJVERTEX pPointBuffer = NULL );

	bool	ExportPureFace( LPARCH3D_FACE pFace, LPOBJVERTEX pvOut, int* pPointCountOut );
	bool	ExportTriangulatedFace( LPARCH3D_FACE pFace, LPOBJVERTEX pvOut, int* pPointCountOut, LPSIZE pSrcSize = NULL, LPRECT pSubRect = NULL );

	bool	SetPointUV( LPARCH3D_FACE pFace, int nPoint, short u, short v );
	bool	SetFaceUV( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect );
	bool	SetFaceUVOld( LPARCH3D_FACE pFace, long nCornerCount, LPOBJVERTEX pCornerVertexBuffer, LPSIZE pSrcSize, LPRECT pSubRect );
	bool	SaveObject();

	long	ResolveObjectID( long ObjectID );

private:	// data
	bool			m_bIsObjectOpen;
	char*			m_pData;
	long			m_nDataLength;
	LPARCH3D_HEADER m_pHeader;
	LPARCH3D_POINT	m_pPointList;
	LPARCH3D_POINT	m_pNormalList;
	long			m_lVersion;
	long			m_nCurObject;

	LPBSA_RECORDDIRECTORYSHORT	m_pRecordDirectory;

	THashArray<UINT, 1999>		m_haObjectID;
};


class BSABlocks : public BSAArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	BSABlocks();
	virtual ~BSABlocks();

public:		// methods
	bool	Open( LPCSTR pszPath, BOOL bReadOnly = TRUE );
	void	Close();
	bool	OpenBlock( long nBlock );
	void	CloseBlock();
	bool	IsBlockOpen() {return m_bIsBlockOpen;}

	long	GetBlockSubRecordCount() {return m_nBlockSubRecordCount;}
	long	GetOutsideBlockSubRecordCount() {return m_nOutsideBlockSubRecordCount;}
	long	GetInsideBlockSubRecordCount() {return m_nInsideBlockSubRecordCount;}

	LPRMBFLD			GetFLD() {return m_pFLD;}
	LPRMB_BLOCKHEADER	GetOutsideBlockSubRecord( long nRecord );
	LPRMB_BLOCKHEADER	GetInsideBlockSubRecord( long nRecord );

	LPRMB_BLOCK3DOBJECTS	GetBlockSubRecord3DObjects( LPRMB_BLOCKHEADER pBlockHeader );
	LPRMB_BLOCK3DOBJECTS	GetBlock3DObjects();

	bool	GetBlockAutomap( LPBLKIMG_AUTOMAP pAutomapOut );

private:	// data
	bool			m_bIsBlockOpen;
	char*			m_pData;
	long			m_nDataLength;
	long			m_nCurBlock;

	long			m_nBlockSubRecordCount;
	long			m_nOutsideBlockSubRecordCount;
	long			m_nInsideBlockSubRecordCount;

	LPRMBFLD		m_pFLD;

	LPBSA_RECORDDIRECTORYLONG	m_pRecordDirectory;
};


class BSAMaps : public BSAArchive
{
public:		// construction / destruction
	friend class CDaggerTool;
	BSAMaps();
	virtual ~BSAMaps();

public:		// methods
	bool	Open( LPCSTR pszPath, BOOL bReadOnly = TRUE );
	void	Close();

private:	// data
	LPBSA_RECORDDIRECTORYLONG	m_pRecordDirectory;
};


class CDaggerTool
{
public:		// construction / destruction
	CDaggerTool();
	virtual ~CDaggerTool();

public:		// methods
	BOOL	OpenArena2( LPCSTR pszPath, BOOL bReadOnly = TRUE );
	void	CloseArena2();
	BOOL	IsArena2Open() {return m_bArena2Open;}

	BOOL	OpenTextureArchive( LPCSTR pszName );
	BOOL	OpenCIFArchive( LPCSTR pszName );

	BOOL	GetIMGDesc( LPCSTR pszName, LPIMG_IMAGEDESC pIMGDescOut );
	BOOL	GetIMGImage( LPCSTR pszName, char* pBufferOut, LPIMG_IMAGEDESC pIMGDesc = NULL );

	const char*	GetLastIMGName() {return m_strLastIMGName.GetBuffer(1);}

public:		// objects
	CString		m_strArena2Path;
	CString		m_strLastIMGName;
	DTXArchive	m_dtxArchive;
	CIFArchive	m_cifArchive;
	BSAArch3D	m_Arch3D;
	BSABlocks	m_ArchBlocks;
	BSAMaps		m_ArchMaps;

protected:	// methods
	BOOL	TestArena2Path( LPCSTR pszPath );

private:	// data
	BOOL	m_bArena2Open;
};


#endif // !defined(AFX_DAGGERTOOL_H__51AEEE08_B1F9_4F58_836D_78D53196E7CC__INCLUDED_)
