#include <iostream>

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <array>
#include <vector>
#include <thread>
#include "Protocol.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#include <Windows.h>
#define CORE 4
#define UNIT_ID_START UNIT_START
#define MAX_PLAYER (UNIT_START)
#define MAX_UNIT (MAX_USER-UNIT_ID_START)

using namespace std;
unsigned long start;
enum OP_TYPE {OP_RECV,OP_SEND,OP_ACCEPT};
enum PL_STATE {PLST_FREE, PLST_CONNECTED, PLST_INGAME, PLST_READY};

struct UNIT_OVER {
	WSAOVERLAPPED m_over;
	OP_TYPE m_op;
};
struct EX_OVER : public UNIT_OVER{
	WSABUF m_wsabuf[1];
	unsigned char m_packetbuf[MAX_BUFFER];
	SOCKET m_csocket; //OP_ACCEPT에서만 사용
};

constexpr size_t SEND_EXOVER_INCREASEMENT_SIZE = 128;

struct SESSION {
	mutex m_slock;
	PL_STATE m_state;
	SOCKET m_socket;
	int id;
	EX_OVER m_recv_over;
	int m_prev_size;
	//게임의 내용물
	char m_name[100];
	float x, y,z;
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
	//cout << "test" << p_type;
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

void do_recv(int c_id)
{
	players[c_id].m_recv_over.m_wsabuf[0].buf = reinterpret_cast<char*>(players[c_id].m_recv_over.m_packetbuf) + players[c_id].m_prev_size;
	players[c_id].m_recv_over.m_wsabuf[0].len = MAX_BUFFER - players[c_id].m_prev_size; //buffer=임시 저장공간
	memset(&players[c_id].m_recv_over.m_over, 0, sizeof(players[c_id].m_recv_over.m_over));
	DWORD r_flag = 0;
	auto ret = WSARecv(players[c_id].m_socket, players[c_id].m_recv_over.m_wsabuf, 1, NULL, &r_flag, &players[c_id].m_recv_over.m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			display_error("Error in Recv: ", err_no);
	}
}

int get_new_player_id(SOCKET p_socket)
{
	for (int i = SERVER_ID + 1; i < MAX_USER; ++i)
	{
		lock_guard<mutex>lg{ players[i].m_slock };//lock_guard 초기값으로 가지는 가드(자동언락)
		if (PLST_FREE == players[i].m_state) {
			players[i].m_state = PLST_CONNECTED;
			players[i].m_socket = p_socket;
			players[i].m_name[0] = 0;
			return i;
		}
	}
	return -1;
}

void send_add_player(int c_id, int p_id) 
{
	StoC_add_player p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = StoC_ADD_PLAYER;
	p.x = players[p_id].x;
	p.y = players[p_id].y;
	p.race = 0;
	send_packet(c_id, &p);
}

void send_remove_player(int c_id, int p_id)
{
	StoC_remove_player p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = StoC_REMOVE_PLAYER;
	send_packet(c_id, &p);
}

void send_move_packet(int c_id, int p_id)
{
	StoC_move_player p;
	cout<<"send_move_packet" << players[p_id].x << players[p_id].y << players[p_id].z <<" "<<p_id << endl;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = StoC_MOVE_PLAYER;
	p.x = players[p_id].x;
	p.y = players[p_id].y;
	p.z = players[p_id].z;
	send_packet(c_id, &p);
	send_packet(p_id, &p);
}

void send_all_ready_packet(int p_id)
{
	StoC_all_ready p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = StoC_ALL_READY;
	send_packet(p_id, &p);
}

void send_start_packet(int c_id, int p_id)
{
	StoC_start p;
	p.id = p_id;
	p.size = sizeof(p);
	p.type = StoC_START;
	p.time = GetTickCount64();
	send_packet(c_id, &p);
}

void do_move(int p_id, float p_x, float p_y, float p_z)
{
	players[p_id].x = p_x;
	players[p_id].y = p_y;
	players[p_id].z = p_z;

	for (auto& pl : players) {
		if (p_id != pl.id) {
			lock_guard<mutex>gl{ pl.m_slock };
			if (PLST_INGAME == pl.m_state) {
				send_move_packet(pl.id, p_id);
				send_move_packet(p_id, pl.id);
			}
		}
	}
}


void proccess_packet(int p_id, unsigned char* p_buf) 
{
	
	switch (p_buf[1])
	{
		case CtoS_LOGIN: {
			CtoS_login* packet = reinterpret_cast<CtoS_login*>(p_buf);
			{
				lock_guard<mutex> gl2{ players[p_id].m_slock };
				players[p_id].m_state = PLST_INGAME;
			}
			
			//strcpy_s(players[p_id].m_name, packet->name);
			send_login_ok_packet(p_id);
			//주위에 누가 있는지 알려줘야함(시야공유 등)
			for (auto& pl : players) {
				if (p_id != pl.id) {
					lock_guard<mutex>gl{ pl.m_slock };
					if (PLST_INGAME == pl.m_state) {
						send_add_player(pl.id, p_id);
						send_add_player(p_id, pl.id);
					}
				}
			}
		}
			break;
		case CtoS_MOVE: {
			CtoS_move* packet = reinterpret_cast<CtoS_move*>(p_buf);
			cout << " CtoS_move" << packet->x << " " << packet->y << " " << packet->z << " " << p_id << endl;
			do_move(p_id,packet->x, packet->y, packet->z);
		}
			break;
		default:
			cout << "Unknown Packet Type from Client[" << p_id << "] Packet Type [" << p_buf[1] << "]" << endl;
			while (true);
	}
}

void disconnect(int p_id)
{
	{
		lock_guard<mutex> gl{ players[p_id].m_slock };
		closesocket(players[p_id].m_socket);
		players[p_id].m_state = PLST_FREE; //데드락 조심 범위지정
	}
	for (auto& pl : players) {
		lock_guard<mutex> gl2{ pl.m_slock };
		if (PLST_INGAME == pl.m_state)
			send_remove_player(pl.id, p_id);
	}
}


void worker_thread(HANDLE h_iocp, SOCKET l_socket)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR ikey;
		WSAOVERLAPPED* over;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &ikey, &over, INFINITE);
		int key = static_cast<int>(ikey);
		if (FALSE == ret) {
			if (SERVER_ID == key) {
				display_error("GQCS:", WSAGetLastError());
				exit(-1);
			}
			else {
				display_error("GQCS:", WSAGetLastError());
				disconnect(key);
			}
		}
		if ((key != SERVER_ID) && (num_bytes == 0)) {
			disconnect(key);
			continue;
		}
		EX_OVER* ex_over = reinterpret_cast<EX_OVER*> (over);
		switch (ex_over->m_op)
		{
			case OP_RECV: {
				//패킷 조립 및 처리
				unsigned char* packet_ptr = ex_over->m_packetbuf;
				int num_data = num_bytes + players[key].m_prev_size;
				int packet_size = packet_ptr[0];
				while (num_data >= packet_size) {
					proccess_packet(key, packet_ptr);
					num_data -= packet_size;
					packet_ptr += packet_size;
					if (0 >= num_data) break;
					packet_size = packet_ptr[0];
				}
				players[key].m_prev_size = num_data;
				if (0 != num_data)
					memcpy(ex_over->m_packetbuf, packet_ptr, num_data);
				do_recv(key);
				break;
			}

			case OP_SEND:
				delete ex_over;
				break;

			case OP_ACCEPT: {
				int c_id = get_new_player_id(ex_over->m_csocket);
				if (-1 != c_id) {
					players[c_id].m_recv_over.m_op = OP_RECV;
					players[c_id].m_prev_size = 0;
					CreateIoCompletionPort(reinterpret_cast<HANDLE>(ex_over->m_csocket), h_iocp, c_id, 0);
					do_recv(c_id);
				}
				else {
					closesocket(players[c_id].m_socket);
				}
				memset(&ex_over->m_over, 0, sizeof(ex_over->m_over));
				SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				ex_over->m_csocket = c_socket;
				AcceptEx(l_socket, c_socket, ex_over->m_packetbuf, 0, 32, 32, NULL, &ex_over->m_over);
			}
			break;
		}
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

	vector<thread> mult_threads;
	for (int i = 0; i < CORE; ++i) {
		mult_threads.emplace_back(worker_thread, h_iocp, listenSocket);	

	}
		
	for (auto& th : mult_threads)
		th.join();
	closesocket(listenSocket);
	WSACleanup();

}