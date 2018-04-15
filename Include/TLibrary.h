/****************************************************************************\
*
* Filename:		TLibrary.h
* Purpose:		Standard includes for TLibrary
* Version:		1.0
* Author:		Gavin Clayton
*
* Last Updated:	21/08/2001
*
* Copyright 2000-2001. Gavin Clayton. All Rights Reserved.
*
\****************************************************************************/


#if !defined(AFX_TLIBRARY_H__INCLUDED_)
#define AFX_TLIBRARY_H__INCLUDED_


// Standard TLibrary includes
#include <crtdbg.h>
#include <stdio.h>
#include "TLog.h"
#include "TException.h"
#include "TRandom.h"
#include "TLinkedList.h"
#include "TStaticArray.h"
#include "THashArray.h"
#include "TString.h"
#include "TGuid.h"
#include "TQuaternion.h"
#include "TFileStore.h"

// DirectX encapsulation
#include "TFlexibleMesh.h"
#include "TDirectX8.h"
#include "TDX8Audio.h"
#include "TDX8Graphics.h"
#include "TDX8Input.h"

// DirectX tools
#include "d3dUtil.h"

// Engine encapsulation
#include "TAlchemy.h"


#endif // !defined(AFX_TLIBRARY_H__INCLUDED_)