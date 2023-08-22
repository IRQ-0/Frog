#include "spc.h"

Spc::Spc(RS rs) {
	this->rs = rs;
}

int Spc::connect(void) {
	std::cout << "Connecting..." << std::endl;

	if (this->rs.RsInit() != 0) {
		std::cout << "Cannot connect to Server Power Controller (error: " << GetLastError() << ")" << std::endl;
		return 1;
	}

	std::cout << "Conected" << std::endl;

	return 0;
}

int Spc::getStatus(void) {
	HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int err = 0;

	this->rs.RsSend(STATUS_COMMAND);
	
	std::string str = this->rs.RsRecv(256, &err);

	if (err <= 0) {
		std::cout << "Error reading data from Server Power Controller" << std::endl;
		return 1;
	}

	int relays = str[0];

	printf("\nStatus (relays: %d)\n\n", relays);

	for (int i = 1; i <= relays; i++) {
		SetConsoleTextAttribute(hconsole, COLOR_DEFAULT);
		std::cout << "Relay nr. " << i << " -> status: ";

		switch (str[i]) {
			case '1':
				SetConsoleTextAttribute(hconsole, COLOR_ON);
				std::cout << "ON" << std::endl;
			break;

			case '0':
				SetConsoleTextAttribute(hconsole, COLOR_OFF);
				std::cout << "OFF" << std::endl;
			break;

			default:
				std::cout << "Invalid data" << std::endl;
		}
	}

	SetConsoleTextAttribute(hconsole, COLOR_DEFAULT);

	this->rs.RsClose();

	return 0;
}

spcProps Spc::readSpc(std::string filePath, int* err) {
	std::fstream f;
	std::string raw, buffer;

	f.open(filePath, std::ios::in);
	if (!f.good()) {
		std::cout << "Error reading Spc file" << std::endl;
		*err = 1;
	}

	std::getline(f, raw);
	f.close();

	Json js(raw);
	js.parse();

	spcProps props;

	props.alias = js.getValue("alias");
	if (props.alias.compare("") == 0) {
		std::cout << "Error while reading data (alias)" << std::endl;
		*err = 1;
	}

	//

	props.comPort = js.getValue("comPort");
	if (props.comPort.compare("") == 0) {
		std::cout << "Error while reading data (comPort)" << std::endl;
		*err = 1;
	}

	//

	buffer = js.getValue("baud");
	if (buffer.compare("") == 0) {
		std::cout << "Error while reading data (baud)" << std::endl;
		*err = 1;
	}

	props.baud = atoi(buffer.c_str());
	if (props.baud <= 0) {
		std::cout << "Error while reading data baud" << std::endl;
		*err = 1;
	}

	//

	buffer = js.getValue("fParity");
	if (buffer.compare("") == 0) {
		std::cout << "Error while reading data (fParity)" << std::endl;
		*err = 1;
	}
	
	props.fParity = atoi(buffer.c_str());

	//

	buffer = js.getValue("parity");
	if (buffer.compare("") == 0) {
		std::cout << "Error while reading data (parity)" << std::endl;
		*err = 1;
	}
	
	props.parity = atoi(buffer.c_str());

	//

	buffer = js.getValue("stopBits");
	if (buffer.compare("") == 0) {
		std::cout << "Error while reading data (stopBits)" << std::endl;
		*err = 1;
	}
	
	props.stopBits = atoi(buffer.c_str());
	if (props.stopBits <= 0) {
		std::cout << "Error while reading data (stopBits)" << std::endl;
		*err = 1;
	}

	switch (props.stopBits) {
		case 1:
			props.stopBits = ONESTOPBIT;
		break;

		case 15:
			props.stopBits = ONE5STOPBITS;
		break;

		case 2:
			props.stopBits = TWOSTOPBITS;
		break;

		default:
			std::cout << "Error while reading data (stopBits)" << std::endl;
			*err = 1;
	}
	//

	buffer = js.getValue("byteSize");
	if (buffer.compare("") == 0) {
		std::cout << "Error while reading data (byteSize)" << std::endl;
		*err = 1;
	}
	
	props.byteSize = atoi(buffer.c_str());
	if (props.byteSize <= 0) {
		std::cout << "Error while reading data (byteSize)" << std::endl;
		*err = 1;
	}


	return props;
}

int Spc::relayOn(unsigned char number) {
	char tab[] = {ON_COMMAND, (char) number, '\0'};
	std::string buffer = std::string(tab);
	
	this->rs.RsSend(buffer);
	
	this->rs.RsClose();

	return 0;
}

int Spc::relayOff(unsigned char number) {
	char tab[] = {OFF_COMMAND, (char) number, '\0'};
	std::string buffer = std::string(tab);
	
	this->rs.RsSend(buffer);
	
	this->rs.RsClose();

	return 0;
}