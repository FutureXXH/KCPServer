#ifndef __APPMANAGER_H
#define __APPMANAGER_H

#include "engine/INetBase.h"
#include "share/ShareFunction.h"
#include "AppGlobal.h"
#include "TestApp.h"

using namespace std;
class AppManager
{
public:
	AppManager();
    ~AppManager();
	void init();
};

extern int run();

#endif // !__APPMANAGER_H

