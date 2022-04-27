#include "stdafx.h"
#include "Network.h"


sf::TcpSocket socket;
using namespace std;
constexpr auto BUF_SIZE = MAX_BUFFER;
int g_myid;
float g_x = 0, g_y = 0;
OBJECT avatar;
OBJECT players[MAX_USER];
CPlayer* S_Player;
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
			S_Player->Move(XMFLOAT3(g_x,g_y,0.f));
			
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

void send_move_packet(DIRECTION dir,int x, int y)
{
	cout << "서버에서 보냄: " << x << " : " << y << endl;
	CtoS_move packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_MOVE;
	packet.dir = dir;
	packet.x = x;
	packet.y = y;
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}

void send_login_packet()
{
	CtoS_login packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_LOGIN;
	//strcpy_s(packet.name, name.c_str());
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}

void client_main(CPlayer* m_pPlayer)
{
	
	char net_buf[BUF_SIZE];
	size_t	received;
	S_Player = m_pPlayer;
	auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러!";
		while (true);
	}
	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);
}

int main()
{
	if (players->init)
	{
		sf::Socket::Status status = socket.connect("127.0.0.1", SERVER_PORT);
		socket.setBlocking(false);
		if (status != sf::Socket::Done) {
			wcout << L"서버와 연결할 수 없습니다.\n";
			while (true);
		}
		players->init = false;
		send_login_packet();
	}
	
	
	return 0;
}