#pragma once
#include "pch.h"
#include "framework.h"
#pragma pack(push)
#pragma pack(1)
class CPacket
{
public:
	CPacket() :sHead(0), nLength(0), sCmd(0), sSum(0) {}
	CPacket(WORD nCmd, const BYTE* pData, size_t nSize) {
		sHead = 0xFEFF;
		nLength = nSize + 4;
		sCmd = nCmd;
		if (nSize > 0) {
			strData.resize(nSize);
			memcpy((void*)strData.c_str(), pData, nSize);
		}
		else {
			strData.clear();
		}
		sSum = 0;
		for (size_t j = 0; j < strData.size(); j++)
		{
			sSum += BYTE(strData[j]) & 0xFF;
		}
	}
	CPacket(const CPacket& pack) {
		sHead = pack.sHead;
		nLength = pack.nLength;
		sCmd = pack.sCmd;
		strData = pack.strData;
		sSum = pack.sSum;
	}
	CPacket(const BYTE* pData, size_t& nSize) {
		size_t i = 0;
		if (nSize < 2) { nSize = 0; return; } // ����Ƿ����㹻�ֽ�

		// ���Ұ�ͷ
		for (; i + 1 < nSize; i++) {
			WORD head;
			memcpy(&head, pData + i, sizeof(WORD));
			if (head == 0xFEFF) {
				sHead = head;
				i += 2;
				break;
			}
		}

		if (sHead != 0xFEFF) { nSize = 0; return; }

		if (i + sizeof(DWORD) + sizeof(WORD) + sizeof(WORD) > nSize) { nSize = 0; return; }
		memcpy(&nLength, pData + i, sizeof(DWORD)); i += 4;

		if (i + nLength > nSize) { nSize = 0; return; }

		memcpy(&sCmd, pData + i, sizeof(WORD)); i += 2;

		size_t dataLen = nLength - 4;
		if (dataLen > 0) {
			strData.assign((const char*)(pData + i), dataLen);
			i += dataLen;
		}

		memcpy(&sSum, pData + i, sizeof(WORD)); i += 2;

		WORD sum = 0;
		for (BYTE ch : strData) sum += ch;
		if (sum != sSum) { nSize = 0; return; }

		nSize = i; // ��������������
	}

	~CPacket() {}
	CPacket& operator=(const CPacket& pack) {
		if (this != &pack) {
			sHead = pack.sHead;
			nLength = pack.nLength;
			sCmd = pack.sCmd;
			strData = pack.strData;
			sSum = pack.sSum;
		}
		return *this;
	}
	int Size() {//�����ݵĴ�С
		return nLength + 6;
	}
	const char* Data() {
		strOut.resize(nLength + 6);
		BYTE* pData = (BYTE*)strOut.c_str();
		*(WORD*)pData = sHead; pData += 2;
		*(DWORD*)(pData) = nLength; pData += 4;
		*(WORD*)pData = sCmd; pData += 2;
		memcpy(pData, strData.c_str(), strData.size()); pData += strData.size();
		*(WORD*)pData = sSum;
		return strOut.c_str();
	}

public:
	WORD sHead;//�̶�λ 0xFEFF
	DWORD nLength;//�����ȣ��ӿ������ʼ������У�������
	WORD sCmd;//��������
	std::string strData;//������
	WORD sSum;//��У��
	std::string strOut;//������������
};
#pragma pack(pop)
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

#define BUFFER_SIZE 4096
	int DealCommand() {
		if (m_sock == -1) return -1;
		static char buffer[BUFFER_SIZE] = { 0 };
		static size_t index = 0;

		while (true) {
			size_t len = recv(m_sock, buffer + index, BUFFER_SIZE - index, 0);
			if ((len <= 0) && (index <= 0)) {
				return -1; // ���ӶϿ������
			}
			if (len > 0) index += len;

			size_t parseLen = index; // ��ʱ��index
			CPacket packet((BYTE*)buffer, parseLen);

			if (parseLen > 0) { // �ɹ�����
				m_packet = packet;
				memmove(buffer, buffer + parseLen, index - parseLen);
				index -= parseLen;
				return m_packet.sCmd;
			}

			if (index >= BUFFER_SIZE) {
				// ���������������Է�����
				index = 0;
			}

			// ��� len == 0 �� index > 0��˵�����ݲ�����������ѭ������
		}
		return -1;
	}
	bool Send(const char* pdata, size_t nSize) {
		if (m_client == -1) return false; // ȷ���ͻ���������
		if (send(m_client, pdata, nSize, 0) == -1) return false;
		return true;
	}
private:
	SOCKET m_sock;
	SOCKET m_client;
	CPacket m_packet;
	CServerSocket() {
		m_sock = -1;  // ��ʼ���׽���Ϊ��Чֵ
		m_client = -1; // ��ʼ���ͻ����׽���Ϊ��Чֵ
		if (InitSockEnv() == false) {
			MessageBox(NULL,_T("Socket������ʼ��ʧ��,�����������"),_T("��ʼ������"),MB_OK|MB_ICONERROR);
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
			delete CServerSocket::m_instance;  // ������ʵ��
			CServerSocket::m_instance = nullptr;
		}
	}
	static CServerSocket* m_instance;
	class CHelper {
	public:
		CHelper() {
			CServerSocket::getInstance();  // ȷ�������ڳ���ʼʱ������
		}
		~CHelper() {
			releaseInstance();
		}
	};
	static CHelper m_helper;  // ��̬��Ա��ȷ���ڳ������ʱ�ͷŵ���ʵ��
};

