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
constexpr int SERVER_PORT = 3500;

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

constexpr int SEND_BUF_SIZE = sizeof(Player);

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
int currentPlayerId = 0;
char Board[MAPSIZE][MAPSIZE];

void display_error(const char* msg, int err_no);
int clamp(int value, int min, int max);
void gotoxy(int x, int y);
bool getKeyInput(Player& player, int keyCode);
void sendEveryPlayersInfo(SOCKETINFO* socketInfo);
void sendEveryPlayersInfo(SOCKETINFO* socketInfo);
void CALLBACK sendPlayerIDCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void recvPlayerKeyInput(SOCKETINFO& socketInfo);
void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD Inflags);
void sendPlayerID(SOCKETINFO& client);
void CALLBACK sendPlayerInfosCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);


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

void sendEveryPlayersInfo(SOCKETINFO* socketInfo) {
	const auto clientCnt = clients.size();
	if (clientCnt == 0) {
		return;
	}

	auto lter0 = clients.begin();
	for (int i = 0; lter0 != clients.end(); ++lter0, ++i) {
		reinterpret_cast<SendPacket*>(socketInfo->messageBuffer)->players[i] = lter0->second.player;
	}

	auto lter1 = clients.begin();
	while (lter1 != clients.end()) {
		auto& info = clients[lter1->first];
		clientsFromSend1[&info.sendOverLapped[1]] = &info;

		memcpy(reinterpret_cast<SendPacket*>(info.messageBuffer)->players, reinterpret_cast<SendPacket*>(socketInfo->messageBuffer)->players, clientCnt * sizeof(Player));
		reinterpret_cast<SendPacket*>(info.messageBuffer)->playerCnt = (unsigned char)clientCnt;

		info.sendDateBuffer.buf = info.messageBuffer;
		info.sendDateBuffer.len = (SEND_BUF_SIZE * clientCnt) + sizeof(SendPacket::playerCnt);
		memset(&(info.sendOverLapped[1]), 0, sizeof(WSAOVERLAPPED)); // 재사용하기위해 0으로 초기화
		WSASend(info.socket, &(info.sendDateBuffer), 1, NULL, 0, &info.sendOverLapped[1], socketInfo == &info ? sendPlayerInfosCallback : 0);
		++lter1;
	}
}


void CALLBACK sendPlayerIDCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags) {
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

void recvPlayerKeyInput(SOCKETINFO& socketInfo) {
	DWORD flags = 0;
	clientsFromRecv[&socketInfo.recvOverLapped] = &socketInfo;
	memset(&(socketInfo.recvOverLapped), 0, sizeof(WSAOVERLAPPED));
	socketInfo.recvDateBuffer.buf = socketInfo.messageBuffer;
	socketInfo.recvDateBuffer.len = 1;
	auto ret = WSARecv(socketInfo.socket, &socketInfo.recvDateBuffer, 1, 0, &flags, &socketInfo.recvOverLapped, recv_callback);
	if (ret == SOCKET_ERROR) {
		display_error("Send error : ", WSAGetLastError());
	}
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
	cout << "ID : " << (int)client.player.id << endl;

	getKeyInput(client.player, (int)client.messageBuffer[0]);
	sendEveryPlayersInfo(&client);
}

void sendPlayerID(SOCKETINFO& client) {
	client.sendDateBuffer.len = sizeof(client.player.id);
	client.sendDateBuffer.buf = reinterpret_cast<char*>(&client.player.id);
	memset(&client.sendOverLapped[0], 0, sizeof(WSAOVERLAPPED));
	DWORD flags = 0;

	clientsFromSend0[&client.sendOverLapped[0]] = &client;
	auto result = WSASend(client.socket, &client.sendDateBuffer, 1, NULL, 0, &client.sendOverLapped[0], sendPlayerIDCallback);

}

void CALLBACK sendPlayerInfosCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags) {
	auto& socket = clientsFromSend1[overlapped]->socket;
	auto& client = clients[socket];

	if (dataBytes == 0) {
		cout << "send: dataBytes == 0 Remove socket from list" << endl;
		closesocket(client.socket);
		clients.erase(socket);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우

	Player* sendedInfo = (Player*)client.sendDateBuffer.buf;
	cout << "TRACE - Send message : " << (unsigned)sendedInfo->id << ": " << (unsigned)sendedInfo->posX << ": " << (unsigned)sendedInfo->posY << ": " << " (" << dataBytes << " bytes)\n";
	recvPlayerKeyInput(client);
}

int main(){
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	listen(listenSocket, 5);
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);

	while (true) {
		SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
		clients[clientSocket] = SOCKETINFO{};
		clients[clientSocket].socket = clientSocket;
		clients[clientSocket].player.id = currentPlayerId++;
		sendPlayerID(clients[clientSocket]);
	}
	closesocket(listenSocket);
	WSACleanup();
}