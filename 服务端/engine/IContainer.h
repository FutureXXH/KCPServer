
#ifndef ICONTAIONER_H
#define ICONTAIONER_H
#include <iostream>
#include "INetBase.h"
//哈希数组
template <class T>
class HashArray
{
public:
	int length;
	int size;
	//void* pointer;
	T* pointer;

	HashArray()
	{
		size = sizeof(T);
		length = 0;
		pointer = nullptr;
	}

	HashArray(int counter)
	{
		if (counter < 1)return;
		size = sizeof(T);
		if (size == 0) return;
		length = counter;
		pointer = new T[length];
		//pointer = malloc(size * length);


	}


	virtual ~HashArray()
	{
		if (pointer != nullptr)
		{
			//free(pointer);
			delete[] pointer;
			pointer = nullptr;
		}
	}

	T* at(const int index)
	{
		//T* temp = (T*)pointer;
		return &pointer[index];
	}
};

class IUDPContainer
{
public:
	IUDPContainer() {};
	virtual ~IUDPContainer() {};
	virtual void onUDPInit() {};
	virtual void onUDPUpdate() {};
	virtual bool onUDPServerCommand(IUDPServer* ts, UDP_BASE* c, const u16 cmd) { return true; };
	
};


#endif // !ICONTAIONER_H

