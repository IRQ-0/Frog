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

#define DEFAULT_HOSTS_LOCATION "y:\\cpp\\sysmon\\hosts"

#define PROMPT "frg> "

#define FILENAME_EXTENSION ".host"

std::vector<Host> loadHosts(std::string);
void printHelp(void);
void writeHost(std::string, std::string);
void deleteHost(std::string, std::string);
void listHosts(std::vector<Host>);
void monitorStart(std::vector<Host>);
SOCKET hostConnect(Host);
void thr(hostMonitor, int);
void gotoxy(int, int);


int main(int argc, char** argv) {
    std::string hostsLocation;
    std::vector<Host> hosts;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    //SetConsoleTextAttribute(console, DEFAULT_COLOR);

    //HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursor;
    cursor.dwSize = 100;
    cursor.bVisible = TRUE;

    SetConsoleCursorInfo(console, &cursor);

    std::cout << "FROG v1.0" << std::endl << std::endl;
    if (argc < 2) {
        std::cout << "No hosts location provided, using default: " << DEFAULT_HOSTS_LOCATION << std::endl;
        hostsLocation = DEFAULT_HOSTS_LOCATION;

    } else {
        std::cout << "Hosts location: " << std::string(argv[1]) << std::endl;
        hostsLocation = std::string(argv[1]);
    }

    std::cout << std::endl << "Loading hosts..." << std::endl;
    hosts = loadHosts(hostsLocation);
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
            

        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }


    return 0;
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
        filename = location + hostName + std::string(FILENAME_EXTENSION);
    } else {
        filename = location + std::string("\\") + hostName + std::string(FILENAME_EXTENSION);
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
        filename = location + hp.alias + std::string(FILENAME_EXTENSION);
    } else {
        filename = location + std::string("\\") + hp.alias + std::string(FILENAME_EXTENSION);
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
    std::cout << " help                             = display this" << std::endl;
    std::cout << " addhost <alias> <ip addr> <port> = add host to hosts location" << std::endl;
    std::cout << " delhost <alias>                  = delete host from hosts location" << std::endl;
    std::cout << " listhost                         = list all loaded hosts" << std::endl;
    std::cout << " reloadhost                       = reload hosts" << std::endl;
    std::cout << " run                              = start monitor" << std::endl;
    std::cout << " exit                             = exit from application" << std::endl;

    std::cout << std::endl;
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