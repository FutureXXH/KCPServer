#ifndef __INETBASE_H
#define __INETBASE_H


#include "IDefine.h"
#include "ikcp.h"


enum USOCKET_STATE
{
	UFree = 0,
	UConnect = 1
};

enum PROTOCOL_TYPE
{
	PT_UDP = 0,
    PT_KCP = 1
};


#pragma pack(push,packing)
#pragma pack(1)
struct UDP_BASE
{
	u8 state; // 0 未连接 1连接
	s32 ID;
	char strip[16];
	u32 ip;
	u16 port;
	UDPSOCKET socketfd;
	bool is_RecvCompleted;
	char* recvBuf;
	s32  recv_Head;
	s32 recv_Tail;
	s32 recv_TempHead;
	s32 recv_TempTail;
	s32 time_Heart;
	s32 time_AutoConnect;
	sockaddr_in addr;

	ikcpcb* kcp;

	void init(u32 length);
	void reset();
	void initdata();

	inline bool isT(s32 id)
	{
		if (id == ID)return true;
		return false;
	}
	inline bool isT(u32 ipvalue, u16 portvalue)
	{
		if (ipvalue == ip && port == portvalue)return true;
		return false;
	}


};

#pragma pack(pop,packing)
class IUDPServer;
//通知事件
using UDPSERVERNOTIFY_EVENT = void(*)(IUDPServer* udp, UDP_BASE* c, const s32 code, const char* err);


class IUDPServer
{
public:
	virtual ~IUDPServer(){};
	virtual int runServer()=0;
	virtual void stopServer()=0;
	virtual  int sendData(const char* ip, int port, const char* buf)= 0;
	virtual int sendData(const void* buf, const u32 size, const char* ip, const int port) = 0;
	virtual  int sendData(const s32 id,const void*buf,const u32 size,const u8 protocolType) = 0;
	virtual void parseCommand() = 0;
	virtual void  registerCommand(int cmd, void* container) = 0;
	virtual void initCommands() = 0;
	virtual int KcpSend(s32 id, u16 cmd, char* buff, int size) = 0;
	virtual void setOnUDPClientAccept(UDPSERVERNOTIFY_EVENT event) = 0;
	virtual void setOnUDPClientDisconnect(UDPSERVERNOTIFY_EVENT event) = 0;


};



extern IUDPServer* NewUDPServer();


#endif // !__INETBASE_H

