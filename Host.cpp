#include "Host.h"

Host::Host(std::string filename) {
	this->filename = filename;
	std::fstream f;

	f.open(this->filename, std::ios::in);
	if (!f.good()) {
		std::cout << "Cannot find file" << std::endl;
	}

	std::getline(f, this->raw);
	f.close();

	Json js(this->raw);
	js.parse();

	this->alias = js.getValue("alias");
	if (this->alias.compare("") == 0) {
		std::cout << "Invalid alias value" << std::endl;
	}

	this->ip = js.getValue("ip");
	if (this - ip.compare("") == 0) {
		std::cout << "Invalid ip value" << std::endl;
	}

	if (js.getValue("port").compare("") == 0) {
		std::cout << "Invalid port value" << std::endl;
	}
	this->port = atoi(js.getValue("port").c_str());
}

std::string Host::getFilename(void) {
	return this->filename;
}