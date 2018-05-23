#include "SockClient.h"
#include "WinSock2Async.h"

class CSockListener :
	public CWinSock2Async
{
public:
	CSockListener();
	~CSockListener();

private:
	void OnAccept(int nError);

	//Attributes
private:
	CSockClient* m_psockClient;    //Client connection

};

