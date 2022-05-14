#include "stdafx.h"
#include "Network.h"
#include "Object.h"
#include "Camera.h"
#include "GameFramework.h"
#include "Scene.h"
unsigned long start = 0;
sf::TcpSocket socket;
using namespace std;
constexpr auto BUF_SIZE = MAX_BUFFER;
int g_myid;
float g_x = 0, g_y = 0, g_z = 0;

class OBJECT : public CGameObject {
public:
	int o_x, o_y, o_z;

};

OBJECT players[MAX_USER];
OBJECT avatar;


void ProcessPacket(char* ptr) {
	static bool first_time = true;
	switch (ptr[1])
	{
	case StoC_LOGIN_OK:
	{
		StoC_login_ok* packet = reinterpret_cast<StoC_login_ok*>(ptr);
		g_myid = packet->id;
		cout << "로그인 아이디: " << g_myid << endl;
		

	}
	break;
	case StoC_ADD_PLAYER:
	{
		StoC_add_player* my_packet = reinterpret_cast<StoC_add_player*>(ptr);
		int id = my_packet->id;

		if (id < MAX_USER) {
			players[id].SetPosition(my_packet->x, my_packet->y, my_packet->z);
		}
		break;
	}
	case StoC_MOVE_PLAYER:
	{
		StoC_move_player* my_packet = reinterpret_cast<StoC_move_player*>(ptr);
		int packet_id = my_packet->id;
		int test = 0;
		if (packet_id == g_myid) {
			
			avatar.SetPosition(my_packet->x, my_packet->y,my_packet->z);
			g_x = my_packet->x;
			g_y = my_packet->y;
			g_z = my_packet->z;
			
		}
		else if (packet_id < MAX_USER) {
			//avatar.GetPosition();
			players[packet_id].SetPosition(my_packet->x, my_packet->y, my_packet->z);
		}
		test++;
		cout << "test" << test << endl;
		break;
	}
	case StoC_REMOVE_PLAYER:
	{
		StoC_remove_player* my_packet = reinterpret_cast<StoC_remove_player*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			//avatar.hide();
		}
		else if (other_id < MAX_USER) {
			//players[other_id].hide();
		}
		break;
	}
	case StoC_START:
		StoC_start* packet = reinterpret_cast<StoC_start*>(ptr);
		start = packet->time;
		cout << start << endl;
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

void send_move_packet(float x, float y,float z)
{
	CtoS_move packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_MOVE;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	cout << "send move packe : " << x << " " << y << " " << z << endl;
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}

void send_timer()
{
	CtoS_start packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_START;
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



void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;
	auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
	//cout << start << endl;
	
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
	sf::Socket::Status status = socket.connect("127.0.0.1", SERVER_PORT);
		socket.setBlocking(false);
		if (status != sf::Socket::Done) {
			wcout << L"서버와 연결할 수 없습니다.\n";
			while (true);
		}
		send_login_packet();
	return 0;
}