#include "pch.h"
#include <stdio.h>
#include <Windows.h>
#include "SendMessage.h"
#include <string.h>

struct wxStr
{
	wchar_t* pStr;
	int strLen;
	int strLen2;
};

DWORD GetWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}

VOID SendTextMessage(wchar_t* msg)
{ 
	//char szSendBuf[100];
	//sprintf_s(szSendBuf, "%s", msg);//注意，ctime()返回的字符串后面带了'\n'

	//FILE *fp;
	//fopen_s(&fp, "aa.txt","w");
 //

	//fprintf(fp, "你的消息是：",szSendBuf);


	//fclose(fp);

	//return;

	wchar_t ejflag[] = L"_ejflag_";
	wchar_t* p = wcsstr(msg, L"_ejflag_");
	int offset = lstrlen(ejflag);
	p[0] = '\0';
	wchar_t* wxId = msg;
	wchar_t* message = p + offset;
	struct wxStr vxidS, vxMsg;
	vxidS.pStr = wxId;
	vxidS.strLen = wcslen(wxId);
	vxidS.strLen2 = vxidS.strLen * 2;
	vxMsg.pStr = message;
	vxMsg.strLen = wcslen(message);
	vxMsg.strLen2 = vxMsg.strLen;
	DWORD sendCall = GetWeChatWin() + 0x2EB4E0;
	char* pWxid = (char*)& vxidS.pStr;
	char* pMessage = (char*)& vxMsg.pStr;
	char buff[0x81c] = { 0 };
	__asm
	{
		mov edx, pWxid;
		push 0x1;
		push 0;
		mov ebx, pWxmsg;
		push ebx;
		lea ecx, buff;
		call dwSendCallAddr;
		add esp, 0xC;
	}
}