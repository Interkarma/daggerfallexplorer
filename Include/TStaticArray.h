/****************************************************************************\
*
* Filename:		TStaticArray.h
* Purpose:		Template for <typename T>TStaticArray class
* Version:		1.42
* Author:		Gavin Clayton
*
* Notes:
*	TStaticArray manages pointers to objects of type T.
*	These pointers are stored within a contiguous array.
*	The T objects themselves might reside anywhere in memory.
*	These objects are referenced by their index, which is of type HPOLYOBJ.
*	Objects are created and destroyed using the New() and Delete() methods.
*	The method AddPool() adds a block of objects to the array
*	When an object is successfully created the index is returned as type HPOLYOBJ.
*	If the object fails to be created the return value is HSTOPERR.
*	The HPOLYOBJ value will be constant for the lifetime of the object and can be stored.
*	The method IsHandleValid() is used to determine if an index is valid.
*	To iterate through the array use the Find methods.
*	When there are no more objects to iterate all Find methods will return HSTOPERR.
*	If the array is contiguous iteration can be performed using absolute indices.
*	The object itself can be retrieved using the operator [] (&reference) or the method GetObject() (*pointer)
*	The pointer array itself can be returned with GetArray() (**pointer to a pointer array).
*	Objects can be inserted into a specific entry point in the array, but only if that entry point is available.
*	The method IsEntryValid() can be used to verify if an entry point is available.
*	If no entry point is specified then TStaticArray will automatically locate one.
*	The pointer array will grow automatically if it becomes full.
*	Use SetGrow or SetLength to change how large the array will grow by each time growth is required.
*	The methods Delete() and DeleteAll() will destroy objects and free locations inside the pointer array.
*	The method Destroy() will delete all objects and remove the pointer array.
*	The array will be destroyed automatically when it passes out of scope.
*	The pointer array will be automatically reinstated when using the New(), NewPool() or SetLength() methods.
*
* Hints:
*	HPOLYOBJ is really just an unsigned int index into the array, and can be stored.
*	Always use SetLength to initialise an appropriate start size and growth of the array.
*	If using stable contiguous data always allocate with AddPool and iterate with absolute index.
*	If using volatile data always allocate with New and iterate with Find methods.
*	Only use GetArray when the contents are going to be predictable.
*	Never use delete[] on the pointer array after calling GetArray.
*
* Strengths:
*	Very fast allocation and destruction of objects.
*	Very fast iteration when using absolute indices.
*	An object index will be static for the lifetime of the object.
*	Array can be accessed randomly very quickly if an index is known.
*	Thin layer over actual object so overheads are small.
*
* Limitations:
*	Absolute access can only be assured when the array is created in contiguous blocks.
*	When using Delete more frequently than New array can become fragmented which impacts iteration.
*	Nature of static indices prevents array from being sorted in most cases.
*	Designed for in-memory operations, so low-memory situations are far from optimal.
*	Sequential access using Find methods considerably slower (approx 32%) to similar linked list.
*	Absolute access is both random and faster (approx 2%) than similar linked list, but prerequisites are restrictive.
*	Checking for HSTOPERR rather than a simple NULL or 0 adds programmer overhead and is inconsistent with similar storage classes.
*
* Usage:
*	When using long term in-memory objects that are allocated and destroyed in blocks.
*	When using a management scheme requiring indices that can be stored.
*	For management of n-sized object arrays with low programmer overhead.
*
* Last Updated:	24/12/2001
*
* Copyright 1999-2001. Gavin Clayton. All Rights Reserved.
*
\****************************************************************************/


#if !defined(_TSTATICARRAY_H__06C32D63_B862_11D3_9576_00E0290A2804__INCLUDED_)
#define _TSTATICARRAY_H__06C32D63_B862_11D3_9576_00E0290A2804__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// Type Definitions
//////////////////////////////////////////////////////////////////////


// Type definitions
typedef unsigned int HPOLYOBJ;


// Const declarations
const HPOLYOBJ HSTOPERR = 0xffffffff;


//////////////////////////////////////////////////////////////////////
// TStaticArray
//////////////////////////////////////////////////////////////////////


//
// Definition and implementation of TStaticArray
// Note:	All implementation is built into the class definition.
//			This circumvents a bug in Intellisense for MSVC++ v6.0.
//			Refer to MSDN article Q190966 for more information.
//
template<typename T> class TStaticArray
{


//////////////////////////////////////////////////////////////////////
// TStaticArray Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:


TStaticArray()
{
	// Initialise
	m_pArray = NULL;
	m_hNext = m_nCount = m_nLength = 0;
	m_nGrow = 1;
}/* TStaticArray */


~TStaticArray()
{
	// Clean up the entire array
	Destroy();
}/* ~TStaticArray */


//////////////////////////////////////////////////////////////////////
// TStaticArray Inline Member Functions
//////////////////////////////////////////////////////////////////////
public:


inline T& operator[]( HPOLYOBJ hObject )
{
	// Ensure the object is valid
	_ASSERT( IsHandleValid( hObject ) );

	// Return the pointer to the specified object
	return *m_pArray[hObject];
}/* operator[] */


inline T* GetObject( HPOLYOBJ hObject )
{
	// Ensure the object is valid
	_ASSERT( IsHandleValid( hObject ) );

	// Return the pointer to the specified object
	return m_pArray[hObject];
}/* GetObject */


inline T** GetArray()
{
	// Ensure array is created
	_ASSERT( m_pArray );

	// Return the array
	return m_pArray;
}/* GetArray */


inline unsigned int GetLength()
{
	return m_nLength;
}/* GetLength */


inline unsigned int GetCount()
{
	return m_nCount;
}/* GetCount */


inline int GetGrow()
{
	return m_nGrow;
}/* GetCount */


inline int SetGrow( int nGrow )
{
	SetLength( GetLength(), nGrow );
}/* SetGrow */


inline bool Grow()
{
	return SetLength( GetLength() + m_nGrow );
}/* Grow */


//////////////////////////////////////////////////////////////////////
// TStaticArray Member Functions
//////////////////////////////////////////////////////////////////////


//
// HPOLYOBJ New( HPOLYOBJ hEntry = HSTOPERR )
// Creates a new instance of T and adds it to the pointer array
// The hEntry value enables an entry point to be specified, but the entry must be available
// If hEntry is HSTOPERR an entry point will be chosen automatically
// If the array is too small for another object, it will grow automatically
// Return: The HPOLYOBJ of the new object if successful, HSTOPERR if failed
//
HPOLYOBJ New( HPOLYOBJ hEntry = HSTOPERR )
{
	T* pObject = NULL;

	// Ensure the list is large enough to carry more objects
	if ( GetLength() == GetCount() )
	{
		if ( !Grow() )
		{
			return HSTOPERR;
		}// end if ( !Grow() )
	}// end if ( GetLength() == GetCount() )

	// Ensure the entry point is valid if specified
	if ( hEntry != HSTOPERR )
	{
		// This assertion is tripped if the specified entry point is not valid
		_ASSERT( IsEntryValid( hEntry ) );
		if ( !IsEntryValid( hEntry ) )
		{
			return HSTOPERR;
		}// end if ( !IsEntryValid( hEntry ) )

		// Force the new entry point
		m_hNext = hEntry;
	}// end if ( hEntry != HSTOPERR )

	// Attempt to create the new object
	pObject = new T;
	if ( !pObject )
	{
		return HSTOPERR;
	}// end if ( !pObject )

	// Add the new object into the first available position
	if ( IsEntryValid( m_hNext ) )
	{
		// Add object to next position
		m_pArray[m_hNext] = pObject;
		m_nCount++;
		return m_hNext++;	// The next entry is typically free
	}
	else
	{
		// Find next valid entry point
		for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
		{
			if ( m_pArray[nIndex] == NULL )
			{
				m_pArray[nIndex] = pObject;
				m_nCount++;
				m_hNext = nIndex + 1;
				return nIndex;
			}// end if ( m_pArray[nIndex] == NULL )
		}// end for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
	}// end if ( IsEntryValid(m_hNext) )

	// Somehow the operation has failed
	// Clean up the temporary object and return HSTOPERR
	delete pObject;
	return HSTOPERR;
}/* New */


//
// HPOLYOBJ NewPool( unsigned int nPoolSize, unsigned int* pCreateCountOut )
// Creates nPoolSize new objects and adds them to the array
// If successful, user can assume indices n-nPoolSize are now valid
// Return: The HPOLYOBJ of the first object created, HSTOPERR if no objects are created
// Note: Always check value pCreateCountOut for how many objects were created
//
HPOLYOBJ NewPool( unsigned int nPoolSize, unsigned int* pCreateCountOut = NULL )
{
	HPOLYOBJ hFirst = HSTOPERR;
	T* pObject = NULL;

	// nPoolSize cannot be 0
	_ASSERT( nPoolSize > 0 );

	// Ensure there is enough contiguous space at the end of the storage array
	unsigned int nBase = FindLastObject();
	( nBase == HSTOPERR ) ? nBase = 0 : nBase++;
	unsigned int nFree = GetUnusedSpace();
	if ( nFree < nPoolSize ) {
		if ( !SetLength( m_nLength + (nPoolSize - nFree) ) )
			return HSTOPERR;
	}

	// Create the objects
	hFirst = m_hNext = nBase;
	for ( unsigned int i = 0; i < nPoolSize; i++ )
	{
		// Attempt to create the new object
		pObject = new T;
		if ( !pObject ) {
			if ( pCreateCountOut ) *pCreateCountOut = i - 1;
			return hFirst;
		}

		// Add object to next position
		m_pArray[m_hNext] = pObject;
		m_nCount++;
		m_hNext++;
	}

	if ( pCreateCountOut ) *pCreateCountOut = nPoolSize;

	return hFirst;
}/* NewPool */


//
// HPOLYOBJ FindFirstObject()
// Attempts to locate the first valid object in the array
// Return: The HPOLYOBJ of the located object, HSTOPERR if no object found or array has no length
//
HPOLYOBJ FindFirstObject()
{
	// Cannot search if array does not exist
	if ( !m_pArray )
	{
		return HSTOPERR;
	}// end if ( !m_pArray )

	// Search for the first valid object
	for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
	{
		if ( m_pArray[nIndex] != NULL )
		{
			return nIndex;
		}// end if ( m_pArray[nIndex] != NULL )
	}// end for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )

	// No valid objects found
	return HSTOPERR;
}/* FindFirstObject */


//
// HPOLYOBJ FindNextObject( HPOLYOBJ hPrevObj )
// Attempts to locate the next valid object in the array, based on the start value
// Return: The HPOLYOBJ of the located object, HSTOPERR if there are no more objects to be found
//
HPOLYOBJ FindNextObject( HPOLYOBJ hPrevObj )
{
	// Cannot search if array does not exist or is out of bounds
	if ( (!m_pArray) || (hPrevObj > GetLength()) )
	{
		return HSTOPERR;
	}// end if ( (!m_pArray) || (hPrevObj > GetLength()) )

	// Search for the next valid object
	for ( unsigned int nIndex = ++hPrevObj; nIndex < GetLength(); nIndex++ )
	{
		if ( m_pArray[nIndex] != NULL )
		{
			return nIndex;
		}// end if ( m_pArray[nIndex] != NULL )
	}// end for ( unsigned int nIndex = ++hPrevObj; nIndex < GetLength(); nIndex++ )

	// No valid objects found
	return HSTOPERR;
}/* FindNextObject */


//
// HPOLYOBJ FindLastObject()
// Attempts to locate the last valid object in the array
// Return: The HPOLYOBJ of the located object, HSTOPERR if no object found or array has no length
//
HPOLYOBJ FindLastObject()
{
	// Cannot search if array does not exist
	if ( !m_pArray )
	{
		return HSTOPERR;
	}// end if ( !m_pArray )

	// Search for the last valid object
	for ( unsigned int nIndex = GetLength() - 1; nIndex >= 0; nIndex-- )
	{
		if ( m_pArray[nIndex] != NULL )
		{
			return nIndex;
		}// end if ( m_pArray[nIndex] != NULL )
	}// end for ( unsigned int nIndex = GetLength() - 1; nIndex >= 0; nIndex-- )

	// No valid objects found
	return HSTOPERR;
}/* FindLastObject */


//
// HPOLYOBJ FindPreviousObject( HPOLYOBJ hPrevObj )
// Attempts to locate the previous valid object in the array, based on the start value
// Return: The HPOLYOBJ of the located object, HSTOPERR if there are no more objects to be found
//
HPOLYOBJ FindPreviousObject( HPOLYOBJ hPrevObj )
{
	// Cannot search if array does not exist or is out of bounds
	if ( (!m_pArray) || (hPrevObj > GetLength()) )
	{
		return HSTOPERR;
	}// end if ( (!m_pArray) || (hPrevObj > GetLength()) )

	// Search for the next valid object
	for ( unsigned int nIndex = --hPrevObj; nIndex >= 0; nIndex-- )
	{
		if ( m_pArray[nIndex] != NULL )
		{
			return nIndex;
		}// end if ( m_pArray[nIndex] != NULL )
	}// end for ( unsigned int nIndex = --hPrevObj; nIndex >= 0; nIndex-- )

	// No valid objects found
	return HSTOPERR;
}/* FindPreviousObject */


//
// HPOLYOBJ FindFirstEntry( bool bSet = false )
// Attempts to locate the first valid entry point into the array
// Return: The HPOLYOBJ of the first entry point, HSTOPERR if array is full or has no length
//
HPOLYOBJ FindFirstEntry( bool bSet = false )
{
	// Cannot search if array does not exist
	if ( !m_pArray )
	{
		return HSTOPERR;
	}// end if ( !m_pArray )

	// Search for the first entry point
	for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
	{
		if ( m_pArray[nIndex] == NULL )
		{
			if ( bSet ) {
				m_hNext = nIndex;
			}

			return nIndex;
		}// end if ( m_pArray[nIndex] == NULL )
	}// end for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )

	// No entry point found
	return HSTOPERR;
}/* FindFirstEntry */


//
// HPOLYOBJ FindNextEntry( HPOLYOBJ hPrevEntry )
// Attempts to locate the next valid entry point into the array
// Return: The HPOLYOBJ of the located entry point, HSTOPERR if the array is full
//
HPOLYOBJ FindNextEntry( HPOLYOBJ hPrevEntry )
{
	// Cannot search if array does not exist or is out of bounds
	if ( (!m_pArray) || (hPrevObj > GetLength()) )
	{
		return HSTOPERR;
	}// end if ( (!m_pArray) || (hPrevEntry > GetLength()) )

	// Search for the next valid entry point
	for ( unsigned int nIndex = ++hPrevEntry; nIndex < GetLength(); nIndex++ )
	{
		if ( m_pArray[nIndex] == NULL )
		{
			return nIndex;
		}// end if ( m_pArray[nIndex] == NULL )
	}// end for ( unsigned int nIndex = ++hPrevEntry; nIndex < GetLength(); nIndex++ )

	// No entry point found
	return HSTOPERR;
}/* FindNextEntry */


//
// void Delete( HPOLYOBJ hObject )
// Destroy the object at the specified index
// The specified entry will now be available for use
//
void Delete( HPOLYOBJ hObject )
{
	// Ensure the object is valid
	_ASSERT( IsHandleValid( hObject ) );

	// Cannot delete if array does not exist
	if ( !m_pArray )
	{
		return;
	}// end if ( !m_pArray )

	// Destroy the object
	if ( m_pArray[hObject] != NULL )
	{
		delete m_pArray[hObject];
		m_pArray[hObject] = NULL;
		m_hNext = hObject;
		m_nCount--;
	}// end if ( m_pArray[hObject] != NULL )
}/* Delete */


//
// void DeleteAll()
// Destroy all objects in the array, but retain array length
// All entries up to GetLength() will now be available
//
void DeleteAll()
{
	HPOLYOBJ hObject;

	// Destroy all objects
	hObject = FindFirstObject();
	while ( hObject != HSTOPERR )
	{
		Delete( hObject );
		hObject = FindNextObject( hObject );
	}// end while ( hObject != HSTOPERR )

	m_hNext = 0;
}/* DeleteAll */


//
// void Destroy()
// Destroy the entire array and every object in it
// There will no longer be any available entry points after this function
// Either SetLength() or New( HSTOPERR ) must be called to reinstate the array
//
void Destroy()
{
	// Cannot destroy if an array does not exist
	if ( !m_pArray )
	{
		return;
	}// end if ( !m_pArray )

	// Delete all managed objects
	if ( GetCount() > 0 )
	{
		for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
		{
			if ( m_pArray[nIndex] != NULL )
			{
				delete m_pArray[nIndex];
			}// end if ( m_pArray[nIndex] != NULL )
		}// end for ( unsigned int nIndex = 0; nIndex < GetLength(); nIndex++ )
	}// end if ( GetCount() > 0 )

	// Delete the pointer array and reset state information
	delete[] m_pArray;
	m_pArray = NULL;
	m_hNext = HSTOPERR;
	m_nCount = 0;
	m_nLength = 0;
}/* Destroy */


//
// unsigned int GetUnusedSpace()
// Returns the number of contiguous free entry points at the very end of the array
//
unsigned int GetUnusedSpace()
{
	unsigned int nFree = 0;

	// Check that an array exists
	if ( !m_pArray )
	{
		return 0;
	}// end if ( !m_pArray )

	// Count backwards from the end until a used pointer is encountered
	unsigned int nIndex = GetLength() - 1;
	do
	{
		if ( m_pArray[nIndex] == NULL )
		{
			nFree++;
		}
		else
		{
			return nFree;
		}// end if ( m_pArray[nIndex] == NULL )
	} while ( nIndex-- > 0 );

	return nFree;
}/* GetUnusedSpace */


//
// bool SetLength( unsigned int nLength, int nGrow = -1 )
// Shrinks or grows the array by nLength
// The array can be truncated only if there are enough contiguous free entry points at the very end of the array
// If the array needs to grow automatically at any time, it will grow by nGrow
// Any nGrow value < 1 will be ignored
// An nLength value of the current length will be ignored, and the array will not change size
// An nLength value of 0 is similar to Destroy(), but operates only if the object count is zero
// Return: true if successful, false if failed
//
bool SetLength( unsigned int nLength, int nGrow = -1 )
{
	T** pTemp = NULL;

	// Set the new nGrow if specified
	if ( nGrow >= 1 )
	{
		m_nGrow = nGrow;
	}// end if ( nGrow >= 1 )

	// Do nothing if length is the same
	if ( m_nLength == nLength )
	{
		return true;
	}// end if ( m_nLength == nLength )

	// Remove array if new length is zero
	if ( nLength == 0 )
	{
		// Cannot remove array if it does not already exist
		if ( !m_pArray )
		{
			return false;
		}// end if ( !m_pArray )

		// Can remove only if no objects are being managed
		if ( GetCount() == 0 )
		{
			delete[] m_pArray;
			m_pArray = NULL;
			m_hNext = HSTOPERR;
			m_nLength = 0;
			return true;
		}
		else
		{
			return false;
		}// end if ( GetCount() == 0 )
	}// end if ( nLength == 0 )

	// If the new size is larger, grow the array
	if ( m_nLength < nLength )
	{
		// Create the new array
		pTemp = new T*[nLength];
		if ( !pTemp )
		{
			return false;
		}// end if ( !pTemp )

		// Initialise the new array with zero throughout
		memset( pTemp, 0, (nLength * sizeof (T*)) );

		// Copy all pointers from the old array then delete it
		if ( m_pArray )
		{
			memcpy( pTemp, m_pArray, (m_nLength * sizeof (T*)) );
			delete[] m_pArray;
		}// end if ( m_pArray )
		
		// Store the new array and set the new length
		m_pArray = pTemp;
		m_nLength = nLength;

		return true;
	}// end if ( m_nLength < nLength )

	// If the new size is smaller, shrink the array
	if ( m_nLength > nLength )
	{
		// Array must exist
		if ( !m_pArray )
		{
			return false;
		}// if ( !m_pArray )

		// Amount to shrink by must not exceed the free space available
		if ( (m_nLength - nLength) > GetUnusedSpace() )
		{
			return false;
		}// end if ( (m_nLength - nLength) > GetUnusedSpace() )

		// Create the new array
		pTemp = new T*[nLength];
		if ( !pTemp )
		{
			return false;
		}// end if ( !pTemp )

		// Initialise the new array with zero throughout
		memset( pTemp, 0, (nLength * sizeof (T*)) );

		// Copy data from the old array and delete it
		memcpy( pTemp, m_pArray, (nLength * sizeof (T*)) );
		delete[] m_pArray;
		
		// Store the new array and set the new length
		m_pArray = pTemp;
		m_nLength = nLength;

		return true;
	}// end if ( m_nLength > nLength )

	return true;
}/* SetLength */


//
// bool IsHandleValid( HPOLYOBJ hObject )
// Determines if the specified handle is valid
// Return: true if handle is valid, otherwise false
//
inline
bool IsHandleValid( HPOLYOBJ hObject )
{
	if ( !m_pArray )
		return false;					// Error if there is no array

	if ( hObject == HSTOPERR )		// Error if handle is explicitly invalid
		return false;

	if ( hObject > GetLength() )
		return false;					// Error if above length of array

	if ( m_pArray[hObject] == NULL )
		return false;					// Error if index is unassigned

	return true;						// Handle appears to be valid
}/* IsHandleValid */


//
// bool IsEntryValid( HPOLYOBJ hEntry )
// Determines if the specified entry point is valid
// Return: true if entry point is valid and available, otherwise false
//
inline
bool IsEntryValid( HPOLYOBJ hEntry )
{
	if ( !m_pArray )
		return false;					// Error if there is no array

	if ( hEntry == HSTOPERR )			// Error if handle is explicitly invalid
		return false;

	if ( hEntry > GetLength() )
		return false;					// Error if above length of array

	if ( m_pArray[hEntry] != NULL )
		return false;					// Error if index is assigned

	return true;						// Handle appears to be valid
}/* IsEntryValid */


private:	// data
	T**		m_pArray;
	unsigned int	m_nCount;
	unsigned int	m_nLength;
	int				m_nGrow;
	HPOLYOBJ		m_hNext;


}; /* TStaticArray */


#endif // !defined(_TSTATICARRAY_H__06C32D63_B862_11D3_9576_00E0290A2804__INCLUDED_)
