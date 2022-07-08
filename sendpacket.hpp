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

void error(const char *);

template <typename T, size_t N>
int sendPacket(T (&buffer)[N], const char *ip)
{
  int sock, n;
  unsigned int length;
  struct sockaddr_in server;
  struct hostent *hp;
  hp = gethostbyname(ip);
  if (hp == 0)
    error("Unknown host");

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    error("socket");

  server.sin_family = AF_INET;
  bcopy((char *)hp->h_addr,
        (char *)&server.sin_addr,
        hp->h_length);
  server.sin_port = htons(atoi("56700"));
  length = sizeof(struct sockaddr_in);

  n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *)&server, length);
  if (n < 0)
    error("Sendto");
  // printf("Sending Packet...\n");

  close(sock);
  return 0;
}

void error(const char *msg)
{
  perror(msg);
  exit(1);
}