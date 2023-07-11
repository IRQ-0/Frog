#include "static.h"

Static::Static(SOCKET sock) {
	this->mainSock = sock;
}

int Static::getTop(void) {
	std::string top;
	sendData(this->mainSock, "T");
	
	top = recvData(this->mainSock);

	if (top.length() <= 0) {
		std::cout << "Error while reading input data" << std::endl;
		return 1;
	}

	std::cout << top << std::endl;

	return 0;
}

int Static::getDf(void) {
	std::string df;
	sendData(this->mainSock, "D");
	
	df = recvData(this->mainSock);

	if (df.length() <= 0) {
		std::cout << "Error while reading input data" << std::endl;
		return 1;
	}

	std::cout << df << std::endl;

	return 0;
}

int Static::getBlk(void) {
	std::string blk;
	sendData(this->mainSock, "B");
	
	blk = recvData(this->mainSock);

	if (blk.length() <= 0) {
		std::cout << "Error while reading input data" << std::endl;
		return 1;
	}

	std::cout << blk << std::endl;

	return 0;
}

int Static::getIf(void) {
	std::string interfaces;
	sendData(this->mainSock, "I");
	
	interfaces = recvData(this->mainSock);

	if (interfaces.length() <= 0) {
		std::cout << "Error while reading input data" << std::endl;
		return 1;
	}

	std::cout << interfaces << std::endl;

	return 0;
}