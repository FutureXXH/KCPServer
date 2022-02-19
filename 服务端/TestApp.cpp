#include "TestApp.h"

TestApp* __TestApp = nullptr;
bool TestApp::onUDPServerCommand(IUDPServer* ts, UDP_BASE* c, const u16 cmd)
{
	
	if (cmd == 666)
	{
		int len = c->recv_TempTail - c->recv_TempHead;
		long start = -1;
		memcpy(&start, c->recvBuf + c->recv_TempHead, 4);

		SERVERPRINT_INFO << "收到测试数据:" << start << "  " << len << endl;
		
		char sendbuff[4];
		memcpy(sendbuff, c->recvBuf + c->recv_TempHead, 4);
		ts->KcpSend(c->ID, 666, sendbuff, 4);
		return true;
	}
	else
	{
		SERVERPRINT_INFO << "数据错误" << endl;
		return false;
	}


}
