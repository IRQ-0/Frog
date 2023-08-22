#ifndef SPC_H
#define SPC_H

#include <iostream>
#include <fstream>
#include "RS.h"
#include "Json.h"

#define COLOR_DEFAULT 15
#define COLOR_ON 10
#define COLOR_OFF 12

#define STATUS_COMMAND "S"
#define ON_COMMAND 'O'
#define OFF_COMMAND 'F'

struct spcProps {
	std::string alias;

	std::string comPort;
	int baud;
	int fParity;
	int parity;
	int stopBits;
	int byteSize;
};

class Spc {
	public:
		Spc() {};
		Spc(RS);

		spcProps readSpc(std::string, int*);

		int connect(void);
		int getStatus(void);
		int relayOn(unsigned char);
		int relayOff(unsigned char);
	
	private:
		RS rs;
};

#endif