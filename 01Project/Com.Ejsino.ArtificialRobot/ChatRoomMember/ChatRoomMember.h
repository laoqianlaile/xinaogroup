#pragma once
#pragma once
#include "stdafx.h"
#define D3D_API_API __declspec(dllexport)

//ԭ����50
struct UserInfo {
	wchar_t UserId[100];
	wchar_t UserNumber[100];
	wchar_t UserNickName[100];
	wchar_t UserGroupName[100];
};


extern "C" D3D_API_API VOID GetChatRoomUser(wchar_t* chatroomwxid);//��ȡ΢��Ⱥ���Ա�б����ԣ�
VOID GetChatRoomUser(wchar_t* chatroomwxid);
VOID GetUserInfoByWxid(wchar_t* userwxid, UserInfo *userinfo);
LPCWSTR ShowChatRoomUser(wchar_t* chatroomwxid);

//���Ͱ�����Ϣwchar_t* chatroomid, wchar_t* memberwxid, wchar_t* membernickname, wchar_t* msg
extern "C" D3D_API_API void SendRoomAtMsg(wchar_t* suibianchuan);

//extern "C" D3D_API_API VOID GetUserInfoByWxid(wchar_t* userwxid);