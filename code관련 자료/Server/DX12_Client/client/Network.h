#pragma once
#include "stdafx.h"
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
void send_move_packet(DIRECTION dir);
void send_login_packet();
void client_main();
int main();

