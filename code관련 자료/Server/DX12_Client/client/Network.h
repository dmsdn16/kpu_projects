#pragma once
#define SFML_STATIC 1
#include <SFML/Network.hpp>
#include <iostream>
#include <chrono>
using namespace std;

#ifdef _DEBUG
#pragma comment (lib, "lib/sfml-window-s-d.lib")
#pragma comment (lib, "lib/sfml-system-s-d.lib")
#pragma comment (lib, "lib/sfml-network-s-d.lib")
#else
#pragma comment (lib, "lib/sfml-window-s.lib")
#pragma comment (lib, "lib/sfml-system-s.lib")
#pragma comment (lib, "lib/sfml-network-s.lib")
#endif
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include "../../Server/Server/Server/Protocol.h"

class OBJECT {
public:
	bool m_showing = false;
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
void send_login_packet(string& name);