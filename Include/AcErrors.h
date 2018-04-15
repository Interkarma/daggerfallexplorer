#if !defined(ACERRORS_H)
#define ACERRORS_H


#define MAKE_ACHRESULT( code ) MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, code )

#define ACOK						MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0)
#define ACERR_OUTOFMEMORY			MAKE_ACHRESULT(1)
#define ACERR_INVALIDPARAMS			MAKE_ACHRESULT(2)
#define ACERR_WIN32FAIL				MAKE_ACHRESULT(3)
#define ACERR_TDX8FAIL				MAKE_ACHRESULT(4)
#define ACERR_NOTCREATED			MAKE_ACHRESULT(5)
#define ACERR_LOADFAILED			MAKE_ACHRESULT(6)
#define ACERR_DEVICELOST			MAKE_ACHRESULT(7)
#define ACERR_CREATESPRITEFAILED	MAKE_ACHRESULT(8)
#define ACERR_INVALIDORDINAL		MAKE_ACHRESULT(9)

#define ACSTR_OUTOFMEMORY				"Failed to allocate memory for operation."
#define ACSTR_INVALIDPARAMS				"One or more parameters are invalid as specified."
#define ACSTR_NOTCREATED				"Alchemy engine must be created."
#define ACSTR_LOADMESHFAILED			"Failed to load mesh resource."
#define ACSTR_INITDEVICESTATEFAILED		"Failed to initialise device state."
#define ACSTR_DEVICELOST				"Method failed because the Graphics device is in a lost state."
#define ACSTR_CREATETEXTUREFAILED		"IDirect3DDevice8->CreateTexture failed."
#define ACSTR_GETSURFACELEVELFAILED		"IDirect3DDevice8->GetSurfaceLevel failed."
#define ACSTR_SETTRANSFORMFAILED		"IDirect3DDevice8->SetTransform failed."
#define ACSTR_SETTXSSFAILED				"IDirect3DDevice8->SetTextureStageState failed."
#define ACSTR_SETRENDERSTATEFAILED		"IDirect3DDevice8->SetRenderState failed."
#define ACSTR_CLEARFAILED				"IDirect3DDevice8->Clear failed."
#define ACSTR_CREATESPRITEFAILED		"D3DXCreateSprite() failed."
#define ACSTR_INVALIDORDINAL			"The object ordinal is invalid."
#define ACSTR_FAILDXCREATE				"Failed to create DirectX. Installed version could be lower than required. [DirectX 8.1]."


#endif // !defined(ACERRORS_H)