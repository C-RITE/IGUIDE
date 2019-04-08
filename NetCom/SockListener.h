#include "SockClient.h"
#include "WinSock2Async.h"

class CSockListener : public CWinSock2Async
{
public:
	CSockListener();
	~CSockListener();

	void SetParent(CPupilTrackerMainFrame* pParent);
	bool isConnectionEstablished();
private:
	void OnAccept(int nError);
	bool m_bConnectionEstablished;

	//Attributes
private:
	CSockClient* m_psockClient;    //Client connection
	CPupilTrackerMainFrame* m_pParent;

};

