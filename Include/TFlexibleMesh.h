// TFlexibleMesh.h: interface for the TFlexibleMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFLEXIBLEMESH_H__AA1AAB3E_4691_467A_B4C7_B8E288FE4847__INCLUDED_)
#define AFX_TFLEXIBLEMESH_H__AA1AAB3E_4691_467A_B4C7_B8E288FE4847__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TString.h"
#include "TStaticArray.h"


/*

.FMF File

Header
Texture Source String
Vertex Array
Face Array
Face Texture Array
Texture Table
	- Name
	- Width
	- Height
	- Bits

*/


// Declarations
class TFlexibleMesh;
typedef TFlexibleMesh FLEXIBLEMESH;
typedef TFlexibleMesh* LPFLEXIBLEMESH;


// Defines
#define AFMF_SIGNATURE 0x464d4641
#define AFMF_CURRENT_VERSION 1
#define	AFMF_LOWEST_SUPPORTED_VERSION 1
#define AFMF_HIGHEST_SUPPORTED_VERSION 1

// Mesh flags
enum AFMF_MESHFLAGS
{
	AFMFFLAG_DRAWWIREFRAME		= 0x01,			// Alchemy to render mesh as wireframe
};

// Supported textures sources
enum AFMF_TEXTURESOURCE
{
	AFMFTS_NONE,		// No textures
	AFMFTS_LOCAL,		// Stored locally
	AFMFTS_FILE,		// Filesystem
	AFMFTS_STORE,		// Alchemy volatile texture store
	AFMFTS_PACK,		// Alchemy resource package
};

// Centre types
enum AFMF_CENTREFLAGS
{
	AFMFCENTRE_X			= 0x01,
	AFMFCENTRE_Y			= 0x02,
	AFMFCENTRE_Z			= 0x04,
	AFMFCENTRE_CENTREALL	= 0x07,
	AFMFCENTRE_MINX			= 0x08,
	AFMFCENTRE_MINY			= 0x10,
	AFMFCENTRE_MINZ			= 0x20,
	AFMFCENTRE_MAXX			= 0x30,
	AFMFCENTRE_MAXY			= 0x40,
	AFMFCENTRE_MAXZ			= 0x50,
};

// Header definition
typedef struct _tagTFMFHeader
{
	DWORD		dwSignature;			// Set to 'AFMF'
	DWORD		dwVersion;				// Version of this mesh
	DWORD		dwFlags;				// Flags for this mesh
	DWORD		dwVertexOffset;			// Offset to vertex array
	DWORD		dwVertexCount;			// Number of vertices in this mesh
	DWORD		dwPrimitiveType;		// Of enum D3DPRIMITIVETYPE
	DWORD		dwFaceOffset;			// Offset to face array
	DWORD		dwFaceTexturesOffset;	// Offset to face texture array
	DWORD		dwFaceCount;			// Number of faces in this mesh
	DWORD		dwTextureTableOffset;	// Offset to texture table
	DWORD		dwTextureTableCount;	// Number of items in texture table
	DWORD		dwTextureSourceType;	// Of enum AFMF_TEXTURESOURCE
	DWORD		dwTextureSrcStrLength;	// Length of strTextureSource in bytes
} AFMF_HEADER, *LPAFMF_HEADER;

// Mesh vertex description - ACVERTEXDESC compatible
typedef struct _tagTFMFVertexDescription
{
	float		x, y, z;				// Position of this vertex in local coordinates
	float		nx, ny, nz;				// Direction of vertex normal
	float		tu, tv;					// Texture coordinates
} AFMF_VERTEXDESC, *LPAFMF_VERTEXDESC;
#define D3DFVF_AFMF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

// Face description - ACVERTEXDESC compatible
typedef struct _tagTFMFFaceDescription
{
	unsigned short	v0, v1, v2;			// Index to vertex description
} AFMF_FACEDESC, *LPAFMF_FACEDESC;

// Texture table entry
typedef struct _tagTFMFTextureTableEntry
{
	_tagTFMFTextureTableEntry() {
		Width = 0;
		Pitch = 0;
		Height = 0;
		pBits = NULL;
		dwColorKey = 0;
		nTexture = NULL;
	}

	~_tagTFMFTextureTableEntry() {
		if ( pBits ) {
			delete[] pBits;
			pBits = NULL;
		}
	}

	TString			strName;		// Unique friendly name
	unsigned short	Width;			// Width of texture
	unsigned short	Pitch;			// Pitch of texture
	unsigned short	Height;			// Height of texture
	RECT			rctTexture;		// Rect of actual texture pixels within bits
	DWORD			dwColorKey;		// Colour key used for texture caching operations
	UINT			nTexture;		// Set by Alchemy to texture resource in store
	char*			pBits;			// Pointer to R8G8B8 bits if texture stored locally
} AFMF_TEXTURE, *LPAFMF_TEXTURE;


// Texture table file header
typedef struct _tagTFMFTextureTableFileEntry
{
	DWORD			dwLength;		// Total length of this subrecord including header
	DWORD			dwNameLength;	// Length of texture name in bytes
	DWORD			dwBitsOffset;	// Offset to texture bits if available
	unsigned short	Width;			// Width of texture
	unsigned short	Pitch;			// Pitch of texture
	unsigned short	Height;			// Height of texture
	RECT			rctTexture;		// Rect of actual texture pixels within bits
	DWORD			dwColorKey;		// Colour key used for texture caching operations
} AFMF_FILETEXTURE, *LPAFMF_FILETEXTURE;


class TFlexibleMesh  
{
public:		// construction / destruction
	TFlexibleMesh();
	TFlexibleMesh( DWORD dwVertexCount, DWORD dwFaceCount, LPCSTR pszTextureSource = NULL, DWORD dwFlags = 0, DWORD dwPrimitiveType = D3DPT_TRIANGLELIST );
	TFlexibleMesh( TFlexibleMesh& Mesh );
	TFlexibleMesh( TFlexibleMesh* pMesh );
	TFlexibleMesh( LPCSTR pszPath );
	virtual ~TFlexibleMesh();

public:		// methods
	BOOL	Create( DWORD dwVertexCount = 0, DWORD dwFaceCount = 0, LPCSTR pszTextureSource = NULL, DWORD dwFlags = 0, DWORD dwPrimitiveType = D3DPT_TRIANGLELIST );
	BOOL	CreateFromFMF( TFlexibleMesh* pMesh );
	BOOL	MergeFromFMF( TFlexibleMesh* pMesh );
	BOOL	IsOpen() {return m_bIsOpen;}
	BOOL	Open( LPCSTR pszPath );
	BOOL	Save( LPCSTR pszPath );
	BOOL	Optimise();
	BOOL	IsOptimised() {return m_bIsOptimised;}
	void	Close();

	BOOL	SetVertexCount( DWORD dwVertexCount );
	BOOL	SetFaceCount( DWORD dwFaceCount );

	BOOL	AddTexture( LPCSTR pszTexture );
	BOOL	AddTextureBits( LPCSTR pszTexture, DWORD dwDstWidth, DWORD dwDstHeight, LPRECT pSrcRect, DWORD dwSrcPitch, LPVOID pBits, DWORD dwColorKey = 0xff000000 );
	BOOL	FindTexture( LPCSTR pszTexture );

	void	SetVertex( DWORD dwVertex, LPAFMF_VERTEXDESC pVertexDesc );
	void	SetFace( DWORD dwFace, LPAFMF_FACEDESC pFaceDesc, LPCSTR pszTexture = NULL, DWORD dwColorKey = 0xff000000 );

	DWORD	GetPrimitiveType() {return m_dwPrimitiveType;}
	DWORD	GetVertexCount() {return m_dwVertexCount;}
	DWORD	GetFaceCount() {return m_dwFaceCount;}
	DWORD	GetTextureCount() {return m_dwTextureCount;}
	DWORD	GetFlags() {return m_dwFlags;}

	DWORD	GetTextureSource( char* pBufferOut, DWORD dwBufferLength );
	DWORD	GetTextureSourceType() {return m_dwTextureSourceType;}

	LPAFMF_VERTEXDESC GetVertexData() {return m_pVerts;}
	LPAFMF_FACEDESC GetFaceData() {return m_pFaces;}
	UINT* GetFaceTextureData() {return m_pFaceTextures;}

	BOOL	Translate( float fX, float fY, float fZ );
	BOOL	Scale( float fX, float fY, float fZ );
	BOOL	RotateDegrees( float fX, float fY, float fZ );
	BOOL	RotateRadians( float fX, float fY, float fZ );
	BOOL	GetSize( LPD3DXVECTOR3 pvOut );
	BOOL	CentreAxis( DWORD dwFlags = AFMFCENTRE_CENTREALL );
	BOOL	DropAxis();

public:		// operators
	void operator = ( TFlexibleMesh& Mesh );
	void operator = ( LPCSTR pszFilename );
	TFlexibleMesh& operator += ( TFlexibleMesh& Mesh );
	void operator += ( LPCSTR pszFilename );

private:	// methods
	void	Initialise();

public:		// data
	DWORD				m_dwFlags;
	DWORD				m_dwPrimitiveType;
	DWORD				m_dwVertexCount;
	DWORD				m_dwFaceCount;
	UINT				m_dwTextureCount;
	LPAFMF_VERTEXDESC	m_pVerts;
	LPAFMF_FACEDESC		m_pFaces;
	UINT*				m_pFaceTextures;
	TString				m_strTextureSource;
	DWORD				m_dwTextureSourceType;

public:		// objects
	TStaticArray<AFMF_TEXTURE> m_oaTextureTable;

private:	// data
	BOOL				m_bIsOptimised;
	BOOL				m_bIsOpen;
};

#endif // !defined(AFX_TFLEXIBLEMESH_H__AA1AAB3E_4691_467A_B4C7_B8E288FE4847__INCLUDED_)
