#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
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
	int retval;

	//원속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	//MessageBox(NULL, "원속 초기화 성공", "알림", MB_OK);

	//socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	printf("소켓이 생성되었습니다\n");
	//MessageBox(NULL, "TCP 소켓성공", "알림", MB_OK);

	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(9000);

	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	printf("Bind 완료 되었습니다.\n");

	//listen()
	listen(listen_sock, SOMAXCONN);
	printf("connect 연결을 기다리는 중..\n");

	//통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	printf("요청을 받았습니다.\n");

	//recv()
	retval = recv(client_sock, buf, BUFSIZE, 0);
	printf("메세지를 수신하였습니다.\n");

	//받는 데이터 출력
	buf[retval] = '\0';
	printf("[TCP/%s:%d] %s\n",
		inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port), buf);
	//closesocket
	closesocket(listen_sock);

	//원속 종료
	WSACleanup();
	return 0;

}