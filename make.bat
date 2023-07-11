@echo off

del sysmon.exe
g++ -std=c++11 main.cpp Host.cpp Json.cpp telnet.cpp hostMonitor.cpp static.cpp -lws2_32 -o sysmon.exe
sysmon.exe
