#include "pch.h"
#define D3D_API_API __declspec(dllexport)

struct WxMessage
{
	wchar_t wxId[10];
	wchar_t message[10];
};

extern "C" D3D_API_API VOID SendTextMessage(wchar_t* msg);