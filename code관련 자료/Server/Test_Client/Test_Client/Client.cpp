#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")
using namespace std;

#pragma pack(1)

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

	gotoxy(0, 10);

	initBoard();
	drawMap();
	while (true)
	{
		drawMap();

		Sleep(60);
	}
}