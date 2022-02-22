#ifndef __UDPSERVER_H
#define __UDPSERVER_H



#include "IContainer.h"
#include "INetBase.h"
#include <unordered_set>
#include <mutex>
using namespace std;





class UDPServer:public IUDPServer
{
public:
	UDPServer();
	virtual ~UDPServer();
	virtual int runServer();
	virtual int initSocket();
	virtual void stopServer();
	int sendData(const s32 id, void* buf, u32 size, sockaddr_in* addr);
	int sendData(const s32 id, void* buf, u32 size);
	virtual int sendData(const s32 id, const void* buf, const u32 size, const u8 protocolType);
	virtual int sendData(const void* buf, const u32 size, const char* ip, const int port);

	virtual int sendData(const char* ip, int port, const char* buf);
	virtual int KcpSend(s32 id, u16 cmd, char* buff, int size);

	UDP_BASE* findClient(const int id, u32 ip, u16 port);
	UDP_BASE* findClient(const int id, int state);
	UDP_BASE* findClient(const int id);
	virtual void parseCommand();
	virtual void setOnUDPClientAccept(UDPSERVERNOTIFY_EVENT event);
	virtual void setOnUDPClientDisconnect(UDPSERVERNOTIFY_EVENT event);
	virtual void initCommands();
	virtual void registerCommand(int cmd, void* container);

	void createKcp(UDP_BASE* c);
	void releaseKcp(UDP_BASE* c);
	void updateKcp(UDP_BASE* c);
private:

	 void parseCommand(UDP_BASE *c);
	 void parseCommand(UDP_BASE* c,const u16 cmd);
	void onRecv_SaveData(char *buf,UDP_BASE*c,const u32 recvBytes);
	void runThread();
	int recvData();
	int recvData_kcp(char* buf, s32 recvBytes, s32 ip, u16 port);
	void onAccept(char* buff, DATA_HEAD head, sockaddr_in clientAddr);
	void onDisconnect(int cID);
	bool checkIsConnect(u32 ip, u16 port);
	void checkConnect(UDP_BASE* c);
	bool addCheckIsConnect(u32 ip, u16 port);
	bool DeleteCheckIsConnect(u32 ip, u16 port);
	u64 getIPPORTKey(u32 ip, u16 port);


	UDP_BASE* findFreeUDP_BASE();

	static void run(UDPServer* udp);


	UDPSOCKET socketfd;
	shared_ptr<thread> m_workthread;
	HashArray<UDP_BASE>* Linkers;

	unordered_set<u64> CheckConnectSet;


	mutex CheckConnectSetMutex;



	UDPSERVERNOTIFY_EVENT onAcceptEvent;
	UDPSERVERNOTIFY_EVENT onDisconnectEvent;

	

};







#endif // ! __UDPSERVER_H




