#include "SockClient.h"
#include "WinSock2Async.h"

class CSockListener : public CWinSock2Async
{
public:
	CSockListener(CString*, HANDLE*);
	~CSockListener();

	CString* inpBuf;
	HANDLE* netEvent;

private:
	void OnAccept(int nError);

	//Attributes
private:
	CSockClient* m_psockClient;    //Client connection

};

