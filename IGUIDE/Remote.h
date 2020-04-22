#pragma once
#include "..\NetCom\sockclient.h"
#include "Properties.h"
#include "IGUIDEDoc.h"

enum Connection {
	NONE = 0,
	AOSACA = 1,
	ICANDI = 2,
	BOTH = 3
};

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

	bool Connect2AOSACA();
	bool Connect2ICANDI();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT Remote::ConnectionFailure(WPARAM w, LPARAM l);
	afx_msg LRESULT Remote::ConnectionClosed(WPARAM w, LPARAM l);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

