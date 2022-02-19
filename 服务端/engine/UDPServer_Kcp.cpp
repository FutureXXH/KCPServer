#include "UDPServer.h"
#include "KcpInterface.h"


int kcpOutPut_server(const char* buf, int size, ikcpcb* kcp, void* user)
{
	UDP_BASE* c = (UDP_BASE*)user;
	int sendBytes = sendto(c->socketfd, buf, size, 0, (sockaddr*)&c->addr, sizeof(sockaddr_in));
	if (sendBytes >= 0)
	{
		SERVERPRINT_INFO << "kcp发送" << c->ID << " " << sendBytes << endl;
		return sendBytes;
	}
	else
	{
		SERVERPRINT_ERROR << "发送错误  " << sendBytes << endl;
		return -1;
	}



}

int UDPServer::recvData_kcp(char* buf, s32 recvBytes, s32 ip, u16 port)
{

	if (recvBytes < 24)return recvBytes;
	int id = ikcp_getid(buf);
	
	auto c = findClient(id, ip, port);
	if (c == NULL)
	{
		SERVERPRINT_WARNING << "查找客户端对象失败:" << id << endl;
		return recvBytes;
	}

	int ret = ikcp_input(c->kcp, buf, recvBytes);
	if (recvBytes <= 24 || ret < 0)return recvBytes;
	while (true)
	{
		char buf_temp[MAX_UDP_BUF];
		memset(buf_temp, 0, MAX_UDP_BUF);
		//kcp 通过此函数将数据接收出来，如果被分片，将在此自动重组，可能有多条包所有要循环
		ret = ikcp_recv(c->kcp, buf_temp, MAX_UDP_BUF);
		if (ret >= 6)onRecv_SaveData(buf_temp, c, ret);
		else if (ret < 0)break;
	}


	return recvBytes;
}



void UDPServer::createKcp(UDP_BASE* c)
{
	releaseKcp(c);

	ikcpcb* kcp= ikcp_create(c->ID, c);
	kcp->output = kcpOutPut_server;
	kcp->rx_minrto = 10;
	ikcp_nodelay(kcp, 1, 10, 2, 1);
	ikcp_wndsize(kcp, 128, 128);
	c->kcp = kcp;

}

void UDPServer::releaseKcp(UDP_BASE* c)
{
	if (c->kcp == nullptr)return;
	ikcp_release(c->kcp);
	c->kcp = nullptr;


}

void UDPServer::updateKcp(UDP_BASE* c)
{
	if (c->kcp == nullptr)return;
	ikcp_update(c->kcp, iclock());


}

s32 nextUpdateFrame = 0;