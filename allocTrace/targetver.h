#pragma once

// ���� SDKDDKVer.h ��������õ���߰汾�� Windows ƽ̨��

// ���ҪΪ��ǰ�� Windows ƽ̨����Ӧ�ó�������� WinSDKVer.h������
// WIN32_WINNT ������ΪҪ֧�ֵ�ƽ̨��Ȼ���ٰ��� SDKDDKVer.h��
#ifdef WIN
#define _WIN32_WINNT 0x0500
#include <SDKDDKVer.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#endif
