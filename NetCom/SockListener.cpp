#include "stdafx.h"
#include "SockListener.h"

// ***************************************************************************
//DESCRIPTION:
//      Called by the framework to notify a listening socket that it can 
//      accept pending connection requests by calling the Accept member 
//      function.
//PARAMS:
//      The most recent error on a socket. The following error codes 
//      applies to the OnAccept member function: 
//       0            The function executed successfully.
//       WSAENETDOWN  The Windows Sockets implementation detected 
//                    that the network subsystem failed. 
//CREATED:
//      2-12-2000, 7:10:28 AM by john@mctainsh.com
// ***************************************************************************

CSockListener::CSockListener(CString* inputBuf, HANDLE* netMsgEvent)
{
	m_psockClient = NULL;
	inpBuf = inputBuf;
	netEvent = netMsgEvent;

}

CSockListener::~CSockListener()
{
	delete m_psockClient;
}

void CSockListener::OnAccept(int nError)
{
	TRACE(_T("CSockListener::OnAccept(%d)\n"), nError);
	//Close of the old connection and create a new one

	if (m_psockClient)
		delete m_psockClient;
		inpBuf->Empty();
	Sleep(1);

	//Create the new connection
	m_psockClient = new CSockClient(inpBuf, netEvent);

	if (Accept(m_psockClient) == 0)
	{
		TCHAR szErrMessage[WSA_ERROR_LEN + 1];
		WSAGetLastErrorMessage(szErrMessage);
		TRACE(_T("ERROR on Accept %s\n"), szErrMessage);
		return;
	}

	//Display connection details
	TCHAR szAddress[17];
	int nPort;
	if (m_psockClient->GetPeerName(szAddress, &nPort) == 0)
		TRACE(_T("Connected to %s on port %d\n"), szAddress, nPort);

}

