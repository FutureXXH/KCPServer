#include "UDPServer.h"



void UDPServer::runThread()
{
	m_workthread.reset(new std::thread(UDPServer::run, this));
	m_workthread->detach();
}




void UDPServer::run(UDPServer* udp)
{
	cout << "Run WorkThread...." << endl;
	while (true)
	{
		int err = udp->recvData();
		if (err) continue;

		cout << "Wait 1s" << endl;
		UDPSleep(1);
	}

	cout << "Exit WorkThread" << endl;
}