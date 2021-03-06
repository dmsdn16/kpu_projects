#pragma once
#define SERVER_IP = "127.0.0.1"

constexpr int MAX_NAME = 100;
constexpr int MAX_BUFFER = 1024;
constexpr short SERVER_PORT = 3500;
constexpr int WORLD_X_SIZE = 400;
constexpr int WORLD_Y_SIZE = 400;
constexpr int MAX_USER = 30000;
constexpr int MAX_UNIT = 5000;


constexpr unsigned char CtoS_LOGIN = 11;
constexpr unsigned char CtoS_MOVE = 12;
constexpr unsigned char CtoS_UNIT_LOGIN = 13;


constexpr unsigned char StoC_LOGIN_OK = 21;
constexpr unsigned char StoC_ADD_PLAYER = 22;
constexpr unsigned char StoC_MOVE_PLAYER = 23;
constexpr unsigned char StoC_REMOVE_PLAYER = 24;
constexpr unsigned char StoC_UNIT_LOGIN_OK = 25;


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
	float x, y,z;
	int move_time; // 클라이언트에서 패킷을 보낸 시간이고 millisecond단위
};

struct CtoS_unit_login {
	unsigned char size;
	unsigned char type;
	float x, y, z;
	//char name[MAX_NAME];
};

struct StoC_login_ok {
	unsigned char size;
	unsigned char type;
	int id;
	int hp, level;
	int race;
	float x, y, z;
};

struct StoC_unit_login_ok {
	unsigned char size;
	unsigned char type;
	int id;
	int hp, level;
	int race;
	float x, y, z;
};

struct StoC_add_player {
	unsigned char size;
	unsigned char type;
	int		id;
	short	x, y,z;
	int		race;
};

struct StoC_move_player {
	unsigned char size;
	unsigned char type;
	int id;
	int move_time;
	float x, y, z;
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