// SockClient.h: interface for the CSockClient class.
//
//////////////////////////////////////////////////////////////////////
#include "WinSock2Async.h"

#if !defined(AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_)
#define AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CSockClient : public virtual CWinSock2Async  
{
public:
	CSockClient(CString* inputBuf, HANDLE* netMsgEvent);
	virtual ~CSockClient();

	bool pending;
	void setParent(CWnd* wnd) { pParent = wnd; }

private:

	void OnRecieve( int nError );
	void OnSend( int nError );
	void OnClose( int nError );
	void OnConnect(int nError);

	CString* inpBuf;
	HANDLE* netEvent;
	CWnd* pParent;

};

#endif // !defined(AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_)
