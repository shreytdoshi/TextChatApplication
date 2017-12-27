/**
 * @shreytej_assignment1
 * @author  Shrey Tejash Doshi <shreytej@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/global.h"
#include "../include/logger.h"
#include <string.h>

#include "../include/common.h"
#include "../include/linkedlist.h"
#define CLIENT "c"
#define SERVER "s"
#define CMD_SIZE 500

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define BUFFER_SIZE 256
#define MSG_SIZE 256

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));
	
	/*Start Here*/
	struct Node *serverhead = (struct Node*) malloc(sizeof(struct Node));
	struct Node *clienthead = (struct Node*) malloc(sizeof(struct Node));

	//Code for Server
	if(!strcmp(argv[1], SERVER)){
		char *serverAddress;//Store server address
		serverAddress = getIPAddress();
		//printf("IP:%s",serverAddress);

		int port, server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
		struct sockaddr_in server_addr, client_addr;
		fd_set master_list, watch_list;

		/* Socket */
		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(server_socket < 0)
			perror("Cannot create socket");

		/* Fill up sockaddr_in struct */
		port = atoi(argv[2]);
		bzero(&server_addr, sizeof(server_addr));

		//printf("%s\n",INADDR_ANY);
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(serverAddress);
		server_addr.sin_port = htons(port);

		/* Bind */
		if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
			perror("Bind failed");

		/* Listen */
		if(listen(server_socket, BACKLOG) < 0)
			perror("Unable to listen on port");

		/* ---------------------------------------------------------------------------- */

		/* Zero select FD sets */
		FD_ZERO(&master_list);
		FD_ZERO(&watch_list);

		/* Register the listening socket */
		FD_SET(server_socket, &master_list);
		/* Register STDIN */
		FD_SET(STDIN, &master_list);

		head_socket = server_socket;

		while(TRUE){
			memcpy(&watch_list, &master_list, sizeof(master_list));

			//printf("\n[PA1-Server@CSE489/589]$ ");
			//fflush(stdout);

			/* select() system call. This will BLOCK */
			selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
			if(selret < 0)
				perror("select failed.");

			/* Check if we have sockets/STDIN to process */
			if(selret > 0){
				/* Loop through socket descriptors to check which ones are ready */
				for(sock_index=0; sock_index<=head_socket; sock_index+=1){

					if(FD_ISSET(sock_index, &watch_list)){
						/* Check if new command on STDIN */
						if (sock_index == STDIN){
							char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

							memset(cmd, '\0', CMD_SIZE);
							if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
								exit(-1);

							//printf("\nI got: %s\n", cmd);

							//Process PA1 commands here ...

							int l = strlen(cmd);
							if(cmd[l-1]=='\n'){
								cmd[l-1] = '\0';
							}
							if(!strncmp(cmd, "AUTHOR", 6)){
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "shreytej");
								cse4589_print_and_log("[%s:END]\n", cmd);
							}
							if(!strncmp(cmd, "PORT", 4)){
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("PORT: %d", port);
								cse4589_print_and_log("[%s:END]\n", cmd);
							}
							if(!strncmp(cmd, "IP", 2)){
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("IP: %s", serverAddress);
								cse4589_print_and_log("[%s:END]\n", cmd);
							}
							if(!strncmp(cmd, "LIST", 4)){
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								printList(serverhead);
								cse4589_print_and_log("[%s:END]\n", cmd);
							}

							free(cmd);
						}

						/* Check if new client is requesting connection */
						else if(sock_index == server_socket){
							caddr_len = sizeof(client_addr);
							fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
							if(fdaccept < 0)
								perror("Accept failed.");

							//TODO: Add to list
							char clientIP[INET_ADDRSTRLEN];
							//char clientPort[20];
							int cport;
							char * clientHost;

							struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;
							struct in_addr ipAddr = pV4Addr->sin_addr;

							inet_ntop(AF_INET, &ipAddr, clientIP, INET_ADDRSTRLEN);

							clientHost = getHostname(clientIP);

							cport = (int) ntohs(client_addr.sin_port);

							if(serverhead->port == 0){
								serverhead->port = cport;
								serverhead->ip = clientIP;
								serverhead->hostname = clientHost;
								serverhead->fd = fdaccept;
							}
							else {
								push(&serverhead, cport, clientIP, clientHost, fdaccept);
							}

							MergeSort(&serverhead);
							printList(serverhead);

							struct Node *temp = serverhead;

							char infomsg[256];
							memset(infomsg, '\0', 256);
							strcat(infomsg, "LIST");
							while(temp!=NULL){
								char port[10];
								sprintf(port, "%d", temp->port);
								strcat(infomsg, " ");
								strcat(infomsg, temp->ip);
								strcat(infomsg, " ");
								strcat(infomsg, port);
								strcat(infomsg, " ");
								strcat(infomsg, temp->hostname);
								strcat(infomsg, " ");
								temp = temp->next;
							}
							strcat(infomsg, "LISTEND");

							if(send(fdaccept, infomsg, strlen(infomsg), 0) == strlen(infomsg));
							//printf("Sent LIST!\n");

							//printf("\nRemote Host connected!\n");

							/* Add to watched socket list */
							FD_SET(fdaccept, &master_list);
							if(fdaccept > head_socket) head_socket = fdaccept;
						}

						/* Read from existing clients */
						else {
							/* Initialize buffer to receive response */
							char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
							memset(buffer, '\0', BUFFER_SIZE);

							if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
								close(sock_index);
								printf("Remote Host terminated connection!\n");

								/* Remove from watched list */
								FD_CLR(sock_index, &master_list);
							}
							else {
								//Process incoming data from existing clients here ...
								char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
								memset(msg, '\0', MSG_SIZE);
								char * sendto;
								char * sentFrom;
								int sendtoport = 0;
								int sendtoSock;

								if(!strncmp(buffer, "SEND", 4)){
									int count = 0;
									char *d_array[50];
									char *arg = strtok(buffer, " ");
									while(arg){
										d_array[count] = arg;
										count++;
										arg = strtok(NULL, " ");
									}
									sendto = d_array[1];
									//int sendtoport;
									struct Node *temp = serverhead;
									while(temp!=NULL){
										if(!strcmp(sendto, temp->ip)){
											sendtoport = temp->port;
											sendtoSock = temp->fd;
											break;
										}
										temp = temp->next;
									}
									if(!sendtoport){
										//printf("Client not logged in");
									}

									int i = 2;

									while(strncmp(d_array[i],"SENDEND", 7)){
										strcat(msg, d_array[i]);
										strcat(msg, " ");
										i++;
									}
									sentFrom = d_array[i+1];
									strcat(msg, sentFrom);

								}
								//TODO: check if client to send is loggedin in the list, if yes, use that and its socket connection with the server to send the message
								//printf("\nClient sent me: %s %s\n", buffer, msg);
								//printf("ECHOing it back to the remote host ... ");
								if(send(sendtoSock, msg, strlen(msg), 0) == strlen(msg))
									printf("Done!\n");
								fflush(stdout);
							}

							free(buffer);

						}

					}
				}
			}
		}

		return 0;
	}


	//Code for Client
	if(!strcmp(argv[1], CLIENT)){
		int fdsocket;

		while(TRUE){
			char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

			memset(cmd, '\0', CMD_SIZE);
			if(fgets(cmd, CMD_SIZE-1, stdin) == NULL){
				exit(-1);
				//Mind the newline character that will be written to cmd
			}
			else {

				int l = strlen(cmd);
				if(cmd[l-1]=='\n'){
					cmd[l-1] = '\0';
				}
				if(!strncmp(cmd, "AUTHOR", 6)){
					cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
					cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "shreytej");
					cse4589_print_and_log("[%s:END]\n", cmd);
				}
				if(!strncmp(cmd, "PORT", 4)){
					cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
					cse4589_print_and_log("PORT: %d", atoi(argv[2]));
					cse4589_print_and_log("[%s:END]\n", cmd);
				}
				if(!strncmp(cmd, "IP", 2)){
					cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
					cse4589_print_and_log("IP: %s", getIPAddress());
					cse4589_print_and_log("[%s:END]\n", cmd);
				}
				if(strstr(cmd, "LOGIN")){
					int count = 0;
					//int fdsocket;
					//struct sockaddr_in remote_server_addr;
					char *d_array[10];
					char *arg = strtok(cmd, " ");
					while(arg){
						d_array[count] = arg;
						count++;
						arg = strtok(NULL, " ");
					}

					char * serverIPAddress;
					serverIPAddress = d_array[1];
					int serverPort = atoi(d_array[2]);

					//TODO: send connection request to server
					fdsocket = connect_to_host(serverIPAddress, serverPort, atoi(argv[2]));
					//TODO: Add client IP and Port to login list
					//TODO: getHostname()

					char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
					memset(buffer, '\0', BUFFER_SIZE);

					if(recv(fdsocket, buffer, BUFFER_SIZE, 0) >= 0){
						//printf("Server responded (LIST): %s", buffer);

						if(strstr(buffer, "LIST")){
							int count = 0;
							int fdsocket;
							//struct sockaddr_in remote_server_addr;
							char *d_array[50];
							char *arg = strtok(buffer, " ");
							while(arg){
								d_array[count] = arg;
								count++;
								arg = strtok(NULL, " ");
							}

							char * ip;
							int port;
							char * hostname;

							int i = 1;
							int j = 2;
							int k = 3;
							//strncmp(darra, "IP", 2)
							while(strncmp(d_array[i],"LISTEND", 7)){
								ip = d_array[i];
								port = atoi(d_array[j]);
								hostname = d_array[k];
								i+=3;
								j+=3;
								k+=3;
								if(clienthead->port == 0){
									clienthead->port = port;
									clienthead->ip = ip;
									clienthead->hostname = hostname;
									clienthead->fd = 0;
								}
								else {
									push(&clienthead, port, ip, hostname,0);
								}

							}

						}

						fflush(stdout);
					}
					MergeSort(&clienthead);
				}

				if(!strncmp(cmd, "LIST", 4)){
					cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
					printList(clienthead);
					cse4589_print_and_log("[%s:END]\n", cmd);
				}

				if(strstr(cmd, "SEND")){
					strcat(cmd, " SENDEND");
					strcat(cmd, " ");
					strcat(cmd, getIPAddress());
					//printf("I got: %s(size:%d chars)", cmd, strlen(cmd));

					//printf("\nSENDing it to the remote server ... ");

					if(send(fdsocket, cmd, strlen(cmd), 0) == strlen(cmd))
						//printf("Done!\n");
					fflush(stdout);

					/* Initialize buffer to receieve response */
					char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
					memset(buffer, '\0', BUFFER_SIZE);

					if(recv(fdsocket, buffer, BUFFER_SIZE, 0) >= 0){
						//printf("Server responded: %s", buffer);
						fflush(stdout);
					}
				}


			}
		}
	}
	return 0;
}


