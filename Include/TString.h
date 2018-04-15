/****************************************************************************\
*
* Filename:		TString.h
* Purpose:		Definition of TString class
* Version:		1.0
* Author:		Gavin Clayton
*
* Last Updated:	08/12/2001
*
* Copyright 2000. Gavin Clayton. All Rights Reserved.
*
\****************************************************************************/


#if !defined(AFX_TSTRING_H__730E6634_F4CD_4F13_BC8D_BB74B84B910A__INCLUDED_)
#define AFX_TSTRING_H__730E6634_F4CD_4F13_BC8D_BB74B84B910A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TString  
{
public:		// construction / destruction
	TString();
	TString( const TString& stringSrc );
	TString( char ch, int nRepeat = 1 );
	TString( const unsigned char* psz );
	TString( const char* psz );
	virtual ~TString();

public:		// methods
	inline int		GetLength() {return m_nLength;}
	inline char*	GetBuffer() {return m_pszBuffer;}
	bool			New( int nLength = 0 );

public:		// operators
	const TString& operator = ( const TString& stringSrc );
	const TString& operator = ( const char ch );
	const TString& operator = ( const unsigned char* psz );
	const TString& operator = ( const char* psz );

	const TString& operator + ( const TString& stringSrc );
	const TString& operator + ( const char ch );
	const TString& operator + ( const unsigned char* psz );
	const TString& operator + ( const char* psz );

	const TString& operator += ( const TString& stringSrc );
	const TString& operator += ( const char ch );
	const TString& operator += ( const unsigned char* psz );
	const TString& operator += ( const char* psz );
	const TString& operator += ( const int n );

	bool operator == ( const TString& stringSrc );
	bool operator == ( const char ch );
	bool operator == ( const unsigned char* psz );
	bool operator == ( const char* psz );

	bool operator != ( const TString& stringSrc );
	bool operator != ( const char ch );
	bool operator != ( const unsigned char* psz );
	bool operator != ( const char* psz );

	const char operator[] ( const int nIndex );

	operator LPCTSTR() {return m_pszBuffer;}

	bool	AssignString( const char* pszSrc, int nLengthIn = 0 );
	bool	AppendString( const char* pszSrc, int nLengthIn = 0 );
	bool	CompareString( const char* pszSrc, int nLengthIn = 0 );

	//Left();
	//Right();
	//Mid();
	//Format();

private:	// methods
	inline char*	NewBuffer( int nLength );
	inline void		DeleteBuffer( char* pszBuffer );

	//bool	InsertString();
	//bool	ExtractString();

private:	// data
	char*	m_pszBuffer;
	int		m_nLength;

	unsigned int m_nAllocRef;
};


#endif // !defined(AFX_TSTRING_H__730E6634_F4CD_4F13_BC8D_BB74B84B910A__INCLUDED_)
