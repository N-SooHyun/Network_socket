#include <stdio.h>
#include <WinSock2.h>

//������ ���� ������ �޽���
#pragma comment(lib, "ws2_32.lib")

#define PORT 50050
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5 //�ִ� ���� ���� Ŭ���̾�Ʈ ��

//���� ���� Ŭ���̾�Ʈ ����ü
typedef struct {
	SOCKET client_socket;
	int client_id;
}ClientThreadArgs;

//�� Ŭ���̾�Ʈ ���Ӹ��� �����带 �����Ͽ� �����尡 Ŭ���̾�Ʈ ����� ���

//Ŭ���̾�Ʈ ���� �� ������ ID�ο��� �ĺ�

void HandelClient(void* args);

int main() {
	//���� ���̺귯���� ����ü
	WSADATA wsa;
	//������ ������ �޴� �������ϰ� ���� �� ����� ���������� Ŭ���̾�Ʈ �������� ����
	SOCKET server_socket, client_socket;
	//IPV4�� �ּ�ü�踦 ��Ÿ���� ����ü server�� client����
	struct sockaddr_in server, client;
	char buffer[BUFFER_SIZE];
	int client_len, recv_size, client_id = 1;
	//������ ���� ����
	HANDLE client_threads[MAX_CLIENTS];
	int i;

	//1. ������ ������ �ʱ�ȭ �۾�
	printf("���� �ʱ�ȭ.. \n");
	//2.2���� ����� �ǹ�
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("���� �ʱ�ȭ ���� : %d\n", WSAGetLastError());
		return 1;
	}

	//2. ���ϻ���
	//AF_INET:Domain, SOCK_STREAM:type(TCP), 0:Protocol(TCP)
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		//INVALID_SOCKET�� ���� ������ �����Ѱ���
		printf("���� ���� ���� : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("���� ���� \n");

	//3. ������ �ּҿ� ��Ʈ ���� Bind()
	//AF_INET : 2 ���� �ǹ� : internetwork: UDP, TCP, etc.
	//AF_INET:�ּ�ü��(IPv4�� �ǹ�), sin_addr:32��Ʈ IP�ּ�����(����Ʈ ������ ����), sin_port:16��Ʈ ��Ʈ����(����Ʈ ������ ����)
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	//���Խ�Ų IP�� PORT��ȣ�� ���ؼ� bind��Ű�� �ڵ尡 error�Ͻ� ����
	//bind(int sockfd, struct sockaddr* myaddr, int addrlen)�� �̷���� �Լ�
	//sockfd : �ּҸ� �Ҵ��ϰ����ϴ� ����, myaddr�Ҵ��ϰ��� �ϴ� �ּ� ������ ���ϰ� �ִ� sockaddr_in����ü ������ ������ ���� ����
	//addrlen : ���ڷ� ���޵� �ּ����� ����ü�� ����
	if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("BIND���� ���� �ڵ� : %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	printf("�ּ� ���� \n");

	//4. Ŭ���̾�Ʈ�� ���� ���� �غ� Listen()
	listen(server_socket, MAX_CLIENTS);//3���� ��⿭
	printf("���� ����� ..\n");

	//5. Ŭ���̾�Ʈ ���ӽ� ������� ����
	while (1) {
		client_len = sizeof(struct sockaddr_in);

		//���� ���
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len)) == INVALID_SOCKET) {
			printf("���� ���� : %d\n", WSAGetLastError());
			closesocket(server_socket);
			WSACleanup();
			return 1;
		}
	}
	printf("���� ����\n");

	//������ ���� �� �����忡 Ŭ���̾�Ʈ ���� ����
	//����ü ������ ���� �� �����޸𸮷� ������ ���� ����
	ClientThreadArgs* args = (ClientThreadArgs*)malloc(sizeof(ClientThreadArgs));
	args->client_socket = client_socket;//������ ���Ͽ� Ŭ���̾�Ʈ ���� �ֱ�
	args->client_id = client_id++;//id �ֱ�
	//������ ID�� ������ ���� �ڵ� ����
	client_threads[args->client_id - 1] = (HANDLE)_beginthread(HandleClient, 0, (void*)args);


	//���� ���� MAX�� ���� �� ������ �ڵ鷯 �۾�����
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (client_threads[i] != NULL) {
			WaitForSingleObject(client_threads[i], INFINITE);
			CloseHandle(client_threads[i]);
		}
	}
	closesocket(server_socket);
	WSACleanup();


	/*
	//5. ���ӽ� ��� Accept()
	//accept(int sock, struct sockaddr* addr, int *addrlen)�Լ�
	//sock:������ ��������, addr : �����û���� Ŭ���̾�Ʈ�� �ּ�, addrlen : ���޵� �ּ��� ���� ũ��(����Ʈ)
	client_len = sizeof(struct sockaddr_in);
	if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len)) == INVALID_SOCKET) {
		printf("���� ��� ���� : %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}
	printf("���� ��� �Ϸ� \n");
	*/
	/*
	//6. ������ �ޱ� recv
	while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) != 0) {
		buffer[recv_size] = '\0';
		printf("Client Message : %s\n", buffer);
		send(client_socket, buffer, recv_size, 0);
	}
	*/
	

	/*
	//8. ����
	closesocket(client_socket);
	closesocket(server_socket);
	WSACleanup();
	*/
	return 0;
}


void HandleClient(void* args) {
	//Ŭ���̾�Ʈ ���ӽ� ������ �����Ͽ� ����
	ClientThreadArgs* client_args = (ClientThreadArgs*)args;
	//����ü ������ client_socket�� ����
	SOCKET client_socket = client_args->client_socket;
	//����ü ������ Client_id�ο�
	int client_id = client_args->client_id;
	char buffer[BUFFER_SIZE];
	int recv_size;

	//Ŭ���̾�Ʈ���� ID����
	sprintf(buffer, "���� �ĺ� ID : %d\n", client_id);
	send(client_socket, buffer, strlen(buffer), 0);

	//Ŭ���̾�Ʈ���� ��� Ŭ���̾�Ʈ�� � �޽����� �Ⱥ����� ��������
	while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		buffer[recv_size] = '\0';
		printf("Client %d message: %s\n", client_id, buffer);

		// Ŭ���̾�Ʈ���� ����
		send(client_socket, buffer, recv_size, 0);
	}

	if (recv_size == 0) {
		printf("Ŭ���̾�Ʈ %d ���� ����\n", client_id);
	}
	else if (recv_size == SOCKET_ERROR) {
		printf("Ŭ���̾�Ʈ %d ���� %d\n", client_id, WSAGetLastError());
	}

	closesocket(client_socket);
	free(client_args);
	_endthread();
}
