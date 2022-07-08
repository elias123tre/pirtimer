#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <string>
#include <pthread.h>

void senderror(const char* msg);
//template <typename T, size_t N>
//int sendPacket(T(&buffer)[N], const char* ip);
int sendPacket(uint8_t* buffer, const char* ip);