

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
#define PORT 50050

//소켓 구조체 선언
typedef struct {
    SOCKET socket;
    int id;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];//클라이언트 전용 소켓구조체 배열
int clientCount = 0;            //클라이언트 명수
HANDLE clientThreads[MAX_CLIENTS]; //클라이언트 관리 스레드 개수 배열

void InitializeWinsock();
SOCKET CreateServerSocket();
void BindSocket(SOCKET serverSocket);
void ListenForConnections(SOCKET serverSocket);
void AcceptConnections(SOCKET serverSocket);
DWORD WINAPI ClientHandler(LPVOID clientInfo);
void Cleanup(SOCKET serverSocket);

int main() {
    SOCKET serverSocket;//서버 전용 소켓 선언
    
    //소켓 초기화
    InitializeWinsock();

    //소켓 생성 후 주입
    serverSocket = CreateServerSocket();

    //소켓 주소 정의
    BindSocket(serverSocket);

    //소켓에서 접속 대기
    ListenForConnections(serverSocket);

    //소켓에 접속하는 클라이언트 허용
    //이 함수 안에 스레드가 동작함
    AcceptConnections(serverSocket);

    //소켓 통신 종료 서버다운
    Cleanup(serverSocket);

    return 0;
}

void InitializeWinsock() {
    //윈도우 소켓 초기화
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("윈도우 소켓 초기화 실패 : %d\n", result);
        exit(EXIT_FAILURE);
    }
    printf("소켓 초기화 완료\n");
}

SOCKET CreateServerSocket() {
    //서버용 소켓 생성(접속 받는 소켓)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("서버 소켓 생성 실패 : %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    //성공 서버 소켓 반환
    printf("소켓 생성 완료\n");
    return serverSocket;
}

void BindSocket(SOCKET serverSocket) {
    //서버 소켓에 주소 주입
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    int result = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        printf("주소 삽입 실패 : %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("소켓 주소 주입 완료\n");
}

void ListenForConnections(SOCKET serverSocket) {
    //소켓으로 클라이언트 접속 가능하게 open
    int result = listen(serverSocket, MAX_CLIENTS);
    if (result == SOCKET_ERROR) {
        printf("듣기 실패 : %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("소켓 오픈 완료\n");
}

void AcceptConnections(SOCKET serverSocket) {
    //소켓에 연결시 허용
    printf("소켓 연결 허용중...\n");
    /*
    //접속한 클라이언트 전용 소켓 생성
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        printf("접속 실패 : %d\n", WSAGetLastError());
    }

    //접속자 수가 5명 이하면
    if (clientCount < MAX_CLIENTS) {
        printf("소켓 접속 완료, 남은 접속 수 : %d\n", MAX_CLIENTS - clientCount - 1);
        //클라이언트 전용 구조체에 소켓 위임
        clients[clientCount].socket = clientSocket;
        clients[clientCount].id = clientCount;
        //이후 스레드 생성 후 스레드에 소켓던지고 관리 위임
        //ClientHandler라는 스레드 동작 메소드에 소켓 위임
        CreateThread(NULL, 0, ClientHandler, (void*)&clients[clientCount], 0, NULL);
        clientCount++;
    }
    else {
        //5이상이면 접속 못함을 알리고 소켓 삭제
        printf("5명 초과 접속 접속 불가능 \n");
        closesocket(clientSocket);
    }*/
    
    while (1) {
        //접속한 클라이언트 전용 소켓 생성
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            printf("접속 실패 : %d\n", WSAGetLastError());
            continue;
        }
        
        //접속자 수가 5명 이하면
        if (clientCount < MAX_CLIENTS) {
            printf("소켓 접속 완료, 남은 접속 수 : %d\n", MAX_CLIENTS-clientCount-1);
            //클라이언트 전용 구조체에 소켓 위임
            clients[clientCount].socket = clientSocket;
            clients[clientCount].id = clientCount;
            //이후 스레드 생성 후 스레드에 소켓던지고 관리 위임
            //ClientHandler라는 스레드 동작 메소드에 소켓 위임
            CreateThread(NULL, 0, ClientHandler, (void*)&clients[clientCount], 0, NULL);
            clientCount++;
        }
        else {
            //5이상이면 접속 못함을 알리고 소켓 삭제
            printf("5명 초과 접속 접속 불가능 \n");
            closesocket(clientSocket);
        }
    }
}

DWORD WINAPI ClientHandler(LPVOID clientInfo) {
    ClientInfo* client = (ClientInfo*)clientInfo;
    char buffer[BUFFER_SIZE];
    int result;

    //ID 보냄
    sprintf_s(buffer, "Your ID is : %d\n", client->id);
    //send(client->socket, buffer, strlen(buffer), 0);

    //에코 통신 시작
    while (1) {
        //메모리 buffer를 0으로 초기화
        memset(buffer, 0, BUFFER_SIZE);
        //client로 부터 데이터 받고 데이터를 안보내면 =0이면 소켓 종료
        result = recv(client->socket, buffer, BUFFER_SIZE, 0);
        if (result > 0) {
            //데이터를 보내면 에코
            send(client->socket, buffer, result, 0);
        }
        else if (result == 0) {
            printf("Client %d의 통신종료 요청\n", client->id);
            break;
        }
        else {
            printf("전송 실패 : %d\n", WSAGetLastError());
            break;
        }
    }

    //소켓종료
    closesocket(client->socket);
    return 0;
}

void Cleanup(SOCKET serverSocket) {
    //소켓 종료
    closesocket(serverSocket);
    WSACleanup();
}

