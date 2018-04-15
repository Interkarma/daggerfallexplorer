// TLog.h: interface for the TLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLOG_H__4DDCABA8_8A30_4541_9455_85E278D745D2__INCLUDED_)
#define AFX_TLOG_H__4DDCABA8_8A30_4541_9455_85E278D745D2__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TString.h"
#include "TLinkedList.h"


// Defines
#define TLOGCALLSTACKLENGTH 64

// Each log entry is composed from this structure
typedef struct _tagLogEntry
{
	HRESULT		hError;			// Error associated with log entry
	TString		strMsg;			// Log message
	TString		strExtended;	// Extended error message
} TLOGENTRY, *LPTLOGENTRY;


class TLog  
{
public:		// construction / destruction
	TLog();
	virtual ~TLog();

public:		// methods
	BOOL		Log( LPCSTR pszMsg, HRESULT hError = S_OK, BOOL bDumpCallStack = FALSE );
	void		GetLastLogEntry( LPTLOGENTRY pLogEntryOut );
	void		GetLastErrorEntry( LPTLOGENTRY pLogEntryOut );
	void		GetLogEntry( UINT nEntry, LPTLOGENTRY pLogEntryOut );
	int			GetLogSize() {return m_oaLog.GetCount();}
	int			GetMaxLogSize() {return m_nMaxLogSize;}
	int			SetMaxLogSize( int nMaxSizeIn ) {int nOld = m_nMaxLogSize; m_nMaxLogSize = nMaxSizeIn; return nOld;}
	void		PurgeLog() {m_oaLog.Destroy(); m_nLastLogEntry = NULL;}
	void		SetPrefix( LPCSTR pszPrefix ) {m_strPrefix = pszPrefix;}
	void		Redirect( TLog* pRedirect ) {m_pRedirect = pRedirect;}
	void		Undirect() {m_pRedirect = NULL;}
	BOOL		Save( LPCSTR pszPath );

	void		PushCall( LPCSTR pszName );
	void		PopCall();
	void		DumpCallStack();

public:		// objects
	TLinkedList<TLOGENTRY>	m_oaLog;
	TLinkedList<TString>	m_oaCallStack;

public:		// data
	TString		m_strPrefix;
	UINT		m_nLastErrorEntry;
	UINT		m_nLastLogEntry;
	int			m_nMaxLogSize;

private:	// data
	TLog*		m_pRedirect;
	UINT		m_nCallStackPos;
	UINT		m_nCallStackCount;
};


// Call stack helper - used to push with constructor, and pop with destructor when passing out of scope
typedef struct _tagCallStackHelper
{
	_tagCallStackHelper( TLog& Log, LPCSTR pszCall ) {
		m_pLog = &Log;
		m_pLog->PushCall( pszCall );
	}

	~_tagCallStackHelper() {
		m_pLog->PopCall();
	}

	TLog* m_pLog;
} TLOGCALLSTACKHELPER, *LPTLOGCALLSTACKHELPER;


// Helper defines
#define PUSHCALL( _log, _strcall ) TLOGCALLSTACKHELPER _callstackhelper( _log, _strcall )


#endif // !defined(AFX_TLOG_H__4DDCABA8_8A30_4541_9455_85E278D745D2__INCLUDED_)
