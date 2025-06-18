﻿// RemoteCtrl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ServerSocket.h"


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
			CServerSocket* pserver = CServerSocket::getInstance(); // 获取单例实例
            if (pserver->InitSocket() == false) {
					MessageBox(NULL, _T("Socket环境初始化失败,检查网络设置"), _T("初始化错误"), MB_OK | MB_ICONERROR);
					exit(0);
            }
            int count = 0;
            if (pserver) {
                if (pserver->AcceptClient() == false) {
                if (count >= 3) {
                    MessageBox(NULL, _T("多次无法接入，退出"), _T("连接错误"), MB_OK | MB_ICONERROR);
					exit(0);
                }
					MessageBox(NULL, _T("客户端连接失败"), _T("连接错误"), MB_OK | MB_ICONERROR);
                    count++;
                    
                }
            }
			int ret = pserver->DealCommand();

        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        //123
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
