// ChatRoomMember.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include "ChatRoomMember.h"
#include <string>
#include <winuser.h>
#include <stdlib.h>
#include <memory>

using namespace std;




DWORD GetWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}

//测试获取群组成员列表
VOID GetChatRoomUser(wchar_t* chatroomwxid) {
	LPCWSTR UserStr = ShowChatRoomUser(chatroomwxid);

	HWND h = FindWindow(NULL, L"WeChatBot");
	if (h == NULL)
	{
		MessageBoxA(NULL, "未找到ChatRoomMember窗口", "错误", 0);
		return;
	}
	COPYDATASTRUCT cpd;
	cpd.dwData = 2;//保存一个数值, 可以用来作标志等
	cpd.cbData = lstrlenW(UserStr) * 2;//待发送的数据的长
	cpd.lpData = (void*)UserStr;

	SendMessage(h, WM_COPYDATA, 0, (LPARAM)& cpd);
	free((void *)UserStr);
}

//************************************************************
// 函数名称: ShowChatRoomUser
// 函数说明: 显示群成员
// 作    者: GuiShou
// 时    间: 2019/7/13
// 参    数: void
// 返 回 值: void 
//************************************************************
LPCWSTR ShowChatRoomUser(wchar_t* chatroomwxid)
{
	//1.先获取群成员的微信ID
	struct wxStr
	{
		wchar_t * pStr;
		int strLen;
		int strMaxLen;
		int fill = 0;
		int fill2 = 0;
	};
	//准备缓冲区
	DWORD dwWxidArr = 0;	//保存微信ID数据的地址
	char buff[0x164] = { 0 };

	char * asmWxid = (char *)&chatroomwxid;

	//调用call
	DWORD dwCall1 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x414680;
	DWORD dwCall2 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x2BCF20;
	DWORD dwCall3 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x41CAE0;
	DWORD dwCall4 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x415170;

	//获取群成员
	__asm {
		lea ecx, buff[16]
		call dwCall1
		lea eax, buff[16]
		push eax
		mov ebx, asmWxid
		push ebx
		call dwCall2
		mov ecx, eax
		call dwCall3
		lea eax, buff
		push eax
		lea ecx, buff[16]
		call dwCall4
		mov dwWxidArr, eax
	}

	//拿到微信ID
	wchar_t tempWxid[0x100] = { 0 };
	DWORD userList = *((DWORD *)dwWxidArr);
	DWORD testTmp = dwWxidArr + 0xB0;
	int Len = *((int *)testTmp);


	wchar_t *endStr = (wchar_t*)malloc(999999);//最终返回的字符串
	wchar_t *up = &endStr[0];

	//群组信息
	UserInfo wxGroup = { 0 };
	GetUserInfoByWxid(chatroomwxid, &wxGroup);
	int h = swprintf_s(up, 200, L"%s|&|%s_&&_", chatroomwxid, wxGroup.UserNickName);//原来是100
	up += h;

	for (int i = 0; i < Len; i++)
	{
		DWORD temWxidAdd = userList + (i * 20);
		swprintf_s(tempWxid, 256, L"%s", (wchar_t*)*((LPVOID *)temWxidAdd));

		// @todo 打印wid
		UserInfo u = { 0 };
		GetUserInfoByWxid(tempWxid, &u);
		int l = swprintf_s(up, 200, L"%s|&|%s|&|%s_&_", u.UserId, u.UserNumber, u.UserNickName);//原来是100
		up += l;

	}
	return endStr;
}


//************************************************************
// 函数名称: GetUserInfoByWxid
// 函数说明: 通过微信ID获取用户信息
// 作    者: GuiShou
// 时    间: 2019/7/13
// 参    数: userwxid 微信ID
// 返 回 值: void 
//************************************************************
VOID GetUserInfoByWxid(wchar_t* userwxid, UserInfo *userinfo)
{

	struct wechatText
	{
		wchar_t* pStr;
		int strLen;
		int iStrLen;
	};
	DWORD dwCall1 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x481900;
	DWORD dwCall2 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x4FFB0;
	DWORD dwCall3 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x273280;

	char* asmWxid = (char*)& userwxid;
	char buff[0x3D8] = { 0 };
	DWORD userData = 0;		//用户数据的地址
	__asm
	{
		lea edi, buff
		push edi
		sub esp, 0x14
		mov eax, asmWxid
		mov ecx, esp
		mov dword ptr ss : [ebp - 0x2C], esp
		push eax
		call dwCall1
		call dwCall2
		call dwCall3
		mov userData, edi
	}


	DWORD wxidAdd = userData + 0x8;		//微信ID
	DWORD wxNickAdd = userData + 0x64;	//昵称
	DWORD wxuserIDAdd = userData + 0x1C;	//微信账号



	swprintf_s(userinfo->UserId, 100, L"%s", (wchar_t*)(*((LPVOID*)wxidAdd)));
	swprintf_s(userinfo->UserNickName, 100, L"%s", (wchar_t*)(*((LPVOID*)wxNickAdd)));
	swprintf_s(userinfo->UserNumber, 100, L"%s", (wchar_t*)(*((LPVOID*)wxuserIDAdd)));
}



//发送艾特消息需要的数据结构
class TEXT_WX
{
public:
	wchar_t* pWxid = nullptr;
	DWORD length = 0;
	DWORD maxLength = 0;
	DWORD fill1 = 0;
	DWORD fill2 = 0;
	wchar_t wxid[1024] = { 0 };

	TEXT_WX(wstring wsWxid)
	{
		const wchar_t* temp = wsWxid.c_str();
		wmemcpy(wxid, temp, wsWxid.length());
		length = wsWxid.length();
		maxLength = wsWxid.capacity();
		fill1 = 0;
		fill2 = 0;
		pWxid = wxid;
	}
};


class ROOM_AT
{
public:
	DWORD at_WxidList = 0;
	DWORD at_end1 = 0;
	DWORD at_end2 = 0;
};


class  TEXT_WXID
{
public:
	wchar_t* pWxid = nullptr;
	DWORD length = 0;
	DWORD maxLength = 0;
	DWORD fill1 = 0;
	DWORD fill2 = 0;
};


struct wechatText
{
	wchar_t* pStr;
	int strLen;
	int iStrLen;
};

//用于调用微信发送消息call的结构体
struct wxMsg
{
	wchar_t* pMsg;		//微信ID
	int msgLen;			//ID长度
	int buffLen;		//缓冲区长度
	int fill1 = 0;
	int fill2 = 0;
};


//************************************************************
// 函数名称: SendRoomAtMsg
// 函数说明: 发送艾特消息
// 作    者: GuiShou
// 时    间: 2019/7/26
// 参    数: chatroomid 群ID memberwxid 群成员微信ID membernickname群成员昵称 msg 消息内容
// 返 回 值: void  wchar_t* chatroomid, wchar_t* memberwxid, wchar_t* membernickname, wchar_t* msg
//************************************************************
void SendRoomAtMsg(wchar_t* wxRoomID)
{
	//拿到发送消息的call的地址
	DWORD dwSendCallAddr = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll")) + 0x2EB4E0;

	//组装微信ID/群ID的结构体
	wxMsg id = { 0 };
	id.pMsg = wxRoomID;
	id.msgLen = wcslen(wxRoomID);
	id.buffLen = wcslen(wxRoomID) * 2;

	//消息内容
	wxMsg text = { 0 };
	text.pMsg = L"收到";
	text.msgLen = wcslen(L"收到");
	text.buffLen = wcslen(L"收到") * 2;

	//取出微信ID和消息的地址
	char* pWxid = (char*)&id.pMsg;
	char* pWxmsg = (char*)&text.pMsg;

	char buff[0x81C] = { 0 };

	//调用微信发送消息call
	__asm {
		mov edx, pWxid;
		push 0x1;
		push 0;
		mov ebx, pWxmsg;
		push ebx;
		lea ecx, buff;
		call dwSendCallAddr;
		add esp, 0xC;
	}


#pragma region 何总的
	//HMODULE dllAdress = GetModuleHandleA("WeChatWin.dll");
	//DWORD callAdd2 = (DWORD)dllAdress + 0x2EB4E0;
	//wechatText pWxid2 = { 0 };
	//pWxid2.pStr = fromWxid;
	//pWxid2.strLen = wcslen(fromWxid);
	//pWxid2.iStrLen = wcslen(fromWxid) * 2;

	//char* asmWxid2 = (char*)&pWxid2.pStr;
	//char buff2[0x10] = { 0 };
	//char *asmBuff2 = buff2;

	////获取@结构
	//wechatText pWxid = { 0 };
	//wechatText pMessage = { 0 };
	////缓冲区
	//char buff[0x81C] = { 0 };
	//__asm {
	//	mov eax, asmWxid2
	//	push eax
	//	mov ecx, asmBuff2
	//	call callAdd2
	//}

	//pWxid.pStr = wxid;
	//pWxid.strLen = wcslen(wxid);
	//pWxid.iStrLen = wcslen(wxid) * 2;

	//pMessage.pStr = message;
	//pMessage.strLen = wcslen(message);
	//pMessage.iStrLen = wcslen(message) * 2;
	//char* asmWxid = (char*)&pWxid.pStr;
	//char* asmMsg = (char*)&pMessage.pStr;
	////call地址
	//DWORD callAdd = (DWORD)dllAdress + 0x2EB4E0;

#pragma endregion


	//测试是否通畅
	//////TCHAR *buff = (TCHAR*)malloc(999999);//最终返回的字符串
	//////swprintf_s(buff, 4096, L"123132");



	//////FILE* fp = NULL; // 文件指针
	//////errno_t eResult;
	////////随机数
	//////int r = rand() % (99999999 - 1 + 1) + 1;

	//////char filename[50] = { 0 };
	//////sprintf_s(filename, "d:\\%d.txt", r);

	//////// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
	//////eResult = fopen_s(&fp, filename, "ab+");

	//////// 打开文件失败
	//////if (eResult != 0)
	//////	exit(-1);

	//////fwrite(buff, sizeof(wchar_t), wcslen(buff), fp);
	//////fclose(fp);
	//////free((void *)buff);




	//HMODULE dllAdress = GetModuleHandleA("WeChatWin.dll");
	//DWORD callAddress_SendText = (DWORD)dllAdress + 0x2EB4E0;
	//
	//TEXT_WX wxId(L"5652594809@chatroom");//chatroomid
	//wchar_t tempmsg[100] = { 0 };
	//swprintf_s(tempmsg, L"@%s %s", L"李文章", "你好，我在测试");//membernickname
	//TEXT_WX wxMsg(tempmsg);

	// 

	//ROOM_AT roomAt;
	//roomAt.at_WxidList = (DWORD)&"wen-zhang9299";
	//roomAt.at_end1 = roomAt.at_WxidList + 5 * 4;
	//roomAt.at_end2 = roomAt.at_end1;

	////定义一个缓冲区
	//BYTE buff[0x81C] = { 0 };

	////执行汇编调用
	//__asm
	//{
	//	lea edx, wxId

	//	//传递参数
	//	push 0x1

	//	lea eax, roomAt
	//	push eax

	//	//微信消息内容
	//	lea ebx, wxMsg

	//	push ebx
	//	lea ecx, buff

	//	//调用函数
	//	call callAddress_SendText

	//	//平衡堆栈
	//	add esp, 0xC
//}
}