// SockClient.cpp: implementation of the CSockClient class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "SockClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSockClient::CSockClient()
{

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
		//TODO: Use the data here...
		//Modify the data and echo
		for (int nPos = 0; nPos < nRead; nPos++)
		{
			TRACE(L"%c ", chBuff[nPos]);
			chBuff[nPos] = chBuff[nPos] + 1;
		}
		Send(chBuff, nRead);
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
}

