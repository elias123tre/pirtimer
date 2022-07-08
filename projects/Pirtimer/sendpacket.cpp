#include "sendpacket.h"

//template <typename T, size_t N>
//int sendPacket(T(&buffer)[N], const char* ip)
int sendPacket(uint8_t* buffer, const char* ip)
{
	int sock, n;
	unsigned int length;
	struct sockaddr_in server;
	struct hostent* hp;
	hp = gethostbyname(ip);
	if (hp == 0)
		senderror("Unknown host");

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
		senderror("socket");

	server.sin_family = AF_INET;
	bcopy((char*)hp->h_addr,
		(char*)&server.sin_addr,
		hp->h_length);
	server.sin_port = htons(56700);
	length = sizeof(struct sockaddr_in);

	n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr*)&server, length);
	if (n < 0)
		senderror("Sendto");
	// printf("Sending Packet...\n");

	close(sock);
	return 0;
}

void senderror(const char* msg)
{
	perror(msg);
	exit(1);
}