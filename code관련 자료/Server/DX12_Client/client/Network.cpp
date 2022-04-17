#define SFML_STATIC 1
#include <SFML/Network.hpp>
#include <iostream>
#include <chrono>
#include <WS2tcpip.h>
using namespace std;

#ifdef _DEBUG
#pragma comment (lib, "lib/sfml-window-s-d.lib")
#pragma comment (lib, "lib/sfml-system-s-d.lib")
#pragma comment (lib, "lib/sfml-network-s-d.lib")
#else
#pragma comment (lib, "lib/sfml-window-s.lib")
#pragma comment (lib, "lib/sfml-system-s.lib")
#pragma comment (lib, "lib/sfml-network-s.lib")
#endif
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")


#include "../../Server/Server/Server/Protocol.h"

constexpr auto BUF_SIZE = MAX_BUFFER;

sf::TcpSocket socket;

int g_x;
int g_y;
int g_myid;
SOCKET server;

class OBJECT {
public:
	bool m_showing = false;
	float m_x, m_y;
	
	void move(float x, float y) {
		m_x = x;
		m_y = y;
	}

	void show(){
		m_showing = true;
	}

	void hide() {
		m_showing = false;
	}
};

OBJECT avatar;
OBJECT players[MAX_USER];


void display_error(const char* msg, int err_no) {
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

void ProcessPacket(char* ptr) {
	static bool first_time = true;
	switch (ptr[1])
	{
	case StoC_LOGIN_OK:
	{
		StoC_login_ok* packet = reinterpret_cast<StoC_login_ok*>(ptr);
		g_myid = packet->id;
		avatar.m_x = packet->x;
		avatar.m_y = packet->y;
		g_x = packet->x;
		g_y = packet->y;
		avatar.move(packet->x, packet->y);
		avatar.show();
	}
	break;
	case StoC_ADD_PLAYER:
	{
		StoC_add_player* my_packet = reinterpret_cast<StoC_add_player*>(ptr);
		int id = my_packet->id;

		if (id < MAX_USER) {
			players[id].move(my_packet->x, my_packet->y);
		}
		break;
	}
	case StoC_MOVE_PLAYER:
	{
		StoC_move_player* my_packet = reinterpret_cast<StoC_move_player*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_x = my_packet->x;
			g_y = my_packet->y;
		}
		else if (other_id < MAX_USER) {
			players[other_id].move(my_packet->x, my_packet->y);
		}
		break;
	}
	case StoC_REMOVE_PLAYER:
	{
		StoC_remove_player* my_packet = reinterpret_cast<StoC_remove_player*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else if (other_id < MAX_USER) {
			players[other_id].hide();
		}
		break;
	}
	default:
		//에러 출력
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0!= io_byte){
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void send_move_packet(DIRECTION dir)
{
	CtoS_move packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_MOVE;
	packet.dir = dir;
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}

void send_login_packet(string& name)
{
	CtoS_login packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_LOGIN;
	strcpy_s(packet.name, name.c_str());
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}