
#include "stdafx.h"
#include "FriendList.h"
#include <stdio.h>
#include "shellapi.h"
#include <string>
#include <sstream>
#include <string>
#include <iomanip>
#include <strstream>
#include <list>
#include <iostream>
#include <tuple>
#include<fstream>
#pragma comment(lib, "Version.lib")
using namespace std;

//HOOK需要用到的的全局变量
DWORD overWritedCallAdd;
DWORD hookAddress;
DWORD jumBackAddress;
BYTE bBackCode[5] = { 0 };
DWORD r_esi = 0;

typedef tuple <
	//wxid1
	wstring,
	//wxName
	wstring,
	//v1
	wstring,
	//nickName
	wstring
> USER_INFO;

//定义7000个用户列表
list<USER_INFO> userInfoList(1);

//用户信息结构体
struct UserInfo
{
	wchar_t UserId[80];
	wchar_t UserNumber[80];
	wchar_t UserRemark[80];
	wchar_t UserNickName[80];
};

//************************************************************
// 函数名称: HookGetFriendList
// 函数说明: HOOK获取好友列表的call 
// 作    者: GuiShou
// 时    间: 2019/7/4
// 参    数: void
// 返 回 值: void 
//************************************************************
void HookGetFriendList()
{

	DWORD wxBaseAddress = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll"));
	//需要hook的地址
	hookAddress = wxBaseAddress + 0x428965;
	//被覆盖的CALL目标
	overWritedCallAdd = wxBaseAddress + 0x481F20;

	//跳回的地址
	jumBackAddress = hookAddress + 5;

	//组装跳转数据
	BYTE jmpCode[5] = { 0 };
	jmpCode[0] = 0xE9;


	//计算偏移
	*(DWORD*)& jmpCode[1] = (DWORD)GetUserListInfo - hookAddress - 5;

	// 保存以前的属性用于还原
	DWORD OldProtext = 0;

	// 因为要往代码段写入数据，又因为代码段是不可写的，所以需要修改属性
	VirtualProtect((LPVOID)hookAddress, 5, PAGE_EXECUTE_READWRITE, &OldProtext);

	//保存原有的指令
	memcpy(bBackCode, (void*)hookAddress, 5);

	//写入自己的代码
	memcpy((void*)hookAddress, jmpCode, 5);

	// 执行完了操作之后需要进行还原
	VirtualProtect((LPVOID)hookAddress, 5, OldProtext, &OldProtext);
}

//************************************************************
// 函数名称: GetUserListInfo
// 函数说明: 获取用户信息
// 作    者: GuiShou
// 时    间: 2019/7/4
// 参    数: void
// 返 回 值: void 
//************************************************************
__declspec(naked) void GetUserListInfo()
{
	//保存现场
	__asm
	{
		//提取eax寄存器内容，放在一个变量中
		mov r_esi, esi

		//保存寄存器
		pushad
		pushf
	}

	//调用接收消息的函数
	SendUserListInfo();

	//恢复现场
	__asm
	{
		popf
		popad

		//补充被覆盖的代码
		call overWritedCallAdd

		//跳回被HOOK指令的下一条指令
		jmp jumBackAddress
	}
}

//************************************************************
// 函数名称: SendUserListInfo
// 函数说明: 发送好友列表
// 作    者: GuiShou
// 时    间: 2019/7/4
// 参    数: void
// 返 回 值: void 
//************************************************************
void SendUserListInfo()
{
	//个人微信号、群号
	wstring wxid1 = GetMsgByAddress(r_esi + 0x10);
	wstring wxName = GetMsgByAddress(r_esi + 0x8C);
	wstring v1 = GetMsgByAddress(r_esi + 0x58);
	//个人微信昵称、群昵称
	wstring nickName = GetMsgByAddress(r_esi + 0x8C);
	USER_INFO userInfo(wxid1, wxName, v1, nickName);

	for (auto& userInfoOld : userInfoList)
	{
		wstring wxid = get<0>(userInfoOld);
		if (wxid == wxid1)
		{
			return;
		}
	}
	userInfoList.push_front(userInfo);


	UserInfo *user = new UserInfo;
	LPVOID pUserWxid = *((LPVOID *)(r_esi + 0x10));
	LPVOID pUserNumber = *((LPVOID *)(r_esi + 0x44));
	LPVOID pUserNick = *((LPVOID *)(r_esi + 0x8C));
	LPVOID pUserReMark = *((LPVOID *)(r_esi + 0x78));


	swprintf_s(user->UserId, L"%s", (wchar_t*)pUserWxid);
	swprintf_s(user->UserNumber, L"%s", (wchar_t*)pUserNumber);
	swprintf_s(user->UserNickName, L"%s", (wchar_t*)pUserNick);
	swprintf_s(user->UserRemark, L"%s", (wchar_t*)pUserReMark);


	////发送到控制端
	//HWND hWnd = FindWindow(NULL, TEXT("微信助手"));
	//if (hWnd == NULL)
	//{
	//	//创建线程 发送消息 防止漏掉联系人
	//	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReSendUser, user, 0, NULL);
	//}

	//COPYDATASTRUCT userinfo;
	//userinfo.dwData = WM_GetFriendList;//保存一个数值, 可以用来作标志等
	//userinfo.cbData = sizeof(UserInfo);// strlen(szSendBuf);//待发送的数据的长
	//userinfo.lpData = user;// szSendBuf;//待发送的数据的起始地址(可以为NULL)
	//SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&userinfo);
}


//************************************************************
// 函数名称: GetMsgByAddress
// 函数说明: 从地址中获取消息内容
// 作    者: GuiShou
// 时    间: 2019/7/6
// 参    数: DWORD memAddress  目标地址
// 返 回 值: LPCWSTR	消息内容
//************************************************************
LPCWSTR GetMsgByAddress(DWORD memAddress)
{
	//获取字符串长度
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength == 0)
	{
		WCHAR* msg = new WCHAR[1];
		msg[0] = 0;
		return msg;
	}

	WCHAR* msg = new WCHAR[msgLength + 1];
	ZeroMemory(msg, msgLength + 1);

	//复制内容
	wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
	return msg;
}