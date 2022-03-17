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


class Player {
public:
	int id = 0;
	int posX = 0;
	int posY = 0;
};

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

int main()
{

}