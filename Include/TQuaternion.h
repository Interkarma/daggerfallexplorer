// TQuaternion.h: interface for the TQuaternion class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_TQUATERNION_H__88B66ADD_4444_4216_8190_09223456E624__INCLUDED_)
#define AFX_TQUATERNION_H__88B66ADD_4444_4216_8190_09223456E624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TDirectX8.h"


class TQuaternion  
{
public:		// construction / destruction
	TQuaternion();
	TQuaternion( LPD3DXVECTOR3 pvAxis, float fAngle );
	TQuaternion( float x, float y, float z, float fAngle );
	virtual ~TQuaternion();

public:		// methods
	void Build( LPD3DXVECTOR3 pvAxis, float fAngle );
	void Build( float x, float y, float z, float fAngle );

	const TQuaternion& operator = ( TQuaternion& qIn );
	const TQuaternion& operator = ( D3DXQUATERNION qIn );
	const TQuaternion& operator *= ( TQuaternion& qIn );

public:		// data
	D3DXQUATERNION	quat;
};


#endif // !defined(AFX_TQUATERNION_H__88B66ADD_4444_4216_8190_09223456E624__INCLUDED_)
