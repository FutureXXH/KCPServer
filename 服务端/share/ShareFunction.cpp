#include "ShareFunction.h"

namespace share
{
	int LoadServerXML(string filename)
	{
	

		std::string loadfilepath(FileExePath);
		loadfilepath += filename;
		
	
		SERVERPRINT_INFO << "正在加载" << loadfilepath.c_str() << std::endl;
		tinyxml2::XMLDocument xml;
		if (xml.LoadFile(loadfilepath.c_str()) != 0)
		{
			SERVERPRINT_WARNING << "加载服务器配置文件失败!" << std::endl;
			return -1;
		}
		//获取根节点
		tinyxml2::XMLElement* xmlRoot = xml.RootElement();
		if (xmlRoot == NULL)
		{
			SERVERPRINT_WARNING << "加载服务器配置文件时根节点不存在!" << std::endl;
			return -1;
		}
		
		//获取子节点 数据
		tinyxml2::XMLElement* xmlNode = xmlRoot->FirstChildElement("server");

		__UDPServerInfo->Port = atoi(xmlNode->Attribute("Port"));
		memcpy(__UDPServerInfo->SafeCode, xmlNode->Attribute("SafeCode"), 20);
		memcpy(__UDPServerInfo->Head, xmlNode->Attribute("Head"), 3);
		__UDPServerInfo->ID = atoi(xmlNode->Attribute("ID"));
		__UDPServerInfo->RCode = atoi(xmlNode->Attribute("RCode"));
		__UDPServerInfo->MaxUser = atoi(xmlNode->Attribute("MaxUser"));
		__UDPServerInfo->MaxConnect = atoi(xmlNode->Attribute("MaxConnect"));
		__UDPServerInfo->ReceOne = atoi(xmlNode->Attribute("RecvOne"));
		__UDPServerInfo->ReceMax = atoi(xmlNode->Attribute("RecvMax"));
		__UDPServerInfo->HeartTime = atoi(xmlNode->Attribute("HeartTime"));
		__UDPServerInfo->HeartTimeMax = atoi(xmlNode->Attribute("HeartTimeMax"));
		__UDPServerInfo->AutoTime = atoi(xmlNode->Attribute("AutoTime"));
		SERVERPRINT_INFO << "加载服务器配置文件成功" << std::endl;
		SERVERPRINT_INFO << "====================================" << std::endl;
		SERVERPRINT_INFO << "服务器端口：" << __UDPServerInfo->Port << std::endl;
		SERVERPRINT_INFO << "安全码：" << __UDPServerInfo->SafeCode << std::endl;
		//SERVERPRINT_INFO << "IO线程数：" << IOthreadnum << std::endl;
		//SERVERPRINT_INFO << "业务线程数："<< Servicethreadnum << std::endl;
		SERVERPRINT_INFO << "最大连接数："<< __UDPServerInfo->MaxConnect << std::endl;
		SERVERPRINT_INFO << "最大在线玩家数："<< __UDPServerInfo->MaxUser << std::endl;
		SERVERPRINT_INFO << "RecvOne：" << __UDPServerInfo->ReceOne << std::endl;
		SERVERPRINT_INFO << "ReceMax：" << __UDPServerInfo->ReceMax << std::endl;
		SERVERPRINT_INFO << "====================================" << std::endl;

		return 0;
	}



	bool InitData()
	{
		
		//获取当前exe路径 
		memset(FileExePath, 0, MAX_EXE_LEN);
		GetModuleFileNameA(NULL, (LPSTR)FileExePath, MAX_EXE_LEN);
		std::string strPath(FileExePath);
		size_t pos = strPath.find_last_of('\\'); //除去最后的exe文件名 获取当前文件目录
		strPath = strPath.substr(0, pos + 1);
		memcpy(FileExePath, strPath.c_str(), MAX_EXE_LEN);

		__UDPServerInfo = new ConfigXML();
		int errs = LoadServerXML("UDPServerConfig.xml");
		if (errs < 0)return false;

		return true;
	}



}

