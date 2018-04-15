/*************************************************************************************************\
*
* Filename:		DaggerfallExplorer.h
* Purpose:		Define CDaggerfallExplorerApp class
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


#if !defined(AFX_DAGGERFALLEXPLORER_H__62D08791_7699_4947_9740_8F102F21CE2D__INCLUDED_)
#define AFX_DAGGERFALLEXPLORER_H__62D08791_7699_4947_9740_8F102F21CE2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ChildView.h"
#include "ArenaBar.h"
#include "LogDialog.h"
#include "DaggerTool.h"
#include "PropertiesBar.h"
#include "MainFrm.h"
#include "resource.h"       // main symbols


// Defines
#define DE_CACHETEXTURE_WIDTH 256
#define DE_CACHETEXTURE_HEIGHT 256
#define DE_CACHETEXTURE_PITCH 768
#define DE_CACHETEXTURE_BYTES 196608

#define DE_START_TERRAIN TERRAIN_TEMPERATE
#define DE_START_OBJECT 5557				// Shedungent
#define DE_START_BLOCK 835					// Testing
//#define DE_START_BLOCK 267				// Castle Wayrest
//#define DE_START_BLOCK 577				// Castle Sentinel
//#define DE_START_BLOCK ?					// Castle Daggerfall
#define DE_START_DRAW OBJDRAW_TEXTURED

#define DEOBJECT_STARTPOS D3DXVECTOR3( 0.0f, 0.0f, 1800.0f )
#define DEBLOCK_STARTPOSOBJECT D3DXVECTOR3( -37.5f, -350.0f, 4900.0f )
#define DEBLOCK_STARTPOSCAMERA D3DXVECTOR3( 0.0f, -900.0f, 4200.0f )
#define DECAMERA_STARTPOS D3DXVECTOR( 0.0f, 0.0f, 0.0f )
#define DECOLOUR_SPRITE D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f )
#define DECOLOUR_SELECTEDSPRITE D3DXCOLOR( 1.0f, 0.0f, 1.0f, 1.0f )


// Timer type enum
enum
{
	DETIMER_STEPBITMAPS,
	DETIMER_POLLKEYBOARD,
};


// Node Types - All types of nodes possible in the tree view
typedef enum _tagNodeType
{
	NT_NONE,
	NT_BASICROOT,
	NT_LOCALEROOT,
	NT_PLACEROOT,
	NT_DUNGEONROOT,
	NT_ARCH3D,
	NT_BLOCKS,
	NT_SUBBLOCKS,
	NT_TEXTUREARCHIVE,
	NT_IMGFILE,
	NT_CIFFILE,
	NT_SKYFILE,
	NTGENERIC_BITMAP,
	NTGENERIC_OBJECT,
} NODETYPE;

// Mouse modes - Actions that can be assigned to the view
enum
{
	MOUSEMODE_OBJECT,
	MOUSEMODE_CAMERA,
};

// Render modes - How an object will be drawn
enum
{
	OBJDRAW_PUREFACES,
	OBJDRAW_WIREFRAME,
	OBJDRAW_SOLID,
	OBJDRAW_TEXTURED,
};

// Object properties - for generic bitmaps
enum {
	BMPPROP_FILENAME,
	BMPPROP_DESCRIPTION,
	BMPPROP_RECORDCOUNT,
};

// Object properties - for 3D objects
enum {
	OBJPROP_OBJECT,
	OBJPROP_OBJECTID,
	OBJPROP_OBJECTVERSION,
	OBJPROP_FACECOUNT,
	OBJPROP_SELFACE,
	OBJPROP_POINTCOUNT,
	OBJPROP_CORNERCOUNT,
	OBJPROP_CORNERPOINTS,
	OBJPROP_ARCHIVE,
	OBJPROP_TEXTUREDIM,
	OBJPROP_UVPAIRS,
};

// Block properties - for block objects
enum {
	BLKPROP_OBJECT,
	BLKPROP_FILENAME,
	BLKPROP_NUMRECORDS1,
	BLKPROP_OUTSIDE_NUM3DOBJECTS,
	BLKPROP_OUTSIDE_NUMFLATOBJECTS,
	BLKPROP_OUTSIDE_NUMPEOPLE,
	BLKPROP_OUTSIDE_NUMDOORS,
	BLKPROP_INSIDE_NUM3DOBJECTS,
	BLKPROP_INSIDE_NUMFLATOBJECTS,
	BLKPROP_INSIDE_NUMPEOPLE,
	BLKPROP_INSIDE_NUMDOORS,
};


// Stores a single face during triangulation
typedef struct _WIREFACE {
	TString		strTexture;
	DWORD		dwVertCount;
	OBJVERTEX	verts[64];
} WIREFACE;


// Stores a combine texture object during texture cache operations
typedef struct _DFTEXTURECACHE
{
	_DFTEXTURECACHE() {
		UINT nTexture = ACNULL;
		cx = maxWidth = cy = 0;
		memset( pBits, 0, DE_CACHETEXTURE_BYTES );
	}

	TString		strName;							// Name of this texture in the store
	int			cx;									// Position of current column
	int			maxWidth;							// Maximum width of this column
	int			cy;									// Height of current column
	char		pBits[DE_CACHETEXTURE_BYTES];		// R8G8B8 texture bits for this combine texture
	RECT		rctTexture;							// Rect of this texture
	TLinkedList<RECT> oaSubset;						// List of rectangles to reference subset of surface
} DFTEXTURECACHE, *LPDFTEXTURECACHE;

// Reference a combine texture object and an offset
typedef struct _DFTEXTUREREF
{
	UINT		nTexture;						// Reference for texture in cache table
	UINT		nSubset;						// Reference for subset of texture in cache table
} DFTEXTUREREF, *LPDFTEXTUREREF;


/////////////////////////////////////////////////////////////////////////////
// CDaggerfallExplorerApp:
// See Daggerfall Explorer.cpp for the implementation of this class
//

class CDaggerfallExplorerApp : public CWinApp
{
public:
	CDaggerfallExplorerApp();
	virtual ~CDaggerfallExplorerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDaggerfallExplorerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CDaggerfallExplorerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpenArena2();
	afx_msg void OnFileCloseArena2();
	afx_msg void OnUpdateFileCloseArena2(CCmdUI* pCmdUI);
	afx_msg void OnObjectPrevious();
	afx_msg void OnUpdateObjectPrevious(CCmdUI* pCmdUI);
	afx_msg void OnObjectNext();
	afx_msg void OnUpdateObjectNext(CCmdUI* pCmdUI);
	afx_msg void OnObjectGoto();
	afx_msg void OnUpdateObjectGoto(CCmdUI* pCmdUI);
	afx_msg void OnObjectPureFaces();
	afx_msg void OnUpdateObjectPureFaces(CCmdUI* pCmdUI);
	afx_msg void OnObjectWireframe();
	afx_msg void OnUpdateObjectWireframe(CCmdUI* pCmdUI);
	afx_msg void OnObjectSolid();
	afx_msg void OnUpdateObjectSolid(CCmdUI* pCmdUI);
	afx_msg void OnObjectTextured();
	afx_msg void OnUpdateObjectTextured(CCmdUI* pCmdUI);
	afx_msg void OnBitmapAnimation();
	afx_msg void OnObjectFaceNextFace();
	afx_msg void OnUpdateObjectFaceNextFace(CCmdUI* pCmdUI);
	afx_msg void OnObjectFacePreviousFace();
	afx_msg void OnUpdateObjectFacePreviousFace(CCmdUI* pCmdUI);
	afx_msg void OnBitmapZoomIn();
	afx_msg void OnUpdateBitmapZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnBitmapZoomOut();
	afx_msg void OnUpdateBitmapZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnObjectFaceGotoFace();
	afx_msg void OnUpdateObjectFaceGotoFace(CCmdUI* pCmdUI);
	afx_msg void OnPlayUVEditor();
	afx_msg void OnUpdatePlayUVEditor(CCmdUI* pCmdUI);
	afx_msg void OnViewArenaView();
	afx_msg void OnUpdateViewArenaView(CCmdUI* pCmdUI);
	afx_msg void OnViewPropertiesView();
	afx_msg void OnUpdateViewPropertiesView(CCmdUI* pCmdUI);
	afx_msg void OnViewLogView();
	afx_msg void OnUpdateViewLogView(CCmdUI* pCmdUI);
	afx_msg void OnBitmapToggleAnimation();
	afx_msg void OnUpdateBitmapToggleAnimation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBitmapAnimation(CCmdUI* pCmdUI);
	afx_msg void OnToolsAlchemyStatistics();
	afx_msg void OnUpdateObjectCamera(CCmdUI *pCmdUI);
	afx_msg void OnTerrainDesert();
	afx_msg void OnUpdateTerrainDesert(CCmdUI *pCmdUI);
	afx_msg void OnTerrainMountain();
	afx_msg void OnUpdateTerrainMountain(CCmdUI *pCmdUI);
	afx_msg void OnTerrainTemperate();
	afx_msg void OnUpdateTerrainTemperate(CCmdUI *pCmdUI);
	afx_msg void OnTerrainSwamp();
	afx_msg void OnUpdateTerrainSwamp(CCmdUI *pCmdUI);
	afx_msg void OnTerrainNormal();
	afx_msg void OnUpdateTerrainNormal(CCmdUI *pCmdUI);
	afx_msg void OnTerrainWinter();
	afx_msg void OnUpdateTerrainWinter(CCmdUI *pCmdUI);
	afx_msg void OnTerrainRain();
	afx_msg void OnUpdateTerrainRain(CCmdUI *pCmdUI);
	afx_msg void OnViewObjectmode();
	afx_msg void OnUpdateViewObjectmode(CCmdUI *pCmdUI);
	afx_msg void OnViewCameramode();
	afx_msg void OnUpdateViewCameramode(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:		// methods
	void	HandleMouseMessages( const MSG* pMsg, int x, int y );
	void	HandleChildMessage( LPNMHDR pnmh );
	void	OnDrawMediaView();
	void	OnSizeMediaView( int cx, int cy );
	void	OnScrollMediaView( int nBar, UINT nSBCode, UINT nPos );
	void	OnStepBitmaps();
	void	OnPollKeyboard();
	void	OnLMouseDrag( int x, int y );
	void	OnRMouseDrag( int x, int y );

private:	// methods
	BOOL	FailInitApp();
	BOOL	BuildMediaView( BOOL bReset = FALSE );
	BOOL	DrawMediaView();
	BOOL	InitialiseViews();
	void	OnSelectNode( UINT hNode );
	void	SelectSprite( int x, int y );
	BOOL	BuildTextureArchiveNode();
	BOOL	BuildIMGNode();
	BOOL	BuildCIFNode();
	BOOL	BuildRCINode();
	BOOL	BuildLocaleNode();
	BOOL	BuildMesh( long nObject, TFlexibleMesh* pMeshOut );
	BOOL	BuildBlockGroundMesh( LPRMBFLD_SMALLMAPDATA pMap, TFlexibleMesh* pMeshOut );
	BOOL	BuildObjectView( BOOL bResetObject = FALSE );
	BOOL	BuildBlockView( BOOL bResetObject = FALSE );
	BOOL	BuildBlockObjects( LPRMBFLD pFLD );
	BOOL	BuildInsideBlockObject( LPRMBFLD pFLD );
	BOOL	BuildPropertyView();
	BOOL	SetPropertyView();
	BOOL	CacheTextureArchive( LPCSTR pszName );
	BOOL	CacheIMGImage( LPCSTR pszName );
	BOOL	CacheCIFArchive( LPCSTR pszName );
	BOOL	CacheSkyArchive( LPCSTR pszName );
	BOOL	FormatSpritePage();
	BOOL	SetPageScrollers();
	BOOL	InitialiseMeshObject( TFlexibleMesh* pMesh,DWORD dwVertCount = 0, DWORD dwFaceCount = 0 );
	BOOL	CacheDFTextureCombine( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut, TString& strCacheNameOut );
	BOOL	CacheDFTextureVerbose( LPCSTR pszName, long nArchive, long nRecord, LPRECT pRectOut );
	BOOL	CommitDFTextures();
	void	ReleaseDFTextures();
	BOOL	ApplyRegion( long *pArchive );
	void	ResetObject();
	void	ResetCamera();

	void	AutoBlockTests();

public:		// objects
	TLog	m_Log;

private:	// objects
	TAlchemy		m_Alchemy;
	CChildView*		m_pMediaView;
	CArenaBar*		m_pArenaBar;
	CLogDialog*		m_pLogDlg;
	CPropertiesBar*	m_pPropertiesBar;
	CLogBar*		m_pLogBar;
	CMainFrame*		m_pFrame;
	CDaggerTool		m_DaggerTool;
	TFlexibleMesh	m_meshView;

private:	// data
	UINT	m_nLastLogEntry;
	BOOL	m_bTextureArchiveNodeBuilt;
	BOOL	m_bIMGRootNodeBuilt;
	BOOL	m_bCIFRootNodeBuilt;
	BOOL	m_bLocaleRootNodeBuilt;
	UINT	m_nSelectedSprite;
	UINT	m_nAutomapSprite;
	UINT	m_nViewActor;
	UINT	m_nDefaultFont;

	IMG_IMAGEDESC m_idCurIMG;

	HTREEITEM m_hTexturesRootNode;
	HTREEITEM m_hIMGRootNode;
	HTREEITEM m_hCIFRootNode;
	HTREEITEM m_hRCIRootNode;
	HTREEITEM m_hSkyRootNode;
	HTREEITEM m_h3DObjectsRootNode;
	HTREEITEM m_hBlocksRootNode;
	HTREEITEM m_hLocaleRootNode;

	UINT	m_nCurNode;
	long	m_nCurArch3DObject;
	long	m_nCurBlockObject;
	long	m_nCurSubBlockObject;
	long	m_nSubBlockObjectCount;
	long	m_nCurFace;
	DWORD	m_dwMouseMode;
	DWORD	m_dwDrawMode;
	DWORD	m_dwCurNodeType;
	DWORD	m_dwCurNodeGeneric;
	float	m_fBitmapScale;
	BOOL	m_bSupressAnimateBitmaps;
	BOOL	m_bLockView;
	BOOL	m_bViewInterior;
	BOOL	m_bDrawGround;
	BOOL	m_bDrawAllBlocks;

	int		m_nLastMouseX;
	int		m_nLastMouseY;
	int		m_nScrollX;
	int		m_nScrollY;
	int		m_nPageWidth;
	int		m_nPageHeight;

	BOOL	m_bAnimateBitmaps;
	DWORD	m_dwBitmapFPS;

	CString	m_strCurTextureArchive;
	TString m_strStartDirectory;

	UINT_PTR	m_nAnimTimer;
	UINT_PTR	m_nKeyTimer;

	UINT m_nLastDFTextureCache;
	TLinkedList<DFTEXTURECACHE> m_oaDFTextureCache;
	THashArray<DFTEXTUREREF, 97> m_haDFTextureCacheIndexCombine;
	THashArray<DFTEXTUREREF, 97> m_haDFTextureCacheIndexVerbose;
	TStaticArray<CPropTreeItem*> m_oaPropCache;

	DWORD	m_dwTerrain;
	DWORD	m_dwWeather;

	char	m_kbState[256];
public:
	afx_msg void OnViewReset();
	afx_msg void OnUpdateViewReset(CCmdUI *pCmdUI);
	afx_msg void OnObjectPreviousBlock();
	afx_msg void OnUpdateObjectPreviousBlock(CCmdUI *pCmdUI);
	afx_msg void OnObjectNextBlock();
	afx_msg void OnUpdateObjectNextBlock(CCmdUI *pCmdUI);
	afx_msg void OnViewAllblocks();
	afx_msg void OnUpdateViewAllblocks(CCmdUI *pCmdUI);
	afx_msg void OnViewGroundplane();
	afx_msg void OnUpdateViewGroundplane(CCmdUI *pCmdUI);
	afx_msg void OnViewInterior();
	afx_msg void OnUpdateViewInterior(CCmdUI *pCmdUI);
	afx_msg void OnTerrainNone();
	afx_msg void OnUpdateTerrainNone(CCmdUI *pCmdUI);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAGGERFALLEXPLORER_H__62D08791_7699_4947_9740_8F102F21CE2D__INCLUDED_)
