#include "KCPClient.h"
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
int getError()
{
#ifdef ___WIN32__
	return WSAGetLastError();
#else
	return errno;
#endif
}

void setNonblockingSocket(bool isNon, UDPSOCKET socketfd)
{
	if (isNon == false)return;
	unsigned long u1 = 1;
	ioctlsocket(socketfd, FIONBIO, (unsigned long*)&u1);

}













bool KCPClient::Init(const char* ip, int port)
{
	WSADATA wsData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error != 0)
	{
		perror("WSAStartup error");
		exit(1);
	}

	socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketfd < 0)
	{
		perror("socketfd error");
		exit(1);
	}

	setNonblockingSocket(false, socketfd);


	addrserver.sin_family = AF_INET;
	addrserver.sin_addr.S_un.S_addr = inet_addr(ip);
	addrserver.sin_port = htons(8888);

	// 忽略10054错误
	BOOL b = FALSE;
	DWORD dw = 0;
	WSAIoctl(socketfd, SIO_UDP_CONNRESET, &b, sizeof(b), NULL, 0, &dw, NULL, NULL);



	SERVERPRINT_INFO << "UDP init success" << endl;

	return false;
}

bool KCPClient::connectServer()
{
	SERVERPRINT_INFO << "正在连接服务器"  << endl;
	SendServer(65001, nullptr, 0,false);
	return false;
}

int KCPClient::SendServer(u16 cmd, char* buff, int size,bool haveHead)
{
	char sendbuff[512] = {0};
	int SendId = ID ^ RCode;
	cmd ^= RCode;
	if (haveHead)
	{
		memcpy(sendbuff, Head, 3);
		memcpy(sendbuff + 3, (char*)&size, 2);
		memcpy(sendbuff + 5, (char*)&SendId, 4);
		memcpy(sendbuff + 9, (char*)&cmd, 2);
		memcpy(sendbuff + 11, buff, size);

		sendto(socketfd, sendbuff, size + 11, 0, (sockaddr*)&addrserver, sizeof(sockaddr_in));
		return 0;
	}
	else
	{

		memcpy(sendbuff , (char*)&SendId, 4);
		memcpy(sendbuff + 4, (char*)&cmd, 2);
		memcpy(sendbuff + 6, buff, size);

		sendto(socketfd, sendbuff, size + 6, 0, (sockaddr*)&addrserver, sizeof(sockaddr_in));
		return 0;
	}




}

int KCPClient::RecvServer()
{
	char buf[MAX_UDP_BUF];
	memset(buf, 0, MAX_UDP_BUF);
	sockaddr_in RecvAddr;
	int addrlen = sizeof(RecvAddr);
	int recvByte = recvfrom(socketfd, buf, 512, 0, (sockaddr*)&RecvAddr, &addrlen);
	if (recvByte < 6)
	{  
		if (getError() == 0)return -1;
		
		SERVERPRINT_ERROR << "Recv Data Error  " << recvByte << "  " << getError() << endl;

		return-1;
	}
	string ipstr = inet_ntoa(RecvAddr.sin_addr);
	u32 ip = RecvAddr.sin_addr.S_un.S_addr;
	int port = ntohs(RecvAddr.sin_port);
	s32 recvID;
	u16 cmd;
	memcpy(&recvID, buf, 4);
	memcpy(&cmd, buf + 4, 2);
	recvID ^= RCode;
	cmd ^= RCode;
	
	
	switch (cmd)
	{
	case 65001://请求连接
	{
		onAccept(buf, recvID, cmd, RecvAddr);
		return recvByte;
	}

	default:
		break;
	}
	if (recvID != ID)
	{
		//cout << "Recv KCP Data" << recvByte << "   " << ipstr.c_str() << ":" << port << endl;
		recvData_kcp(buf, recvByte);
		return recvByte;
	}
	else 
	{
		//cout << "Recv UDP Data" << recvByte << "   " << ipstr.c_str() << ":" << port << endl;
		onRecv_SaveData(buf, recvByte);
		return recvByte;
	}
	return -1;
}

void KCPClient::onRecv_SaveData(char* buf, int recvBytes)
{


	if (recv_Head == recv_Tail)
	{
		recv_Head = 0;
		recv_Tail = 0;
	}
	if (recv_Tail + recvBytes >= MAX_UDP_BUF)
	{
		SERVERPRINT_INFO << "缓冲区已满" << endl;
		return;
	}

	memcpy(RecvBuff+recv_Tail, buf, recvBytes);
	recv_Tail += recvBytes;
	is_RecvCompleted = true;



}

int KCPClient::recvData_kcp(char* buf, s32 recvBytes)
{

	if (recvBytes < 24)return recvBytes;
	int id = ikcp_getid(buf);

	if (id != ID)
	{

		return recvBytes;
	}

	int ret = ikcp_input(kcp, buf, recvBytes);
	if (recvBytes <= 24 || ret < 0)return recvBytes;
	while (true)
	{
		char buf_temp[MAX_UDP_BUF];
		memset(buf_temp, 0, MAX_UDP_BUF);
		//kcp 通过此函数将数据接收出来，如果被分片，将在此自动重组，可能有多条包所有要循环
		ret = ikcp_recv(kcp, buf_temp, MAX_UDP_BUF);
		if (ret >= 6)onRecv_SaveData(buf_temp, ret);
		else if (ret < 0)break;
	}


	return recvBytes;
}

bool KCPClient::StartThread()
{
	threadp = new thread(KCPClient::ThreadRun, this);

	return true;
}

int kcpOutPut_server(const char* buf, int size, ikcpcb* kcp, void* user)
{
	KCPClient* S = (KCPClient*)user;
	int sendBytes = sendto(S->socketfd, buf, size, 0, (sockaddr*)&S->addrserver, sizeof(sockaddr_in));
	if (sendBytes >= 0)
	{
		//SERVERPRINT_INFO << "kcp发送" << S->ID << " " << sendBytes << endl;
		return sendBytes;
	}
	else
	{
		SERVERPRINT_ERROR << "发送错误  " << sendBytes << endl;
		return -1;
	}
}

void KCPClient::ThreadRun(KCPClient* kcp)
{
	SERVERPRINT_INFO << "开启线程 " << endl;
	while (true)
	{
		int err = kcp->RecvServer();
		if (err) continue;

	}
	SERVERPRINT_INFO << "线程退出" << endl;
}

int KCPClient::onAccept(char* buff, s32 RecvID, u16 cmd, sockaddr_in clientAddr)
{
	if (RecvID == ID && state == Connect)return -1;
	SERVERPRINT_INFO << "连接成功 " << RecvID << endl;
	ID = RecvID;
	state = Connect;

	 kcp = ikcp_create(ID, this);
	kcp->output = kcpOutPut_server;
	kcp->rx_minrto = 10;
	ikcp_nodelay(kcp, 1, 10, 2, 1);
	ikcp_wndsize(kcp, 32, 32);
	return 0;
}

int KCPClient::KcpSend(u16 cmd, char* buff, int size)
{
	char sendbuff[512] = { 0 };
	s32 sendid = ID ^ RCode;
	cmd ^= RCode;
	memcpy(sendbuff, Head, 3);
	memcpy(sendbuff + 3, (char*)&size, 2);
	memcpy(sendbuff + 5, (char*)&sendid, 4);
	memcpy(sendbuff + 9, (char*)&cmd, 2);
	memcpy(sendbuff + 11, buff, size);

	ikcp_send(kcp, sendbuff, size + 11);
	return 0;

}

void KCPClient::parseCommand()
{
	updateKcp();
	if (!is_RecvCompleted)return;
	//SERVERPRINT_INFO << "正在解析消息" << endl;

	while (recv_Tail -recv_Head >= 11)
	{
		if (RecvBuff[recv_Head] != Head[0] || RecvBuff[recv_Head + 1] != Head[1] || RecvBuff[recv_Head + 2] != Head[2])
		{
			//SERVERPRINT_WARNING << "解析头错误" << endl;
			recv_Head++;
			return;
		}
		u16 len;
		memcpy(&len, RecvBuff + 3 + recv_Head, 2);

		DATA_HEAD data;
		memcpy(&data.ID, RecvBuff +recv_Head + 5, 4);
		memcpy(&data.cmd, RecvBuff + recv_Head + 9, 2);
		data.getSecure(RCode);


		if (recv_Tail < recv_Head + len)
		{
			SERVERPRINT_WARNING << "解析长度错误" << endl;
			return;
		}

		recv_TempHead = recv_Head + 11;
		recv_TempTail = recv_Head + 11+len;
		parseCommand(data.cmd);
		recv_Head += len;

	}
	is_RecvCompleted = false;
}

void KCPClient::parseCommand(const u16 cmd)
{
	switch (cmd)
	{
	case 666:
	{
		int len = recv_TempTail - recv_TempHead;
		long start = -1;
		memcpy(&start, RecvBuff + recv_TempHead, 4);

		clock_t end = clock();
		SERVERPRINT_INFO << "收到测试数据  延迟:" << (double)(end - start) / CLOCKS_PER_SEC << "  " << len << endl;

	}
	break;
	default:
		break;
	}
}

void KCPClient::updateKcp()
{
	if (kcp == nullptr)return;
	ikcp_update(kcp, iclock());

	
}


