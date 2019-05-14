#pragma once
#include "..\NetCom\sockclient.h"
#include "Properties.h"

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
	
	void init(Properties* p); // Initialize remote with values from Properties class
	void connect();
	void connect(Connection c);
	void disconnect(Connection c);

	Connection getPendingConnections();
	Connection getActiveConnections();

private:
	CString mode;
	CString AOSACA_IP;
	CString ICANDI_IP;

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

