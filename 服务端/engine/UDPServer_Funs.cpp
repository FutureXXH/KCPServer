#include "UDPServer.h"

vector<IUDPContainer*> __Commands;

void UDPServer::parseCommand()
{
	for (u32 i = 0; i < Linkers->length; i++)
	{
		auto c = Linkers->at(i);
		if (c->ID < 0)continue;
		if (c->state == UFree)continue;

		//判断心跳时间 暂时没写


		updateKcp(c);
		parseCommand(c);
	}

}

void UDPServer::setOnUDPClientAccept(UDPSERVERNOTIFY_EVENT event)
{
	onAcceptEvent = event;
}

void UDPServer::setOnUDPClientDisconnect(UDPSERVERNOTIFY_EVENT event)
{
	onDisconnectEvent = event;
}

void UDPServer::initCommands()
{
	__Commands.reserve(MAX_COMMAND_LEN);
	for (int i = 0; i < MAX_COMMAND_LEN; i++)
	{
		__Commands.emplace_back(nullptr);
	}
}

void UDPServer::registerCommand(int cmd, void* container)
{
	if (cmd >= MAX_COMMAND_LEN)return;
	IUDPContainer* icon = (IUDPContainer*)container;
	if (icon == nullptr)return;
	__Commands[cmd] = icon;
}




void UDPServer::parseCommand(UDP_BASE* c)
{
	if (!c->is_RecvCompleted)return;
	//SERVERPRINT_INFO << "正在解析消息" << endl;

	while (c->recv_Tail - c->recv_Head >= 11)
	{
		if (c->recvBuf[c->recv_Head] != __UDPServerInfo->Head[0] || c->recvBuf[c->recv_Head+1] != __UDPServerInfo->Head[1] || c->recvBuf[c->recv_Head+2] != __UDPServerInfo->Head[2])
		{
			//SERVERPRINT_WARNING << "解析头错误" << endl;
			c->recv_Head++;
			return;
		}
		u16 len;
		memcpy(&len, c->recvBuf + 3 + c->recv_Head,2);

		DATA_HEAD data;
		memcpy(&data.ID, c->recvBuf + c->recv_Head+5, 4);
		memcpy(&data.cmd, c->recvBuf + c->recv_Head + 9, 2);
	
		data.getSecure(__UDPServerInfo->RCode);
		

		if (c->recv_Tail < c->recv_Head + len)
		{
			SERVERPRINT_WARNING << "解析长度错误" << endl;
			return;
		}

		c->recv_TempHead = c->recv_Head + 11;
		c->recv_TempTail = c->recv_Head + 11+ len;
		parseCommand(c, data.cmd);
		c->recv_Head += len;

	}
	c->is_RecvCompleted = false;
}

void UDPServer::parseCommand(UDP_BASE* c, const u16 cmd)
{
	auto container = __Commands[cmd];
	if (container == nullptr)
	{
		SERVERPRINT_ERROR << "指令未注册:" << cmd << endl;
		return;
	}
	container->onUDPServerCommand(this,c,cmd);
}