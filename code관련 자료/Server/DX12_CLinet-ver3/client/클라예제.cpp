#include "stdafx.h"
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3500

struct SOCKETINFO {
    WSAOVERLAPPED overlapped;
    WSABUF dataBuffer;
    int receiveBytes;
    int sendBytes;
};

int _tmain(int argc, _TCHAR* argv[]) {
    // winsock.dll �ε�
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
        printf("Error - Can not load 'winsock.dll' file\n");
        return 1;
    }

    // ������ ���������� ���� ���� ���� �ʿ�
    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        printf("Error - Invalid socket\n");
        return 1;
    }

    // ���� ���� ��ü
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

    // �����û connect
    if (connect(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error - Fail to connect\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    else {
        printf("Server Connected\n* Enter Message\n->");
    }

    SOCKETINFO* socketInfo;
    DWORD sendBytes;
    DWORD receiveBytes;
    DWORD flags;

    while (1) {
        char messageBuffer[MAX_BUFFER];
        int i, bufferLen;
        for (i = 0; 1; i++) {
            messageBuffer[i] = getchar();
            if (messageBuffer[i] == '\n') {
                messageBuffer[i++] = '\0';
                break;
            }
        }
        bufferLen = i;

        socketInfo = (struct SOCKETINFO*)malloc(sizeof(struct SOCKETINFO));
        memset((void*)socketInfo, 0x00, sizeof(struct SOCKETINFO));
        socketInfo->dataBuffer.len = bufferLen;
        socketInfo->dataBuffer.buf = messageBuffer;

        int sendBytes = send(listenSocket, messageBuffer, bufferLen, 0);
        if (sendBytes > 0) {
            printf("TRACE - Sen message : %s (%d bytes)\n", messageBuffer, sendBytes);

            int receiveBytes = recv(listenSocket, messageBuffer, MAX_BUFFER, 0);
            if (receiveBytes > 0) {
                printf("TRACE - Receive message : %s (%d bytes)\n* Enter Message\n->", messageBuffer, receiveBytes);
            }
        }
    }

    //���� ����
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
