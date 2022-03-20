#include <iostream>
#include <map>
#include <vector>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
#pragma pack(1)

constexpr int MAPSIZE = 8;
constexpr int KEY_UP = 72;
constexpr int KEY_DOWN = 80;
constexpr int KEY_LEFT = 75;
constexpr int KEY_RIGHT = 77;
constexpr int MAX_PLAYER_CNT = 2;
constexpr int MAX_BUFFER = 1024;

class Player {
public:
	int id = 0;
	int posX = 0;
	int posY = 0;
};

class SendPacket {
public:
	unsigned char playerCnt;
	Player players[MAX_PLAYER_CNT];
};

struct SOCKETINFO{
	WSAOVERLAPPED recvOverLapped; //구조체 주소를 사용할때 사용
	WSAOVERLAPPED sendOverLapped[2]; // 0은 ID, 1은 플레이어 정보
	WSABUF sendDateBuffer;
	WSABUF recvDateBuffer;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
	Player player;

};

map<SOCKET, SOCKETINFO> clients;
map<LPWSAOVERLAPPED, SOCKETINFO*>clientsFromRecv;
map<LPWSAOVERLAPPED, SOCKETINFO*> clientsFromSend0;
map<LPWSAOVERLAPPED, SOCKETINFO*> clientsFromSend1;

void display_error(const char* msg, int err_no) {
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

int clamp(int value, int min, int max) {
	return value < min ? min : (value > max ? max : value);
}

void gotoxy(int x, int y) {
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

bool getKeyInput(Player& player, int keyCode) {
	switch (keyCode) {
	case KEY_UP: // 위
		player.posY--;
		break;
	case KEY_DOWN: // 아래
		player.posY++;
		break;
	case KEY_LEFT: // 왼
		player.posX--;
		break;
	case KEY_RIGHT: // 오른
		player.posX++;
		break;
	default:
		return false;
	}
	player.posX = clamp(player.posX, 0, MAPSIZE - 1);
	player.posY = clamp(player.posY, 0, MAPSIZE - 1);
	return true;
}

void sendEveryPlayersInfo(SOCKETINFO* socketInfo){

}

void CALLBACK sendPlayerIdCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags) {
	DWORD flags = 0;
	auto& socket = clientsFromSend0[overlapped]->socket;
	auto& client = clients[socket];

	if (dataBytes == 0) {
		cout << "send: dataBytes == 0 Remove socket from list" << endl;
		closesocket(client.socket);
		clients.erase(socket);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	auto id = *(unsigned short*)client.sendDateBuffer.buf;
	cout << "TRACE - Send message : " << id << ": " << " (" << dataBytes << " bytes)\n";

	sendEveryPlayersInfo(&client);
}

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD Inflags){
	auto& socket = clientsFromRecv[overlapped]->socket;
	auto& client = clients[socket];

	if (dataBytes == 0)//클라이언트 closecoket 한경우
	{
		cout << "recv: dataBytes == 0 Remove socket from list" << endl;
		closesocket(client.socket);
		clients.erase(socket);
		return;
	}
	cout << "From client : " << (int)client.messageBuffer[0] << " ( " << dataBytes << ") bytes)\n";

	getKeyInput(client.player, (int)client.messageBuffer[0]);
	sendEveryPlayersInfo(&client);
}

int main(){
	wcout.imbue(locale("korean"));

}