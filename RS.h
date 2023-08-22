#ifndef RS_H
#define RS_H

#include <iostream>
#include <string>
#include <windows.h>

class RS {
	public:
		RS() {};
		RS(std::string, int, int, int, int, int);

		int RsInit(void);
		int RsSend(std::string);
		std::string RsRecv(int, int*);
		void RsClose(void);
		HANDLE RsGetHandle(void);
		
	private:
		std::string port;
		int baudRate;
		int fParity;
		int parity;
		int stopBits;
		int byteSize;

		HANDLE portHandle;
};

#endif
