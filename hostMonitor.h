#ifndef HOST_MONITOR_H
#define HOST_MONITOR_H

#include <iostream>
#include <thread>
#include <vector>
#include "json.h"
#include "Host.h"
#include "telnet.h"

#define COLOR_CPU 9
#define COLOR_PROCS 10
#define COLOR_RAM 11
#define COLOR_SWAP 12
#define COLOR_HIGH 13
#define COLOR_DEFAULT 15

// Json keys
#define PROCESS_COUNT "processcount"
#define CPULOAD_KEY "cpuload"

#define FREE_RAM_KEY "freeram"
#define USED_RAM_KEY "usedram"
#define TOTAL_RAM_KEY "totalram"

#define FREE_SWAP_KEY "freeswap"
#define USED_SWAP_KEY "usedswap"
#define TOTAL_SWAP_KEY "totalswap"

#define FREE_HIGH_KEY "freehigh"
#define USED_HIGH_KEY "usedhigh"
#define TOTAL_HIGH_KEY "totalhigh"

struct statusData {
  int processCount;
  int cpuLoad;
  
  int procRam;
  long long usedRam;
  long long totalRam;

  int procSwap;
  long long usedSwap;
  long long totalSwap;
  
  int procHigh;
  long long usedHigh;
  long long totalHigh;
};

class hostMonitor {
	public:
		hostMonitor() {};
		
		hostMonitor(std::vector<SOCKET>, std::vector<Host>);

		void initDisplay(void);
		void printLine(statusData, int);
		int checkNumber(std::string);

		void setColor(int color);

		statusData readData(int, int*);
	
	private:
		std::vector<SOCKET> sockets;
		std::vector<Host> hosts;	
};

#endif