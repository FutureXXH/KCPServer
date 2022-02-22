#include "UDPServer.h"


/*

*/


IUDPServer* NewUDPServer()
{
	return new UDPServer();
}



UDPServer::UDPServer()
{
	socketfd = -1;
}

UDPServer::~UDPServer()
{
}

void UDPServer::stopServer()
{

}

int UDPServer::sendData(const s32 id, void* buf, u32 size, sockaddr_in* addr)
{
	int sendBytes = 0;
	if (id == -1)
	{
		if (addr == nullptr)return -1;
		sendBytes = sendto(socketfd, (char*)buf, size,0, (sockaddr*)addr, sizeof(sockaddr_in));
	}
	else
	{
		auto c = findClient(id,UConnect);
		if (c == nullptr)return -1;
		sendBytes = sendto(socketfd, (char*)buf, size, 0, (sockaddr*)&c->addr, sizeof(sockaddr_in));


	}

	if (sendBytes != size)
	{
		SERVERPRINT_WARNING << "发送数据错误 " << sendBytes << "|" << size << "  " << getError();
		return -1;
	}
	return sendBytes;
}

int UDPServer::sendData(const s32 id, void* buf, u32 size)
{
	int sendBytes = 0;
	auto c = findClient(id, UConnect);
	if (c == nullptr)return -1;
	sendBytes = sendto(socketfd, (char*)buf, size, 0, (sockaddr*)&c->addr, sizeof(sockaddr_in));

	if (sendBytes != size)
	{
		SERVERPRINT_WARNING << "发送数据错误 " << sendBytes << "|" << size << "  " << getError();
		return -1;
	}
	return sendBytes;
}

int UDPServer::sendData(const s32 id, const void* buf, const u32 size, const u8 protocolType)
{

	auto c = findClient(id, UConnect);
	if (c == nullptr)return -1;

	if (protocolType == PT_UDP)
	{
		return sendto(socketfd, (char*)buf, size, 0, (sockaddr*)&c->addr, sizeof(sockaddr_in));
	}
	else if (protocolType == PT_KCP)
	{
		return ikcp_send(c->kcp, (char*)buf, size);
	}
	return -1;
}

int UDPServer::sendData(const void* buf, const u32 size, const char* ip, const int port)
{
	sockaddr_in  clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	clientAddr.sin_port = htons(port);
	int sendBytes = sendto(socketfd, (char*)buf, size, 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
	if (sendBytes != size)
	{
		cout << size << "  " << getError() << endl;
		return -1;
	}
	return sendBytes;
}

int UDPServer::runServer()
{

	Linkers = new HashArray<UDP_BASE>(__UDPServerInfo->MaxConnect);
	for (int i = 0; i < Linkers->length; i++)
	{
		UDP_BASE* client = Linkers->at(i);
		client->init(__UDPServerInfo->ReceMax);
		client->ID = i;
	}
	initCommands();
	initSocket();
	runThread();
	return 0;
}

int UDPServer::initSocket()
{

	socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketfd < 0)
	{
		perror("socketfd error");
		exit(1);
	}

	setNonblockingSocket(false,socketfd);

	sockaddr_in addserver;
	addserver.sin_family = AF_INET;
	addserver.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addserver.sin_port = htons(8888);

	int error = bind(socketfd, (sockaddr*)&(addserver), sizeof(addserver));
	if (error < 0)
	{
		perror("bind error");
		exit(1);
	}
	// 忽略10054错误
#ifdef ___WIN32__
	BOOL b = FALSE;
	DWORD dw = 0;
	WSAIoctl(socketfd, SIO_UDP_CONNRESET, &b, sizeof(b), NULL, 0, &dw, NULL, NULL);
#endif // !___WIN32__


	SERVERPRINT_INFO << "UDP init success" << endl;
	return 1;
}

int UDPServer::sendData(const char* ip, int port, const char* buf)
{


	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	clientAddr.sin_port = htons(port);


	int len = strlen(buf);
	int sendByte = sendto(socketfd, buf, len, 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
	if (sendByte != len)
	{
		SERVERPRINT_ERROR << "sendData Error" << sendByte << "|" << len << "  " << getError();
		return-1;
	}
	SERVERPRINT_INFO << "sendData Success" << sendByte << endl;
	return sendByte;
}

int UDPServer::KcpSend(s32 id, u16 cmd, char* buff, int size)
{
	auto c = findClient(id);
	if (c == nullptr || c->state != UConnect || c->kcp == nullptr)return -1;
	char sendbuff[512] = { 0 };
	id ^= __UDPServerInfo->RCode;
	cmd ^= __UDPServerInfo->RCode;
	memcpy(sendbuff, __UDPServerInfo->Head, 3);
	memcpy(sendbuff + 3, (char*)&size, 2);
	memcpy(sendbuff + 5, (char*)&id, 4);
	memcpy(sendbuff + 9, (char*)&cmd, 2);
	memcpy(sendbuff + 11, buff, size);
   int temp =ikcp_send(c->kcp, sendbuff, size + 11);
	return temp;
}

UDP_BASE* UDPServer::findClient(const int id, u32 ip, u16 port)
{
	if (id < 0 || id >= Linkers->length)return nullptr;

	auto c = Linkers->at(id);
	if (c->state != UConnect)return nullptr;

	if (c->ip != ip || c->port != port)return nullptr;



	return c;
}

UDP_BASE* UDPServer::findClient(const int id, int state)
{
	if (id < 0 || id >= Linkers->length)return nullptr;
	auto c = Linkers->at(id);
	if (c->state == state)return c;
	return nullptr;
}

UDP_BASE* UDPServer::findClient(const int id)
{
	if (id < 0 || id >= Linkers->length)return nullptr;
	auto c = Linkers->at(id);
	return c;
}


int UDPServer::recvData()
{
	SERVERPRINT_INFO << "Recv Data====================" << endl;

	char buf[MAX_UDP_BUF];
	memset(buf, 0, MAX_UDP_BUF);
	sockaddr_in clientAddr;
	int addrlen = sizeof(clientAddr);
	int recvByte = recvfrom(socketfd, buf, 512, 0, (sockaddr*)&clientAddr, &addrlen);
	if (recvByte < 6)
	{
		SERVERPRINT_ERROR << "Recv Data Error  " << recvByte << "  " << getError() << endl;

		return-1;
	}


	string ipstr = inet_ntoa(clientAddr.sin_addr);
	u32 ip = clientAddr.sin_addr.S_un.S_addr;
	int port = ntohs(clientAddr.sin_port);

	DATA_HEAD head;
	memcpy(&head.ID, buf, 4);
	memcpy(&head.cmd, buf+4, 2);
	head.getSecure(__UDPServerInfo->RCode);
	
	switch (head.cmd)
	{
	case 65000://断开连接
	{
		onDisconnect(head.ID);
		return 0;
	}
	case 65001://请求连接
	{
		onAccept(buf, head, clientAddr);
		return recvByte;
	}


	default:
		break;
	}

	auto client = findClient(head.ID, ip, port);
	if (client == NULL)
	{
		if (head.cmd == 65002)
		{
			//发现心跳包 当前已经断开需要重新连接
			return recvByte;
		}
		//cout << "Recv KCP Data" << recvByte << "   " << ipstr.c_str() << ":" << port << endl;
		recvData_kcp(buf, recvByte, ip, port);
		return recvByte;
	}
	else if (client != NULL)
	{
		//发现心跳包
		if (head.cmd == 65002)
		{
			SERVERPRINT_INFO << "收到心跳包" << endl;
			client->time_Heart = time(NULL);
			return recvByte;
		}
		//cout << "Recv UDP Data" << recvByte << "   " << ipstr.c_str() << ":" << port << endl;
		onRecv_SaveData(buf, client, recvByte);
		return recvByte;
	}






	//cout << "recvDate .. OK" << endl << buf << endl << recvByte << "   " << ipstr.c_str() << ":" << port << endl;
	return -1;
}



void UDPServer::onAccept(char* buff, DATA_HEAD head, sockaddr_in clientAddr)
{

	if (head.ID != __UDPServerInfo->Version)
	{
		SERVERPRINT_WARNING << "首次连接版本号错误  " << head.ID << "|" << __UDPServerInfo->Version << endl;
		return;
	}

	string strip = inet_ntoa(clientAddr.sin_addr);
	u32 ip = clientAddr.sin_addr.S_un.S_addr;
	int port = ntohs(clientAddr.sin_port);

	if (checkIsConnect(ip, port))
	{
		SERVERPRINT_WARNING << "连接已经存在 " << endl;
		return;
	}

	UDP_BASE* client = findFreeUDP_BASE();
	if (client == nullptr)
	{
		SERVERPRINT_WARNING << "服务器连接已达到最大值" << endl;
		return;
	}

	client->socketfd = socketfd;
	client->state = UConnect;
	client->port = port;
	client->ip = ip;
	client->time_Heart = time(NULL);
	memcpy(client->strip, strip.c_str(), 16);
	memcpy(&client->addr, &clientAddr, sizeof(sockaddr_in));
	addCheckIsConnect(ip, port);
	createKcp(client);
	
	for (int i = 0; i < 3; i++)
	{
		DATA_HEAD temp;
		temp.ID = client->ID;
		temp.cmd = 65001;
		temp.setSecure(__UDPServerInfo->RCode);
		sendData(client->ID, &temp, sizeof(DATA_HEAD));
	}

	if (onAcceptEvent != nullptr)this->onAcceptEvent(this, client, 0, "");


}

void UDPServer::onDisconnect(int cID)
{
	UDP_BASE* client = findClient(cID,UConnect);
	if (client == nullptr)return;
	SERVERPRINT_INFO << "正在断开连接 " << cID << endl;;
	DeleteCheckIsConnect(client->ip, client->port);
	releaseKcp(client);

	for (int i = 0; i < 3; i++)
	{
		DATA_HEAD temp;
		temp.ID = client->ID;
		temp.cmd = 65000;
		temp.setSecure(__UDPServerInfo->RCode);
		sendData(client->ID, &temp, sizeof(DATA_HEAD));
	}

	client->reset();
	client->state = UFree;
	//if (onDisconnectEvent != nullptr)this->onDisconnectEvent(this, client, 0, "");
}

bool UDPServer::checkIsConnect(u32 ip, u16 port)
{
	auto key = getIPPORTKey(ip, port);
	if (CheckConnectSet.count(key))
	{
		return true;
	}
	return false;
}

void UDPServer::checkConnect(UDP_BASE* c)
{

	s32 temp = (s32)time(NULL) - c->time_Heart;
	if (temp < __UDPServerInfo->HeartTimeMax)return;

	//删除
	DeleteCheckIsConnect(c->ip, c->port);
	//
	
	if (onDisconnectEvent != nullptr)this->onDisconnectEvent(this, c, 1001, "heart time out");
	return ;
}

bool UDPServer::addCheckIsConnect(u32 ip, u16 port)
{
	auto key = getIPPORTKey(ip, port);
	{
		lock_guard<mutex> guard(CheckConnectSetMutex);
		CheckConnectSet.insert(key);
	}


	return true;
}

bool UDPServer::DeleteCheckIsConnect(u32 ip, u16 port)
{
	auto key = getIPPORTKey(ip, port);


	{
		lock_guard<mutex> guard(CheckConnectSetMutex);
		if (CheckConnectSet.count(key))
		{
			return false;
		}
		CheckConnectSet.erase(key);
	}
	return true;
}

u64 UDPServer::getIPPORTKey(u32 ip, u16 port)
{


	return ip*1000000+port;

}





void UDPServer::onRecv_SaveData(char* buf, UDP_BASE* c, const u32 recvBytes)
{


	if (c->recv_Head == c->recv_Tail)
	{
		c->recv_Head = 0;
		c->recv_Tail = 0;
	}
	if (c->recv_Tail + recvBytes >= __UDPServerInfo->ReceMax)
	{
		SERVERPRINT_INFO << "缓冲区已满" << endl;
		return;
	}

	memcpy(&c->recvBuf[c->recv_Tail], buf, recvBytes);
	c->recv_Tail += recvBytes;
	c->is_RecvCompleted = true;

}

UDP_BASE* UDPServer::findFreeUDP_BASE()
{
	for (int i = 0; i < Linkers->length; i++)
	{
		UDP_BASE* client = Linkers->at(i);
		if (client->state != UFree)continue;
		client->reset();
		client->ID = i;
		return client;
	}

	return nullptr;
}
