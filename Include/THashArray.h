/****************************************************************************\
*
* Filename:		THashArray.h
* Purpose:		Template for <typename T, int N>THashArray class
* Version:		1.0
* Author:		Gavin Clayton
*
* Last Updated:	11/05/2002
*
* Notes:
*	Selecting an appropriate value for N will improve efficiency.
*	N should be large enough so collisions are uncommon.
*	N should be a prime number.
*
* Copyright 2001 - 2002. Gavin Clayton. All Rights Reserved.
*
\****************************************************************************/


#if !defined(AFX_THASHARRAY_H__64032C83_6A9C_4AF1_8FCC_3541A6323925__INCLUDED_)
#define AFX_THASHARRAY_H__64032C83_6A9C_4AF1_8FCC_3541A6323925__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TStaticArray.h"
#include "TLinkedList.h"


//////////////////////////////////////////////////////////////////////
// THashArray
//////////////////////////////////////////////////////////////////////


//
// Definition and implementation of THashArray
// Note:	All implementation is built into the class definition.
//			This circumvents a bug in Intellisense for MSVC++ v6.0.
//			Refer to MSDN article Q190966 for more information.
//
template<typename T, int N> class THashArray  
{


//////////////////////////////////////////////////////////////////////
// Type Definitions
//////////////////////////////////////////////////////////////////////


// Defines container for hashed objects
typedef struct _HASHLOGIC {
	_HASHLOGIC() {pszName = NULL; pObject = NULL;}
	~_HASHLOGIC() {if ( pszName ) delete[] pszName; if ( pObject ) delete pObject;}
	char*			pszName;
	T*				pObject;
} HASHLOGIC, *LPHASHLOGIC;

// Defines type for hash collision-resolution list
typedef TLinkedList<HASHLOGIC> HASHLIST, *LPHASHLIST;


//////////////////////////////////////////////////////////////////////
// THashArray Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:


THashArray() {
	// Initialise
	m_nCreateCount = 0;
	m_HashArray.NewPool( N, &m_nCreateCount );
}/* THashArray */
	

virtual ~THashArray() {
	// Clean up
	Destroy();
}/* ~THashArray */


//////////////////////////////////////////////////////////////////////
// THashArray Inline Methods
//////////////////////////////////////////////////////////////////////
public:


//
// T* GetObject( const char* pszName )
// Obtain a pointer to named object
// Return: Pointer to object if successful, otherwise NULL
//
inline T* GetObject( const char* pszName )
{
	T* pObject = NULL;

	// Validate
	_ASSERT( m_nCreateCount == N );

	// Get key for object
	unsigned int nKey = ElfHash( pszName );

	// Locate hash list
	LPHASHLIST phl = m_HashArray.GetObject( nKey );

	// Locate object in list
	HLINKPOS h = phl->GetRoot();
	while ( h ) {
		LPHASHLOGIC phlog = phl->GetObject( h );
		if ( strcmp( pszName, phlog->pszName ) == 0 ) {
			pObject = phlog->pObject;
			break;
		}

		h = phl->GetNext( h );
	}

	return pObject;
}/* GetObject */


//////////////////////////////////////////////////////////////////////
// THashArray Methods
//////////////////////////////////////////////////////////////////////


//
// T* New( const char* pszName )
// Creates a new instance of type T
// Return: Pointer to new object if successful, otherwise NULL
//
T* New( const char* pszName )
{
	T* pObject = NULL;

	// Validate
	_ASSERT( m_nCreateCount == N );

	// Get key for new object
	unsigned int nKey = ElfHash( pszName );

	// Create new object
	pObject = new T;
	if ( !pObject )
		return NULL;

	// Locate hash list
	LPHASHLIST phl = m_HashArray.GetObject( nKey );

	// Check for duplicate name
	HLINKPOS h = phl->GetRoot();
	while ( h ) {
		if ( strcmp( pszName, phl->GetObject( h )->pszName ) == 0 ) {
			delete pObject;
			return NULL;
		}

		h = phl->GetNext( h );
	}

	// Name is unique
	h = phl->NewTail();
	if ( !h ) {
		delete pObject;
		return NULL;
	}

	// Copy name to memory
	char* pszStore = new char[strlen(pszName)+1];
	if ( !pszStore) {
		phl->Delete( h );
		delete pObject;
		return NULL;
	}
	else
	{
		strcpy( pszStore, pszName );
	}

	// Set values in hash container
	phl->GetObject( h )->pszName = pszStore;
	phl->GetObject( h )->pObject = pObject;

	return pObject;
}/* New */


//
// void Delete( const char* pszName )
// Delete an object in the hash table
//
void Delete( const char* pszName )
{
	// Validate
	_ASSERT( m_nCreateCount == N );

	if ( !pszName )
		return;

	// Get key for object
	unsigned int nKey = ElfHash( pszName );

	// Locate hash list
	LPHASHLIST phl = m_HashArray.GetObject( nKey );

	// Locate object
	HLINKPOS h = phl->GetRoot();
	while ( h ) {
		LPHASHLOGIC phlog = phl->GetObject( h );
		if ( strcmp( pszName, phlog->pszName ) == 0 ) {
			if ( phlog->pszName ) {
				delete[] phlog->pszName;
				phlog->pszName = NULL;
			}
			if ( phlog->pObject ) {
				delete phlog->pObject;
				phlog->pObject = NULL;
			}

			phl->Delete( h );
			return;
		}

		h = phl->GetNext( h );
	}
}/* Delete */


//
// void Destroy()
// Destroy all objects in the hash table
//
void Destroy()
{
	LPHASHLIST phl = NULL;

	// Validate
	_ASSERT( m_nCreateCount == N );

	// Destroy all populated hash lists
	for ( unsigned int n = 0; n < N; n++ ) {
		phl = m_HashArray.GetObject( n );
		if ( phl->GetCount() ) {
			HLINKPOS h = phl->GetRoot();
			while ( h ) {
				LPHASHLOGIC phlog = phl->GetObject( h );
				if ( phlog->pszName ) {
					delete[] phlog->pszName;
					phlog->pszName = NULL;
				}
				if ( phlog->pObject ) {
					delete phlog->pObject;
					phlog->pObject = NULL;
				}
				h = phl->GetNext( h );
			}// end while ( h )
			phl->Destroy();
		}// end if ( phl->GetCount() )
	}// end for ( unsigned int n = 0; n < N; n++ )
}/* Destroy */


private:	// methods


//
// unsigned int ElfHash( const char* pszName )
// Hash a string to an unsigned int
// Return: Hash key composed from string
//
unsigned int ElfHash( const char* pszName )
{
	unsigned int h = 0, g;

	while ( *pszName ) {
		h = ( h << 4 ) + *pszName++;
		if ( g = h & 0xf0000000 )
			h ^= g >> 24;

		h &= ~g;
	}

	return h % N;
}/* ElfHash */


private:	// data
	unsigned int	m_nCreateCount;
	TStaticArray<HASHLIST> m_HashArray;
};


#endif // !defined(AFX_THASHARRAY_H__64032C83_6A9C_4AF1_8FCC_3541A6323925__INCLUDED_)
