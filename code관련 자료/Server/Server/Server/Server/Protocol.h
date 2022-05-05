#pragma once

constexpr int MAX_NAME = 100;
constexpr int MAX_BUFFER = 1024;
constexpr short SERVER_PORT = 3500;
constexpr int WORLD_X_SIZE = 400;
constexpr int WORLD_Y_SIZE = 400;
constexpr int MAX_USER = 5000;
constexpr int MAX_UNIT = 5000;

constexpr unsigned char CtoS_LOGIN = 1;
constexpr unsigned char CtoS_MOVE = 2;
constexpr unsigned char CtoS_READY = 3;
constexpr unsigned char CtoS_START = 4;
constexpr unsigned char StoC_LOGIN_OK = 5;
constexpr unsigned char StoC_ADD_PLAYER = 6;
constexpr unsigned char StoC_MOVE_PLAYER = 7;
constexpr unsigned char StoC_REMOVE_PLAYER = 8;
constexpr unsigned char StoC_ALL_READY = 9;
constexpr unsigned char StoC_START = 10;

#pragma pack(push, 1)
enum DIRECTION {D_N,D_S,D_W,D_E,D_NO,D_LB,D_RB};

struct CtoS_login {
	unsigned char size;
	unsigned char type;
	//char name[MAX_NAME];
};

struct CtoS_move {
	unsigned char size;
	unsigned char type;
	DIRECTION dir;
	int x, y;
	int move_time; // 클라이언트에서 패킷을 보낸 시간이고 millisecond단위
};

struct CtoS_ready {
	unsigned char size;
	unsigned char type;
};

struct CtoS_start {
	unsigned char size;
	unsigned char type;
};

struct StoC_login_ok {
	unsigned char size;
	unsigned char type;
	int id;
	int hp, level;
	int race;
	short x, y;
};

struct StoC_add_player {
	unsigned char size;
	unsigned char type;
	int		id;
	short	x, y;
	int		race;
};

struct StoC_move_player {
	unsigned char size;
	unsigned char type;
	int id;
	int move_time;
	short x, y;
};

struct StoC_remove_player {
	unsigned char size;
	unsigned char type;
	int id;
};

struct StoC_all_ready {
	unsigned char size;
	unsigned char type;
	int id;
};

struct StoC_start {
	unsigned char size;
	unsigned char type;
	int id;
	unsigned long time;
};

#pragma pack(pop)