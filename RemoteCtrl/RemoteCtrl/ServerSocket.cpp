#include "pch.h"
#include "ServerSocket.h"

CServerSocket* CServerSocket::m_instance = nullptr;  // ��̬��Ա������ʼ��
CServerSocket* server = CServerSocket::getInstance();  // ��ȡ����ʵ��
CServerSocket::CHelper CServerSocket::m_helper;  // ȷ�������ڳ���ʼʱ������