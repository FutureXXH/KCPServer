#ifndef __IDEFINE_H
#define __IDEFINE_H


#include <WinSock2.h>
#include <string>

#pragma comment(lib,"ws2_32")

using UDPSOCKET = SOCKET;
#define UDPSleep(value) Sleep(value)
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#include <vector>
#include <thread>
#include <string>
#include <iostream>
#include "ConsoleColor.h"


#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout << green <<"[INFO]" << white << ":"
#define SERVERPRINT_WARNING NOWTIME();std::cout<< yellow <<"[WARNING]" << white << ":"
#define SERVERPRINT_ERROR NOWTIME();std::cout  << red<<"[ERROR]" << white << ":"

#define MAX_UDP_BUF  512
#define MAX_IP_LEN  20
#define MAX_EXE_LEN 200
#define MAX_COMMAND_LEN 800000

using s8 = char;
using s16 = short;
using s32 = int;
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using f32 = float;
using f64 = double;

#ifdef ___WIN32__

using s64 = long long;
using u64 = unsigned long long;
#else
using s64 = long int;
using u64 = unsigned long int;

#endif // ___WIN32__


struct ConfigXML
{
	s32 ID;
	u8 Type;
	u16 Port;
	s32 MaxUser;
	s32 MaxConnect;
	u8 RCode;
	s32 Version;
	s32 ReceOne;
	s32 ReceMax;
	s32 HeartTimeMax;
	s32 HeartTime;
	s32 AutoTime;

	char SafeCode[20];
	char Head[3];
	char IP[MAX_IP_LEN];
};

struct ServerListXML
{
	s32 ID;
	u16 Port;
	char IP[MAX_IP_LEN];
};

#pragma pack(push,packing)
#pragma pack(1)
struct DATA_HEAD
{
	s32 ID;
	u16 cmd;
	inline void setSecure(u32 code)
	{
		ID = ID ^ code;
		cmd = cmd ^ code;

	}
	inline void getSecure(u32 code)
	{
		ID = ID ^ code;
		cmd = cmd ^ code;
	}
};


struct DATA_BASE
{
	char head[3];
	u16 length;
	char buf[MAX_UDP_BUF];
	inline void reset()
	{
		memset(this, 0, sizeof(DATA_BASE));
	}


};
#pragma pack(pop,packing)


extern int getError();
extern void setNonblockingSocket(bool isNon, UDPSOCKET socketfd);




extern char FileExePath[MAX_EXE_LEN];

extern std::vector<ServerListXML*>__UDPServerListInfo;
extern void(*MD5Str)(char* output, unsigned char* input, int len);
extern bool InitData();


#endif // !__IDEFINE_H

