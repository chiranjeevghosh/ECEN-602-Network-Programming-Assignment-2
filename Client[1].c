// ************************************ Client Code *********************************************
// Author        :      Chiranjeev Ghosh (chiranjeev.ghosh@tamu.edu)
// Organisation  :      Texas A&M University, CS for ECEN 602 Assignment 2
// Description   :      
// Last_Modified :      10/13/2017


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <utils.h>



int main(int argc,char *argv[])
{
  int sockfd, inet_p2n_ret, conn_ret, n;
  char sendline[MAX_STR_LEN];
  int ret = 0;
  int port_number ;
  char *p;
  long conv_arg_to_int = strtol(argv[3], &p, 10);
  struct SBCP_Message_Format *SBCP_Message_to_Server;
  struct SBCP_Message_Format *SBCP_Message_from_Server;
  //char buf_username_send[16];
  //char buf_message_send[512];
  int i, len_ip, bytes_read;
  fd_set Master;                         
  fd_set Read_FDs;
  
  

  if (argc != 4){
    err_sys ("USAGE: ./client <Username> <IP_Address> <Port_Number>");
    return 0;
  }

  port_number = conv_arg_to_int;
  
  
  struct sockaddr_in servaddr;
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if (sockfd < 0)
    err_sys ("ERR: Socket Error");

  bzero(&servaddr,sizeof servaddr);
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(port_number);
  inet_p2n_ret = inet_pton(AF_INET,argv[2],&(servaddr.sin_addr));
  if (inet_p2n_ret <= 0)
    err_sys ("ERR: inet_pton error");

  conn_ret = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
  if (conn_ret < 0)
    err_sys ("ERR: Connect Error");

  //bzero( sendline, MAX_STR_LEN);
  printf ("CLIENT: Username: %s \n", argv[1]);
  //fgets(sendline,MAX_STR_LEN,stdin); /*stdin = 0 , for standard input */
  SBCP_Message_to_Server = malloc(sizeof(struct SBCP_Message_Format));
  SBCP_Message_to_Server->Version = 3;                                 // SBCP Message Version indicator 
  SBCP_Message_to_Server->Type = 2;                                    // SBCP Message type indicating JOIN 
  SBCP_Message_to_Server->Length = 24;                                 // SBCP Message containing username length field 
  SBCP_Message_to_Server->SBCP_Attributes.Type = 2;                    // Attr Type indicating payload containing username
  SBCP_Message_to_Server->SBCP_Attributes.Length = 20;                 // Attr Length indicating length of payload containing username
  strcpy(SBCP_Message_to_Server->SBCP_Attributes.Payload, argv[1]);    // Copying username to payload
  printf ("CLIENT: Attempting to JOIN chat room \n");
  if (write(sockfd, SBCP_Message_to_Server, sizeof(struct SBCP_Message_Format)) == -1) err_sys ("ERR: Write Error");
  
  // printf ("CLIENT: JOIN attempt to server \n");
  // wait for read of ACK from server to join.
  // if client receives ACK, write structure with Type = 4 (SEND) with Payload as chat text taken from command line.
  // parallelly wait for texts from other clients.
  FD_SET(0, &Read_FDs);              // add user input from keyboard to the Read_FDs set
  FD_SET(sockfd, &Read_FDs);         // add sockfd to the Read_FDs set
  while (1) {
    if (select(sockfd+1, &Read_FDs, NULL, NULL, NULL) == -1) {
      err_sys("ERR: Select Error");
      exit(6);
    }
    //printf ("sockfd: %d\n", sockfd);
    for (i=0; i<=sockfd; i++) {
      
      if (FD_ISSET(i, &Read_FDs)){
	if (i == 0) {                                                           // Data to be read from keyboard of user
	   //printf("Me: ");
	   bzero(sendline, MAX_STR_LEN);
           fgets(sendline,MAX_STR_LEN,stdin);
	   len_ip = strlen(sendline) - 1;
	   if(sendline[len_ip] == '\n') sendline[len_ip] = '\0';                // Provision to detect unceremonious exit from client 
           SBCP_Message_to_Server = malloc(sizeof(struct SBCP_Message_Format));
           strcpy(SBCP_Message_to_Server->SBCP_Attributes.Payload, sendline);
           SBCP_Message_to_Server->Version = 3;                                 // SBCP Message Version indicator 
           SBCP_Message_to_Server->Type = 4;                                    // SBCP Message type indicating SEND
           SBCP_Message_to_Server->Length = 520;                                // SBCP Message containing username length field 
           SBCP_Message_to_Server->SBCP_Attributes.Type = 4;                    // Attr type indicating payload containing message
           SBCP_Message_to_Server->SBCP_Attributes.Length = 524;                // Attr Length indicating length of payload containing username
           if (write(sockfd, SBCP_Message_to_Server, sizeof(struct SBCP_Message_Format)) == -1) err_sys ("ERR: Write Error");
	}		
	if (i == sockfd) {                                                      // Server data to be read
          SBCP_Message_from_Server = malloc(sizeof(struct SBCP_Message_Format));
          //printf ("Debug pos 1");
          bytes_read = read(sockfd, SBCP_Message_from_Server, sizeof(struct SBCP_Message_Format));
          printf ("%s \n", SBCP_Message_from_Server->SBCP_Attributes.Payload);
          free(SBCP_Message_from_Server);
          if(SBCP_Message_from_Server->Type == 5 && SBCP_Message_from_Server->SBCP_Attributes.Type == 1)      // NACK bonus feature
            exit(7);              // Try again later
  }     
      }
      FD_SET (0, &Read_FDs);
      FD_SET (sockfd, &Read_FDs);
    }
  }
  close(sockfd);
  return 0;
}









