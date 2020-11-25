
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

//HOOK��Ҫ�õ��ĵ�ȫ�ֱ���
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

//����7000���û��б�
list<USER_INFO> userInfoList(1);

//�û���Ϣ�ṹ��
struct UserInfo
{
	wchar_t UserId[80];
	wchar_t UserNumber[80];
	wchar_t UserRemark[80];
	wchar_t UserNickName[80];
};

//************************************************************
// ��������: HookGetFriendList
// ����˵��: HOOK��ȡ�����б��call 
// ��    ��: GuiShou
// ʱ    ��: 2019/7/4
// ��    ��: void
// �� �� ֵ: void 
//************************************************************
void HookGetFriendList()
{

	DWORD wxBaseAddress = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll"));
	//��Ҫhook�ĵ�ַ
	hookAddress = wxBaseAddress + 0x428965;
	//�����ǵ�CALLĿ��
	overWritedCallAdd = wxBaseAddress + 0x481F20;

	//���صĵ�ַ
	jumBackAddress = hookAddress + 5;

	//��װ��ת����
	BYTE jmpCode[5] = { 0 };
	jmpCode[0] = 0xE9;


	//����ƫ��
	*(DWORD*)& jmpCode[1] = (DWORD)GetUserListInfo - hookAddress - 5;

	// ������ǰ���������ڻ�ԭ
	DWORD OldProtext = 0;

	// ��ΪҪ�������д�����ݣ�����Ϊ������ǲ���д�ģ�������Ҫ�޸�����
	VirtualProtect((LPVOID)hookAddress, 5, PAGE_EXECUTE_READWRITE, &OldProtext);

	//����ԭ�е�ָ��
	memcpy(bBackCode, (void*)hookAddress, 5);

	//д���Լ��Ĵ���
	memcpy((void*)hookAddress, jmpCode, 5);

	// ִ�����˲���֮����Ҫ���л�ԭ
	VirtualProtect((LPVOID)hookAddress, 5, OldProtext, &OldProtext);
}

//************************************************************
// ��������: GetUserListInfo
// ����˵��: ��ȡ�û���Ϣ
// ��    ��: GuiShou
// ʱ    ��: 2019/7/4
// ��    ��: void
// �� �� ֵ: void 
//************************************************************
__declspec(naked) void GetUserListInfo()
{
	//�����ֳ�
	__asm
	{
		//��ȡeax�Ĵ������ݣ�����һ��������
		mov r_esi, esi

		//����Ĵ���
		pushad
		pushf
	}

	//���ý�����Ϣ�ĺ���
	SendUserListInfo();

	//�ָ��ֳ�
	__asm
	{
		popf
		popad

		//���䱻���ǵĴ���
		call overWritedCallAdd

		//���ر�HOOKָ�����һ��ָ��
		jmp jumBackAddress
	}
}

//************************************************************
// ��������: SendUserListInfo
// ����˵��: ���ͺ����б�
// ��    ��: GuiShou
// ʱ    ��: 2019/7/4
// ��    ��: void
// �� �� ֵ: void 
//************************************************************
void SendUserListInfo()
{
	//����΢�źš�Ⱥ��
	wstring wxid1 = GetMsgByAddress(r_esi + 0x10);
	wstring wxName = GetMsgByAddress(r_esi + 0x8C);
	wstring v1 = GetMsgByAddress(r_esi + 0x58);
	//����΢���ǳơ�Ⱥ�ǳ�
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


	////���͵����ƶ�
	//HWND hWnd = FindWindow(NULL, TEXT("΢������"));
	//if (hWnd == NULL)
	//{
	//	//�����߳� ������Ϣ ��ֹ©����ϵ��
	//	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReSendUser, user, 0, NULL);
	//}

	//COPYDATASTRUCT userinfo;
	//userinfo.dwData = WM_GetFriendList;//����һ����ֵ, ������������־��
	//userinfo.cbData = sizeof(UserInfo);// strlen(szSendBuf);//�����͵����ݵĳ�
	//userinfo.lpData = user;// szSendBuf;//�����͵����ݵ���ʼ��ַ(����ΪNULL)
	//SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&userinfo);
}


//************************************************************
// ��������: GetMsgByAddress
// ����˵��: �ӵ�ַ�л�ȡ��Ϣ����
// ��    ��: GuiShou
// ʱ    ��: 2019/7/6
// ��    ��: DWORD memAddress  Ŀ���ַ
// �� �� ֵ: LPCWSTR	��Ϣ����
//************************************************************
LPCWSTR GetMsgByAddress(DWORD memAddress)
{
	//��ȡ�ַ�������
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength == 0)
	{
		WCHAR* msg = new WCHAR[1];
		msg[0] = 0;
		return msg;
	}

	WCHAR* msg = new WCHAR[msgLength + 1];
	ZeroMemory(msg, msgLength + 1);

	//��������
	wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
	return msg;
}