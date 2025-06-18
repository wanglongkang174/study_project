#include "pch.h"
#include "ServerSocket.h"

CServerSocket* CServerSocket::m_instance = nullptr;  // 静态成员变量初始化
CServerSocket* server = CServerSocket::getInstance();  // 获取单例实例
CServerSocket::CHelper CServerSocket::m_helper;  // 确保单例在程序开始时被创建