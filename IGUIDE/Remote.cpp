#include "stdafx.h"
#include "Remote.h"

BEGIN_MESSAGE_MAP(Remote, CWnd)
	ON_MESSAGE(NETCOM_ERROR, ConnectionFailure)
	ON_MESSAGE(NETCOM_CLOSED, ConnectionClosed)
	ON_MESSAGE(NETCOM_RECEIVED, DataReceived)
END_MESSAGE_MAP()

Remote::Remote()
{
	m_pSock_AOSACA = NULL;
	m_pSock_ICANDI = NULL;
	OnCreate(NULL);

}

Remote::~Remote()
{
	delete m_pSock_ICANDI;
	delete m_pSock_AOSACA;
}

// Initialize remote with values from Properties class
void Remote::init(Properties* p)
{
	mode = p->getRemoteCapability();
	AOSACA_IP = p->getAOSACA_IP();
	ICANDI_IP = p->getICANDI_IP();

	// TODO: Add your implementation code here.
}


void Remote::connect()
{
	// by default, connect to what is selected in properties

	if (mode == L"AOSACA" | mode == L"FULL")
		Connect2AOSACA();
	if (mode == L"ICANDI" | mode == L"FULL")
		Connect2ICANDI();

}

void Remote::connect(Connection c)
{
	// manual override

	switch (c) {
	case AOSACA:
		Connect2AOSACA();
		break;
	case ICANDI:
		Connect2ICANDI();
		break;
	}

}

bool Remote::Connect2AOSACA()
{
	// TODO: Add your implementation code here.

	if (m_pSock_AOSACA == NULL) {
		m_pSock_AOSACA = new CSockClient();
		CSockClient::SocketInit();
		m_pSock_AOSACA->Create();
		m_pSock_AOSACA->pending = true;
		m_pSock_AOSACA->setParent(this);
	}

	if (!m_pSock_AOSACA->Connect(AOSACA_IP, 1500)) {
		delete m_pSock_AOSACA;
		m_pSock_AOSACA = NULL;
		Connect2AOSACA();
		return false;
	}

	return true;

}


bool Remote::Connect2ICANDI()
{
	// TODO: Add your implementation code here.

	if (m_pSock_ICANDI == NULL) {
		m_pSock_ICANDI = new CSockClient();
		CSockClient::SocketInit();
		m_pSock_ICANDI->Create();
		m_pSock_ICANDI->pending = true;
		m_pSock_ICANDI->setParent(this);
	}

	if (!m_pSock_ICANDI->Connect(ICANDI_IP, 1400)) {
		delete m_pSock_ICANDI;
		m_pSock_ICANDI = NULL;
		Connect2ICANDI();
		return false;
	}

	return true;

}

Connection Remote::getActiveConnections() {

	bool AOSACA = false;
	bool ICANDI = false;

	if (m_pSock_AOSACA != NULL)
		AOSACA = m_pSock_AOSACA->IsConnected();
	if (m_pSock_ICANDI != NULL)
		ICANDI = m_pSock_ICANDI->IsConnected();

	if (AOSACA && ICANDI)
		return Connection::BOTH;
	if (AOSACA)
		return Connection::AOSACA;
	if (ICANDI)
		return Connection::ICANDI;

	return Connection::NONE;

}

Connection Remote::getPendingConnections() {

	bool AOSACA = false;
	bool ICANDI = false;
	
	if (m_pSock_AOSACA != NULL)
		AOSACA = m_pSock_AOSACA->pending;
	if (m_pSock_ICANDI != NULL)
		ICANDI = m_pSock_ICANDI->pending;

	if (AOSACA && ICANDI)
		return Connection::BOTH;
	if (AOSACA)
		return Connection::AOSACA;
	if (ICANDI)
		return Connection::ICANDI;

	return Connection::NONE;

}


LRESULT Remote::ConnectionFailure(WPARAM w, LPARAM l) {

	CString IPAddress((LPCTSTR)w);
	CString errorDescription((LPCTSTR)l);
	CString errorMessage;

	if (IPAddress.Compare(AOSACA_IP) == 0) {
		errorMessage.Format(L"Couldn't connect to AOSACA!\nReason: " + errorDescription + ".");

		int answer = AfxMessageBox(errorMessage, MB_RETRYCANCEL);

		if (answer == IDRETRY || answer == IDCANCEL) {
			delete m_pSock_AOSACA;
			m_pSock_AOSACA = NULL;
		}

		if (answer == IDRETRY)
			Connect2AOSACA();

	}

	if (IPAddress.Compare(ICANDI_IP) == 0) {
		errorMessage.Format(L"Couldn't connect to ICANDI!\nReason: " + errorDescription + ".");

		int answer = AfxMessageBox(errorMessage, MB_RETRYCANCEL);

		if (answer == IDRETRY || answer == IDCANCEL) {
			delete m_pSock_ICANDI;
			m_pSock_ICANDI = NULL;
		}

		if (answer == IDRETRY)
			Connect2ICANDI();

	}

	return 0L;

}

LRESULT Remote::ConnectionClosed(WPARAM w, LPARAM l) {

	// clean up after connection has been closed from remote side
	CString IPAddress((LPCTSTR)w);
	
	if (IPAddress.Compare(AOSACA_IP) == 0) {

		delete m_pSock_AOSACA;
		m_pSock_AOSACA = NULL;

	}

	if (IPAddress.Compare(ICANDI_IP) == 0) {

		delete m_pSock_ICANDI;
		m_pSock_ICANDI = NULL;

	}

	return 0;

}

LRESULT Remote::DataReceived(WPARAM w, LPARAM l)
{
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	
	CString* command = (CString*)w;
	CString* value = (CString*)l;
	
	if (command->Compare(L"AOSACA_DEFOCUS") == 0)
		pDoc->setDefocus(*value);

	pDoc->UpdateAllViews(NULL);

	return 0;
}


BOOL Remote::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	int ret;

	if (pMsg->message == WM_KEYDOWN && m_pSock_ICANDI) {
		switch (pMsg->wParam) {
			char msg;

			// ICANDI remote commands go here

		case 'R':
			msg = (char)pMsg->wParam;
			ret = m_pSock_ICANDI->Send(&msg, 1, 0);
			break;
		case VK_SPACE:
			msg = 'V';
			ret = m_pSock_ICANDI->Send(&msg, 1, 0);
			break;

		}

	}

	if (pMsg->message == WM_KEYDOWN && m_pSock_AOSACA) {
		switch (pMsg->wParam) {
			char msg;

			// AOSACA remote commands go here

		case VK_RETURN:
			msg = 'F';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case 0x6B:
			msg = '+';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case 0x6D:
			msg = '-';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		case VK_NUMPAD0:
			msg = '0';
			ret = m_pSock_AOSACA->Send(&msg, 1, 0);
			break;
		}

	}

	return CWnd::PreTranslateMessage(pMsg);

}
