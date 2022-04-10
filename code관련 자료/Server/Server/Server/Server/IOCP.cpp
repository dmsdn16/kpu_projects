#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <array>
#include "Protocol.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#define CORE 4

using namespace std;

enum OP_TYPE {OP_RECV,OP_SEND,OP_ACCEPT};
enum PL_STATE {PLST_FREE, PLST_CONNECTED, PLST_INGAME};

struct EX_OVER {
	WSAOVERLAPPED m_over;
	WSABUF m_wsabuf[1];
	unsigned char m_packetbuf[MAX_BUFFER];
	OP_TYPE m_op;
	SOCKET m_csocket; //OP_ACCEPT에서만 사용
};

struct SESSION {
	mutex m_slock;
	PL_STATE m_state;
	SOCKET m_socket;
	int id;
	EX_OVER m_recv_over;
	int m_prev_size;
	//게임의 내용물
	char m_name[100];
	short x, y;
};

constexpr int SERVER_ID = 0;
array <SESSION, MAX_USER + 1> players;

void display_error(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

void send_packet(int p_id, void* p)
{
	int p_size = reinterpret_cast<unsigned char*>(p)[0];
	int p_type = reinterpret_cast<unsigned char*>(p)[1];
	cout << "To client [" << p_id << "]";//디버깅용 callback 에러 생기니 실행할땐 제거
	cout << "Packet [" << p_type << "]\n";
	EX_OVER* s_over = new EX_OVER; //로컬 변수로 사용 X sned를 계속 사용할 예정
	s_over->m_op = OP_SEND;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf[0].buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf[0].len = p_size;
	auto ret = WSASend(players[p_id].m_socket, s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, 0);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			display_error("Error in SendPacket:", err_no);
	}
}

void send_login_ok_packet(int p_id)
{
	StoC_login_ok p;
	p.hp = 100;
	p.id = p_id;
	p.level = 1;
	p.race = 1;
	p.size = sizeof(p);
	p.type = StoC_LOGIN_OK;
	p.x = players[p_id].x;
	p.y = players[p_id].y;
	send_packet(p_id, &p);
}

void do_recv(int p_id)
{
	players[p_id].m_recv_over.m_wsabuf[0].buf = reinterpret_cast<char*>(players[p_id].m_recv_over.m_packetbuf) + players[p_id].m_prev_size;
	players[p_id].m_recv_over.m_wsabuf[0].len = MAX_BUFFER - players[p_id].m_prev_size; //buffer=임시 저장공간
	memset(&players[p_id].m_recv_over.m_over, 0, sizeof(players[p_id].m_recv_over.m_over));
	DWORD r_flag = 0;
	auto ret = WSARecv(players[p_id].m_socket, players[p_id].m_recv_over.m_wsabuf, 1, NULL, &r_flag, &players[p_id].m_recv_over.m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			display_error("Error in SendPacket: ", err_no);
	}
}

int main()
{
	for (int i = 0; i < MAX_USER + 1; ++i) {
		auto& pl = players[i];
		pl.id = i;
		pl.m_state = PLST_FREE;
	}
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	wcout.imbue(locale("korean"));
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, SERVER_ID, 0);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	listen(listenSocket, SOMAXCONN);
	EX_OVER accept_over;
	accept_over.m_op = OP_ACCEPT;
	memset(&accept_over.m_over, 0, sizeof(accept_over.m_over));
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	accept_over.m_csocket = c_socket;
	BOOL ret = AcceptEx(listenSocket, c_socket, accept_over.m_packetbuf, 0, 32, 32, NULL, &accept_over.m_over);
	if (FALSE == ret) {
		int err_num = WSAGetLastError();
		if (err_num != WSA_IO_PENDING)
			display_error("AcceptEX Error", err_num);
	}
}