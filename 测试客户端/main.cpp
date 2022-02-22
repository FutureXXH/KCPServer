

#include "IDefine.h"
#include "KCPClient.h"

//KCP简单测试客户端
int main()
{
    KCPClient* kcp = new KCPClient();
	kcp->Init("192.168.31.179",8888);
	memcpy(kcp->Head, "UDP", 3);
	kcp->StartThread();

	while (!kcp->state)
	{
		kcp->connectServer();
		Sleep(1000);
	}

	u64 sendtestTime = time(NULL);
	u64 sendHeartTime = time(NULL);
	int i = 0;
	char sendTestBuff[4];
	while (true)
	{
		
		kcp->parseCommand();
		if (time(NULL) - sendtestTime > 2)
		{

			long sendtest = clock();
			memcpy(sendTestBuff, (char*)&sendtest, 4);
			kcp->KcpSend(666, sendTestBuff, 4);
			sendtestTime = time(NULL);
		}
		if (time(NULL) - sendHeartTime > 2)
		{
			kcp->SendHeartPack();
			sendHeartTime = time(NULL);
		}
		i++;
	}
}