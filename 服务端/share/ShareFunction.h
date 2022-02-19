#pragma once


#ifndef __SHAREFUNCTION_H
#define __SHAREFUNCTION_H
#include "tinyxml2.h"
#include "..\engine\IDefine.h"
#include <string>
using namespace std;
namespace share
{
	extern int LoadServerXML(string filename);
	extern bool InitData();

}


#endif // !__SHAREFUNCTION_H

