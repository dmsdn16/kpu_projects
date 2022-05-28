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

class OBJECT : public CScene{
public:
	int o_x, o_y, o_z;


};

OBJECT players[MAX_USER];
OBJECT avatar;
ID3D12Device *pd3dDevice;
ID3D12GraphicsCommandList *pd3dCommandList;

void ProcessPacket(char* ptr) {
	static bool first_time = true;
	switch (ptr[1])
	{
	case StoC_LOGIN_OK:
	{
		StoC_login_ok* packet = reinterpret_cast<StoC_login_ok*>(ptr);
		g_myid = packet->id;

		break;
	}
	case StoC_UNIT_LOGIN_OK:
	{
		StoC_unit_login_ok* packet = reinterpret_cast<StoC_unit_login_ok*>(ptr);
		g_myid = packet->id;
		cout << "unit id " << packet->id << " " << packet->x << " " << packet->y << " " << packet->z << endl;
	}
	case StoC_ADD_PLAYER:
	{
		StoC_add_player* packet = reinterpret_cast<StoC_add_player*>(ptr);
		int id = packet->id;
		//if (id < MAX_USER) {
		//	/*cout << "send 다른클라 좌표" << packet->x << " " << packet->y << " " << packet->z << endl;
		//	players[id].Unit1[0]->SetPosition(packet->x, packet->y, packet->z);*/
		//}
		if(g_myid != id)
		{
			cout << "send 다른클라 좌표" << packet->x << " " << packet->y << " " << packet->z << endl;
			players[g_myid].BuildObjects(pd3dDevice, pd3dCommandList);
			players[g_myid].Unit1[0]->SetPosition(packet->x, packet->y, packet->z);
		}

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

//void send_move_packet(float x, float y,float z)
//{
//	CtoS_move packet;
//	packet.size = sizeof(packet);
//	packet.type = CtoS_MOVE;
//	packet.x = x;
//	packet.y = y;
//	packet.z = z;
//	cout << "send move packe : " << x << " " << y << " " << z << endl;
//	size_t sent = 0;
//	socket.send(&packet, sizeof(packet), sent);
//}


void send_login_packet()
{
	CtoS_login packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_LOGIN;
	//strcpy_s(packet.name, name.c_str());
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}


void send_unit_login_packet(float x, float y, float z)
{
	CtoS_unit_login packet;
	packet.size = sizeof(packet);
	packet.type = CtoS_UNIT_LOGIN;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	//strcpy_s(packet.name, name.c_str());
	size_t sent = 0;
	socket.send(&packet, sizeof(packet), sent);
}


void client_main(ID3D12Device *m_pd3dDevice, ID3D12GraphicsCommandList *m_pd3dCommandList)
{
	char net_buf[BUF_SIZE];
	size_t	received;
	auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
	//cout << start << endl;
	pd3dDevice = m_pd3dDevice;
	pd3dCommandList = m_pd3dCommandList;
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