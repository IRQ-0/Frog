#ifndef HOST_H
#define HOST_H

#include <iostream>
#include <string>
#include <fstream>
#include "Json.h"

/*
Host properties:
- alias = string name for host
- ip    = ip address of a host
- port  = port

Example json:
{"alias":"test","ip":"127.0.0.1","port":"22"}

*/

struct hostProps {
	std::string alias;
	std::string ip;
	int port;
};

class Host {
	public:
		Host() {};
		Host(std::string);

		std::string getFilename(void);

		// Host props
		std::string alias;
		std::string ip;
		int port;

	private:
		std::string filename;
		std::string raw;

};

#endif