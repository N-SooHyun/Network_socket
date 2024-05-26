#include <stdio.h>
#include <WinSock2.h>

//윈도우 전용 지시자 메시지
#pragma comment(lib, "ws2_32.lib")

#define PORT 50050
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5 //최대 동시 접속 클라이언트 수

//동시 접속 클라이언트 구조체
typedef struct {
	SOCKET client_socket;
	int client_id;
}ClientThreadArgs;

//각 클라이언트 접속마다 스레드를 생성하여 스레드가 클라이언트 통신을 담당

//클라이언트 접속 시 고유한 ID부여로 식별

void HandelClient(void* args);

int main() {
	//소켓 라이브러리의 구조체
	WSADATA wsa;
	//서버는 접속을 받는 서버소켓과 접속 후 통신을 유지시켜줄 클라이언트 소켓으로 나뉨
	SOCKET server_socket, client_socket;
	//IPV4의 주소체계를 나타내는 구조체 server와 client선언
	struct sockaddr_in server, client;
	char buffer[BUFFER_SIZE];
	int client_len, recv_size, client_id = 1;
	//스레드 개수 제한
	HANDLE client_threads[MAX_CLIENTS];
	int i;

	//1. 윈도우 소켓의 초기화 작업
	printf("소켓 초기화.. \n");
	//2.2버전 사용을 의미
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("소켓 초기화 실패 : %d\n", WSAGetLastError());
		return 1;
	}

	//2. 소켓생성
	//AF_INET:Domain, SOCK_STREAM:type(TCP), 0:Protocol(TCP)
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		//INVALID_SOCKET와 값이 같으면 실패한거임
		printf("소켓 생성 실패 : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("소켓 생성 \n");

	//3. 서버의 주소와 포트 설정 Bind()
	//AF_INET : 2 숫자 의미 : internetwork: UDP, TCP, etc.
	//AF_INET:주소체계(IPv4를 의미), sin_addr:32비트 IP주소정보(바이트 순서로 저장), sin_port:16비트 포트정보(바이트 순서로 저장)
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	//주입시킨 IP와 PORT번호를 통해서 bind시키고 코드가 error일시 실패
	//bind(int sockfd, struct sockaddr* myaddr, int addrlen)로 이루어진 함수
	//sockfd : 주소를 할당하고자하는 소켓, myaddr할당하고자 하는 주소 정보를 지니고 있는 sockaddr_in구조체 변수의 포인터 인자 전달
	//addrlen : 인자로 전달된 주소정보 구조체의 길이
	if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("BIND실패 에러 코드 : %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	printf("주소 생성 \n");

	//4. 클라이언트의 접속 받을 준비 Listen()
	listen(server_socket, MAX_CLIENTS);//3개의 대기열
	printf("연결 대기중 ..\n");

	//5. 클라이언트 접속시 스레드로 관리
	while (1) {
		client_len = sizeof(struct sockaddr_in);

		//연결 허용
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len)) == INVALID_SOCKET) {
			printf("연결 실패 : %d\n", WSAGetLastError());
			closesocket(server_socket);
			WSACleanup();
			return 1;
		}
	}
	printf("연결 성공\n");

	//스레드 생성 후 스레드에 클라이언트 소켓 관리
	//구조체 포인터 생성 후 동적메모리로 스레드 스택 생성
	ClientThreadArgs* args = (ClientThreadArgs*)malloc(sizeof(ClientThreadArgs));
	args->client_socket = client_socket;//스레드 소켓에 클라이언트 소켓 주기
	args->client_id = client_id++;//id 주기
	//스레드 ID에 스레드 동작 핸들 실행
	client_threads[args->client_id - 1] = (HANDLE)_beginthread(HandleClient, 0, (void*)args);


	//접속 수가 MAX를 넘을 때 스레드 핸들러 작업종료
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (client_threads[i] != NULL) {
			WaitForSingleObject(client_threads[i], INFINITE);
			CloseHandle(client_threads[i]);
		}
	}
	closesocket(server_socket);
	WSACleanup();


	/*
	//5. 접속시 허용 Accept()
	//accept(int sock, struct sockaddr* addr, int *addrlen)함수
	//sock:허용받은 서버소켓, addr : 연결요청을한 클라이언트의 주소, addrlen : 전달된 주소의 변수 크기(바이트)
	client_len = sizeof(struct sockaddr_in);
	if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len)) == INVALID_SOCKET) {
		printf("접속 허용 실패 : %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}
	printf("접속 허용 완료 \n");
	*/
	/*
	//6. 데이터 받기 recv
	while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) != 0) {
		buffer[recv_size] = '\0';
		printf("Client Message : %s\n", buffer);
		send(client_socket, buffer, recv_size, 0);
	}
	*/
	

	/*
	//8. 종료
	closesocket(client_socket);
	closesocket(server_socket);
	WSACleanup();
	*/
	return 0;
}


void HandleClient(void* args) {
	//클라이언트 접속시 스레드 생성하여 관리
	ClientThreadArgs* client_args = (ClientThreadArgs*)args;
	//구조체 포인터 client_socket에 삽입
	SOCKET client_socket = client_args->client_socket;
	//구조체 포인터 Client_id부여
	int client_id = client_args->client_id;
	char buffer[BUFFER_SIZE];
	int recv_size;

	//클라이언트에게 ID전송
	sprintf(buffer, "너의 식별 ID : %d\n", client_id);
	send(client_socket, buffer, strlen(buffer), 0);

	//클라이언트와의 통신 클라이언트가 어떤 메시지도 안보내면 빠져나옴
	while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		buffer[recv_size] = '\0';
		printf("Client %d message: %s\n", client_id, buffer);

		// 클라이언트에게 에코
		send(client_socket, buffer, recv_size, 0);
	}

	if (recv_size == 0) {
		printf("클라이언트 %d 연결 종료\n", client_id);
	}
	else if (recv_size == SOCKET_ERROR) {
		printf("클라이언트 %d 에러 %d\n", client_id, WSAGetLastError());
	}

	closesocket(client_socket);
	free(client_args);
	_endthread();
}
