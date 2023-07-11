#ifndef STATIC_H
#define STATIC_H

#include <iostream>
#include "telnet.h"

class Static {
	public:
		Static() {};
		Static(SOCKET);
		
		int getTop(void);
		int getDf(void);
		int getBlk(void);
		int getIf(void);

	private:
		SOCKET mainSock;
};

#endif