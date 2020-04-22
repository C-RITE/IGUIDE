// SockClient.cpp: implementation of the CSockClient class.
//
//////////////////////////////////////////////////////////////////////


#include "../IGUIDE/stdafx.h"
#include "SockClient.h"
#include "MainFrm.h"

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



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSockClient::CSockClient(CString* inputBuf, HANDLE* netMsgEvent)
{
	pending = false;
	inpBuf = inputBuf;
	netEvent = netMsgEvent;

}

CSockClient::~CSockClient()
{
}


// ***************************************************************************
//DESCRIPTION:
//      Recieved data event.
//      For FD_READ network event, network event recording and event object 
//      signaling are level-triggered. This means that if Recieve() routine
//      is called and data is still in the input buffer after the call, the 
//      FD_READ event is recorded and FD_READ event object is set. This allows 
//      an application to be event-driven and not be concerned with the amount 
//      of data that arrives at any one time
//      With these semantics, an application need not read all available data 
//PARAMS:
//      nError    Error code
//CREATED:
//      26-11-2000, 16:54:46 by john@mctainsh.com
// ***************************************************************************
void CSockClient::OnRecieve(int nError)
{
	TRACE(_T("CSockClient::OnRecieve( %d )\n"), nError);
	
	
	char chBuff[WINSOCK_READ_BUFF_SIZE + 1];
	int nRead;

	while ((nRead = Recieve(chBuff, WINSOCK_READ_BUFF_SIZE)) > 0)
	{
	
		chBuff[nRead] = '\0';
		inpBuf->Format(L"%S",chBuff);
		SetEvent(*netEvent);

	}
	
}

void CSockClient::OnConnect(int nError)
{
	//TRACE(_T("CWinSock2Async::OnConnect(%d)\n"), nError);
	if (nError == 0) {
		m_bConnected = true;
		pending = false;
	}

	if (nError > 0) {
		ErrorEntry err;
		for (int i = 0; i < NUM_WSERROR_MESSAGES; i++) {
			if (g_aErrorList[i].nID == nError)
				err = g_aErrorList[i];
		}

		pParent->PostMessage(NETCOM_ERROR, (WPARAM)m_IP, (LPARAM)err.pcMessage);

	}

}

// ***************************************************************************
//DESCRIPTION:
//      An FD_WRITE network event is recorded when a socket is first connected 
//      with connect/WSAConnect or accepted with accept/WSAAccept, and then 
//      after a send fails with WSAEWOULDBLOCK and buffer space becomes 
//      available. Therefore, an application can assume that sends are 
//      possible starting from the first FD_WRITE network event setting and 
//      lasting until a send returns WSAEWOULDBLOCK. After such a failure the 
//      application will find out that sends are again possible when OnSend 
//      is fired.
//PARAMS:
//      nError    Error code
//CREATED:
//      26-11-2000, 19:46:38 by john@mctainsh.com
// ***************************************************************************
void CSockClient::OnSend( int nError )
{
    _tprintf( _T("CSockClient::OnSend( %d )\n"), nError );

}



// ***************************************************************************
//DESCRIPTION:
//      The FD_CLOSE network event is recorded when a close indication is 
//      received for the virtual circuit corresponding to the socket. In TCP 
//      terms, this means that the FD_CLOSE is recorded when the connection 
//      goes into the TIME WAIT or CLOSE WAIT states. This results from the 
//      remote end performing a shutdown on the send side or a closesocket. 
//      FD_CLOSE being posted after all data is read from a socket. An 
//      application should check for remaining data upon receipt of FD_CLOSE 
//      to avoid any possibility of losing data.
//PARAMS:
//      nError    Error code
//CREATED:
//      26-11-2000, 16:23:21 by john@mctainsh.com
// ***************************************************************************
void CSockClient::OnClose( int nError )
{
    _tprintf( _T("CSockClient::OnClose( %d )\n"), nError );
    
	//Check for any remaining data
    char chBuff[WINSOCK_READ_BUFF_SIZE+1];
    while( Recieve( chBuff, WINSOCK_READ_BUFF_SIZE ) )
    {
        //TODO : Process the read data.

    }

	if (nError > 0) {
		ErrorEntry err;
		for (int i = 0; i < NUM_WSERROR_MESSAGES; i++) {
			if (g_aErrorList[i].nID == nError)
				err = g_aErrorList[i];
		}

		pParent->PostMessage(NETCOM_ERROR, (WPARAM)m_IP, (LPARAM)err.pcMessage);
	}

	pParent->PostMessage(NETCOM_CLOSED, (WPARAM)m_IP, NULL);

}

