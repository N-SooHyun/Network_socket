

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
#define PORT 50050

//���� ����ü ����
typedef struct {
    SOCKET socket;
    int id;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];//Ŭ���̾�Ʈ ���� ���ϱ���ü �迭
int clientCount = 0;            //Ŭ���̾�Ʈ ���
HANDLE clientThreads[MAX_CLIENTS]; //Ŭ���̾�Ʈ ���� ������ ���� �迭

void InitializeWinsock();
SOCKET CreateServerSocket();
void BindSocket(SOCKET serverSocket);
void ListenForConnections(SOCKET serverSocket);
void AcceptConnections(SOCKET serverSocket);
DWORD WINAPI ClientHandler(LPVOID clientInfo);
void Cleanup(SOCKET serverSocket);

int main() {
    SOCKET serverSocket;//���� ���� ���� ����
    
    //���� �ʱ�ȭ
    InitializeWinsock();

    //���� ���� �� ����
    serverSocket = CreateServerSocket();

    //���� �ּ� ����
    BindSocket(serverSocket);

    //���Ͽ��� ���� ���
    ListenForConnections(serverSocket);

    //���Ͽ� �����ϴ� Ŭ���̾�Ʈ ���
    //�� �Լ� �ȿ� �����尡 ������
    AcceptConnections(serverSocket);

    //���� ��� ���� �����ٿ�
    Cleanup(serverSocket);

    return 0;
}

void InitializeWinsock() {
    //������ ���� �ʱ�ȭ
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("������ ���� �ʱ�ȭ ���� : %d\n", result);
        exit(EXIT_FAILURE);
    }
    printf("���� �ʱ�ȭ �Ϸ�\n");
}

SOCKET CreateServerSocket() {
    //������ ���� ����(���� �޴� ����)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("���� ���� ���� ���� : %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    //���� ���� ���� ��ȯ
    printf("���� ���� �Ϸ�\n");
    return serverSocket;
}

void BindSocket(SOCKET serverSocket) {
    //���� ���Ͽ� �ּ� ����
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    int result = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        printf("�ּ� ���� ���� : %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("���� �ּ� ���� �Ϸ�\n");
}

void ListenForConnections(SOCKET serverSocket) {
    //�������� Ŭ���̾�Ʈ ���� �����ϰ� open
    int result = listen(serverSocket, MAX_CLIENTS);
    if (result == SOCKET_ERROR) {
        printf("��� ���� : %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("���� ���� �Ϸ�\n");
}

void AcceptConnections(SOCKET serverSocket) {
    //���Ͽ� ����� ���
    printf("���� ���� �����...\n");
    /*
    //������ Ŭ���̾�Ʈ ���� ���� ����
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        printf("���� ���� : %d\n", WSAGetLastError());
    }

    //������ ���� 5�� ���ϸ�
    if (clientCount < MAX_CLIENTS) {
        printf("���� ���� �Ϸ�, ���� ���� �� : %d\n", MAX_CLIENTS - clientCount - 1);
        //Ŭ���̾�Ʈ ���� ����ü�� ���� ����
        clients[clientCount].socket = clientSocket;
        clients[clientCount].id = clientCount;
        //���� ������ ���� �� �����忡 ���ϴ����� ���� ����
        //ClientHandler��� ������ ���� �޼ҵ忡 ���� ����
        CreateThread(NULL, 0, ClientHandler, (void*)&clients[clientCount], 0, NULL);
        clientCount++;
    }
    else {
        //5�̻��̸� ���� ������ �˸��� ���� ����
        printf("5�� �ʰ� ���� ���� �Ұ��� \n");
        closesocket(clientSocket);
    }*/
    
    while (1) {
        //������ Ŭ���̾�Ʈ ���� ���� ����
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            printf("���� ���� : %d\n", WSAGetLastError());
            continue;
        }
        
        //������ ���� 5�� ���ϸ�
        if (clientCount < MAX_CLIENTS) {
            printf("���� ���� �Ϸ�, ���� ���� �� : %d\n", MAX_CLIENTS-clientCount-1);
            //Ŭ���̾�Ʈ ���� ����ü�� ���� ����
            clients[clientCount].socket = clientSocket;
            clients[clientCount].id = clientCount;
            //���� ������ ���� �� �����忡 ���ϴ����� ���� ����
            //ClientHandler��� ������ ���� �޼ҵ忡 ���� ����
            CreateThread(NULL, 0, ClientHandler, (void*)&clients[clientCount], 0, NULL);
            clientCount++;
        }
        else {
            //5�̻��̸� ���� ������ �˸��� ���� ����
            printf("5�� �ʰ� ���� ���� �Ұ��� \n");
            closesocket(clientSocket);
        }
    }
}

DWORD WINAPI ClientHandler(LPVOID clientInfo) {
    ClientInfo* client = (ClientInfo*)clientInfo;
    char buffer[BUFFER_SIZE];
    int result;

    //ID ����
    sprintf_s(buffer, "Your ID is : %d\n", client->id);
    //send(client->socket, buffer, strlen(buffer), 0);

    //���� ��� ����
    while (1) {
        //�޸� buffer�� 0���� �ʱ�ȭ
        memset(buffer, 0, BUFFER_SIZE);
        //client�� ���� ������ �ް� �����͸� �Ⱥ����� =0�̸� ���� ����
        result = recv(client->socket, buffer, BUFFER_SIZE, 0);
        if (result > 0) {
            //�����͸� ������ ����
            send(client->socket, buffer, result, 0);
        }
        else if (result == 0) {
            printf("Client %d�� ������� ��û\n", client->id);
            break;
        }
        else {
            printf("���� ���� : %d\n", WSAGetLastError());
            break;
        }
    }

    //��������
    closesocket(client->socket);
    return 0;
}

void Cleanup(SOCKET serverSocket) {
    //���� ����
    closesocket(serverSocket);
    WSACleanup();
}

