


#ifdef ___WIN32__
#include <WinSock2.h>
#include <string>

#pragma comment(lib,"ws2_32")

using UDPSOCKET = SOCKET;
#define UDPSleep(value) Sleep(value)
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#else
#include <cstdio>
#include <signal.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
using UDPSOCKET = int;
#define UDPSleep(value) usleep(value*1000);
#endif


#include <iostream>
#include "AppManager.h"
using namespace std;





int main()
{
#ifndef ___WIN32__
	signal(SIGPIPE, SIG_IGN);
#endif // !___WIN32__
	run();
	return 0;
}


