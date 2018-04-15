/****************************************************************************\
*
* Filename:		TLinkedList.h
* Purpose:		Template for <typename T>TLinkedList class
* Version:		1.3
* Author:		Gavin Clayton
*
* Last Updated:	16/09/2001
*
* Copyright 2000 - 2001. Gavin Clayton. All Rights Reserved.
*
\****************************************************************************/


#if !defined(TLINKEDLIST_H__F38D5A92_B392_4994_AC94_431492442F9B__INCLUDED_)
#define TLINKEDLIST_H__F38D5A92_B392_4994_AC94_431492442F9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// Support
//////////////////////////////////////////////////////////////////////


// Type used to reference T objects in the list
typedef UINT HLINKPOS;


//////////////////////////////////////////////////////////////////////
// TLinkedList
//////////////////////////////////////////////////////////////////////


//
// Definition and implementation of TLinkedList
// Note:	All implementation is built into the class definition.
//			This circumvents a bug in Intellisense for MSVC++ v6.0.
//			Refer to MSDN article Q190966 for more information.
//
template<typename T> class TLinkedList
{


// Used internally by TLinkedList to reference T connectivity
// Applications using TLinkedList can ignore this structure
typedef struct _LINKLOGIC
{
	_LINKLOGIC() { pNext = pPrev = pThis = NULL; pObject = NULL;}
	_LINKLOGIC*	pNext;		// Pointer to next node
	_LINKLOGIC*	pPrev;		// Pointer to previous node
	_LINKLOGIC* pThis;		// Pointer to this node
	T*			pObject;	// Pointer to the T object
} LINKLOGIC, *LPLINKLOGIC;


//////////////////////////////////////////////////////////////////////
// TLinkedList Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:


TLinkedList()
{
	// Initialise
	m_pRoot = m_pTail = NULL;
	m_nCount = 0;
}/* TLinkedList */


virtual ~TLinkedList()
{
	// Clean up the entire list
	Destroy();
}/* ~TLinkedList */


//////////////////////////////////////////////////////////////////////
// TLinkedList Inline Methods
//////////////////////////////////////////////////////////////////////
public:


//
// HLINKPOS GetRoot()
// Return: A handle to the root T object
//
inline HLINKPOS GetRoot()
{
	return (HLINKPOS)m_pRoot;
}/* GetRoot */


//
// HLINKPOS GetTail()
// Return: A handle to the tail T object
//
inline HLINKPOS GetTail()
{
	return (HLINKPOS)m_pTail;
}/* GetTail */


//
// HLINKPOS GetNext( HLINKPOS hPrevObj )
// Return: A handle to the next T object
//
inline HLINKPOS GetNext( HLINKPOS hPrevObj )
{
	LPLINKLOGIC pLinkObj;

	// hPrevObj cannot be NULL
	_ASSERT( hPrevObj != NULL );

	// Reference object
	pLinkObj = (LPLINKLOGIC)hPrevObj;

	return (HLINKPOS)pLinkObj->pNext;
}/* GetNext */


//
// HLINKPOS GetPrevious( HLINKPOS hPrevObj )
// Return: A handle to the previous T object
//
inline HLINKPOS GetPrevious( HLINKPOS hPrevObj )
{
	LPLINKLOGIC pLinkObj;

	// hPrevObj cannot be NULL
	_ASSERT( hPrevObj != NULL );

	// Reference object
	pLinkObj = (LPLINKLOGIC)hPrevObj;

	return (HLINKPOS)pLinkObj->pPrev;
}/* GetPrevious */


//
// T* GetObject( HLINKPOS hObject )
// Return: A pointer to the specified object
//
inline T* GetObject( HLINKPOS hObject )
{
	LPLINKLOGIC pLinkObj;

	// hObject cannot be NULL
	_ASSERT( hObject != NULL );

	// Reference object
	pLinkObj = (LPLINKLOGIC)hObject;

	return pLinkObj->pObject;
}/* GetObject */


//
// T& operator[]( HLINKPOS hObject )
// Return: A reference to the specified object
//
inline T& operator[]( HLINKPOS hObject )
{
	LPLINKLOGIC pLinkObj;

	// hObject cannot be NULL
	_ASSERT( hObject != NULL );

	// Reference object
	pLinkObj = (LPLINKLOGIC)hObject;

	return *pLinkObj->pObject;
}/* operator[] */


//
// unsigned int GetCount()
// Return: The current object count
//
inline unsigned int GetCount()
{
	return m_nCount;
}/* GetCount */


//////////////////////////////////////////////////////////////////////
// TLinkedList Methods
//////////////////////////////////////////////////////////////////////
public:


//
// HLINKPOS NewAt( HLINKPOS hInsertAt )
// Creates a new instance of T and inserts it to hInsertAt
// Return: A handle to the new T object if successful, otherwise NULL
//
HLINKPOS NewAt( HLINKPOS hInsertAt )
{
	T* pObject;
	LPLINKLOGIC pLinkObj, pOldLinkObj;

	// hInsertAt cannot be NULL
	_ASSERT( hInsertAt != NULL );

	// Create the LINKLOGIC container
	pLinkObj = new LINKLOGIC;
	if ( !pLinkObj )
	{
		return NULL;
	}// end if ( !pLinkObj )

	// Create the new object
	pObject = new T;
	if ( !pObject )
	{
		delete pLinkObj;
		return NULL;
	}// end if ( !pObject )

	// Add new object to list
	if ( !m_pRoot )
	{
		// Make this the first object in the list
		m_pRoot = pLinkObj;
		m_pTail = pLinkObj;
		m_pRoot->pObject = pObject;
		m_pRoot->pThis = m_pRoot;
	}
	else if ( m_pTail == (LPLINKLOGIC)hInsertAt )
	{
		// Make this the new tail object
		m_pTail->pNext = pLinkObj;
		pLinkObj->pPrev = m_pTail;
		m_pTail = pLinkObj;
		m_pTail->pObject = pObject;
		m_pTail->pThis = m_pTail;
	}
	else if ( m_pRoot == (LPLINKLOGIC)hInsertAt )
	{
		// Make this the new root object
		m_pRoot->pPrev = pLinkObj;
		pLinkObj->pNext = m_pRoot;
		m_pRoot = pLinkObj;
		m_pRoot->pObject = pObject;
		m_pRoot->pThis = m_pRoot;
	}
	else
	{
		// Reference object
		pOldLinkObj = (LPLINKLOGIC)hInsertAt;

		// Insert somewhere in the middle of the list
		pOldLinkObj->pPrev->pNext = pLinkObj;
		pLinkObj->pPrev = pOldLinkObj->pPrev;
		pOldLinkObj->pPrev = pLinkObj;
		pLinkObj->pNext = pOldLinkObj;
		pLinkObj->pObject = pObject;
		pLinkObj->pThis = pLinkObj;
	}// end if ( hInsertAt == m_pRoot )

	m_nCount++;
	return (HLINKPOS)pLinkObj;
}/* NewAt */


//
// HLINKPOS NewRoot()
// Creates a new instance of T and adds it to the root of the list
// Return: A handle to the new T object if successful, otherwise NULL
//
HLINKPOS NewRoot()
{
	T* pObject;
	LPLINKLOGIC pLinkObj;

	// Create the LINKLOGIC container
	pLinkObj = new LINKLOGIC;
	if ( !pLinkObj )
	{
		return NULL;
	}// end if ( !pLinkObj )

	// Create the new object
	pObject = new T;
	if ( !pObject )
	{
		delete pLinkObj;
		return NULL;
	}// end if ( !pObject )

	// Add a new object if there is no list
	if ( !m_pRoot )
	{
		// Make this the first object in the list
		m_pRoot = pLinkObj;
		m_pTail = pLinkObj;
		m_pRoot->pObject = pObject;
		m_pRoot->pThis = m_pRoot;
	}
	else
	{
		// Make this the new root object
		m_pRoot->pPrev = pLinkObj;
		pLinkObj->pNext = m_pRoot;
		m_pRoot = pLinkObj;
		m_pRoot->pObject = pObject;
		m_pRoot->pThis = m_pRoot;
	}// end // end if ( !m_pRoot )

	m_nCount++;
	return (HLINKPOS)pLinkObj;
}/* NewRoot */


//
// HLINKPOS NewTail()
// Creates a new instance of T and adds it to the tail of the list
// Return: A handle to the new T object if successful, otherwise NULL
//
HLINKPOS NewTail()
{
	T* pObject;
	LPLINKLOGIC pLinkObj;

	// Create the LINKLOGIC container
	pLinkObj = new LINKLOGIC;
	if ( !pLinkObj )
	{
		return NULL;
	}// end if ( !pLinkObj )

	// Create the new object
	pObject = new T;
	if ( !pObject )
	{
		delete pLinkObj;
		return NULL;
	}// end if ( !pObject )

	// Add a new object if there is no list
	if ( !m_pRoot )
	{
		// Make this the first object in the list
		m_pRoot = pLinkObj;
		m_pTail = pLinkObj;
		m_pRoot->pObject = pObject;
		m_pRoot->pThis = m_pRoot;
	}
	else
	{
		// Make this the new tail object
		m_pTail->pNext = pLinkObj;
		pLinkObj->pPrev = m_pTail;
		m_pTail = pLinkObj;
		m_pTail->pObject = pObject;
		m_pTail->pThis = m_pTail;
	}// end if ( !m_pRoot )

	m_nCount++;
	return (HLINKPOS)pLinkObj;
}/* NewTail */


//
// void Delete( HLINKPOS hObject )
// Deletes the specified object from the list
//
void Delete( HLINKPOS hObject )
{
	LPLINKLOGIC pLinkObj;

	// hObject cannot be NULL
	_ASSERT( hObject != NULL );

	// Reference object
	pLinkObj = (LPLINKLOGIC)hObject;

	// Reset neighbours in the chain then delete container and object
	if ( pLinkObj->pPrev )
	{
		pLinkObj->pPrev->pNext = pLinkObj->pNext;
	}
	else
	{
		if ( pLinkObj->pNext )
		{
			// Make object new root
			m_pRoot = pLinkObj->pNext;
			m_pRoot->pPrev = NULL;
		}
		else
		{
			// This is the final object - clean up
			Destroy();
			return;
		}// end if ( pLinkObj->pNext )
	}// end if ( pLinkObj->pPrev )
	if ( pLinkObj->pNext )
	{
		pLinkObj->pNext->pPrev = pLinkObj->pPrev;
	}
	else
	{
		if ( pLinkObj->pPrev )
		{
			// Make object new tail
			m_pTail = pLinkObj->pPrev;
			m_pTail->pNext = NULL;
		}
		else
		{
			// This is the final object - clean up
			Destroy();
			return;
		}
	}// end if ( pLinkObj->pNext )
	delete pLinkObj->pObject;
	delete pLinkObj;
	m_nCount--;

	// Reset list data if there are no more objects
	if ( GetCount() == 0 )
	{
		m_pRoot = m_pTail = NULL;
	}// end if ( GetCount() == 0 )
}/* Delete */


//
// void Destroy()
// Destroys the entire list
//
void Destroy()
{
	HLINKPOS hObject;
	LPLINKLOGIC pLinkObj;

	// Simply exit if there are no objects
	if ( !GetCount() )
		return;

	// Destroy all objects and containers in the list
	hObject = GetRoot();
	while ( hObject != NULL )
	{
		pLinkObj = (LPLINKLOGIC)hObject;
		hObject = GetNext( hObject );
		delete pLinkObj->pObject;
		delete pLinkObj;
	}// end while ( hObject != NULL )

	// Reset list data
	m_pRoot = m_pTail = NULL;
	m_nCount = 0;
}/* Destroy */


//////////////////////////////////////////////////////////////////////
// TLinkedList Data
//////////////////////////////////////////////////////////////////////
private:
	LPLINKLOGIC	m_pRoot;
	LPLINKLOGIC	m_pTail;
	unsigned int m_nCount;


}; /* TLinkedList */


#endif // !defined(_TLINKEDLIST_H__F38D5A92_B392_4994_AC94_431492442F9B__INCLUDED_)
