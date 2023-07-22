#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <dirent.h>
#include <sstream>
#include <stdexcept>
#include <conio.h>
#include <thread>
#include "Json.h"
#include "Host.h"
#include "telnet.h"
#include "hostMonitor.h"
#include "static.h"
#include "spc.h"

#define DEFAULT_HOSTS_LOCATION ".\\hosts"
#define DEFAULT_SPCS_LOCATION ".\\spcs"

#define PROMPT "frg> "

#define HOSTS_EXTENSION ".host"
#define SPCS_EXTENSION ".spc"

#define SPC_CONF_BAUD 4800
#define SPC_CONF_FPARITY 0
#define SPC_CONF_PARITY 0
#define SPC_CONF_STOPBITS ONESTOPBIT
#define SPC_CONF_BYTESIZE 8

std::vector<Host> loadHosts(std::string);
std::vector<spcProps> loadSpcs(std::string);
void printHelp(void);
void writeHost(std::string, std::string);
void deleteHost(std::string, std::string);
void listHosts(std::vector<Host>);
void monitorStart(std::vector<Host>);
SOCKET hostConnect(Host);
void thr(hostMonitor, int);
void gotoxy(int, int);

void getTop(std::string, std::vector<Host>);
void getDf(std::string, std::vector<Host>);
void getBlk(std::string, std::vector<Host>);
void getIf(std::string, std::vector<Host>);

void spcStatus(std::string);
void spcAdd(std::string, std::string);
void spcDel(std::string, std::string);
void spcList(std::vector<spcProps>);
void spcHelp(void);


/*
TCP COMMANDS:
R - get mem, cpu, proc etc. load info
T - top page
D - df screen
B - lsblk screen
I - ip a and ifconfig sreen
*/


int main(int argc, char** argv) {
    std::string hostsLocation, spcsLocation;
    int spcsFromParam = 0, hostsFromParam = 0;
    std::vector<Host> hosts;
    std::vector<spcProps> spcs;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    //SetConsoleTextAttribute(console, DEFAULT_COLOR);

    //HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursor;
    cursor.dwSize = 100;
    cursor.bVisible = TRUE;

    SetConsoleCursorInfo(console, &cursor);

    SetConsoleOutputCP(CP_UTF8);

    std::cout << "FROG v1.0" << std::endl << std::endl;

    if (argc == 2 && ((std::string(argv[1]).compare("--help") == 0) || (std::string(argv[1]).compare("-h") == 0))) {
        std::cout << "FROG command line options:" << std::endl;
        std::cout << " --help or -h         -> display help" << std::endl;
        std::cout << " --hosts <loaction>   -> specify hosts folder location" << std::endl;
        std::cout << " --spcs <location>    -> specify spcs folder location" << std::endl;

        return 2;
    }

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]).compare("--hosts") == 0) {
            hostsLocation = std::string(argv[++i]);
            hostsFromParam = 1;
        } else if (std::string(argv[i]).compare("--spcs") == 0) {
            spcsLocation = std::string(argv[++i]);
            spcsFromParam = 1;
        }
    }

    if (hostsFromParam) {
        std::cout << "Hosts location: " << hostsLocation << std::endl;
    } else {
        std::cout << "No hosts location provided, using default: " << DEFAULT_HOSTS_LOCATION << std::endl;
        hostsLocation = DEFAULT_HOSTS_LOCATION;
    }

    if (spcsFromParam) {
        std::cout << "SPCs location: " << spcsLocation << std::endl;
    } else {
        std::cout << "No SPCs location provided, using default: " << DEFAULT_SPCS_LOCATION << std::endl;
        spcsLocation = DEFAULT_SPCS_LOCATION;
    }    

    std::cout << std::endl << "Loading hosts..." << std::endl;
    hosts = loadHosts(hostsLocation);
    
    std::cout << std::endl << "Loading SPCs..." << std::endl;
    spcs = loadSpcs(spcsLocation);

    std::cout << std::endl << std::endl;
    
    // Main command line
    std::string commandBuffer;

    for ( ; ; ) {
        std::cout << PROMPT;
        std::getline(std::cin, commandBuffer);

        if (commandBuffer.compare("help") == 0) {
            printHelp();

        } else if (commandBuffer.substr(0, 7).compare("addhost") == 0) {
            try {
                writeHost(hostsLocation, commandBuffer.substr(8, (commandBuffer.length() - 8)));
            } catch (const std::out_of_range& oor) {
                std::cout << "No arguments" << std::endl;
            }

        } else if (commandBuffer.substr(0, 7).compare("delhost") == 0) {
            try {
                deleteHost(hostsLocation, commandBuffer.substr(8, (commandBuffer.length() - 8)));
            } catch (const std::out_of_range& oor) {
                std::cout << "No arguments" << std::endl;
            }
            

        } else if (commandBuffer.substr(0, 4).compare("exit") == 0) {
            exit(0);

        } else if (commandBuffer.substr(0, 8).compare("listhost") == 0) {
            listHosts(hosts);
            
        } else if (commandBuffer.substr(0, 10).compare("reloadhost") == 0) {
            hosts = loadHosts(hostsLocation);

        } else if (commandBuffer.substr(0, 3).compare("run") == 0) {
            monitorStart(hosts);

        } else if (commandBuffer.substr(0, 6).compare("gettop") == 0) {
            try {
                getTop(commandBuffer.substr(7, (commandBuffer.length() - 7)), hosts);
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }

        } else if (commandBuffer.substr(0, 5).compare("getdf") == 0) {
            try {
                getDf(commandBuffer.substr(6, (commandBuffer.length() - 6)), hosts);
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }

        } else if (commandBuffer.substr(0, 6).compare("getblk") == 0) {
            try {
                getBlk(commandBuffer.substr(7, (commandBuffer.length() - 7)), hosts);
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }

        } else if (commandBuffer.substr(0, 5).compare("getif") == 0) {
            try {
                getIf(commandBuffer.substr(6, (commandBuffer.length() - 6)), hosts);
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }
            
        } else if (commandBuffer.substr(0, 9).compare("spcstatus") == 0) {
            try {
                spcStatus(commandBuffer.substr(10, (commandBuffer.length() - 10)));
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }

        } else if (commandBuffer.substr(0, 7).compare("spchelp") == 0) {
            spcHelp();

        } else if (commandBuffer.substr(0, 6).compare("spcadd") == 0) {
            try {
                spcAdd(commandBuffer.substr(7, (commandBuffer.length() - 7)), spcsLocation);
            } catch (const std::out_of_range& oor) {
                std::cout << "No arguments" << std::endl;
            }

        } else if (commandBuffer.substr(0, 7).compare("spclist") == 0) {
            spcList(spcs);

        } else if (commandBuffer.substr(0, 6).compare("spcdel") == 0) {
            try {
                spcDel(spcsLocation, commandBuffer.substr(7, (commandBuffer.length() - 7)));
            } catch (const std::out_of_range& oor) {
                std::cout << "No argument" << std::endl;
            }

        } else if (commandBuffer.substr(0, 9).compare("spcreload") == 0) {
            spcs = loadSpcs(spcsLocation);

        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }


    return 0;
}

void spcDel(std::string location, std::string alias) {
    std::string filename;

    if (location.back() == '\\') {
        filename = location + alias + std::string(SPCS_EXTENSION);
    } else {
        filename = location + std::string("\\") + alias + std::string(SPCS_EXTENSION);
    }

    std::fstream f(filename.c_str());
    if (!f.good()) {
        std::cout << "SPC " << alias << " doesn't exist" << std::endl;
        return;
    }
    f.close();

    if (!DeleteFile(filename.c_str())) {
        std::cout << "Error while deleting SPC " << alias << "(Error: " << GetLastError() << ")" << std::endl;
    }
}

void spcList(std::vector<spcProps> spcs) {
    for (spcProps props : spcs) {
        std::cout << "Loaded SPC: " << props.alias << " [" << props.comPort << ":" << props.baud << ":" << props.fParity << ":" << props.parity << ":" << props.stopBits << ":" << props.byteSize << "]" << std::endl;
    }
}

void spcHelp(void) {
    std::cout << std::endl;
    std::cout << "Help for adding SPC:" << std::endl;
    std::cout << "spcadd <COM port> <baud rate> <parity checking> <parity> <stop bits> <byte size>" << std::endl;
    std::cout << " <COM port>          = COM port where your device is connected to (for example COM1, COM2 etc.)" << std::endl;
    std::cout << " <baud rate>         = baud rate your device uses (common values: 4800, 9600 etc.)" << std::endl;
    std::cout << " <parity checking>   = parity checking and throwing errors (take a look to DCB structure description). Possible values: true or false" << std::endl;
    std::cout << " <parity>            = parity type checking. Possible values: " << std::endl;
    std::cout << "   0 - no checking" << std::endl;
    std::cout << "   1 - odd parity" << std::endl;
    std::cout << "   2 - even parity" << std::endl;
    std::cout << "   3 - mark parity" << std::endl;
    std::cout << "   4 - space parity" << std::endl;
    std::cout << " <stop bits>         = stop bits at the end of packet. Possible values:" << std::endl;
    std::cout << "   1 - one stop bit" << std::endl;
    std::cout << "   15 - one and a half stop bits (look at DCB structure)" << std::endl;
    std::cout << "   2 - two stop bits" << std::endl;
    std::cout << " <byte size>         = length of data section of a packet (common values: from 5 to 8)" << std::endl << std::endl;
}

inline int checkNumber(std::string in) {
    for (int i = 0; i < in.length(); i++) {
        if (!isdigit(in[i])) {
            return 1;
        }
    }

    return 0;
}

void spcAdd(std::string commandLine, std::string location) {
    std::stringstream ss(commandLine);
    std::string alias, comPort, baud, fParityOld, fParityNew, parity, stopBits, byteSize;

    std::getline(ss, alias, ' ');

    std::string filename;
    if (location.back() == '\\') {
        filename = location + alias + std::string(SPCS_EXTENSION);
    } else {
        filename = location + std::string("\\") + alias + std::string(SPCS_EXTENSION);
    }

    std::getline(ss, comPort, ' ');
    std::getline(ss, baud, ' ');
    std::getline(ss, fParityOld, ' ');
    std::getline(ss, parity, ' ');
    std::getline(ss, stopBits, ' ');
    std::getline(ss, byteSize, ' ');

    std::cout << comPort << " " << baud << " " << fParityOld << " " << parity << " " << stopBits << " " << byteSize << std::endl;

    if (comPort.compare("") == 0) {
        std::cout << "Invalid COM port value" << std::endl;
        return;
    }

    //

    if ((baud.compare("") == 0) || (checkNumber(baud) != 0)) {
        std::cout << "Invalid baud rate value" << std::endl;
        return;
    }

    //

    if (fParityOld.compare("") == 0) {
        std::cout << "Invalid parity cheking value" << std::endl;
        return;
    }

    for (int i = 0; i < fParityOld.length(); i++) {
        fParityNew[i] = tolower(fParityOld[i]);
    }

    if ((fParityNew.compare("true") == 0) || (fParityNew.compare("false") == 0)) {
        std::cout << "Invalid parity cheking value (use true or false)" << std::endl;
        return;
    }

    //

    if ((parity.compare("") == 0) || (checkNumber(parity) != 0)) {
        std::cout << "Invalid parity value" << std::endl;
        return;
    }

    //

    if ((stopBits.compare("") == 0) || (checkNumber(stopBits) != 0)) {
        std::cout << "Invalid stop bits value 1" << std::endl;
        return;
    }

    //

    if ((byteSize.compare("") == 0) || (checkNumber(byteSize) != 0)) {
        std::cout << "Invalid byte size value" << std::endl;
        return;
    }

    //

    int numParity = atoi(parity.c_str());
    if ((numParity < 0) || (numParity > 4)) {
        std::cout << "Invalid parity value" << std::endl;
        return;
    }

    int numStopBits = atoi(stopBits.c_str());
    std::cout << numStopBits << std::endl;
    if ((numStopBits <= 0) && (numStopBits != 1) && (numStopBits != 2) && (numStopBits != 15)) {
        std::cout << "Invalid stop bits value 2" << std::endl;
        return;
    }


    // Check if file already exists
    std::ifstream check(filename.c_str());
    if (check.good()) {
        std::cout << "File already exists" << std::endl;
        return;
    }
    
    // Write to file
    std::fstream f;
    f.open(filename, std::ios::out);

    if (!f.good()) {
        std::cout << "Error while writing host file" << std::endl;
        return;
    }
    
    std::string json = "{\"alias\":\"" + alias + "\",\"comPort\":\"" + comPort + "\",\"baud\":\"" + baud + "\",\"fParity\":\"";

    if (fParityNew.compare("true") == 0) {
        json += std::string("1");
    } else {
        json += std::string("0");
    }

    json += "\",\"parity\":\"" + parity + "\",\"stopBits\":\"" + stopBits + "\",\"byteSize\":\"" + byteSize + "\"}";

    f << json;
    
    f.close();
}

void spcStatus(std::string comPort) {
    RS r(comPort, SPC_CONF_BAUD, SPC_CONF_FPARITY, SPC_CONF_PARITY, SPC_CONF_STOPBITS, SPC_CONF_BYTESIZE);
    Spc spc(r);

    if (spc.connect() != 0) {
        r.RsClose();
        return;
    }

    if (spc.getStatus() != 0) {
        r.RsClose();
        return;
    }

    
}

void getIf(std::string in, std::vector<Host> hosts) {
    SOCKET sock;
    int err = 0;

    for (Host h : hosts) {
        if (h.alias == in) {
            sock = hostConnect(h);

            Static s(sock);
            s.getIf();

            close(sock);
        }
    }
}

void getBlk(std::string in, std::vector<Host> hosts) {
    SOCKET sock;
    int err = 0;

    for (Host h : hosts) {
        if (h.alias == in) {
            sock = hostConnect(h);

            Static s(sock);
            s.getBlk();

            close(sock);
        }
    }
}

void getDf(std::string in, std::vector<Host> hosts) {
    SOCKET sock;
    int err = 0;

    for (Host h : hosts) {
        if (h.alias == in) {
            sock = hostConnect(h);

            Static s(sock);
            s.getDf();

            close(sock);
        }
    }
}

void getTop(std::string in, std::vector<Host> hosts) {
    SOCKET sock;
    int err = 0;

    for (Host h : hosts) {
        if (h.alias == in) {
            sock = hostConnect(h);

            Static s(sock);
            s.getTop();

            close(sock);
        }
    }
}



void gotoxy(int x, int y) {
    COORD p;
    p.X = x;
    p.Y = y;

    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hconsole, p);
}

void thr(hostMonitor hmon, int pos) {
    statusData data;
    int err = 0;

    gotoxy(0, pos + 1);
    data = hmon.readData(pos, &err);

    if (err == 0) {
        hmon.printLine(data, pos);
    }
}

SOCKET hostConnect(Host h) {
    int err = 0;

    std::cout << "Connecting to " << h.alias << "[" << h.ip << ":" << h.port << "]..." << std::endl;

    SOCKET sock = newSocket(h.ip, h.port, AS_CLIENT, &err);

    switch (err) {
        case 0:
            std::cout << "Connected" << std::endl << std::endl;
            break;

        case WSA_STARTUP_ERROR:
            std::cout << "Wsa startup error" << std::endl;
            return (SOCKET) 0;
            break;

        case SOCKET_CREATE_ERROR:
            std::cout << "Socket create error" << std::endl;
            return (SOCKET) 0;
            break;

        case CLIENT_CONNECT_ERRROR:
            std::cout << "Cannot connect to host" << std::endl;
            return (SOCKET) 0;
            break;

        case BIND_ERROR:
            std::cout << "Bind error" << std::endl;
            return (SOCKET) 0;
            break;

        case LISTEN_ERROR:
            std::cout << "Listen error" << std::endl;
            return (SOCKET) 0;
            break;

        case ACCEPT_ERROR:
            std::cout << "Accept error" << std::endl;
            return (SOCKET) 0;
            break;

        default:
            std::cout << "Unknown error" << std::endl;
            return (SOCKET) 0;
    }

    return sock;
}

void monitorStart(std::vector<Host> hosts) {
    std::vector<SOCKET> sockets;
    SOCKET s;

    for (Host h : hosts) {
        s = hostConnect(h);
        if (s != 0) {
            sockets.push_back(s);
        } else {
            continue;
        }
    }

    std::cout << "All hosts connected, make sure you are in fulscreen mode and press any key" << std::endl;
    getch();

    hostMonitor hmon(sockets, hosts);
    hmon.initDisplay();

    /*std::vector<std::thread> threads;

    for (int i = 0; i < hosts.size(); i++) {
        threads.push_back(std::thread(thr, hmon, i));
        threads.at(i).join();
    }*/

    for ( ; ; ) {
        for (int i = 0; i < hosts.size(); i++) {
            thr(hmon, i);
        }
    }

}

void listHosts(std::vector<Host> hosts) {
    std::cout << "Loaded hosts:" << std::endl << std::endl;

    for (Host h : hosts) {
        std::cout << h.alias << " [" << h.ip << ":" << h.port << "]" << std::endl;
    }
}

void deleteHost(std::string location, std::string hostName) {
    std::string filename;

    if (location.back() == '\\') {
        filename = location + hostName + std::string(HOSTS_EXTENSION);
    } else {
        filename = location + std::string("\\") + hostName + std::string(HOSTS_EXTENSION);
    }

    std::cout << filename << std::endl;

    std::fstream f(filename.c_str());
    if (!f.good()) {
        std::cout << "Host " << hostName << " doesn't exist" << std::endl;
        return;
    }
    f.close();

    if (!DeleteFile(filename.c_str())) {
        std::cout << "Error while deleting host " << hostName << "(Error: " << GetLastError() << ")" << std::endl;
    }
}

void writeHost(std::string location, std::string commandLine) {
    hostProps hp;
    std::stringstream ss(commandLine);
    std::string portBuffer;

    std::getline(ss, hp.alias, ' ');
    std::getline(ss, hp.ip, ' ');
    std::getline(ss, portBuffer, ' ');

    if (hp.alias.compare("") == 0) {
        std::cout << "Invalid alias value" << std::endl;
        return;
    }

    if (hp.ip.compare("") == 0) {
        std::cout << "Invalid ip value" << std::endl;
        return;
    }

    if (portBuffer.compare("") == 0) {
        std::cout << "Invalid port value" << std::endl;
        return;
    }

    for (int i = 0; i < portBuffer.length(); i++) {
        if (!isdigit(portBuffer[i])) {
            std::cout << "Invalid port value" << std::endl;
            return;
        }
    }

    hp.port = atoi(portBuffer.c_str());
    
    std::string filename;
    if (location.back() == '\\') {
        filename = location + hp.alias + std::string(HOSTS_EXTENSION);
    } else {
        filename = location + std::string("\\") + hp.alias + std::string(HOSTS_EXTENSION);
    }
    
    // Check if file already exists
    std::ifstream check(filename.c_str());
    if (check.good()) {
        std::cout << "File already exists" << std::endl;
        return;
    }
    
    // Write to file
    std::fstream f;
    f.open(filename, std::ios::out);

    if (!f.good()) {
        std::cout << "Error while writing host file" << std::endl;
        return;
    }
    
    // Create json from data
    std::string json;
    std::ostringstream ostr;
    ostr << hp.port;

    json = "{\"alias\":\"" + hp.alias + "\",\"ip\":\"" + hp.ip + "\",\"port\":\"" + ostr.str() + "\"}";
    f << json;
    f.close();
}

void printHelp(void) {
    std::cout << "Frog v1.0 help:" << std::endl << std::endl;
    std::cout << "HOST MANAGEMENT:" << std::endl;
    std::cout << " addhost <alias> <ip addr> <port> = add host to hosts location" << std::endl;
    std::cout << " delhost <alias>                  = delete host from hosts location" << std::endl;
    std::cout << " listhost                         = list all loaded hosts" << std::endl;
    std::cout << " reloadhost                       = reload hosts" << std::endl;
    std::cout << " run                              = start monitor" << std::endl;
    std::cout << std::endl << "STATIC FUNCTIONS:" << std::endl;
    std::cout << " gettop <alias>                   = get actual top screen" << std::endl;
    std::cout << " getdf <alias>                    = get disk usage screen" << std::endl;
    std::cout << " getblk <alias>                   = get block devices list screen" << std::endl;
    std::cout << std::endl << "SPC FUNCTIONS:" << std::endl;
    std::cout << " spchelp                          = display help screen about adding a SPC" << std::endl;
    std::cout << " spcadd <see spchelp>             = add SCP" << std::endl;
    std::cout << " spcstatus <com port>             = get status from SPC device" << std::endl;
    std::cout << " spclist                          = list all loaded SPCs" << std::endl;
    std::cout << " spcreload                        = reload all SPCs" << std::endl;
    std::cout << " spcdel <alias>                   = delete SPC" << std::endl;
    std::cout << std::endl << "OTHER:" << std::endl;
    std::cout << " help                             = display this" << std::endl;
    std::cout << " exit                             = exit from application" << std::endl;


    std::cout << std::endl;
}

std::vector<spcProps> loadSpcs(std::string location) {
    std::vector<spcProps> spcs;
    DIR *dr;
    struct dirent *en;
    int err = 0;
    Spc spc;

    dr = opendir(location.c_str());

    if (dr == NULL) {
        std::cout << "Error while opening location" << std::endl;
        exit(1);
    }

    if (location.back() != '\\') {
        location.push_back('\\');
    }

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (std::string(en->d_name).compare(".") == 0 || std::string(en->d_name).compare("..") == 0) {
                continue;
            }
            
            spcProps props = spc.readSpc(location + std::string(en->d_name), &err);

            if (err != 0) {
                continue;
            } else {
                spcs.push_back(props);
            }
        }

        closedir(dr);
    }

    for (spcProps props : spcs) {
        std::cout << "Loaded SPC: " << props.alias << " [" << props.comPort << ":" << props.baud << ":" << props.fParity << ":" << props.parity << ":" << props.stopBits << ":" << props.byteSize << "]" << std::endl;
    }
    
    return spcs;
}

std::vector<Host> loadHosts(std::string location) {
    std::vector<Host> vect;
    DIR *dr;
    struct dirent *en;

    dr = opendir(location.c_str());

    if (dr == NULL) {
        std::cout << "Error while opening location" << std::endl;
        exit(1);
    }

    if (location.back() != '\\') {
        location.push_back('\\');
    }

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (std::string(en->d_name).compare(".") == 0 || std::string(en->d_name).compare("..") == 0) {
                continue;
            }

            vect.push_back(Host(location + std::string(en->d_name)));


        }
        closedir(dr);
    }

    for (Host h : vect) {
        std::cout << "Loaded host: " << h.alias << " [" << h.ip << ":" << h.port << "]" << std::endl;
    }

    return vect;
}