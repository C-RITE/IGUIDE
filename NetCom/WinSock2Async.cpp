// ***************************************************************************
//DESCRIPTION:
//		This simple class provides SOME of the functionality of the 
//		MFC CAsyncSocket without using MFC. Most functions are mapped in the
//		same way to enable simple swaping.
//
//NOTE:
//		The events are monitored using another thread to all the On...
//		methods must be thread safe.
//
//REQUIREMENTS:
//		Include		#include <Winsock2.h>		// Socket support
//					#include <windows.h>		//Not necessary if using MFC
//					#include <TChar.h>			//..
//					#include <assert.h>			//..
//		Link with	Ws2_32.lib
// ***************************************************************************
#pragma once
#include "stdafx.h"
#include "WinSock2Async.h"							//Socket for Requesting data


#ifdef __AFX_H__
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#else
	//Trun on and off the tracing display
	#define TRACE				printf
	#define	ASSERT				assert
#endif

//Local functions
DWORD WINAPI StartPlayingThread(LPVOID param);

// WSA Error code descriptions
static struct ErrorEntry
{
	int nID;
	const _TCHAR* pcMessage;
} g_aErrorList[] =
{
	{ 0,                     _T("No error") },
	{ WSAEINTR,              _T("Interrupted system call") },
	{ WSAEBADF,              _T("Bad file number") },
	{ WSAEACCES,             _T("Permission denied") },
	{ WSAEFAULT,             _T("Bad address") },
	{ WSAEINVAL,             _T("Invalid argument") },
	{ WSAEMFILE,             _T("Too many open sockets") },
	{ WSAEWOULDBLOCK,        _T("Operation would block") },
	{ WSAEINPROGRESS,        _T("Operation now in progress") },
	{ WSAEALREADY,           _T("Operation already in progress") },
	{ WSAENOTSOCK,           _T("Socket operation on non-socket") },
	{ WSAEDESTADDRREQ,       _T("Destination address required") },
	{ WSAEMSGSIZE,           _T("Message too long") },
	{ WSAEPROTOTYPE,         _T("Protocol wrong type for socket") },
	{ WSAENOPROTOOPT,        _T("Bad protocol option") },
	{ WSAEPROTONOSUPPORT,    _T("Protocol not supported") },
	{ WSAESOCKTNOSUPPORT,    _T("Socket type not supported") },
	{ WSAEOPNOTSUPP,         _T("Operation not supported on socket") },
	{ WSAEPFNOSUPPORT,       _T("Protocol family not supported") },
	{ WSAEAFNOSUPPORT,       _T("Address family not supported") },
	{ WSAEADDRINUSE,         _T("Address already in use") },
	{ WSAEADDRNOTAVAIL,      _T("Can't assign requested address") },
	{ WSAENETDOWN,           _T("Network is down") },
	{ WSAENETUNREACH,        _T("Network is unreachable") },
	{ WSAENETRESET,          _T("Net connection reset") },
	{ WSAECONNABORTED,       _T("Software caused connection abort") },
	{ WSAECONNRESET,         _T("Connection reset by peer") },
	{ WSAENOBUFS,            _T("No buffer space available") },
	{ WSAEISCONN,            _T("Socket is already connected") },
	{ WSAENOTCONN,           _T("Socket is not connected") },
	{ WSAESHUTDOWN,          _T("Can't send after socket shutdown") },
	{ WSAETOOMANYREFS,       _T("Too many references, can't splice") },
	{ WSAETIMEDOUT,          _T("Connection timed out") },
	{ WSAECONNREFUSED,       _T("Connection refused") },
	{ WSAELOOP,              _T("Too many levels of symbolic links") },
	{ WSAENAMETOOLONG,       _T("File name too long") },
	{ WSAEHOSTDOWN,          _T("Host is down") },
	{ WSAEHOSTUNREACH,       _T("No route to host") },
	{ WSAENOTEMPTY,          _T("Directory not empty") },
	{ WSAEPROCLIM,           _T("Too many processes") },
	{ WSAEUSERS,             _T("Too many users") },
	{ WSAEDQUOT,             _T("Disc quota exceeded") },
	{ WSAESTALE,             _T("Stale NFS file handle") },
	{ WSAEREMOTE,            _T("Too many levels of remote in path") },
	{ WSASYSNOTREADY,        _T("Network subsystem is unavailable") },
	{ WSAVERNOTSUPPORTED,    _T("Winsock version not supported") },
	{ WSANOTINITIALISED,     _T("Winsock not yet initialized") },
	{ WSAHOST_NOT_FOUND,     _T("Host not found") },
	{ WSATRY_AGAIN,          _T("Non-authoritative host not found") },
	{ WSANO_RECOVERY,        _T("Non-recoverable errors") },
	{ WSANO_DATA,            _T("Valid name, no data record of requested type") },
	{ WSAEDISCON,            _T("Graceful disconnect in progress") },
	{ WSASYSCALLFAILURE,     _T("System call failure") },
	{ WSA_NOT_ENOUGH_MEMORY, _T("Insufficient memory available") },
	{ WSA_OPERATION_ABORTED, _T("Overlapped operation aborted") },
	{ WSA_IO_INCOMPLETE,  	 _T("Overlapped I/O object not signalled") },
	{ WSA_IO_PENDING,        _T("Overlapped I/O will complete later") },
	//{ WSAINVALIDPROCTABLE,   _T("Invalid proc. table from service provider") },
	//{ WSAINVALIDPROVIDER,    _T("Invalid service provider version number") },
	//{ WSAPROVIDERFAILEDINIT, _T("Unable to init service provider") },
	{ WSA_INVALID_PARAMETER, _T("One or more parameters are invalid") },
	{ WSA_INVALID_HANDLE,    _T("Event object handle not valid") }
};
const int NUM_WSERROR_MESSAGES = sizeof(g_aErrorList) / sizeof(ErrorEntry);



// **************************************************************************
//DESCRIPTION:
//		Construction/Destruction
// ***************************************************************************
CWinSock2Async::CWinSock2Async() :
	m_Sd(INVALID_SOCKET),
	m_WSAEvent(WSA_INVALID_EVENT),
	m_ThreadState(eThreadNoStarted),
	m_bConnected(false)
{
	TRACE( _T("CWinSock2Async::CWinSock2Async()\n") );
	InitializeCriticalSection( &m_csRecieve );
	InitializeCriticalSection( &m_csSend );

}


CWinSock2Async::~CWinSock2Async()
{
	TRACE( _T("CWinSock2Async::~CWinSock2Async()\n") );
	Close();
	DeleteCriticalSection( &m_csRecieve );
	DeleteCriticalSection( &m_csSend );
	m_bConnected = false;
}


// ***************************************************************************
//DESCRIPTION:
//		Initiates use of Ws2_32.dll by a process and verifys the current 
//		version.
//PARAMS:
//		lpwsaData	Pointer to a WSADATA structure or NULL to use
//					an internal one.
//RETURN:
//		See help on WSAStartup for most return codes
// ***************************************************************************
BOOL CWinSock2Async::SocketInit( WSADATA* lpwsaData )
{
	TRACE( _T("CWinSock2Async::SocketInit( %p ) const\n"), lpwsaData );
	//
	//Load the Required Version of the sockets DLL
	//
	WSADATA wsaData = { 0 };
	if( lpwsaData == NULL )
		lpwsaData = &wsaData;

	int nError = WSAStartup ( MAKEWORD( 2, 2 ), lpwsaData );
	if( nError )
		return nError;

	// Confirm that the WinSock DLL supports 2.2.
	// Note that if the DLL supports versions greater    
	// than 2.2 in addition to 2.2, it will still return 
	// 2.2 in wVersion since that is the version we      
	// requested.                                        
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		nError = WSAVERNOTSUPPORTED;
	    // Tell the user that we could not find a usable 
		// WinSock DLL.                                  
	    WSACleanup( );
	}
	return nError; 
}



// **************************************************************************
//DESCRIPTION:
//		Try to setup the socket
//PARAMS:
//      nPort	If non zero, this will be the port listening will
//				occur on.
//RETURN:
//		True if a connection was made
// ***************************************************************************
bool CWinSock2Async::Create( LPCTSTR sAddress, int nPort )
{
	TRACE( _T("CWinSock2Async::Create()\n") );

	//
	// Create a stream socket
	//
	SOCKET sockNew = socket(AF_INET, SOCK_STREAM, 0);
	if( sockNew == INVALID_SOCKET ) 
		return false;

	//
	//Create the worker thread to monitor the Socket events
	//
	if( !SetupEvents( sockNew ) )
		return false;

	if (nPort == 0) {
		return true;
	}
	//
	//Setup for listening if necessary
	//
	//in_addr nRemoteAddress = LookupAddress(sAddress);
	//if (nRemoteAddress.S_un.S_addr == 0)
	//{
	//	TCHAR szWASError[WSA_ERROR_LEN];
	//	WSAGetLastErrorMessage( szWASError );
	//	TRACE( _T("*** lookup address: %s\n"), szWASError );
	//	return false;
	//}
	
	sockaddr_in sinRemote;
	sinRemote.sin_family = AF_INET;
	sinRemote.sin_addr.S_un.S_addr = INADDR_ANY; // nRemoteAddress
	sinRemote.sin_port = htons((unsigned short)nPort);


	return ( bind( m_Sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in) ) == 0 );

}


// **************************************************************************
//DESCRIPTION:
//		Try to make a connection to the Server
//PARAMS:
//		sAddress	Clear text name of server to connect to
//		nPort		Port to connect to
//RETURN:
//		True if a connection was made
// ***************************************************************************
bool CWinSock2Async::Connect(LPCTSTR sAddress, int nPort)
{
	TRACE(_T("CWinSock2Async::Connect( %s, %d )\n"), sAddress, nPort);

	//
	//Resolve Address
	//

	in_addr nRemoteAddress = LookupAddress(sAddress);
	if (nRemoteAddress.S_un.S_addr == 0) 
	{
		TCHAR szWASError[WSA_ERROR_LEN];
		WSAGetLastErrorMessage( szWASError );
		TRACE( _T("*** lookup address: %s\n"), szWASError );
		return false;
	}

	m_IP = sAddress;
	
	in_addr Address;
	memcpy(&Address, &nRemoteAddress, sizeof(u_long)); 
	TRACE( _T("Connecting to %d : %d\n"), nRemoteAddress.S_un, nPort );

	//
	//Connect
	//
	sockaddr_in sinRemote;
	sinRemote.sin_family = AF_INET;
	sinRemote.sin_addr = nRemoteAddress;
	sinRemote.sin_port = htons((unsigned short)nPort);
	if( connect( m_Sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR ) 
	{
		if( WSAGetLastError() != WSAEWOULDBLOCK )
		{
			m_Sd = INVALID_SOCKET;
			TCHAR szWASError[WSA_ERROR_LEN];
			WSAGetLastErrorMessage( szWASError );
			TRACE( _T("ERROR : Connecting.: %s\n"), szWASError );
			return false;
	
		}


	}
	
	return true;

}


// ***************************************************************************
//DESCRIPTION:
//      Places a socket a state where it is listening for an incoming 
//      connection.
//PARAMS:
//      nConnectionBacklog  Maximum length of the queue of pending connections. 
//                          If set to SOMAXCONN, the underlying service provider 
//                          will set the backlog to a maximum reasonable value.
//RETURN:
//      If no error occurs, returns zero. Otherwise, a value of SOCKET_ERROR 
//      is returned, and a specific error code can be retrieved by calling 
//      WSAGetLastError.
// ***************************************************************************
bool CWinSock2Async::Listen( int nConnectionBacklog )
{
    TRACE( _T("CWinSock2Async::Listen( %d )\n"), nConnectionBacklog );
    return( listen( m_Sd, nConnectionBacklog ) == 0 );
}


// ***************************************************************************
//DESCRIPTION:
//		Gracefully shuts the connection. 
//RETURN:
//		true if we're successful, false otherwise.
// ***************************************************************************
bool CWinSock2Async::ShutdownConnection()
{
	TRACE( _T("CWinSock2Async::ShutdownConnection()\n") );
	// Disallow any further data sends.  This will tell the other side
	// that we want to go away now.  If we skip this step, we don't
	// shut the connection down nicely.
	if( shutdown( m_Sd, SD_SEND ) == SOCKET_ERROR ) 
	{
		m_Sd = INVALID_SOCKET;
		return false;
	}

	// Receive any extra data still sitting on the socket.  After all
	// data is received, this call will block until the remote host
	// acknowledges the TCP control packet sent by the shutdown above.
	// Then we'll get a 0 back from recv, signalling that the remote
	// host has closed its side of the connection.
	char chBuff[WINSOCK_READ_BUFF_SIZE+1];
	int nNewBytes;
	do
	{
		nNewBytes = Recieve( chBuff, WINSOCK_READ_BUFF_SIZE );
		TRACE( _T("FYI, received %d unexpected bytes during shutdown.\n"), nNewBytes );
	} while( nNewBytes > 0 );

	// Close the socket if not SOCKET_ERROR
	if( /*nNewBytes == 0 &&*/ closesocket( m_Sd ) != SOCKET_ERROR )
	{
		m_Sd = INVALID_SOCKET;
		return true;
	}

	//Socket already dead
	m_Sd = INVALID_SOCKET;
	return false;

}


// ***************************************************************************
//DESCRIPTION:
//		Create the event annd event handler for the socket. This must only
//		be done once. This should only be called once.
//PARAMS:
//		New socket handle for this connection.
//RETURN:
//		True if events were sucessfully established.
// ***************************************************************************
bool CWinSock2Async::SetupEvents( SOCKET newSocket )
{
	TRACE( _T("CWinSock2Async::SetupEvents()\n") );

	//Assign the new socket handle
	ASSERT( m_Sd == INVALID_SOCKET );
	m_Sd = newSocket;

	//Create the event
	ASSERT( m_WSAEvent == WSA_INVALID_EVENT );
	m_WSAEvent = WSACreateEvent();
	if( m_WSAEvent == WSA_INVALID_EVENT )
	{
		closesocket( m_Sd );
		m_Sd = INVALID_SOCKET;
		return false;
	}

	//Attach the event to the socket
	int nError = WSAEventSelect( m_Sd, m_WSAEvent, 
		FD_ACCEPT | FD_CONNECT| FD_READ | FD_WRITE | FD_CLOSE );
	if( nError )
	{
		WSACloseEvent( m_WSAEvent );
		m_WSAEvent = WSA_INVALID_EVENT;

		closesocket( m_Sd );
		m_Sd = INVALID_SOCKET;

		return false;
	}

	//Run the monitoting thread.
	DWORD dwThreadId;
	HANDLE h;
	h = CreateThread(
			NULL,									// SD
			102400,									// initial stack size
			StartPlayingThread,						// thread function
			this,									// thread argument
			0,										// creation option
			&dwThreadId );							// thread identifier
	CloseHandle(h);
	return true;
}


// ***************************************************************************
//DESCRIPTION:
//		Gets the address of the peer socket to which the socket is connected
//PARAMS:
//		See help on getpeername
//RETURN:
//		See help on getpeername
// ***************************************************************************
BOOL CWinSock2Async::GetPeerName( LPTSTR sAddress, int* const pnPort )
{
	TRACE( _T("CWinSock2Async::GetPeerName()\n") );
	ASSERT( sAddress );
	ASSERT( pnPort );

	SOCKADDR SockAddr;
	int nSockAddrLen = sizeof( SockAddr );
	int nError = getpeername( m_Sd, &SockAddr, &nSockAddrLen );
	if( nError == 0 )
	{
		LPSOCKADDR_IN psi = (LPSOCKADDR_IN)&SockAddr;
		*pnPort = psi->sin_port;
		swprintf( sAddress,  _T("%d.%d.%d.%d"), 
				psi->sin_addr.S_un.S_un_b.s_b1, 
				psi->sin_addr.S_un.S_un_b.s_b2,
				psi->sin_addr.S_un.S_un_b.s_b3,
				psi->sin_addr.S_un.S_un_b.s_b4 );

	}
	return nError;
}


// **************************************************************************
//DESCRIPTION:
//		Resolve the IP address
//PARAMS:
//		pcHost Name to resolve
//RETURN:
//		IP address in reveres order 10.1.1.23 = 0x1701010A
// ***************************************************************************
in_addr CWinSock2Async::LookupAddress( LPCTSTR szHost )
{
	LPCTSTR term;

	TRACE( _T("CWinSock2Async::LookupAddress(%s) const\n"), szHost );
	in_addr nRemoteAddr;
	
	DWORD result = RtlIpv4StringToAddress(szHost, TRUE, &term, &nRemoteAddr);
	if (result == STATUS_INVALID_PARAMETER) {
		TRACE(_T("CWinSock2Async::LookupAddress failed! Wrong format! Use dotted IPv4 address."));
		return{ 0, 0, 0, 0 };
	}
    return nRemoteAddr;

}


// ***************************************************************************
//DESCRIPTION:
//		The functions reads the WSAGetLastError error code and then
//		tries to match it with a pre defined code and return a copy of that
//		string. The string recieving the error must be bigger than 50 chars.
//PARAMS:
//		sError	String to write error to. ( alteast WSA_ERROR_LEN)
// ***************************************************************************
void CWinSock2Async::WSAGetLastErrorMessage( LPTSTR sError )
{
	//TRACE( _T("CWinSock2Async::WSAGetLastErrorMessage()\n") );

	ASSERT( sError );
	int nLastError = WSAGetLastError();
	int i;
	for( i = 0; i < NUM_WSERROR_MESSAGES; ++i ) 
	{
		if (g_aErrorList[i].nID == nLastError) 
		{
			_tcsncpy( sError,  g_aErrorList[i].pcMessage, WSA_ERROR_LEN );
			sError[WSA_ERROR_LEN-1] = NULL;
			return;
		}
	}
	// Didn't find error in list, so make up a generic one
	_tcsncpy( sError, _T("unknown error"), WSA_ERROR_LEN );
	sError[WSA_ERROR_LEN-1] = NULL;
}



// ***************************************************************************
//DESCRIPTION:
//		Close should shut down everything about the socket connection
//		This will include the worker thread and events
// ***************************************************************************
void CWinSock2Async::Close()
{
	//Wait for worker thread to terminate (3 seconds max)
	if( m_ThreadState == eThreadRunning )
	{
		m_ThreadState = eThreadShuttingDown;
		WSASetEvent( m_WSAEvent );
		int nTrys = 3000;
		while( ( m_ThreadState != eThreadShutdown ) && ( nTrys-- ) )
		{
			Sleep( 1 );
		}
		nTrys++;
	}

	//Free event handler
	if( m_WSAEvent != WSA_INVALID_EVENT )
		WSACloseEvent( m_WSAEvent );

	//Close the conneciton
	ShutdownConnection();

}



// ***************************************************************************
//DESCRIPTION:
//		Called to determine how much data is waiting in the input que for
//		reading with Recieve.
//RETURN:
//		As above.
// ***************************************************************************
DWORD CWinSock2Async::RecievePendingSize()
{
	TRACE( _T("CWinSock2Async::RecievePendingSize()\n") );
	EnterCriticalSection( &m_csRecieve );
	//Determine the amount of data available to read
	DWORD dwRecBufSize = 0;
	if( ioctlsocket( m_Sd, FIONREAD, &dwRecBufSize ) )
		dwRecBufSize = 0;
	LeaveCriticalSection( &m_csRecieve );
	return dwRecBufSize;
}



// ***************************************************************************
//DESCRIPTION:
//		Read a block of data from the input stream. This is a thread safe 
//		call. Call RecievePendingSize() before calling this to determine if any
//		data exists to read.
//PARAMS:
//		pchBuff		Pointer to a char buffer to write data into.
//		nBuffLen	Maximum number of bytes to recieve
//		nFlags		See help on recv
//RETURN:
//		If no error occurs, returns the number of bytes received. 
//		If the connection has been gracefully closed, the return value is zero. 
//		Otherwise, a value of SOCKET_ERROR is returned, and a specific error 
//		code can be retrieved by calling WSAGetLastError.
// ***************************************************************************
int CWinSock2Async::Recieve( char* pchBuff, int nBuffLen, int nFlags )
{
//	TRACE( _T("CWinSock2Async::Recieve(%p, %d, %d )\n"),  pchBuff, nBuffLen, nFlags );
	ASSERT( pchBuff );
	EnterCriticalSection( &m_csRecieve );
	int nNoRead = recv( m_Sd, pchBuff, nBuffLen, nFlags );
	LeaveCriticalSection( &m_csRecieve );
	return nNoRead;
}


// ***************************************************************************
//DESCRIPTION:
//		Used to write outgoing data on a connected socket. For 
//		message-oriented sockets, care must be taken not to exceed the maximum 
//		packet size of the underlying provider, which can be obtained by 
//		using getsockopt to retrieve the value of socket option 
//		SO_MAX_MSG_SIZE.
//WARNING:
//		The successful completion of a send does not indicate that the data 
//		was successfully delivered. 
//PARAMS:
//		pchBuff		Pointer to a char buffer to send data from.
//		nLen		Maximum number of bytes to send from the buffer.
//		nFlags		See help on send
//RETURN:
//		If no error occurs, returns the total number of bytes sent, which can 
//		be less than the number indicated by nLen for nonblocking sockets. 
//		Otherwise, a value of SOCKET_ERROR is returned, and a specific error 
//		code can be retrieved by calling WSAGetLastError.
// ***************************************************************************
int CWinSock2Async::Send( char* pchBuff, int nLen, int nFlags )
{
//	TRACE( _T("CWinSock2Async::Send( %p, %d, %d )\n"), pchBuff, nLen, nFlags );
	ASSERT( nLen > 0 );
	EnterCriticalSection( &m_csSend );
	int nNoSent = send( m_Sd, pchBuff, nLen, nFlags );
	LeaveCriticalSection( &m_csSend );
	return nNoSent;
}


// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
//		Operatons below this line are ONLY called from the worker thread.
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************


// ***************************************************************************
//DESCRIPTION:
//		Recieved data event.
//		For FD_READ network event, network event recording and event object 
//		signaling are level-triggered. This means that if Recieve() routine
//		is called and data is still in the input buffer after the call, the 
//		FD_READ event is recorded and FD_READ event object is set. This allows 
//		an application to be event-driven and not be concerned with the amount 
//		of data that arrives at any one time
//		With these semantics, an application need not read all available data 
//PARAMS:
//		nError	Error code
// ***************************************************************************
void CWinSock2Async::OnRecieve( int nError )
{
	TRACE( _T("CWinSock2Async::OnRecieve(%d)\n"), nError );
}




// ***************************************************************************
//DESCRIPTION:
//		An FD_WRITE network event is recorded when a socket is first connected 
//		with connect/WSAConnect or accepted with accept/WSAAccept, and then 
//		after a send fails with WSAEWOULDBLOCK and buffer space becomes 
//		available. Therefore, an application can assume that sends are 
//		possible starting from the first FD_WRITE network event setting and 
//		lasting until a send returns WSAEWOULDBLOCK. After such a failure the 
//		application will find out that sends are again possible when OnSend 
//		is fired.
//PARAMS:
//		nError	Error code
// ***************************************************************************
void CWinSock2Async::OnSend( int nError )
{
	TRACE( _T("CWinSock2Async::OnSend(%d)\n"), nError );
}



// ***************************************************************************
//DESCRIPTION:
//		Triggered when a connection failes a new connection is sucessful
//		that was blocked when  the connection was requested. If an error is
//		returned then it may be necessary to delete this object in the same
//		manner as when a connection  is closed from the other end.
//PARAMS:
//		nError	 Connection error code.
// ***************************************************************************
void CWinSock2Async::OnConnect( int nError )
{
	TRACE( _T("CWinSock2Async::OnConnect(%d)\n"), nError );
	m_bConnected = ( nError == 0 );

}


// ***************************************************************************
//DESCRIPTION:
//		Called by the framework to notify a listening socket that it can 
//		accept pending connection requests by calling the Accept member 
//		function.
//PARAMS:
//		The most recent error on a socket. The following error codes 
//		applies to the OnAccept member function: 
//			0			The function executed successfully.
//			WSAENETDOWN The Windows Sockets implementation detected 
//						that the network subsystem failed. 
// ***************************************************************************
void CWinSock2Async::OnAccept( int nError )
{
	TRACE( _T("CWinSock2Async::OnAccept(%d)\n"), nError );

	//Typical connection example (note this leaks)
//	CWinSock2Async *pSockNew = new CWinSock2Async();
//	if( Accept( pSockNew ) )
//	{
//		//TODO: Some error display
//	}
	
}


// ***************************************************************************
//DESCRIPTION:
//		Permits an incoming connection attempt on a socket. It is usually
//		called in connection with a OnAccept event.
//PARAMS:
//		pSockNew	New port to create connection to client on.
//		lpSockAddr	NULL or structure to copy connection data into.
//		pnLen		NULL or return size of the above structure.
//RETURN:
//		Nonzero if the function is successful; otherwise 0, and a specific 
//		error code can be retrieved by calling GetLastError.
// ***************************************************************************
bool CWinSock2Async::Accept( CWinSock2Async *pSockNew, sockaddr* lpSockAddr, int* pnLen )
{
	TRACE( _T("CWinSock2Async::Accept( %p, %p, %p )\n"), pSockNew, lpSockAddr, pnLen );
	SOCKET sockNew = accept( m_Sd, lpSockAddr, pnLen );

	if( sockNew == INVALID_SOCKET )
		return false;

	return pSockNew->SetupEvents( sockNew );


}


// ***************************************************************************
//DESCRIPTION:
//		The FD_CLOSE network event is recorded when a close indication is 
//		received for the virtual circuit corresponding to the socket. In TCP 
//		terms, this means that the FD_CLOSE is recorded when the connection 
//		goes into the TIME WAIT or CLOSE WAIT states. This results from the 
//		remote end performing a shutdown on the send side or a closesocket. 
//		FD_CLOSE being posted after all data is read from a socket. An 
//		application should check for remaining data upon receipt of FD_CLOSE 
//		to avoid any possibility of losing data.
//PARAMS:
//		nError	Error code
// ***************************************************************************
void CWinSock2Async::OnClose( int nError )
{
	TRACE( _T("CWinSock2Async::OnClose(%d)\n"), nError );
	m_bConnected = false;

	//Check for any remaining data
	char chBuff[WINSOCK_READ_BUFF_SIZE+1];
	int nRead;
	while( ( nRead = Recieve( chBuff, WINSOCK_READ_BUFF_SIZE ) ) > 0 )
	{
		//TODO : Process the read data.
	}
	
}



// ***************************************************************************
//DESCRIPTION:
//		This is the section of code that is processed by the thread.
//		It MUST NOT be called from any location other than the thread.
//		It will run until the socket is closed.
// ***************************************************************************
void CWinSock2Async::ThreadRunner()
{
	TRACE( _T("CWinSock2Async::ThreadRunner()\n") );
	m_ThreadState = eThreadRunning;
	while( m_ThreadState == eThreadRunning )
	{
        // Wait for an event (or a queued callback function) to wake 
        // us up.  This is an alertable wait state (fAlertable == TRUE). 
        WSAWaitForMultipleEvents(
				1, 
				&m_WSAEvent, 
				FALSE,        
				WSA_INFINITE, 
				TRUE );       

		WSANETWORKEVENTS NetEvents = { 0 };		//Event that occured
   
		// Find out what happened and act accordingly. 
		if( WSAEnumNetworkEvents( m_Sd, m_WSAEvent, &NetEvents) == SOCKET_ERROR ) 
		{ 
			TCHAR szWASError[WSA_ERROR_LEN];
			WSAGetLastErrorMessage( szWASError );
			TRACE( _T("*** ERROR : In Event Enum.: %s\n"), szWASError );
			//TODO : Do something with this error type

		} 
		else 
		{
			//Process each bit in the input event
			if( NetEvents.lNetworkEvents & FD_ACCEPT )
				OnAccept( NetEvents.iErrorCode[FD_ACCEPT_BIT] );

			if( NetEvents.lNetworkEvents & FD_CONNECT )
				OnConnect( NetEvents.iErrorCode[FD_CONNECT_BIT] );

			if( NetEvents.lNetworkEvents & FD_READ )
				OnRecieve( NetEvents.iErrorCode[FD_READ_BIT] );

			if( NetEvents.lNetworkEvents & FD_WRITE )
				OnSend( NetEvents.iErrorCode[FD_WRITE_BIT] );

			if( NetEvents.lNetworkEvents & FD_CLOSE )
				OnClose( NetEvents.iErrorCode[FD_CLOSE_BIT] );

		}
	}
	m_ThreadState = eThreadShutdown;
}



// ***************************************************************************
//DESCRIPTION:
//		Function to launch the class Run method. It is in a seperate
//		thread but the same address space.
//PARAMS:
//		param	Pointer to CWinSock2Async object to run.
//RETURN:
//		Not used.
// ***************************************************************************
DWORD WINAPI StartPlayingThread(LPVOID param)
{
    TRACE( _T("StartPlayingThread(%p)\n"), param );
    CWinSock2Async* pWinSock = (CWinSock2Async*)param;
    ASSERT( pWinSock );
	pWinSock->ThreadRunner();
    return 392;
}
