#pragma once
#include "stdafx.h"
#include "Player.h"
using namespace std;
class OBJECT {
public:
	bool m_showing = false;
	bool init = true;
	float m_x, m_y;

	void move(float x, float y) {
		m_x = x;
		m_y = y;
	};
	void show() { m_showing = true; };
	void hide() { m_showing = false; };
};
void ProcessPacket(char* ptr);
void process_data(char* net_buf, size_t io_byte);
void send_move_packet(DIRECTION dir,int x,int y);
void send_login_packet();
void send_timer();
void client_main();
int main();

