#pragma once
#include "stdafx.h"
#include "Player.h"
#include "GameFramework.h"
using namespace std;

void ProcessPacket(char* ptr);
void process_data(char* net_buf, size_t io_byte);
void send_move_packet(float x, float y, float z);
void send_login_packet();
void send_timer();
void client_main();
int main();
