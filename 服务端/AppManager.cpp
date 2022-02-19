#include "AppManager.h"

AppManager* __AppManager = nullptr;

AppManager::AppManager()
{
}

AppManager::~AppManager()
{
}

void AppManager::init()
{
	bool isload = share::InitData();
	if (!isload)
	{
		SERVERPRINT_ERROR << "Load ERROR !" << endl;
	}


	LoadWindowsDll();
	__IUDPServer = NewUDPServer();
	__IUDPServer->setOnUDPClientAccept(onUDPToClientAccept);
	__IUDPServer->setOnUDPClientDisconnect(onUDPToClientDisconnect);
	__IUDPServer->initCommands();
	__TestApp = new TestApp();
	__IUDPServer->registerCommand(666,__TestApp);


	__IUDPServer->runServer();

   
	while (true)
	{
		__IUDPServer->parseCommand();

		//cout << "5 s" << endl;
		//UDPSleep(5000);
	}
}

int run()
{
	if (__AppManager == nullptr)
	{
		__AppManager = new AppManager();
		__AppManager->init();
	}
	return 0;
}
