// TAlchemy.h: interface for the TAlchemy class.
//
//////////////////////////////////////////////////////////////////////

/*

Pending Changes:

* * Enumerate engine-required caps and store as flags
* Standardize state changes between systems using state blocks
* * Restore resources that are lost when ALT-Tabbing in fullscreen mode
* * More output to logs
* Even more output to logs in debug build
* Allow for easy changes to fullscreen/windowed and states
* Allow for other vertex pipelines
* Split implementation out of header file
* Add scenery support
* Move to a COM object

*/

#if !defined(AFX_TALCHEMY_H__4C926672_0F25_4E97_9341_B811104C1272__INCLUDED_)
#define AFX_TALCHEMY_H__4C926672_0F25_4E97_9341_B811104C1272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDirectX8.h"
#include "TFlexibleMesh.h"
#include "TCamera.h"
#include "TLinkedList.h"
#include "THashArray.h"
#include "TRandom.h"
#include "TQuaternion.h"
#include "AcErrors.h"
#include "D3DFont.h"


// Constant engine values
const UINT	ACNULL = NULL;
const float	ACPHYS_TIMEINTEGRATOR = 40.0f;			// Acceleration and velocity scaled by frame time in milliseconds / ACPHYS_TIMEINTEGRATOR
const float	ACPHYS_GRAVITYCONSTANT = 6.67f;			// Real constant: 6.67*10^-11: Scaled by 10^11 to compensate for reduced mass
const float	ACPHYS_RESTRAINEDMASS = 0.005f;			// Any acceleration below this floor is ignored to simulate a restrained object
const float	ACPHYS_GRAVITYMASSMINIMUM = 1000.0f;	// Only objects with this mass or above will exert gravitational force
const int	MAX_PARTICLE_BUFFER = 100;				// Maximum size of particle vertex buffer in vertices
const UINT	HASHTABLECONST = 241;					// Prime number constant for hash array table size
const UINT	ACMAXBATCH = 128;						// Maximum number of polygons to draw in a single batch
const UINT	ACVBSTREAMLENGTH = 32768;				// Length of pipeline streaming vertex buffer


// Alchemy general definition
enum
{
	AC_DEFAULT		= 0,
	ACBD_16			= 16,
	ACBD_32			= 32,
};


// Alchemy vertex pipeline definition
enum
{
	ACVPIPE_NONE,
	ACVPIPE_PURE,
	ACVPIPE_HARDWARE,
	ACVPIPE_MIXED,
	ACVPIPE_SOFTWARE,

	ACVPIPE_FORCE_DWORD = 0x7fffffff
};


// Defines some relative masses for reference
// Specified in kg / 10^-20
enum
{
	ACMASS_NONE			= 0,
	ACMASS_COMMON		= 1,
	ACMASS_ASTEROID		= 100,
	ACMASS_MOON			= 1000,
	ACMASS_EARTH		= 59800,	// Real mass: 5.98*10^24
	ACMASS_SUN			= 100000,
	ACMASS_BLACKHOLE	= 1000000,

	ACMASS_FORCE_DWORD = 0x7fffffff
};


// Defines available engine object types
enum
{
	ACOBJECT_NONE,
	ACOBJECT_OBJECT,
	ACOBJECT_CAMERA,
	ACOBJECT_ACTOR,
	ACOBJECT_EMITTER,

	ACOBJECT_FORCE_DWORD = 0x7fffffff
};


// Defines available basic resource types
enum
{
	ACRES_NONE,
	ACRES_TEXTURE,
	ACRES_MESH,

	ACRES_FORCE_DWORD = 0x7fffffff
};


// Defines engine capability flags
enum
{
	ACCAP_NONE			= 0x00,
	ACCAP_HWTL			= 0x01,

	ACCAP_FORCE_DWORD = 0x7fffffff
};


// Defines Alchemy create flags
enum
{
	ACCS_ENABLETEXTUREFILTERING		= 0x01,
	ACCS_ENABLEGLOBALGRAVITY		= 0x02,
	ACCS_ENABLELIGHTING				= 0x04,

	ACCS_FORCE_DWORD = 0x7fffffff
};


// Used to initialise TAlchemy
typedef struct _ALCHEMY_CREATESTRUCT
{
	DWORD		dwFlags;			// Various create-time flags
	HINSTANCE	hInstance;			// Handle to instance of parent application
	HWND		hViewWnd;			// Handle to render target window
	HWND		hInputWnd;			// Handle to top-level window
	BOOL		bWindowed;			// TRUE for a windowed application, otherwise FALSE
	DWORD		dwWidth;			// Width of the window (fullscreen only)
	DWORD		dwHeight;			// Height of the window (fullscreen only)
	DWORD		dwColourDepth;		// Colour depth of window (fullscreen only - 16 or 32 bits only)
	DWORD		dwRequiredCaps;		// Flag of required capabilities
	float		fNearPlaneDepth;	// Distance of the near clipping plane
	float		fFarPlaneDepth;		// Distance of the far clipping plane
	D3DXVECTOR3	vGlobalGravity;		// Direction and magnitude of global gravity
	D3DCOLOR	colAmbientLight;	// Colour and intensity of ambient lighting
	D3DCOLOR	colRenderErase;		// Colour used to erase render target prior to rendering
} ALCHEMY_CREATESTRUCT, *LPALCHEMY_CREATESTRUCT;


// Defines common resource properties
struct AcResource
{
	virtual ~AcResource() {
	}

	DWORD			dwRef;			// Reference count of this resource
	TString			strIndex;		// Index string of this resource
};


// Defines a single mesh container for the engine
struct AcMesh : public AcResource
{
	virtual ~AcMesh() {
	}

	TFlexibleMesh	Mesh;			// Mesh resource
	pib_t			ib;				// Static index buffer for mesh
};
typedef AcMesh ACMESH, *LPACMESH;


// Defines a single texture container for the engine
struct AcTexture : public AcResource
{
	AcTexture() {
		dwRef = Width = Height = Depth = CurFrame = FrameCount = TileX = TileY = 0;
		nSource = NULL;
		oaTexture.SetLength( 1, 4 );
	}

	virtual ~AcTexture() {
	}

	UINT		nSource;				// Handle to source texture (if this is a reference texture)
	DWORD		Width;					// Width of this texture
	DWORD		Height;					// Height of this texture
	DWORD		Depth;					// Bit Depth of this texture
	DWORD		CurFrame;				// Current animation frame
	DWORD		FrameCount;				// Total animation frames
	DWORD		TileX;					// Number of horizontal tiles required for Width
	DWORD		TileY;					// Number of vertical tiles required for Height
	TStaticArray<ptx_t>	oaTexture;		// Array is (TileX * TileY * FrameCount) objects in length

	BOOL Manage( LPDIRECT3DTEXTURE8 pd3dTexture, LPRECT ptxrRect, LPRECT pimgRect ) {
		UINT nTexture = oaTexture.New();
		if ( HSTOPERR == nTexture )
			return FALSE;

		oaTexture[nTexture].Set( pd3dTexture, ptxrRect, pimgRect );

		return TRUE;
	}

	LPDIRECT3DTEXTURE8 Get() {
		if ( 0 == oaTexture.GetCount() )
			return NULL;

		return oaTexture[TileX*TileY*CurFrame];
	}

	void Step() {
		if ( ++CurFrame == FrameCount )
			CurFrame = 0;
	}
};
typedef AcTexture ACTEXTURE, *LPACTEXTURE;


// Defines a resource object management unit
typedef struct _ACRESOURCEDESC
{
	_ACRESOURCEDESC() {
		dwType = ACRES_NONE;
		pResource = NULL;
	}

	virtual ~_ACRESOURCEDESC() {
		if ( pResource ) {
			delete pResource;
			pResource = NULL;
		}
		dwType = ACRES_NONE;
	}
	
	DWORD			dwType;				// The type of this resource
	AcResource*		pResource;			// Pointer to the resource itself
	TString			strAlias;			// Alias of this resource
} ACRESOURCEDESC, *LPACRESOURCEDESC;


// Defines settings for DrawViewport
typedef struct _ACDV_SETTINGS
{
	D3DVIEWPORT8	Viewport;		// Viewport settings
	TCamera			Camera;			// Camera location and orientation
	float			w;
	float			h;
	float			zn;
	float			zf;
	BOOL			bShowBorder;	// TRUE to draw borders around viewport
	BOOL			bShowGrid;		// TRUE to draw gridlines in view
	BOOL			bFlatLight;		// TRUE to set flat white ambient light
	BOOL			bOrthogonal;	// TRUE to draw an orthogonal projection
	BOOL			bWireframe;		// TRUE to render wireframe primitives
	BOOL			bBoundingBoxes;	// TRUE to render bounding boxes
	BOOL			bPresent;		// TRUE to present after render
	D3DCOLOR		colBorder;		// Colour of the border
	D3DCOLOR		colGrid;		// Colour of the grid
	D3DCOLOR		colBoundingBox;	// Colour of a bounding box
	DWORD			dwBorderSize;	// Size of the surrounding border in pixels
} ACDV_SETTINGS, *LPACDV_SETTINGS;


// Defines a basic Alchemy world object
// Everything that occupies world space will have these attributes in common (except particles)
class AcObject
{
public:		// construction / destruction
	AcObject() {
		// Initialise
		vPos = vVelocity = vSpinPos = vSpinVelocity = D3DXVECTOR3(0.0f,0.0f,0.0f);
		vRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vFront = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	virtual ~AcObject() {
	}

public:		// data
	D3DXVECTOR3		vPos;				// Location in world space
	D3DXVECTOR3		vVelocity;			// Direction and magnitude of motion
	D3DXVECTOR3		vSpinPos;			// Current spin rotation
	D3DXVECTOR3		vRight;				// Right-facing vector of object
	D3DXVECTOR3		vUp;				// Up-facing vector of object
	D3DXVECTOR3		vFront;				// Front-facing vector of object
	D3DXVECTOR3		vSpinVelocity;		// Direction and magnitude of rotation
	float			fMass;				// Mass of this object
	BOOL			bFixed;				// Do not apply gravity to this object
	D3DMATERIAL8	Material;			// Colour information
	D3DXMATRIX		matRotate;			// Applied to object during frame setup
	D3DXMATRIX		matTranslate;		// Applied to object during frame setup
};

// Defines standard ACOBJECT_OBJECT create parameters
typedef struct _ACSETTINGS_OBJECT
{
	DWORD			dwSize;				// Set to the size of the structure
	D3DXVECTOR3		vPos;				// Location in world space
	D3DXVECTOR3		vSpinPos;			// Current spin rotation
	D3DXVECTOR3		vVelocity;			// Direction and magnitude of motion
	D3DXVECTOR3		vSpinVelocity;		// Direction and magnitude of rotation
	float			fMass;				// Mass of this object
	BOOL			bFixed;				// Do not apply gravity to this object
	D3DMATERIAL8	Material;			// Colour information
	D3DXMATRIX		matRotate;			// Applied to object during frame setup
	D3DXMATRIX		matTranslate;		// Applied to object during frame setup
} ACSETTINGS_OBJECT, *LPACSETTINGS_OBJECT;


// Defines a single Alchemy camera
class AcCamera : public AcObject
{
public:		// construction / destruction
	AcCamera() {
	}

	virtual ~AcCamera() {
	}

public:		// data
	TCamera		Camera;					// Camera management object
};

// Defines settings for AcCamera
typedef struct _ACSETTINGS_CAMERA
{
	ACSETTINGS_OBJECT	Object;			// Common ACOBJECT_OBJECT settings
	D3DXVECTOR3			VLookAt;		// Camera look at point
} ACSETTINGS_CAMERA, *LPACSETTINGS_CAMERA;


// Defines a single Alchemy actor
class AcActor : public AcObject
{
public:		// construction / destruction
	AcActor() {
		nMesh = ACNULL;
	}

	virtual ~AcActor() {
	}

public:		// data
	UINT			nMesh;				// Index to mesh resource
};

// Defines settings for AcActor
typedef struct _ACSETTINGS_ACTOR
{
	ACSETTINGS_OBJECT	Object;				// Common ACOBJECT_OBJECT settings
	UINT				nMesh;				// Index to actor mesh
} ACSETTINGS_ACTOR, *LPACSETTINGS_ACTOR;


// Defines a single Alchemy particle
struct AcParticle
{
	DWORD			ID;								// ID of the system this particle belongs to
	BOOL			bAlive;							// Particle is active or ready for allocation
	DWORD			dwSpawnTime;					// Moment of genesis in engine time
	DWORD			dwTimeToLive;					// Lifespan of particle in milliseconds
	DWORD			dwExpiryTime;					// Moment of expiry in engine time
	D3DXVECTOR3		vPos;							// Location in world space
	D3DXVECTOR3		vVelocity;						// Direction and magnitude of motion
	UINT			nTexture;						// Index to texture resource
	D3DXCOLOR		StartColour;					// Initial colour of the particle
	D3DXCOLOR		EndColour;						// Final colour of the particle
	D3DXCOLOR		CurColour;						// Current colour of the particle
	float			fStartSize;						// Initial size of particles
	float			fEndSize;						// Final size of particles
	float			fCurSize;						// Current size of particle
	BOOL			bFixed;							// Particle will not move from point of emission
	BOOL			bMass;							// Particle has mass
};

// Defines settings for AcParticle
typedef struct _ACSETTINGS_PARTICLE
{
	DWORD			ID;								// ID of the system this particle belongs to
	UINT			nTexture;						// Index to texture resource
	D3DXCOLOR		StartColour;					// Initial colour of the particle
	D3DXCOLOR		EndColour;						// Final colour of the particle
	float			fStartSize;						// Initial size of particles
	float			fEndSize;						// Final size of particles
	BOOL			bFixed;							// Particle will not move from point of emission
	BOOL			bMass;							// Particle has mass
	DWORD			dwTimeToLive;					// TTL in milliseconds
} ACSETTINGS_PARTICLE, *LPACSETTINGS_PARTICLE;

// Defines a single Alchemy particle emitter
class AcEmitter : public AcObject
{
public:		// construction / destruction
	AcEmitter() {
		nLinkObject = ACNULL;
		vLinkOffset = D3DXVECTOR3(0.0f,0.0f,0.0f);
		dwLastEmission = 0;
		dwEmitCount = 0;
		hReadyParticle = NULL;
		bActive = TRUE;
	}

	virtual ~AcEmitter() {
	}

public:		// data
	ACSETTINGS_PARTICLE	Particles;				// Settings for outgoing particles
	BOOL				bActive;				// The emitter is currently active
	UINT				nLinkObject;			// Index to linked object
	D3DXVECTOR3			vLinkOffset;			// Offset from linked object in world units
	D3DXVECTOR3			vEmitVelocity;			// Initial velocity of emitted particle
	D3DXVECTOR3			vEmitSpread;			// Spread range of particle emission
	DWORD				dwEmitFrequency;		// Frequency of emission in milliseconds
	DWORD				dwEmitStep;				// Number of particles to emit simultaneously
	DWORD				dwEmitMax;				// The emitter will die after emitting this many particles
	DWORD				dwLastEmission;			// Time of last emission in engine time
	DWORD				dwEmitCount;			// The number of particles emitted
	DWORD				dwMaxParticles;			// Maximum live particles
	HLINKPOS			hReadyParticle;			// Next particle ready for allocation

	TLinkedList<AcParticle> oaParticle;			// Particle list
};

// Defines settings for AcEmitter
typedef struct _ACSETTINGS_EMITTER
{
	ACSETTINGS_OBJECT	Object;
	ACSETTINGS_PARTICLE	Particles;				// Settings for outgoing particles
	UINT				nLinkObject;			// Index to linked object
	D3DXVECTOR3			vLinkOffset;			// Offset from linked object in world units
	D3DXVECTOR3			vEmitVelocity;			// Initial velocity of emitted particle
	D3DXVECTOR3			vEmitSpread;			// Spread range of particle emission in degrees
	DWORD				dwEmitFrequency;		// Frequency of emission in milliseconds
	DWORD				dwEmitStep;				// Number of particles to emit simultaneously
	DWORD				dwEmitMax;				// Emitter will terminate after emitting this many particles
	DWORD				dwMaxParticles;			// Maximum live particles
} ACSETTINGS_EMITTER, *LPACSETTINGS_EMITTER;

// Defines a point sprite particle vertex
typedef struct _ACVERTEX_PSPARTICLE
{
	D3DXVECTOR3		pos;
	D3DCOLOR		colour;
} ACVERTEX_PSPARTICLE, *LPACVERTEX_PSPARTICLE;
#define D3DFVF_ACPSPARTICLEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


// Defines a world object management unit
typedef struct _ACOBJECTDESC
{
	_ACOBJECTDESC() {
		pObject = NULL;
		objType = ACOBJECT_NONE;
		dwSpawnTime = dwTimeToLive = 0;
		bEnabled = FALSE;
	}
	
	AcObject*		pObject;			// Pointer to the object itself
	DWORD			objType;			// The type of this object
	DWORD			dwSpawnTime;		// Moment of genesis for this object in engine time
	DWORD			dwTimeToLive;		// The lifespan of this object in milliseconds (0 is infinite)
	BOOL			bEnabled;			// TRUE if this object is enabled, otherwise FALSE
} ACOBJECTDESC, *LPACOBJECTDESC;


// Defines a single Alchemy sprite
class AcSprite
{
public:		// construction / destruction
	AcSprite() {
		nTexture = 0;
		dwSpawnTime = 0;
		bEnabled = FALSE;
	}

	virtual ~AcSprite() {
	}

public:		// data
	DWORD			dwSpawnTime;		// Moment of genesis of this sprite in engine time
	BOOL			bEnabled;			// TRUE if this sprite is enabled, otherwise FALSE

	DWORD			dwFlags;			// Flags to control sprite behaviour
	UINT			nTexture;			// Index to current associated texture resource
	D3DXVECTOR2		vPos;				// Position in screen space of top-left corner
	D3DXVECTOR2		vScaling;			// Amount to scale sprite by
	D3DCOLOR		colour;				// Colour of this sprite
};

// Defines settings for a sprite
typedef struct _ACSETTINGS_SPRITE
{
	DWORD			dwSize;				// Set to the size of the structure
	DWORD			dwFlags;			// Flags to control sprite behaviour
	UINT			nTexture;			// Index to current associated texture resource
	D3DXVECTOR2		vPos;				// Position of top-left corner in screen space
	D3DXVECTOR2		vScaling;			// Amount to scale sprite by
	D3DCOLOR		colour;				// Colour of this sprite
} ACSETTINGS_SPRITE, *LPACSETTINGS_SPRITE;


// Defines a single Alchemy font
class AcFont
{
public:		// construction / destruction
	AcFont() {
		pFont = NULL;
	}

	virtual ~AcFont() {
		if ( pFont ) {
			delete pFont;
			pFont = NULL;
		}
	}

public:		// data
	TString			strFont;			// String name of font
	DWORD			dwHeight;			// Height of font
	DWORD			dwFlags;			// Font control flags
	CD3DFont*		pFont;				// Pointer to the font management object
};


// Alchemy vertex description
typedef struct _ACVERTEXDESC
{
	float		x, y, z;				// Position of this vertex in local coordinates
	float		nx, ny, nz;				// Direction of vertex normal
	float		tu, tv;					// Texture coordinates
} ACVERTEXDESC, *LPACVERTEXDESC;
#define D3DFVF_ALCHEMY (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

// Alchemy face description
typedef struct _ACFACEDESC
{
	unsigned short	v0, v1, v2;
} ACFACEDESC, *LPACFACEDESC;

// Alchemy batch flags
enum
{
	ACBATCH_SETTRANSFORM	= 0x01,			// Pipeline to use specified matrix
	ACBATCH_SETFILLMODE		= 0x02,			// Pipeline to use specified fill mode
	ACBATCH_SETMATERIAL		= 0x04,			// Pipeline to use specified material
};

// Alchemy batch description
typedef struct _ACBATCHDESC
{
	DWORD					dwFlags;				// Control flags for this batch
	unsigned short			nBaseVertex;			// Start vertex in stream
	unsigned short			nVertexCount;			// Number of vertices in this batch
	unsigned short			nBaseIndex;				// Start index in IndexBuffer
	unsigned short			nPrimCount;				// Number of primitives in this batch
	UINT					nTextureIndex;			// Index of texture in store
	DWORD					dwFillMode;				// Fill mode
	DWORD					dwPrimitiveType;		// Primitive type
	D3DXMATRIX				Matrix;					// World-View matrix
	D3DMATERIAL8			Material;				// Batch material
	LPDIRECT3DINDEXBUFFER8	pIndexBuffer;			// Index buffer
} ACBATCHDESC, *LPACBATCHDESC;


// Alchemy DoWork flags
enum
{
	ACDOWORK_CLEAR			= 0x01,			// Clear the pending back buffer
	ACDOWORK_DRAWSCENE		= 0x02,			// Render world as viewed from current camera
	ACDOWORK_PRESENT		= 0x04,			// Present scene when done
	ACDOWORK_MOTION			= 0x08,			// Apply velocity and global gravity to objects
	ACDOWORK_GRAVITY		= 0x10,			// Calculate gravity for massive objects
};
#define ACDOWORK_ALL ACDOWORK_CLEAR|ACDOWORK_DRAWSCENE|ACDOWORK_PRESENT|ACDOWORK_MOTION|ACDOWORK_GRAVITY
#define ACDOWORK_CLEARDRAWPRESENT ACDOWORK_CLEAR|ACDOWORK_DRAWSCENE|ACDOWORK_PRESENT
#define ACDOWORK_CLEARDRAW ACDOWORK_CLEAR|ACDOWORK_DRAWSCENE
#define ACDOWORK_PHYSICS ACDOWORK_MOTION|ACDOWORK_GRAVITY


// Alchemy status
enum
{
	ACSTATUS_NOTCREATED,
	ACSTATUS_OPERATIONAL,
	ACSTATUS_DEVICELOST,
};


// Alchemy General Statistics
typedef struct _ACSTAT_GENERAL
{
	TString			strBuildNumber;			// Build number of engine
	DWORD			dwStatus;				// Current status
	DWORD			dwVertexPipeline;		// Current vertex pipeline
	DWORD			dwEngineTime;			// Current engine time
	float			fLastTI;				// Previous time integrator
	float			fAverageFPS;			// Average frames-per-second
	DWORD			dwTextureCount;			// Number of textures in store
	DWORD			dwMeshCount;			// Number of meshes in store
	DWORD			dwObjectCount;			// Number of objects in store
	DWORD			dwSpriteCount;			// Number of sprites in store
	DWORD			dwFontCount;			// Number of fonts in store
} ACSTAT_GENERAL, *LPACSTAT_GENERAL;

// Alchemy Resource Statistics
typedef struct _ACSTAT_RESOURCE
{
	UINT			nHandle;				// Handle of this resource
	TString			strAlias;				// Alias name of this resource
} ACSTAT_RESOURCE, *LPACSTAT_RESOURCE;

// Alchemy Texture Statictics
typedef struct _ACSTAT_TEXTURE
{
	UINT			nHandle;				// Handle of this texture
	TString			strAlias;				// Alias name of this texture
	DWORD			dwRefCount;				// Reference count of this texture
	DWORD			nSource;				// Handle to source texture (if this is a reference texture)
	DWORD			dwWidth;				// Width
	DWORD			dwHeight;				// Height
	DWORD			dwBitDepth;				// Depth
	DWORD			dwTileX;				// Horizontal tile count
	DWORD			dwTileY;				// Vertical tile count
	DWORD			dwFrameCount;			// Number of frames
	DWORD			dwD3DTextures;			// Number of D3D textures (dwTileX*dwTileY*dwFrameCount)
} ACSTAT_TEXTURE, *LPACSTAT_TEXTURE;

// Alchemy Mesh Statictics
typedef struct _ACSTAT_MESH
{
	UINT			nHandle;				// Handle of this mesh
	TString			strAlias;				// Alias name of this mesh
	DWORD			dwRefCount;				// Reference count of this mesh
	DWORD			dwVertCount;			// Vertex count for this mesh
	DWORD			dwFaceCount;			// Face count for this mesh
	DWORD			dwTextureCount;			// Texture count for this mesh
	TString			strTextureAlias;		// List of texture alias' for this mesh
} ACSTAT_MESH, *LPACSTAT_MESH;

// Alchemy Object Statistics
typedef struct _ACSTAT_OBJECT
{
	DWORD			dwType;					// Type of object (object, actor, emitter, etc.)
	DWORD			dwCreateTime;			// Time of creation in engine time
	DWORD			dwExpiryTime;			// Time to expire in engine time
	DWORD			dwVertexCount;			// Number of vertices (actors only)
	DWORD			dwFaceCount;			// Number of faces (actors only)
	DWORD			dwTextureCount;			// Number of associated textures
	DWORD			dwParticleCount;		// Number of child particles (emitters only)
} ACSTAT_OBJECT, *LPSTAT_OBJECT;


// Engine encapsulation
class TAlchemy  
{
public:		// construction / destruction
	TAlchemy();
	virtual ~TAlchemy();

public:		// Engine management methods
	BOOL	Create( LPALCHEMY_CREATESTRUCT pCreateStruct );
	void	Destroy();
	BOOL	IsCreated() {return m_bIsCreated;}
	BOOL	DoWork( DWORD dwFlags = ACDOWORK_ALL );

	BOOL	GetStatistics( LPACSTAT_GENERAL pStatsGeneral );
	BOOL	GetResourceStatistics( UINT *pResourceInOut, LPACSTAT_RESOURCE pStatsResource );
	BOOL	GetTextureStatistics( UINT *pTextureInOut, LPACSTAT_TEXTURE pStatsTexture );
	BOOL	GetMeshStatistics( UINT *pMeshInOut, LPACSTAT_MESH pStatsMesh );

	inline DWORD GetVertexPipeline() {return m_dwVertexPipeline;}
	inline DWORD GetEngineTime() {return m_dwEngineTime;}
	inline float GetFPS() {return m_fFPS;}
	inline float GetTI() {return m_fTI;}

public:		// Object management methods
	UINT	InstallObject( DWORD objType, LPVOID pSettings, BOOL bEnabled = TRUE, DWORD dwTimeToLive = 0 );
	UINT	CopyObject( UINT nObject, LPVOID pSettingsOut );
	void	RemoveObject( UINT nObject, DWORD dwTimeToLive = 0 );
	BOOL	SetObject( UINT nObject, LPVOID pSettings, BOOL bEnabled = TRUE, DWORD dwTimeToLive = 0 );
	BOOL	GetObject( UINT nObject, LPVOID pSettingsOut );
	BOOL	EnableObject( UINT nObject, BOOL bEnable = TRUE );
	BOOL	IsObjectEnabled( UINT nObject );
	DWORD	GetObjectTimeToLive();

	BOOL	ObjectRotateMatrix( UINT nObject, D3DXMATRIX* pM );
	BOOL	ObjectRotateQuaternion( UINT nObject, D3DXQUATERNION* pQ );
	BOOL	ObjectRotateYawPitchRoll( UINT nObject, float fYaw, float fPitch, float fRoll );
	BOOL	ObjectTranslate( UINT nObject, float fX, float fY, float fZ );
	BOOL	ObjectTranslateMatrix( UINT nObject, LPD3DXMATRIX pM );

public:		// Sprite management methods
	UINT	InstallSprite( LPACSETTINGS_SPRITE pSettings, LPCSTR pszIndex = NULL, BOOL bEnabled = TRUE );
	UINT	RemoveSprite( UINT nSprite );
	BOOL	SetSprite( UINT nSprite, LPACSETTINGS_SPRITE pSettings, BOOL bEnabled = TRUE );
	BOOL	GetSprite( UINT nSprite, LPACSETTINGS_SPRITE pSettingsOut );
	UINT	FindSprite( LPCSTR pszIndex );
	BOOL	EnableSprite( UINT nSprite, BOOL bEnable = TRUE );
	BOOL	IsSpriteEnabled( UINT nSprite );
	DWORD	GetSpriteWidth( UINT nSprite );
	DWORD	GetSpriteHeight( UINT nSprite );
	BOOL	GetSpriteRect( UINT nSprite, LPRECT pRctOut );

	UINT	FindFirstSprite();
	UINT	FindNextSprite( UINT nPreviousSprite );
	void	PurgeSpriteStore();
	BOOL	StepSpriteAnim( UINT nSprite );

	BOOL	SpriteTranslate( UINT nSprite, float fX, float fY );

public:		// Font management methods
	UINT	InstallFont( LPCSTR pszFontName, DWORD dwHeight, DWORD dwFlags );
	void	RemoveFont( UINT nFont );
	BOOL	DrawText( UINT nFont, float x, float y, DWORD dwColour, LPCSTR pszText, DWORD dwFlags = 0 );

public:		// Resource management methods
	UINT	InstallMeshFromMemory( TFlexibleMesh* pMesh, LPCSTR pszIndex = NULL );
	UINT	InstallMeshFromFile( LPCSTR pszPath, LPCSTR pszIndex = NULL );
	UINT	InstallTextureFromMemory( UINT nAppend, LPRECT pSrcRect, DWORD dwSrcPitchInPixels, LPVOID pBits, DWORD dwBitDepth = AC_DEFAULT, LPCSTR pszIndex = NULL, DWORD dwColorKey = 0xff000000 );
	//UINT	InstallTextureFromFile( LPCSTR pszPath, DWORD dwBitDepth = AC_DEFAULT, LPCSTR pszIndex = NULL, DWORD dwColorKey = 0xff000000 );
	BOOL	GetTextureRect( UINT nTexture, LPRECT pRectOut );

	UINT	FindResource( LPCSTR pszIndex );
	UINT	GetResourceCount() {return m_oaResource.GetCount();}
	int		GetTextureCount() {return m_nStoreTextureCount;}
	int		GetMeshCount() {return m_nStoreMeshCount;}

	DWORD	AddResRef( UINT nResource );
	DWORD	DecResRef( UINT nResource );
	DWORD	GetResRef( UINT nResource );

public:		// Display and camera management methods
	BOOL	GraphicsGetDevice( LPDIRECT3DDEVICE8* pd3ddOut );
	BOOL	GetTransform( D3DTRANSFORMSTATETYPE Type, LPD3DXMATRIX pMatrixOut );
	BOOL	GetTransformFromObject( UINT nObject, LPD3DXMATRIX pMatrixOut );
	BOOL	EnableLighting( BOOL bEnable = TRUE );
	BOOL	SetAmbientLight( D3DCOLOR colour );
	BOOL	SetDirectionalLight( D3DVECTOR vDirection, D3DCOLOR colour );
	BOOL	SetDisplaySize( DWORD dwWidth, DWORD dwHeight );
	BOOL	GetDisplaySize( LPSIZE pSizeOut );
	BOOL	Clear( BOOL bClearZBuffer = TRUE );
	//BOOL	DrawViewport( LPACDV_SETTINGS pSettings );
	BOOL	Present();
	BOOL	RecoverGraphics();
	BOOL	ResetGraphics();

	BOOL	SetCamera( UINT nCamera, LPACSETTINGS_CAMERA pSettings );
	BOOL	CameraRotate( UINT nCamera, float fYaw, float fPitch, float fRoll );
	BOOL	CameraTranslate( UINT nCamera, float fX, float fY, float fZ );
	BOOL	CameraThrust( UINT nCamera, float fRight, float fUp, float fFront );

public:		// Physics management methods
	BOOL	SetGlobalGravity( LPD3DXVECTOR3 pv );
	BOOL	GetGlobalGravity( LPD3DXVECTOR3 pvOut );
	BOOL	EnableGlobalGravity( BOOL bEnable = TRUE );
	float	ModelAcceleration( float fMass1, float fMass2, LPD3DXVECTOR3 pvPos1, LPD3DXVECTOR3 pvPos2, LPD3DXVECTOR3 pvOut );

public:		// Input management methods
	BOOL	GetKeyboardState( LPVOID pBuffer );

private:	// methods
	BOOL	SetActor( UINT nActor, LPACSETTINGS_ACTOR pSettings );
	BOOL	GetActor( UINT nActor, LPACSETTINGS_ACTOR pSettingsOut );
	BOOL	SetEmitter( UINT nEmitter, LPACSETTINGS_EMITTER pSettings );
	BOOL	GetEmitter( UINT nEmitter, LPACSETTINGS_EMITTER pSettingsOut );
	void	StepEmitter( UINT nEmitter );
	void	StepParticles( UINT nEmitter );
	void	DestroyObjects( UINT nObject = ACNULL );
	void	DestroyResources();
	BOOL	InitDeviceState();
	void	ReleaseFonts();
	void	ResetFonts();
	BOOL	DrawScene();
	BOOL	DrawBatchArray( DWORD dwBatchCount );
	BOOL	DrawPSParticles( AcEmitter* pEmitter );
	BOOL	DrawSprites();

	UINT	InstallResource( DWORD dwType, LPCSTR pszIndex );
	void	DestroyResource( UINT nResource );
	BOOL	InstallTexture( UINT nAppend, LPRECT pSrcRect, DWORD dwSrcPitchInPixels, LPVOID pBits, DWORD dwBitDepth = AC_DEFAULT, LPCSTR pszIndex = NULL, DWORD dwColorKey = 0xff000000 );

	LPACTEXTURE	GetTexture( UINT nTexture );
	LPACMESH	GetMesh( UINT nMesh );

	BOOL	SetViewMatrixFromCamera();

public:		// objects
	TLog	m_Log;

private:	// objects
	TDirectX8					m_DirectX;
	LPDIRECT3DDEVICE8			m_pd3dDevice;
	LPD3DXSPRITE				m_pd3dSprite;

	TLinkedList<TCamera>		m_oaCamera;
	TLinkedList<ACRESOURCEDESC>	m_oaResource;
	TLinkedList<ACOBJECTDESC>	m_oaObject;
	TLinkedList<AcSprite>		m_oaSprite;
	TLinkedList<AcFont>			m_oaFont;

	THashArray<UINT, HASHTABLECONST>	m_haResourceIndex;
	THashArray<UINT, HASHTABLECONST>	m_haSpriteTable;

	TRandom						m_Random;

private:	// Engine management data
	TString		m_strBuildNumber;
	BOOL		m_bIsCreated;
	DWORD		m_dwEngineTime;
	DWORD		m_dwCreateFlags;
	HINSTANCE	m_hInstOwner;
	HWND		m_hViewWnd;
	HWND		m_hInputWnd;
	BOOL		m_bWindowed;
	DWORD		m_dwWidth;
	DWORD		m_dwHeight;
	DWORD		m_dwColourDepth;
	BOOL		m_bGraphicsLost;
	DWORD		m_dwFrameCount;
	float		m_fTI;
	float		m_fFPS;
	DWORD		m_dwVertexPipeline;
	pvb_t		m_vbStream;
	UINT		m_nCurCamera;
	TCamera		m_camDefault;

	TLinkedList<ACBATCHDESC>	m_oaBatch;

	LPDIRECT3DVERTEXBUFFER8 m_pvbParticleBuffer;

private:	// Engine state data
	int			m_nStoreTextureCount;
	int			m_nStoreMeshCount;

	float		m_fNearPlaneDepth;
	float		m_fFarPlaneDepth;

	D3DCOLOR	m_colAmbientLight;
	D3DCOLOR	m_colRenderErase;

	BOOL		m_bEnableGlobalGravity;
	D3DXVECTOR3	m_vGlobalGravity;

	D3DXMATRIX	m_matProjection;
	D3DXMATRIX	m_matView;
};


#endif // !defined(AFX_TALCHEMY_H__4C926672_0F25_4E97_9341_B811104C1272__INCLUDED_)
