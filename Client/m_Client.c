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
		printf("1. ���� ��� \n");
		printf("2. ����\n");
		printf("�Է� : ");
		scanf_s("%c", &menu_choice, 1);

		switch (menu_choice) {
		case '1':
			SockFun();
			break;
		case '2':
			printf("������...\n");
			return 0;
		default:
			printf("��ȿ���� ���� �����̿��� �ٽ� �õ��ϼ��� \n");
			break;
		}
	}
}

int SockFun() {
	WSADATA wsa;
	//Ŭ���̾�Ʈ ���� ����
	SOCKET client_socket;
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE];
	int recv_size;

	//1 ���� �ʱ�ȭ
	printf("���� �ʱ�ȭ �� .. \n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("���� ���� �ڵ� : %d\n", WSAGetLastError());
		return 1;
	}
	printf("�ʱ�ȭ �Ϸ�\n");

	//2. ���� ����
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("���� ���� ���� : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//3. ������ ������ �ּ� ����
	server.sin_family = AF_INET;
	//server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server.sin_port = htons(PORT);
	if (inet_pton(AF_INET, SERVER_ADDR, &server.sin_addr.s_addr) <= 0) {
		printf("��ȿ���� ���� �ּ� \n");
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	//4. �ּ��� server�� ����
	if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("���� ���� : %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}
	printf("���� ���� ����\n");



	//5. ������ ����
	char message[BUFFER_SIZE];
	while (1) {
		fputs("�޽��� �Է�(Q to quit) : ", stdout);
		fgets(message, BUFFER_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		send(client_socket, message, (int)strlen(message), 0);
		recv_size = recv(client_socket, message, BUFFER_SIZE - 1, 0);
		message[recv_size] = 0;
		printf("���� �޽��� : %s\n", message);
	}

	//���� ����
	closesocket(client_socket);
	WSACleanup();


	return 0;
}