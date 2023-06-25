#ifndef TELNET_H
#define TELNET_H

#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>

// Start modes for newsocket()
#define AS_CLIENT 1
#define AS_SERVER 2

// Error macros
#define ERROR_EXIT(a) *error = a; return (long unsigned int) 0;
#define WSA_STARTUP_ERROR 1
#define SOCKET_CREATE_ERROR 2
#define CLIENT_CONNECT_ERRROR 3
#define BIND_ERROR 4
#define LISTEN_ERROR 5
#define ACCEPT_ERROR 6

// Socket status macors
#define CONNECTED 1
#define DISCONNECTED 0

// Info about client socket
struct socketinfo {
	std::string clientsrcadress;
	int clientsrcport;
};

// Recive buffer size
#define BUFFER_SIZE 65536

// -- LIBRARY FUNCTIONS --
SOCKET newSocket(std::string, int, int, int*); 
/* Create new socket (<ip address>,
					  <port>,
					  <AS_CLIENT - connect to server on specified address and port, AS_SERVER - create server on that address and port>,
					  <error pointer>)

Error pointer is 0 if everything is okay, otherwise it is set to error code. If error pointer is not 0 returned socket is invalid.

Returns: created socket.
*/
SOCKET acceptClient(SOCKET, socketinfo*, int*);
/* Accept one client (<server socket>,
					  <pointer to socketinfo struct to get info about client>,
					  <error pointer>)

Error pointer is 0 if everything is okay, otherwise it is set to error code. If error pointer is not 0 returned socket is invalid.

Returns: client socket.
*/

int sendData(SOCKET, std::string);
/* Send data to client (<text data terminated with 0x0>
						<client socket (from acceptClient())>)

Returns: number of sent bytes.
*/

int sendData(SOCKET, const char*, size_t);
/* Send data to client (<pointer to binary data array>
						<size of data array>
						<client socket (from acceptClient())>)

Returns: number of sent bytes.
*/

const char* recvData(SOCKET);
/* Recive text data from client (<client socket>)


Returns: pointer to recived data array.
*/
void close(SOCKET);
/* Close socket (<socket to close>)

Returns: nothing
*/



#endif