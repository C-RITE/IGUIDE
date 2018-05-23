#if !defined(AFX_WinSock2_H__80A343F3_F53F_1173_A131_48FE50004420__INCLUDED_)
#define AFX_WinSock2_H__80A343F3_F53F_1173_A131_48FE50004420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinSock.h : header file
//

#define WINSOCK_READ_BUFF_SIZE		(4096)			//Unused 1k buffer.
#define WSA_ERROR_LEN				(50)			//WSA Error message length

//#include <winnt.h>									//UNICODE def's if necessary
#include "mstcpip.h"

#ifndef _WINSOCK2API_
	#pragma message( "*** Warning : Winsock2.h has not been included and " )
	#pragma message( "              don't forget Ws2_32.lib." )
#endif

// **************************************************************************
//DESCRIPTION:
//		Class to Catch and Send Data from network sockets
// ***************************************************************************
class CWinSock2Async
{
// Attributes
public:

// Operations
public:
	CWinSock2Async();
	~CWinSock2Async();
	void	ThreadRunner();							//NEVER CALL THIS

	bool	Create( LPCTSTR sAddress = L"127.0.0.1", int nPort = 0 );
	bool	Connect( LPCTSTR sAddress, int nPort );
    bool    Listen( int nConnectionBacklog = 5 );
	bool	Accept( CWinSock2Async *pSockNew, sockaddr* pSockAddr = NULL, int* pnLen = NULL );

	DWORD	RecievePendingSize();
	int		Recieve( char* pchBuff, int nBuffLen, int nFlags = 0 );
	int		Send( char* pchBuff, int nLen, int nFlags = 0 );
	BOOL	GetPeerName( LPTSTR sAddress, int* const pnPort );

	//Oneliners
	bool	IsConnected()		{ return m_bConnected; }

//Static methods
public:	
	static in_addr	LookupAddress( LPCTSTR szHost );
	static BOOL		SocketInit( WSADATA* lpwsaData = NULL );
	static void		WSAGetLastErrorMessage( LPTSTR sMessagePrefix );

//Definitions
private:
	//Worker thread states
	enum enumThreadState 
	{ 
		eThreadNoStarted,
		eThreadRunning,	
		eThreadShuttingDown, 
		eThreadShutdown 
	};

//Operators
private:
	bool SetupEvents( SOCKET newSocket );
	bool ShutdownConnection();
	void Close();

//Event handlers
	virtual void OnRecieve( int nError );
	virtual void OnSend( int nError );
	virtual void OnConnect( int nError );
	virtual void OnAccept( int nError );
	virtual void OnClose( int nError );

//Attributes
private:
	SOCKET				m_Sd;						// Actual Socket handle
	WSAEVENT			m_WSAEvent;					// Main event handle
	enumThreadState volatile m_ThreadState;			//State of the worker thread

	CRITICAL_SECTION	m_csRecieve;				//Recieve Critical section handler
	CRITICAL_SECTION	m_csSend;					//Send ...

    bool				m_bConnected;				//True if Connected

};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WinSock2_H__80A343F3_F53F_1173_A131_48FE50004420__INCLUDED_)
