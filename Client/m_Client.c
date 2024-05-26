#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define PORT 50050
#define BUFFER_SIZE 1024

int SockFun();

int main() {
	char menu_choice;
	while (1) {
		system("cls");
		printf("1. 소켓 통신 \n");
		printf("2. 종료\n");
		printf("입력 : ");
		scanf_s("%c", &menu_choice, 1);

		switch (menu_choice) {
		case '1':
			SockFun();
			break;
		case '2':
			printf("종료중...\n");
			return 0;
		default:
			printf("유효하지 않은 선택이에요 다시 시도하세요 \n");
			break;
		}
	}
}

int SockFun() {
	WSADATA wsa;
	//클라이언트 소켓 선언
	SOCKET client_socket;
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE];
	int recv_size;

	//1 소켓 초기화
	printf("소켓 초기화 중 .. \n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("실패 에러 코드 : %d\n", WSAGetLastError());
		return 1;
	}
	printf("초기화 완료\n");

	//2. 소켓 생성
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("소켓 생성 실패 : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//3. 연결할 서버의 주소 선언
	server.sin_family = AF_INET;
	//server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server.sin_port = htons(PORT);
	if (inet_pton(AF_INET, SERVER_ADDR, &server.sin_addr.s_addr) <= 0) {
		printf("유효하지 않은 주소 \n");
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	//4. 주소지 server에 연결
	if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("연결 실패 : %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}
	printf("서버 연결 성공\n");



	//5. 데이터 전송
	char message[BUFFER_SIZE];
	while (1) {
		fputs("메시지 입력(Q to quit) : ", stdout);
		fgets(message, BUFFER_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		send(client_socket, message, (int)strlen(message), 0);
		recv_size = recv(client_socket, message, BUFFER_SIZE - 1, 0);
		message[recv_size] = 0;
		printf("받은 메시지 : %s\n", message);
	}

	//소켓 종료
	closesocket(client_socket);
	WSACleanup();


	return 0;
}