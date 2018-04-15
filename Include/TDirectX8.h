// TDirectX8.h: interface for the TDirectX8 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDIRECTX8_H__12922268_60CC_4BD6_AC5A_E3EA6A9FE895__INCLUDED_)
#define AFX_TDIRECTX8_H__12922268_60CC_4BD6_AC5A_E3EA6A9FE895__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DIRECTINPUT_VERSION 0x0800

#include <d3d8.h>
#include <d3dx8.h>
#include <dinput.h>
#include "TLog.h"
#include "TStaticArray.h"
#include "TString.h"


// Forward declarations
class TDirectX8;
class TDX8Graphics;
class TDX8Audio;
class TDX8Input;


// Redefinitions
typedef TDirectX8		*LPDIRECTX;
typedef TDX8Graphics	*LPDX_GRAPHICS;
typedef TDX8Audio		*LPDX_AUDIO;
typedef TDX8Input		*LPDX_INPUT;


// Specify which elements to create
enum DXCREATE_FLAGS {
	DXCREATE_GRAPHICS	= 0x01,
	DXCREATE_AUDIO		= 0x02,
	DXCREATE_INPUT		= 0x04,
};
#define DXCREATE_DEFAULT DXCREATE_GRAPHICS|DXCREATE_INPUT
#define DXCREATE_ALL DXCREATE_GRAPHICS|DXCREATE_INPUT|DXCREATE_AUDIO


// Standard TDX8Input device types
enum
{
	DXIDEVICE_KEYBOARD	= 0x01,
	DXIDEVICE_MOUSE		= 0x02,
	DXIDEVICE_JOYSTICK	= 0x04,
	DXIDEVICE_CUSTOM	= 0x08,
};
#define DXIDEVICE_DEFAULT DXIDEVICE_KEYBOARD|DXIDEVICE_MOUSE


class TDirectX8  
{
public:		// constructor / destructor
	TDirectX8();
	virtual ~TDirectX8();

public:		// methods
	BOOL	Create( DWORD dwFlags );
	void	Destroy();

	LPDX_GRAPHICS	GetGraphics();
	LPDX_AUDIO		GetAudio();
	LPDX_INPUT		GetInput();

	BOOL GraphicsGetDevice( LPDIRECT3DDEVICE8* pd3ddOut );
	BOOL InputGetKeyboardState( LPVOID pBuffer );

	BOOL		IsCreated() {return m_bCreated;}
	BOOL		IsGraphicsCreated() {return m_bGraphicsCreated;}
	BOOL		IsAudioCreated() {return m_bAudioCreated;}
	BOOL		IsInputCreated() {return m_bInputCreated;}

public:		// objects
	TLog	m_Log;

private:	// methods
	BOOL CreateGraphics();
	BOOL CreateInput();
	BOOL CreateAudio();

private:	// objects
	LPDX_GRAPHICS	m_pGraphics;
	LPDX_AUDIO		m_pAudio;
	LPDX_INPUT		m_pInput;

private:	// data
	HRESULT		m_hLastError;
	HPOLYOBJ	m_hLastLogEntry;
	int			m_nMaxLogSize;
	BOOL		m_bCreated;
	BOOL		m_bGraphicsCreated;
	BOOL		m_bAudioCreated;
	BOOL		m_bInputCreated;

	HINSTANCE	m_hInstOwner;
	HWND		m_hWndOwner;
};


// Helper definitions
#define RELEASE(x) if ( x != NULL ) x->Release(); x = NULL
#define KEYDOWN(name, key) (name[key] & 0x80)


// LPDIRECT3DVERTEXBUFFER8 pointer management object
class pvb_t
{
public:
	pvb_t() {m_pd3dvb = NULL; m_dwVertCount = 0;}
	virtual ~pvb_t() {Release();}

	void operator = (pvb_t& src) {RELEASE(m_pd3dvb); m_pd3dvb = src.Get(); m_dwVertCount = src.GetVertCount();}
	void Set( LPDIRECT3DVERTEXBUFFER8 psrc, DWORD dwVertCount ) {RELEASE(m_pd3dvb); m_pd3dvb = psrc; m_dwVertCount = dwVertCount;}

	inline LPDIRECT3DVERTEXBUFFER8 Get() {if(m_pd3dvb)m_pd3dvb->AddRef(); return m_pd3dvb;}
	inline operator LPDIRECT3DVERTEXBUFFER8 () {return m_pd3dvb;}
	inline LPDIRECT3DVERTEXBUFFER8 operator -> () {return m_pd3dvb;}
	inline DWORD GetVertCount() {return m_dwVertCount;}

	void Release() {RELEASE(m_pd3dvb); m_dwVertCount = 0;}

private:
	DWORD m_dwVertCount;
	LPDIRECT3DVERTEXBUFFER8 m_pd3dvb;
};


// LPDIRECT3DINDEXBUFFER8 pointer management object
class pib_t
{
public:
	pib_t() {m_pd3dib = NULL; m_dwIndexCount = 0;}
	virtual ~pib_t() {Release();}

	void operator = (pib_t& src) {RELEASE(m_pd3dib); m_pd3dib = src.Get(); m_dwIndexCount = src.GetIndexCount();}
	void Set( LPDIRECT3DINDEXBUFFER8 psrc, DWORD dwIndexCount ) {RELEASE(m_pd3dib); m_pd3dib = psrc; m_dwIndexCount = dwIndexCount;}

	inline LPDIRECT3DINDEXBUFFER8 Get() {if(m_pd3dib) m_pd3dib->AddRef(); return m_pd3dib;}
	inline operator LPDIRECT3DINDEXBUFFER8 () {return m_pd3dib;}
	inline LPDIRECT3DINDEXBUFFER8 operator -> () {return m_pd3dib;}
	DWORD GetIndexCount() {return m_dwIndexCount;}

	void Release() {RELEASE(m_pd3dib); m_dwIndexCount = 0;}

private:
	DWORD m_dwIndexCount;
	LPDIRECT3DINDEXBUFFER8 m_pd3dib;
};


// LPDIRECT3DTEXTURE8 pointer management object
class ptx_t
{
public:
	ptx_t() {m_pd3dtx = NULL;}
	virtual ~ptx_t() {RELEASE(m_pd3dtx);}

	void operator = (ptx_t& src) {RELEASE(m_pd3dtx); m_pd3dtx = src.Get(); src.GetRects( &m_txrRect, &m_imgRect );}
	void Set( LPDIRECT3DTEXTURE8 psrc, LPRECT ptxrRect, LPRECT pimgRect ) {RELEASE(m_pd3dtx); m_pd3dtx = psrc; m_txrRect = *ptxrRect; m_imgRect = *pimgRect;}

	inline LPDIRECT3DTEXTURE8 Get() {if(m_pd3dtx) m_pd3dtx->AddRef(); return m_pd3dtx;}
	inline operator LPDIRECT3DTEXTURE8 () {return m_pd3dtx;}
	inline LPDIRECT3DTEXTURE8 operator -> () {return m_pd3dtx;}
	inline GetRects( LPRECT ptxrRectOut, LPRECT pimgRectOut ) {*ptxrRectOut = m_txrRect; *pimgRectOut = m_imgRect;}

	void Release() {RELEASE(m_pd3dtx);}

private:
	RECT	m_txrRect;
	RECT	m_imgRect;
	LPDIRECT3DTEXTURE8 m_pd3dtx;
};


#endif // !defined(AFX_TDIRECTX8_H__12922268_60CC_4BD6_AC5A_E3EA6A9FE895__INCLUDED_)
