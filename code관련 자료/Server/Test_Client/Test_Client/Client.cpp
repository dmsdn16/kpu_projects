#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")
using namespace std;

#pragma pack(1)

SOCKET server;
constexpr int SERVER_PORT = 3500;

class Player {
public:
	int id = 0;
	int posX = 0;
	int posY = 0;
};

#define MAPSIZE 8
char Board[MAPSIZE][MAPSIZE];

void gotoxy(int x, int y)
{
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}

int clamp(int value, int min, int max)
{
	return value< min ? min : value > max ? max : value;
}

void initBoard() {
	for (size_t i = 0; i < MAPSIZE; i++)
	{
		for (size_t j = 0; j < MAPSIZE; j++)
		{
			Board[i][j] = '+';
		}
	}
}

void drawMap()
{
	gotoxy(0, 0);
	for (size_t i = 0; i < MAPSIZE; i++)
	{
		for (size_t j = 0; j < MAPSIZE; j++)
		{
			cout << Board[i][j];
		}
		cout << endl;
	}
}

int main()
{
	char serverIP[64];
	cout << "서버 주소를 입력 : "; //1글자 입력시 127.0.0.1로 고정
	cin >> serverIP;
	if (strlen(serverIP) < 2) {
		strcpy_s(serverIP, "127.0.0.1");
	}
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	server = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, 0);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(server_addr.sin_family, serverIP, &server_addr.sin_addr);  //str IPv4,IPv6를 binary형태로 변환
	WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);

	

	gotoxy(0, 10);

	initBoard();
	drawMap();
	while (true)
	{
		drawMap();

		Sleep(60);
	}
}