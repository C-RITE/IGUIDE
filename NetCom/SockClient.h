// SockClient.h: interface for the CSockClient class.
//
//////////////////////////////////////////////////////////////////////
#include "WinSock2Async.h"

#if !defined(AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_)
#define AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPupilTrackerMainFrame;

class CSockClient : public virtual CWinSock2Async  
{
public:
	CSockClient(CPupilTrackerMainFrame* pParent);
	virtual ~CSockClient();

	bool m_bConnectionClosed;

private:

	void OnRecieve( int nError );
	void OnSend( int nError );
	void OnClose( int nError );

	CPupilTrackerMainFrame* m_pParent;

};

#endif // !defined(AFX_SOCKCLIENT_H__F435A5D7_7556_42D5_83CC_D820BC575D99__INCLUDED_)
