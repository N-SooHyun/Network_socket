#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32");

#define BUFSIZE 512

void err_quit(const char* msg) {
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	exit(-1);

}

int main(int argc, char* argv[]) {
	//원속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	//MessageBox(NULL, "원속 초기화 성공", "알림", MB_OK);

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	printf("소켓이 생성되었습니다.\n");
	//MessageBox(NULL, "TCP 소켓성공", "알림", MB_OK);

	//connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(9000);

	connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	printf("connect 연결요청!!\n");

	//서버로 보낼 스트링 설정
	char buf[BUFSIZE + 1];
	sprintf(buf, "hello world!");

	//send()
	send(sock, buf, strlen(buf), 0);
	printf("메세지를 보냅니다.\n");

	//closesocket()
	closesocket(sock);

	//원속 종료
	WSACleanup();
	return 0;

}