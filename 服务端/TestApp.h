#pragma once
#ifndef __TESTAPP_H
#include "engine/IContainer.h"
using namespace std;
class TestApp :public IUDPContainer
{
	virtual bool onUDPServerCommand(IUDPServer* ts, UDP_BASE* c, const u16 cmd);




};

extern  TestApp* __TestApp;


#endif // !__TESTAPP_H