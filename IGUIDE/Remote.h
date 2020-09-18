#pragma once
#include "..\NetCom\sockclient.h"
#include "Properties.h"
#include "IGUIDEDoc.h"

class Remote : public CWnd
{
public:
	Remote();
	~Remote();
	
	void init(Properties* p, CString* inputBuf, HANDLE* netMsgEvent); // Initialize remote with values from Properties class
	void connect();
	void connect(Connection c);
	void setIPAdress(Connection c);

	Connection getPendingConnections();
	Connection getActiveConnections();

	int onHold;						// true if ICANDI is still recording

private:
	CString mode;
	CString AOSACA_IP;
	CString ICANDI_IP;

	CString* AOSACA_inpBuf;
	CString* ICANDI_inpBuf;
	HANDLE* AOSACA_netEvt;
	HANDLE*	ICANDI_netEvt;

	Properties* m_pProperties;

	CSockClient* m_pSock_AOSACA;
	CSockClient* m_pSock_ICANDI;

	void Connect2AOSACA();
	void Connect2ICANDI();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT Remote::ConnectionFailure(WPARAM w, LPARAM l);
	afx_msg LRESULT Remote::ConnectionClosed(WPARAM w, LPARAM l);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

