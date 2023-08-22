#include "RS.h"

RS::RS(std::string port, int baud, int fParity, int parity, int stopBits, int byteSize) {
    this->port = port;
    this->baudRate = baud;
    this->fParity = fParity;
    this->parity = parity;
    this->stopBits = stopBits;
    this->byteSize = byteSize;
}

int RS::RsInit(void) {
    std::string portString = "\\\\.\\" + this->port;

	DCB dcb = {0};
    this->portHandle = CreateFile((LPCSTR) portString.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    
    if (this->portHandle == INVALID_HANDLE_VALUE) {
    	return 1;
	}
    
    dcb.DCBlength = sizeof(dcb);

    dcb.BaudRate = this->baudRate;
    dcb.fParity = this->fParity;
    dcb.Parity = this->parity;
    dcb.StopBits = this->stopBits;
    dcb.ByteSize = this->byteSize;
	
    if (SetCommState(this->portHandle, &dcb) == 0) {
        return 1;
    }

    return 0;
}

std::string RS::RsRecv(int count, int* recv) {
    char* buffer = new char[count];
    memset(buffer, 0, count);

    ReadFile(this->portHandle, buffer, count, (DWORD*) recv, 0);

    return std::string(buffer);
}

int RS::RsSend(std::string in) {
    int sent = 0;

    WriteFile(this->portHandle, (LPCVOID) in.c_str(), in.length(), (DWORD*) &sent, 0);
    
    return sent;
}

HANDLE RS::RsGetHandle(void) {
    return this->portHandle;
}

void RS::RsClose(void) {
	CloseHandle(this->portHandle);
}
