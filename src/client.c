#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

int connect_to_host(char *server_ip, int server_port, int clientPort)
{
	int fdsocket, len;
	struct sockaddr_in remote_server_addr, client;

	fdsocket = socket(AF_INET, SOCK_STREAM, 0);
	if(fdsocket < 0)
		perror("Failed to create socket");

	client.sin_family = AF_INET;
	//inet_pton(AF_INET, getIPAddress(), &client.sin_addr);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_port = htons(clientPort);

	//Bind
	if(bind(fdsocket, (struct sockaddr *)&client, sizeof(client)) < 0 )
		perror("Bind failed");

	bzero(&remote_server_addr, sizeof(remote_server_addr));
	remote_server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
	remote_server_addr.sin_port = htons(server_port);

	if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
		perror("Connect failed");

	return fdsocket;
}
