// TCamera.h: interface for the TCamera class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_TCAMERA_H__DAE0C38E_C58A_4043_A63F_EE5C9FBF0C32__INCLUDED_)
#define AFX_TCAMERA_H__DAE0C38E_C58A_4043_A63F_EE5C9FBF0C32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDirectX8.h"


class TCamera  
{
public:		// construction / destruction
	TCamera();
	virtual ~TCamera();

public:		// methods
	void Rotate( LPD3DXMATRIX pMatrixOut, float fYaw, float fPitch, float fRoll );
	void Translate( LPD3DXMATRIX pMatrixOut, float fX, float fY, float fZ );
	void Thrust( LPD3DXMATRIX pMatrixOut, float fRight, float fUp, float fFront );
	void ResetAxis( LPD3DXMATRIX pMatrixOut = NULL, bool bRight = true, bool bUp = true, bool bFront = true );
	void GetMatrix( LPD3DXMATRIX pMatrixOut );

public:	// data
	D3DXVECTOR3 vPos, vFront, vRight, vUp;
};


#endif // !defined(AFX_TCAMERA_H__DAE0C38E_C58A_4043_A63F_EE5C9FBF0C32__INCLUDED_)
