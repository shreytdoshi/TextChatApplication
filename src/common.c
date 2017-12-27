#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DNS_SERVER "8.8.8.8"
#define DNS_SERVER_PORT 53
#define TCP 1
#define UDP 2

char * getIPAddress(){
	int sockfd;
	struct sockaddr_in myAddress;
	struct sockaddr_in serverAddress;
	//Create UDP Socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("Failed to create socket");
	}

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, DNS_SERVER, &serverAddress.sin_addr.s_addr);
	//serverAddress.sin_addr.s_addr = htonl(DNS_SERVER);
	serverAddress.sin_port = htons(DNS_SERVER_PORT);

	if(connect(sockfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress))<0){
		perror("Failed to connect to DNS_SERVER");
	}

	socklen_t l = INET_ADDRSTRLEN;

	getsockname(sockfd, (struct sockaddr*)&myAddress, &l);
	char *ip;
	ip = inet_ntoa(myAddress.sin_addr);
	printf("IP....%s /n",ip);
	//printf("IP:::::::%s", ipAddress);
	close(sockfd);
	return ip;
}

char * getHostname(char * ipAddress){
	char * hostname;
	struct hostent *host;
	struct in_addr ipaddr;
	inet_pton(AF_INET, ipAddress, &ipaddr);
	host = gethostbyaddr(&ipaddr, sizeof (ipaddr), AF_INET);
	//printf("Hostname:::::::::: %s\n",he->h_name);
	hostname = host->h_name;
	return hostname;
}
