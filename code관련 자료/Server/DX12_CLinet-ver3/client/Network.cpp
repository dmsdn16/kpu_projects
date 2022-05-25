#include "stdafx.h"
#include "Network.h"
#include "Object.h"
#include "Camera.h"
#include "GameFramework.h"
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
unsigned long start = 0;
using namespace std;
constexpr auto BUF_SIZE = MAX_BUFFER;
int g_myid;
float g_x = 0, g_y = 0, g_z = 0;

CGameObject players[MAX_USER];
CGameObject avatar;
CPlayer* S_Player;
void ProcessPacket(char* ptr) {
	static bool first_time = true;
	switch (ptr[1])
	{
	case StoC_LOGIN_OK:
	{
		StoC_login_ok* packet = reinterpret_cast<StoC_login_ok*>(ptr);
		g_myid = packet->id;
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
	//case StoC_MOVE_PLAYER:
	//{
	//	StoC_move_player* my_packet = reinterpret_cast<StoC_move_player*>(ptr);
	//	int other_id = my_packet->id;
	//	if (other_id == g_myid) {
	//		avatar.SetPosition(my_packet->x, my_packet->y, my_packet->z);
	//		g_x = my_packet->x;
	//		g_y = my_packet->y;
	//		g_z = my_packet->z;

	//	}
	//	else if (other_id < MAX_USER) {
	//		cout << "다른클라에서" << my_packet->x << " " << my_packet->y << " " << my_packet->z << " " << endl;
	//		players[other_id].SetPosition(my_packet->x, my_packet->y, my_packet->z);
	//		//players[other_id].Render(&pd3dCommandList,&pCamera)
	//	}
	//	break;
	//}
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

	while (0 != io_byte) {
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

//void send_move_packet(float x, float y, float z)
//{
//	CtoS_move packet;
//	packet.size = sizeof(packet);
//	packet.type = CtoS_MOVE;
//	packet.x = x;
//	packet.y = y;
//	packet.z = z;
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
	send()
}


void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	if (received > 0) process_data(net_buf, received);
}

int main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 서버와 마찬가지로 먼저 소켓 생성 필요
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) {
		printf("Error - Invalid socket\n");
		return 1;
	}

	// 서버 정보 객체
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

	// 연결요청 connect
	if (connect(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("Error - Fail to connect\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	else {
		printf("Server Connected\n* Enter Message\n->");
	}
	return 0;
}