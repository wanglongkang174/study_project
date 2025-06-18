#pragma once
#include "pch.h"
#include "framework.h"

class CServerSocket
{
public:
	static CServerSocket* getInstance() {
		if (m_instance == nullptr) {
			m_instance = new CServerSocket();
		}
		return m_instance;
	}
	bool InitSocket() {
		if (m_sock == -1) return false;
		sockaddr_in serv_adr;
		memset(&serv_adr, 0, sizeof(serv_adr));
		serv_adr.sin_family = AF_INET;
		serv_adr.sin_addr.s_addr = INADDR_ANY;
		serv_adr.sin_port = htons(9527);
		if(bind(m_sock, (sockaddr*)&serv_adr, sizeof(serv_adr))==-1) return false;
		if(listen(m_sock, 1)==-1) return false;
		return true;
	}
	bool AcceptClient() {
		sockaddr_in clnt_adr;
		int clnt_adr_sz = sizeof(clnt_adr);
		m_client = accept(m_sock, (sockaddr*)&clnt_adr, &clnt_adr_sz);
		if (m_client == -1) return false;
		return true;
	}
	int DealCommand() {
		char buffer[1024];
		while (true) {
			int len = recv(m_client, buffer, sizeof(buffer), 0);
			if (len <= 0) return -1; // 连接关闭或错误
			
		}
	}
	bool Send(const char* pdata, size_t nSize) {
		if (m_client == -1) return false; // 确保客户端已连接
		if (send(m_client, pdata, nSize, 0) == -1) return false;
		return true;
	}
private:
	SOCKET m_sock;
	SOCKET m_client;
	CServerSocket() {
		m_sock = -1;  // 初始化套接字为无效值
		m_client = -1; // 初始化客户端套接字为无效值
		if (InitSockEnv() == false) {
			MessageBox(NULL,_T("Socket环境初始化失败,检查网络设置"),_T("初始化错误"),MB_OK|MB_ICONERROR);
			exit(0);
		}
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		
	}
	CServerSocket(const CServerSocket&) = default;
	CServerSocket& operator=(const CServerSocket&) = default;
	~CServerSocket() {
		closesocket(m_sock);
		WSACleanup();
	}
	BOOL InitSockEnv() {
		WSADATA data;
		return WSAStartup(MAKEWORD(1, 1), &data) == 0;
	}
	static void releaseInstance() {
		if (CServerSocket::m_instance) {
			delete CServerSocket::m_instance;  // 清理单例实例
			CServerSocket::m_instance = nullptr;
		}
	}
	static CServerSocket* m_instance;
	class CHelper {
	public:
		CHelper() {
			CServerSocket::getInstance();  // 确保单例在程序开始时被创建
		}
		~CHelper() {
			releaseInstance();
		}
	};
	static CHelper m_helper;  // 静态成员，确保在程序结束时释放单例实例
};

