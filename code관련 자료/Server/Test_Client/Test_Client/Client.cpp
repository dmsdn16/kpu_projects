#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <map>
#include <conio.h>

#pragma comment(lib, "WS2_32.lib")
using namespace std;

#pragma pack(1)

SOCKET server;
constexpr int SERVER_PORT = 3500;
int ID;
u_long Non_BlockingMode = 1;
int key_value;

#define MAPSIZE 8
char Board[MAPSIZE][MAPSIZE];
constexpr int SEND_BUF_SIZE = 1;

class Player {
public:
	int id = 0;
	int posX = 0;
	int posY = 0;
};

class RecvPaket {
public:
	unsigned char playerCnt;
	Player playerInfos[10];
};

map<unsigned char, Player> playerInfos;




void display_error(const char* msg, int err_no) {
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

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
	gotoxy(0, 1);
	for (size_t i = 0; i < MAPSIZE; i++)
	{
		for (size_t j = 0; j < MAPSIZE; j++)
		{
			cout << Board[i][j];
		}
		cout << endl;
	}
}

void drawPlayer() {
	//플레이어 위치 그림
	auto lter = playerInfos.begin();
	while (lter != playerInfos.end()) {
		auto info = lter->second;
		Board[info.posY][info.posX] = ID == lter->first ? 'M' : 'A';
		lter++;
	}
}

void recvUpdatePlayer() {
	//recv
	RecvPaket recvPacket;
	WSABUF r_wsabuf[1]; //받을버퍼
	r_wsabuf[0].buf = reinterpret_cast<char*>(&recvPacket.playerCnt);
	r_wsabuf[0].len = sizeof(RecvPaket::playerCnt);
	DWORD bytes_recv;
	DWORD recv_flag = 0;
	int ret = WSARecv(server, r_wsabuf, 1, &bytes_recv, &recv_flag, NULL, NULL);
	if (SOCKET_ERROR == ret) {
		display_error("recv_error: ", WSAGetLastError());
		drawPlayer();
		return;
	}

	Player r_mess[10];
	r_wsabuf[0].buf = (char*)r_mess;
	r_wsabuf[0].len = recvPacket.playerCnt * sizeof(Player);
	ret = WSARecv(server, r_wsabuf, 1, &bytes_recv, &recv_flag, NULL, NULL);
	if (SOCKET_ERROR == ret) {
		display_error("recv_error: ", WSAGetLastError());
		drawPlayer();
		return;
	}

	for (int i = 0; i < recvPacket.playerCnt; i++) {
		playerInfos[r_mess[i].id].id = r_mess[i].id;
		playerInfos[r_mess[i].id].posX = r_mess[i].posX;
		playerInfos[r_mess[i].id].posY = r_mess[i].posY;
	}

	drawPlayer();
}

void sendKeyInput() {
	//send
	if (!_kbhit()) {
		return;
	}
	key_value = _getch();
	if (key_value == 0x00 || key_value == 0xE0) {
		//확장키 경우 입력1개 추가
		key_value = _getch();
		cout << key_value << endl;
	}

	char mess[SEND_BUF_SIZE];
	mess[0] = static_cast<char>(key_value);
	key_value = -1;
	WSABUF s_wsabuf[1]; //보낼 버퍼
	s_wsabuf[0].buf = mess;
	s_wsabuf[0].len = SEND_BUF_SIZE;
	DWORD bytes_send;
	int ret = WSASend(server, s_wsabuf, 1, &bytes_send, 0, NULL, NULL);
	if (ret == SOCKET_ERROR) {
		display_error("Send error: ", WSAGetLastError());
		return;
	}
}


int main()
{
	wcout.imbue(locale("korean"));
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
	inet_pton(AF_INET, serverIP, &server_addr.sin_addr);  //str IPv4,IPv6를 binary형태로 변환
	WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);

	WSABUF r_wsabuf[1]; //보낼 버퍼
	r_wsabuf[0].buf = reinterpret_cast<char*>(&ID);
	r_wsabuf[0].len = sizeof(ID);
	DWORD bytes_recv;
	DWORD recv_flag = 0;
	int ret = WSARecv(server, r_wsabuf, 1, &bytes_recv, &recv_flag, NULL, NULL);
	if (SOCKET_ERROR == ret) {
		display_error("201	recv_error: ", WSAGetLastError());
		exit(-1);
	}
	gotoxy(0, 10);
	//cout << "받은 ID (" << (unsigned)ID << ") 받은 바이트 크기 (" << bytes_recv << "\n";

	initBoard();
	drawMap();

	ioctlsocket(server, FIONBIO, &Non_BlockingMode); //논-블럭킹 소켓으로 변환
	while (true){
		recvUpdatePlayer();

		drawMap();

		Sleep(60);

		sendKeyInput();

		initBoard();

	}
}