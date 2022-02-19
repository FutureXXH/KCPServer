#ifndef APPGLOBAL_H
#define APPGLOBAL_H

#include "engine/INetBase.h"
using namespace std;

extern void onUDPToClientAccept(IUDPServer* udp, UDP_BASE* c, const s32 code, const char* err);
extern void onUDPToClientDisconnect(IUDPServer* udp, UDP_BASE* c, const s32 code, const char* err);


extern IUDPServer* __IUDPServer;
#endif // !APPGLOBAL_H

