#include "pch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "receiveMessage.h"

#define HOOK_LEN 5
HANDLE hWHND = 0;
BYTE backCode[HOOK_LEN] = { 0 };
DWORD WinAdd = 0;
DWORD retCallAdd = 0;
DWORD retCallAdd2 = 0;
DWORD retAdd = 0;

//参数的偏移
DWORD dwParam = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x126D7F8;

//计算需要HOOK的地址
DWORD dwHookAddr = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x315E98 - 5;

//返回地址
DWORD RetAddr = dwHookAddr + 5;

VOID HookHandler();
VOID StartHook(LPVOID jmpAdd);
VOID printLog(DWORD msgAdd);


VOID Hook()
{
	hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	WinAdd = (DWORD)LoadLibrary(L"WeChatWin.dll");
	StartHook(&HookHandler);
}

VOID StartHook(LPVOID jmpAdd)
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//我们需要组成一段这样的数据
	// E9 11051111(这里是跳转的地方这个地方不是一个代码地址 而是根据hook地址和跳转的代码地址的距离计算出来的)
	JmpCode[0] = 0xE9;
	//计算跳转的距离公式是固定的
	//计算公式为 跳转的地址(也就是我们函数的地址) - hook的地址 - hook的字节长度
	DWORD jmpLen = (DWORD)jmpAdd - dwHookAddr - HOOK_LEN;
	*(DWORD*)& JmpCode[1] = jmpLen;

	//hook第二步 先备份将要被我们覆盖地址的数据 长度为我们hook的长度 HOOK_LEN 5个字节

	//获取进程句柄

	wchar_t debugBuff[0x100] = { 0 };
	//备份数据
	if (ReadProcessMemory(hWHND, (LPVOID)dwHookAddr, backCode, HOOK_LEN, NULL) == 0) {

		swprintf_s(debugBuff, 256, L"hook地址=%p  进程句柄=%p  错误类型=%d", dwHookAddr, hWHND, GetLastError());
		MessageBox(NULL, debugBuff, L"读取失败", MB_OK);
		//MessageBox(NULL,"hook地址的数据读取失败","读取失败",MB_OK);
		return;
	}

	//真正的hook开始了 把我们要替换的函数地址写进去 让他直接跳到我们函数里面去然后我们处理完毕后再放行吧！
	if (WriteProcessMemory(hWHND, (LPVOID)dwHookAddr, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook写入失败，函数替换失败", L"错误", MB_OK);
		return;
	}
}

DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;
//跳转过来的函数 我们自己的

VOID __declspec(naked) HookHandler()
{
	//pushad: 将所有的32位通用寄存器压入堆栈
	//pushfd:然后将32位标志寄存器EFLAGS压入堆栈
	//popad:将所有的32位通用寄存器取出堆栈
	//popfd:将32位标志寄存器EFLAGS取出堆栈
	//先保存寄存器
	// 使用pushad这些来搞还是不太稳定  还是用变量把寄存器的值保存下来 这样可靠点
	__asm {
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
		mov ecx, dwParam
	}
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	printLog(*(DWORD*)(*(DWORD*)cEsp));
	//retCallAdd = WinAdd + 0x1065EC4;
	//retCallAdd2 = WinAdd + 0x259E90;

	//然后在还原他进来之前的所有数据
	/*popad
		popfd  不太可靠恢复不全 所以才有变量的方式保存下来再赋值过去*/
	__asm {
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		//mov eax, retCallAdd
		//call aaa
		jmp RetAddr
	}
}

VOID SendToClient(char* msg)
{

	HWND h = FindWindow(NULL, L"WeChatBot");
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = strlen(msg);
	cpd.lpData = (void*)msg;

	SendMessage(h, WM_COPYDATA, 0, (LPARAM)& cpd);
	free((void *)msg);
}

//[[esp]] + 0x40 发送方 / 群的vxid
//[[esp]] + 0x68 消息内容
//[[esp]] + 0x114 群消息的发送者vxid / 个人消息值为零

VOID printLog(DWORD msgAdd)
{
	DWORD wxidAdd = msgAdd + 0x40;//群id地址
	DWORD wxid2Add = msgAdd + 0x114;//发送人id地址
	DWORD messageAdd = msgAdd + 0x68;//消息内容地址
	DWORD ImageUrlAdd = msgAdd + 0x150;//消息中的图片地址

	//消息类型
	DWORD msgType = *((DWORD*)(msgAdd + 0x30));
	wchar_t msgTp[50] = { 0 };


	if (*(LPVOID*)wxid2Add <= 0x0) {
	}
	else {
		switch (msgType)
		{
		case 0x01:
			memcpy(msgTp, L"word", sizeof(L"word"));
			break;
		case 0x03:
			memcpy(msgTp, L"image", sizeof(L"image"));
			break;
		default:
			memcpy(msgTp, L"no", sizeof(L"no"));
			return;
			break;
		}

		//TCHAR *buff = (TCHAR*)malloc(999999);//最终返回的字符串
		//swprintf_s(buff, 4096, L"%s|&|%s|&|%s|&|%s|&|%s", *((LPVOID*)wxidAdd), *((LPVOID*)wxid2Add), *((LPVOID*)messageAdd), msgTp, *((LPVOID*)ImageUrlAdd));
		//size_t wcsChars = wcslen(buff);
		//size_t sizeRequired = WideCharToMultiByte(936, 0, buff, -1,
		//	NULL, 0, NULL, NULL);
		//char *target = (char*)malloc(sizeRequired);
		//WideCharToMultiByte(936, 0, buff, -1,
		//	target, sizeRequired, NULL, NULL);
		//SendToClient(target);
		//free((void *)buff);

#pragma region 整理成一条记录不转码写文件

		TCHAR *buff = (TCHAR*)malloc(999999);//最终返回的字符串
		swprintf_s(buff, 4096, L"%s|&|%s|&|%s|&|%s|&|%s", *((LPVOID*)wxidAdd), *((LPVOID*)wxid2Add), *((LPVOID*)messageAdd), msgTp, *((LPVOID*)ImageUrlAdd));

		FILE* fp = NULL; // 文件指针
				//char* szAppendStr = "Text";
		errno_t eResult;

		//随机数
		int r = rand() % (99999999 - 1 + 1) + 1;

		char filename[50] = { 0 };
		sprintf_s(filename, "d:\\jianting\\%d.txt", r);

		// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
		eResult = fopen_s(&fp, filename, "ab+");
		

		// 打开文件失败
		if (eResult != 0)
			exit(-1);

		fwrite(buff, sizeof(wchar_t), wcslen(buff), fp);
		
		//fputws(buff, fp);
		fclose(fp);
		free((void *)buff);
#pragma endregion



#pragma region  逐条信息写入文件并转码



		//FILE* fp = NULL; // 文件指针
		////char* szAppendStr = "Text";
		//errno_t eResult;

		////随机数
		//int r = rand() % (99999999 - 1 + 1) + 1;

		//char filename[50] = { 0 };
		//sprintf_s(filename, "d:\\jianting\\%d.txt", r);

		//// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
		//eResult = fopen_s(&fp, filename, "a+");

		//// 打开文件失败
		//if (eResult != 0)
		//	exit(-1);

		//// 将追加内容写入文件指针当前的位置


		//TCHAR *buff = (TCHAR *)*((LPVOID*)wxidAdd);
		//size_t sizeRequired = WideCharToMultiByte(936, 0, buff, -1,
		//	NULL, 0, NULL, NULL);

		//char *target = (char*)malloc(sizeRequired);
		//WideCharToMultiByte(936, 0, buff, -1,
		//	target, sizeRequired, NULL, NULL);

		//fputs((char *)target, fp);
		//fputs((char *)"|&|", fp);



		//TCHAR *wxid2buff = (TCHAR *)*((LPVOID*)wxid2Add);
		//size_t sizeRequired2 = WideCharToMultiByte(936, 0, wxid2buff, -1,
		//	NULL, 0, NULL, NULL);

		//char *wxid2target = (char*)malloc(sizeRequired2);
		//WideCharToMultiByte(936, 0, wxid2buff, -1,
		//	wxid2target, sizeRequired2, NULL, NULL);

		//fputs((char *)wxid2target, fp);
		//fputs((char *)"|&|", fp);


		////消息内容
		//TCHAR *bf3 = (TCHAR *)*((LPVOID*)messageAdd);
		//size_t size3 = WideCharToMultiByte(936, 0, bf3, -1,
		//	NULL, 0, NULL, NULL);

		//char *target3 = (char*)malloc(size3);
		//WideCharToMultiByte(936, 0, bf3, -1,
		//	target3, size3, NULL, NULL);
		//fputs((char *)target3, fp);
		//fputs((char *)"|&|", fp);


		////消息类型
		//TCHAR *bf4 = msgTp;
		//size_t size4 = WideCharToMultiByte(936, 0, bf4, -1,
		//	NULL, 0, NULL, NULL);

		//char *target4 = (char*)malloc(size4);
		//WideCharToMultiByte(936, 0, bf4, -1,
		//	target4, size4, NULL, NULL);

		//fputs((char *)target4, fp);
		//fputs((char *)"|&|", fp);


		////消息图片地址
		//TCHAR *bf5 = (TCHAR *)*((LPVOID*)ImageUrlAdd);
		//size_t size5 = WideCharToMultiByte(936, 0, bf5, -1,
		//	NULL, 0, NULL, NULL);

		//char *target5 = (char*)malloc(size5);
		//WideCharToMultiByte(936, 0, bf5, -1,
		//	target5, size5, NULL, NULL);

		//fputs((char *)target5, fp);
		//fputs((char *)"|&|", fp);


		//fclose(fp);
		//free((void*)target);
		//free((void*)wxid2target);
		//free((void*)target3);
		//free((void*)target4);
		//free((void*)target5);
#pragma endregion
	}
}

VOID UnHook()
{
	if (WriteProcessMemory(hWHND, (LPVOID)dwHookAddr, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook卸载失败", L"错误", MB_OK);
		return;
	}
}