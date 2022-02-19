#include "INetBase.h"






char FileExePath[MAX_EXE_LEN];
ConfigXML* __UDPServerInfo = nullptr;

std::vector<ServerListXML*>__UDPServerListInfo;
void(*MD5Str)(char* output, unsigned char* input, int len);





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
#ifdef ___WIN32__
	unsigned long u1 = 1;
	ioctlsocket(socketfd, FIONBIO, (unsigned long*)&u1);
#else
	int f = fcntl(socketfd, F_GETFL);
	if (f < 0)return;
	f |= O_NONBLOCK;
	if (fcntl(socketfd, F_SETFL, f) < 0)return;

#endif
}

void LoadWindowsDll()
{
#ifdef ___WIN32__
	WSADATA wsData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error != 0)
	{
		perror("WSAStartup error");
		exit(1);
	}

#endif // ___WIN32__


}

//初始化调用一次
void UDP_BASE::init(u32 length)
{
	recvBuf = new char[length];
	reset();

}

//
void UDP_BASE::reset()
{
	ID = -1;
	ip = 0;
	port = 0;
	memset(&addr, 0, sizeof(sockaddr_in));
	initdata();
}

void UDP_BASE::initdata()
{

	state = 0;
	is_RecvCompleted = true;
	memset(recvBuf, 0, 512);
	recv_Head = 0;
	recv_Tail = 0;
	recv_TempHead = 0;
	recv_TempTail = 0;
	time_Heart = (int)time(NULL);
	time_AutoConnect = (int)time(NULL);


}
