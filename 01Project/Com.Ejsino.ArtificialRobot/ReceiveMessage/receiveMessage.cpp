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

//������ƫ��
DWORD dwParam = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x126D7F8;

//������ҪHOOK�ĵ�ַ
DWORD dwHookAddr = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x315E98 - 5;

//���ص�ַ
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
	//������Ҫ���һ������������
	// E9 11051111(��������ת�ĵط�����ط�����һ�������ַ ���Ǹ���hook��ַ����ת�Ĵ����ַ�ľ�����������)
	JmpCode[0] = 0xE9;
	//������ת�ľ��빫ʽ�ǹ̶���
	//���㹫ʽΪ ��ת�ĵ�ַ(Ҳ�������Ǻ����ĵ�ַ) - hook�ĵ�ַ - hook���ֽڳ���
	DWORD jmpLen = (DWORD)jmpAdd - dwHookAddr - HOOK_LEN;
	*(DWORD*)& JmpCode[1] = jmpLen;

	//hook�ڶ��� �ȱ��ݽ�Ҫ�����Ǹ��ǵ�ַ������ ����Ϊ����hook�ĳ��� HOOK_LEN 5���ֽ�

	//��ȡ���̾��

	wchar_t debugBuff[0x100] = { 0 };
	//��������
	if (ReadProcessMemory(hWHND, (LPVOID)dwHookAddr, backCode, HOOK_LEN, NULL) == 0) {

		swprintf_s(debugBuff, 256, L"hook��ַ=%p  ���̾��=%p  ��������=%d", dwHookAddr, hWHND, GetLastError());
		MessageBox(NULL, debugBuff, L"��ȡʧ��", MB_OK);
		//MessageBox(NULL,"hook��ַ�����ݶ�ȡʧ��","��ȡʧ��",MB_OK);
		return;
	}

	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)dwHookAddr, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hookд��ʧ�ܣ������滻ʧ��", L"����", MB_OK);
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
//��ת�����ĺ��� �����Լ���

VOID __declspec(naked) HookHandler()
{
	//pushad: �����е�32λͨ�üĴ���ѹ���ջ
	//pushfd:Ȼ��32λ��־�Ĵ���EFLAGSѹ���ջ
	//popad:�����е�32λͨ�üĴ���ȡ����ջ
	//popfd:��32λ��־�Ĵ���EFLAGSȡ����ջ
	//�ȱ���Ĵ���
	// ʹ��pushad��Щ���㻹�ǲ�̫�ȶ�  �����ñ����ѼĴ�����ֵ�������� �����ɿ���
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
	//Ȼ����ת�������Լ��Ĵ����� ��������
	printLog(*(DWORD*)(*(DWORD*)cEsp));
	//retCallAdd = WinAdd + 0x1065EC4;
	//retCallAdd2 = WinAdd + 0x259E90;

	//Ȼ���ڻ�ԭ������֮ǰ����������
	/*popad
		popfd  ��̫�ɿ��ָ���ȫ ���Բ��б����ķ�ʽ���������ٸ�ֵ��ȥ*/
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

//[[esp]] + 0x40 ���ͷ� / Ⱥ��vxid
//[[esp]] + 0x68 ��Ϣ����
//[[esp]] + 0x114 Ⱥ��Ϣ�ķ�����vxid / ������ϢֵΪ��

VOID printLog(DWORD msgAdd)
{
	DWORD wxidAdd = msgAdd + 0x40;//Ⱥid��ַ
	DWORD wxid2Add = msgAdd + 0x114;//������id��ַ
	DWORD messageAdd = msgAdd + 0x68;//��Ϣ���ݵ�ַ
	DWORD ImageUrlAdd = msgAdd + 0x150;//��Ϣ�е�ͼƬ��ַ

	//��Ϣ����
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

		//TCHAR *buff = (TCHAR*)malloc(999999);//���շ��ص��ַ���
		//swprintf_s(buff, 4096, L"%s|&|%s|&|%s|&|%s|&|%s", *((LPVOID*)wxidAdd), *((LPVOID*)wxid2Add), *((LPVOID*)messageAdd), msgTp, *((LPVOID*)ImageUrlAdd));
		//size_t wcsChars = wcslen(buff);
		//size_t sizeRequired = WideCharToMultiByte(936, 0, buff, -1,
		//	NULL, 0, NULL, NULL);
		//char *target = (char*)malloc(sizeRequired);
		//WideCharToMultiByte(936, 0, buff, -1,
		//	target, sizeRequired, NULL, NULL);
		//SendToClient(target);
		//free((void *)buff);

#pragma region �����һ����¼��ת��д�ļ�

		TCHAR *buff = (TCHAR*)malloc(999999);//���շ��ص��ַ���
		swprintf_s(buff, 4096, L"%s|&|%s|&|%s|&|%s|&|%s", *((LPVOID*)wxidAdd), *((LPVOID*)wxid2Add), *((LPVOID*)messageAdd), msgTp, *((LPVOID*)ImageUrlAdd));

		FILE* fp = NULL; // �ļ�ָ��
				//char* szAppendStr = "Text";
		errno_t eResult;

		//�����
		int r = rand() % (99999999 - 1 + 1) + 1;

		char filename[50] = { 0 };
		sprintf_s(filename, "d:\\jianting\\%d.txt", r);

		// �Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�, ���û�д��ļ������д�����Ȼ���Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�
		eResult = fopen_s(&fp, filename, "ab+");
		

		// ���ļ�ʧ��
		if (eResult != 0)
			exit(-1);

		fwrite(buff, sizeof(wchar_t), wcslen(buff), fp);
		
		//fputws(buff, fp);
		fclose(fp);
		free((void *)buff);
#pragma endregion



#pragma region  ������Ϣд���ļ���ת��



		//FILE* fp = NULL; // �ļ�ָ��
		////char* szAppendStr = "Text";
		//errno_t eResult;

		////�����
		//int r = rand() % (99999999 - 1 + 1) + 1;

		//char filename[50] = { 0 };
		//sprintf_s(filename, "d:\\jianting\\%d.txt", r);

		//// �Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�, ���û�д��ļ������д�����Ȼ���Ը��ӷ�ʽ�򿪿ɶ�/д���ļ�
		//eResult = fopen_s(&fp, filename, "a+");

		//// ���ļ�ʧ��
		//if (eResult != 0)
		//	exit(-1);

		//// ��׷������д���ļ�ָ�뵱ǰ��λ��


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


		////��Ϣ����
		//TCHAR *bf3 = (TCHAR *)*((LPVOID*)messageAdd);
		//size_t size3 = WideCharToMultiByte(936, 0, bf3, -1,
		//	NULL, 0, NULL, NULL);

		//char *target3 = (char*)malloc(size3);
		//WideCharToMultiByte(936, 0, bf3, -1,
		//	target3, size3, NULL, NULL);
		//fputs((char *)target3, fp);
		//fputs((char *)"|&|", fp);


		////��Ϣ����
		//TCHAR *bf4 = msgTp;
		//size_t size4 = WideCharToMultiByte(936, 0, bf4, -1,
		//	NULL, 0, NULL, NULL);

		//char *target4 = (char*)malloc(size4);
		//WideCharToMultiByte(936, 0, bf4, -1,
		//	target4, size4, NULL, NULL);

		//fputs((char *)target4, fp);
		//fputs((char *)"|&|", fp);


		////��ϢͼƬ��ַ
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
		MessageBox(NULL, L"hookж��ʧ��", L"����", MB_OK);
		return;
	}
}