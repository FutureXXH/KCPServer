#ifndef KCPCLIENT_H
#define KCPCLIENT_H
#include <vector>
#include <thread>
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <unordered_set>
#include <mutex>
#include "ikcp.h"
#pragma comment(lib,"ws2_32")
#include "IDefine.h"
#include "KcpInterface.h"

using namespace std;
enum USOCKET_STATE
{
	DisConnect = 0,
	Connect = 1
};
class KCPClient
{
public:
	SOCKET socketfd;
	sockaddr_in addrserver;
	u32 RCode = 130;
	char Head[3];
	s32 ID = 0;
	char RecvBuff[MAX_UDP_BUF];
	int recv_Head = 0;
	int recv_Tail = 0;
	int recv_TempHead = 0;
	int recv_TempTail = 0;
	bool is_RecvCompleted = false;
	int state = DisConnect;

	ikcpcb* kcp = nullptr;
	thread* threadp;



	bool Init(const char* ip, int port);
	bool connectServer();
	int SendServer(u16 cmd, char* buff, int size, bool haveHead);
	int RecvServer();
	void onRecv_SaveData(char* buf, int recvBytes);
	int recvData_kcp(char* buf, s32 recvBytes);
	bool StartThread();
	static void ThreadRun(KCPClient* kcp);
	int onAccept(char* buff, s32 RecvID, u16 cmd, sockaddr_in clientAddr);
	bool SendHeartPack();

	int KcpSend(u16 cmd, char* buff, int size);
	void parseCommand();
	void parseCommand(const u16 cmd);
	void updateKcp();
};


#endif // !KCPCLIENT_H
