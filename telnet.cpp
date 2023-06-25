#include "Telnet.h"

SOCKET newSocket(std::string ip, int port, int clisrv, int* error) {
	WSADATA wsaData;
	SOCKET sock;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ERROR_EXIT(WSA_STARTUP_ERROR);
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) {
    	WSACleanup();
    	ERROR_EXIT(SOCKET_CREATE_ERROR);
	}

	struct sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip.c_str());
	service.sin_port = htons(port);

	if (clisrv == AS_CLIENT) {
		if (connect(sock, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
			WSACleanup();
			ERROR_EXIT(CLIENT_CONNECT_ERRROR);
		}
	} else {
		if (bind(sock, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
    		close(sock);
    		ERROR_EXIT(BIND_ERROR);
    	}

    	if (listen(sock, 1) == SOCKET_ERROR) {
    		close(sock);
    		ERROR_EXIT(LISTEN_ERROR);
		}
	}

	return sock;
}

SOCKET acceptClient(SOCKET sock, socketinfo* sockinfo, int* error) {
	SOCKET acceptSocket = SOCKET_ERROR;
	sockaddr_in sockaddr;

	size_t len = sizeof(sockaddr);

	while (acceptSocket == SOCKET_ERROR)
		acceptSocket = accept(sock, (SOCKADDR*) &sockaddr, (int*) &len);

	if (acceptSocket == INVALID_SOCKET) {
		ERROR_EXIT(ACCEPT_ERROR);
	}

	sockinfo->clientsrcport = sockaddr.sin_port;
	sockinfo->clientsrcadress = std::string(inet_ntoa(sockaddr.sin_addr));
	
	return acceptSocket;
}

int sendData(SOCKET sock, std::string str) {
	int sent = send(sock, (const char*) str.c_str(), str.length(), 0);

	return sent;
}

int sendData(SOCKET sock, const char* data, size_t size) {
	int sent = send(sock, data, size, 0);

	return sent;
}

const char* recvData(SOCKET sock) {
	char* buffer = new char[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	if (recv(sock, buffer, BUFFER_SIZE, 0) <= 0) {
		return (const char*) NULL;
	}

	return buffer;
}


void close(SOCKET cli) {
	closesocket(cli);
}