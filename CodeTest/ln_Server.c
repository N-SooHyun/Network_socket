/*

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);
int main(int argc, char* argv[]) {
	int serv_sock;
	int clnt_sock;

	//구조체 변수 생성 아마도 헤더에 포함된 구조체 소켓정보를 가지고 있는듯
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr; 

	//socklen_t타입의 변수 생성
	socklen_t clnt_addr_size;

	char message[]	="Hello World!"; // 실제 메시지
	//서버를 먼저 컴파일 시켜서 listen 요청가능하게 하고 //argc값 main 함수가 실행이 될때 2가 아니라면 
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]); 
		exit(1);
	}	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

	if (clnt_sock == -1)
		error_handling("accept() error");

	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	return 0;
}
void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr); exit(1);
}

*/