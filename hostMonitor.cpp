#include "hostMonitor.h"

hostMonitor::hostMonitor(std::vector<SOCKET> s, std::vector<Host> h) {
	this->sockets = s;
	this->hosts = h;
}

void hostMonitor::initDisplay(void) {
	system("cls");

	std::cout << "|      ALIAS      |       IP       |  PORT  |  PROCESSES  |  CPU LOAD  |  RAM TOTAL  |  RAM USED  |  RAM USED % |  SWAP TOTAL  |  SWAP USED  |  SWAP USED %  |  HIGH TOTAL  |  HIGH USED  |  HIGH USED %  |" << std::endl;
}

void hostMonitor::printLine(statusData data, int pos) {
  	std::cout << "|";

  	std::string alias = (this->hosts.at(pos)).alias;
  	std::cout << alias;

  	for (int i = 0; i < (18 - (alias.length()) - 1); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	std::string ip = (this->hosts.at(pos)).ip;
  	std::cout << ip;

  	for (int i = 0; i < (17 - (ip.length()) - 1); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	int port = (this->hosts.at(pos)).port;
  	std::cout << port;

  	for (int i = 0; i < (9 - (std::to_string(port).length()) - 1); i++) {
  	  std::cout << " ";
  	}

  	std::cout << "|";
  
  	setColor(COLOR_PROCS);
  	std::cout << data.processCount;
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (14 - (std::to_string(data.processCount).length()) - 1); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";
  
  	setColor(COLOR_CPU);
  	std::cout << data.cpuLoad << " %";
	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (13 - (std::to_string(data.cpuLoad).length()) - 3); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_RAM);
  	std::cout << data.totalRam << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (14 - (std::to_string(data.totalRam).length()) - 4); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_RAM);
  	std::cout << data.usedRam << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (13 - (std::to_string(data.usedRam).length()) - 4); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_RAM);
  	std::cout << data.procRam << " %";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (14 - (std::to_string(data.procRam).length()) - 3); i++) {
    	std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_SWAP);
  	std::cout << data.totalSwap << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (15 - (std::to_string(data.totalSwap).length()) - 4); i++) {
    	std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_SWAP);
  	std::cout << data.usedSwap << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (14 - (std::to_string(data.usedSwap).length()) - 4); i++) {
    	std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_SWAP);
  	std::cout << data.procSwap << " %";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (16 - (std::to_string(data.procSwap).length()) - 3); i++) {
	    std::cout << " ";
  	}

	std::cout << "|";

  	setColor(COLOR_HIGH);
  	std::cout << data.totalHigh << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (15 - (std::to_string(data.totalHigh).length()) - 4); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|";

  	setColor(COLOR_HIGH);
  	std::cout << data.usedHigh << " MB";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (14 - (std::to_string(data.usedHigh).length()) - 4); i++) {
	    std::cout << " ";
  	}

 	std::cout << "|";

  	setColor(COLOR_HIGH);
  	std::cout << data.procHigh << " %";
  	setColor(COLOR_DEFAULT);

  	for (int i = 0; i < (16 - (std::to_string(data.procHigh).length()) - 3); i++) {
	    std::cout << " ";
  	}

  	std::cout << "|" << std::endl;

}

statusData hostMonitor::readData(int pos, int* err) {
	std::string in;
	statusData out;
	SOCKET sock = (this->sockets).at(pos);

	sendData(sock, "R");

	in = recvData(sock);

	if (in.length() <= 0) {
		std::cout << "Error while reading data" << std::endl;
		*err = 1;
	}

	in = in.substr(0, in.length() - 1);

	Json j(in);
	j.parse();

	std::string buffer;

	buffer = j.getValue(CPULOAD_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.cpuLoad = atoi(buffer.c_str());
	}

	buffer = j.getValue(PROCESS_COUNT);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.processCount = atoi(buffer.c_str());
	}

	// RAM

	buffer = j.getValue(USED_RAM_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.usedRam = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	buffer = j.getValue(TOTAL_RAM_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.totalRam = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	if (out.totalRam == 0) {
		out.procRam = 0;
	} else {
		out.procRam = static_cast<int> (((float) out.usedRam / (float)out.totalRam) * 100.0);
	}

	// SWAP

	buffer = j.getValue(USED_SWAP_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.usedSwap = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	buffer = j.getValue(TOTAL_SWAP_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.totalSwap = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	if (out.totalSwap == 0) {
		out.procSwap = 0;
	} else {
		out.procSwap = static_cast<int> (((float) out.usedSwap / (float) out.totalSwap) * 100);
	}

	// High

	buffer = j.getValue(USED_HIGH_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.usedHigh = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	buffer = j.getValue(TOTAL_HIGH_KEY);
	if (checkNumber(buffer)) {
		std::cout << "Invalid input" << std::endl;
		*err = 1;
	} else {
		out.totalHigh = std::stoll(buffer.c_str()) * (float) 0.000001;
	}

	if (out.totalHigh == 0) {
		out.procHigh = 0;
	} else {
		out.procHigh = static_cast<int> (((float) out.usedHigh / (float) out.totalHigh) * 100);
	}

	return out;

}

int hostMonitor::checkNumber(std::string in) {
	if (in.length() <= 0 || (in.compare("") == 0)) {
		return 1;
	}

	for (int i = 0; i < in.length(); i++) {
		if (!isdigit(in[i])) {
			return 1;
		}
	}

	return 0;
}

void hostMonitor::setColor(int color) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
 	SetConsoleTextAttribute(handle, color);
}