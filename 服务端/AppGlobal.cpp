#include "AppGlobal.h"

IUDPServer* __IUDPServer = nullptr;
void onUDPToClientAccept(IUDPServer* udp, UDP_BASE* c, const s32 code, const char* err)
{
	char s[30];

	SERVERPRINT_INFO << "客户端连接" << c->ID << "  " << c->strip << ":" << c->port << endl;
}

void onUDPToClientDisconnect(IUDPServer* udp, UDP_BASE* c, const s32 code, const char* err)
{
	SERVERPRINT_INFO << "客户端断开" << c->ID << "  " << c->strip << ":" << c->port << endl;

	c->reset();
}