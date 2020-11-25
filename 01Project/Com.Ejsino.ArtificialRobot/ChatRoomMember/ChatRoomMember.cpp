// ChatRoomMember.cpp : ���� DLL Ӧ�ó���ĵ���������
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

//���Ի�ȡȺ���Ա�б�
VOID GetChatRoomUser(wchar_t* chatroomwxid) {
	LPCWSTR UserStr = ShowChatRoomUser(chatroomwxid);

	HWND h = FindWindow(NULL, L"WeChatBot");
	if (h == NULL)
	{
		MessageBoxA(NULL, "δ�ҵ�ChatRoomMember����", "����", 0);
		return;
	}
	COPYDATASTRUCT cpd;
	cpd.dwData = 2;//����һ����ֵ, ������������־��
	cpd.cbData = lstrlenW(UserStr) * 2;//�����͵����ݵĳ�
	cpd.lpData = (void*)UserStr;

	SendMessage(h, WM_COPYDATA, 0, (LPARAM)& cpd);
	free((void *)UserStr);
}

//************************************************************
// ��������: ShowChatRoomUser
// ����˵��: ��ʾȺ��Ա
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: void
// �� �� ֵ: void 
//************************************************************
LPCWSTR ShowChatRoomUser(wchar_t* chatroomwxid)
{
	//1.�Ȼ�ȡȺ��Ա��΢��ID
	struct wxStr
	{
		wchar_t * pStr;
		int strLen;
		int strMaxLen;
		int fill = 0;
		int fill2 = 0;
	};
	//׼��������
	DWORD dwWxidArr = 0;	//����΢��ID���ݵĵ�ַ
	char buff[0x164] = { 0 };

	char * asmWxid = (char *)&chatroomwxid;

	//����call
	DWORD dwCall1 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x414680;
	DWORD dwCall2 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x2BCF20;
	DWORD dwCall3 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x41CAE0;
	DWORD dwCall4 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x415170;

	//��ȡȺ��Ա
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

	//�õ�΢��ID
	wchar_t tempWxid[0x100] = { 0 };
	DWORD userList = *((DWORD *)dwWxidArr);
	DWORD testTmp = dwWxidArr + 0xB0;
	int Len = *((int *)testTmp);


	wchar_t *endStr = (wchar_t*)malloc(999999);//���շ��ص��ַ���
	wchar_t *up = &endStr[0];

	//Ⱥ����Ϣ
	UserInfo wxGroup = { 0 };
	GetUserInfoByWxid(chatroomwxid, &wxGroup);
	int h = swprintf_s(up, 200, L"%s|&|%s_&&_", chatroomwxid, wxGroup.UserNickName);//ԭ����100
	up += h;

	for (int i = 0; i < Len; i++)
	{
		DWORD temWxidAdd = userList + (i * 20);
		swprintf_s(tempWxid, 256, L"%s", (wchar_t*)*((LPVOID *)temWxidAdd));

		// @todo ��ӡwid
		UserInfo u = { 0 };
		GetUserInfoByWxid(tempWxid, &u);
		int l = swprintf_s(up, 200, L"%s|&|%s|&|%s_&_", u.UserId, u.UserNumber, u.UserNickName);//ԭ����100
		up += l;

	}
	return endStr;
}


//************************************************************
// ��������: GetUserInfoByWxid
// ����˵��: ͨ��΢��ID��ȡ�û���Ϣ
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: userwxid ΢��ID
// �� �� ֵ: void 
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
	DWORD userData = 0;		//�û����ݵĵ�ַ
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


	DWORD wxidAdd = userData + 0x8;		//΢��ID
	DWORD wxNickAdd = userData + 0x64;	//�ǳ�
	DWORD wxuserIDAdd = userData + 0x1C;	//΢���˺�



	swprintf_s(userinfo->UserId, 100, L"%s", (wchar_t*)(*((LPVOID*)wxidAdd)));
	swprintf_s(userinfo->UserNickName, 100, L"%s", (wchar_t*)(*((LPVOID*)wxNickAdd)));
	swprintf_s(userinfo->UserNumber, 100, L"%s", (wchar_t*)(*((LPVOID*)wxuserIDAdd)));
}



//���Ͱ�����Ϣ��Ҫ�����ݽṹ
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

//���ڵ���΢�ŷ�����Ϣcall�Ľṹ��
struct wxMsg
{
	wchar_t* pMsg;		//΢��ID
	int msgLen;			//ID����
	int buffLen;		//����������
	int fill1 = 0;
	int fill2 = 0;
};


//************************************************************
// ��������: SendRoomAtMsg
// ����˵��: ���Ͱ�����Ϣ
// ��    ��: GuiShou
// ʱ    ��: 2019/7/26
// ��    ��: chatroomid ȺID memberwxid Ⱥ��Ա΢��ID membernicknameȺ��Ա�ǳ� msg ��Ϣ����
// �� �� ֵ: void  wchar_t* chatroomid, wchar_t* memberwxid, wchar_t* membernickname, wchar_t* msg
//************************************************************
void SendRoomAtMsg(wchar_t* wxRoomID)
{
	//�õ�������Ϣ��call�ĵ�ַ
	DWORD dwSendCallAddr = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll")) + 0x2EB4E0;

	//��װ΢��ID/ȺID�Ľṹ��
	wxMsg id = { 0 };
	id.pMsg = wxRoomID;
	id.msgLen = wcslen(wxRoomID);
	id.buffLen = wcslen(wxRoomID) * 2;

	//��Ϣ����
	wxMsg text = { 0 };
	text.pMsg = L"�յ�";
	text.msgLen = wcslen(L"�յ�");
	text.buffLen = wcslen(L"�յ�") * 2;

	//ȡ��΢��ID����Ϣ�ĵ�ַ
	char* pWxid = (char*)&id.pMsg;
	char* pWxmsg = (char*)&text.pMsg;

	char buff[0x81C] = { 0 };

	//����΢�ŷ�����Ϣcall
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


#pragma region ���ܵ�
	//HMODULE dllAdress = GetModuleHandleA("WeChatWin.dll");
	//DWORD callAdd2 = (DWORD)dllAdress + 0x2EB4E0;
	//wechatText pWxid2 = { 0 };
	//pWxid2.pStr = fromWxid;
	//pWxid2.strLen = wcslen(fromWxid);
	//pWxid2.iStrLen = wcslen(fromWxid) * 2;

	//char* asmWxid2 = (char*)&pWxid2.pStr;
	//char buff2[0x10] = { 0 };
	//char *asmBuff2 = buff2;

	////��ȡ@�ṹ
	//wechatText pWxid = { 0 };
	//wechatText pMessage = { 0 };
	////������
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
	////call��ַ
	//DWORD callAdd = (DWORD)dllAdress + 0x2EB4E0;

#pragma endregion


	//�����Ƿ�ͨ��
	//////TCHAR *buff = (TCHAR*)malloc(999999);//���շ��ص��ַ���
	//////swprintf_s(buff, 4096, L"123132");



	//////FILE* fp = NULL; // �ļ�ָ��
	//////errno_t eResult;
	////////�����
	//////int r = rand() % (99999999 - 1 + 1) + 1;

	//////char filename[50] = { 0 };
	//////sprintf_s(filename, "d:\\%d.txt", r);

	//////// �Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�, ���û�д��ļ������д�����Ȼ���Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�
	//////eResult = fopen_s(&fp, filename, "ab+");

	//////// ���ļ�ʧ��
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
	//swprintf_s(tempmsg, L"@%s %s", L"������", "��ã����ڲ���");//membernickname
	//TEXT_WX wxMsg(tempmsg);

	// 

	//ROOM_AT roomAt;
	//roomAt.at_WxidList = (DWORD)&"wen-zhang9299";
	//roomAt.at_end1 = roomAt.at_WxidList + 5 * 4;
	//roomAt.at_end2 = roomAt.at_end1;

	////����һ��������
	//BYTE buff[0x81C] = { 0 };

	////ִ�л�����
	//__asm
	//{
	//	lea edx, wxId

	//	//���ݲ���
	//	push 0x1

	//	lea eax, roomAt
	//	push eax

	//	//΢����Ϣ����
	//	lea ebx, wxMsg

	//	push ebx
	//	lea ecx, buff

	//	//���ú���
	//	call callAddress_SendText

	//	//ƽ���ջ
	//	add esp, 0xC
//}
}