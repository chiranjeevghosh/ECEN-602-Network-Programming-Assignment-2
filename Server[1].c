// ************************************ Server Code *********************************************
// Author        :      Chiranjeev Ghosh (chiranjeev.ghosh@tamu.edu) 
// Organisation  :      Texas A&M University, CS for ECEN 602 Assignment 2
// Description   :      Establishes an IPv4 socket, takes port number specified from command line
//                      to bind, listen and accept multiple client connections simultaneously. 
//                      Once connection is established, message received is echoed back to the
//                      client.
// Last_Modified :      10/13/2017


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <utils.h>



int main(int argc, char *argv[])
{
  char temp_str;
  char str[MAX_STR_LEN];
  int sockfd, comm_fd, bind_fd, listen_fd;	
  int port_number, max_clients ;
  char *p, *p1;
  int i, g, client_count;
  long conv_arg_to_int = strtol(argv[1], &p, 10);
  long conv_arg_to_int_client_cnt = strtol(argv[2], &p1, 10);
  
  if (argc != 3){
    err_sys ("USAGE: ./server <Port_Number> <Max_Clients>");
    return 0;
  }
  
  port_number = conv_arg_to_int;
  max_clients = conv_arg_to_int_client_cnt;
  int ret_val;
  struct sockaddr_in servaddr;
  fd_set Master;                         
  fd_set Read_FDs;                       
  int    FD_Max;
  struct SBCP_Message_Format *SBCP_Message_from_Client;
  struct SBCP_Message_Format *SBCP_Message_to_Client;
  char usernames[100][16] = {0};
  char names_display[200] = {0};
  char exit_status[30] = {0};
  char Message_from_Client[550] = {0};
  int b, j;
  int max_num_active_clients = max_clients - 1;
  
  FD_ZERO(&Master); 
  FD_ZERO(&Read_FDs);
  
  client_count = 0;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
    err_sys ("ERR: Socket Error");

  bzero( &servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(port_number);
  bind_fd = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  if (bind_fd < 0)
    err_sys ("ERR: Bind Error");
  listen_fd = listen(sockfd, 10);
  if (listen_fd < 0)
    err_sys ("ERR: Listen Error");
  
  // add socket FD to Master set
  FD_SET(sockfd, &Master);
  // track the largest FD
  FD_Max = sockfd; 
  while(1)
  {
    Read_FDs = Master;      // temporary copy
    if (select(FD_Max+1, &Read_FDs, NULL, NULL, NULL) == -1) {
      err_sys("ERR: Select Error");
      exit(10);
    }
  
    printf ("FD_Max: %d \n", FD_Max);
    for(i = 0; i <= FD_Max; i++) {
      printf ("sockfd: %d; i: %d \n", sockfd, i);
      if (FD_ISSET(i, &Read_FDs)) { 
        printf ("Entered FD_ISSET \n");
        if (i == sockfd) {                // New connection detected
          if (client_count < max_clients) {                      // Check if max client limit is reached
            comm_fd = accept(sockfd, (struct sockaddr*) NULL, NULL);
            if(comm_fd == -1)
              err_sys ("ERR: Accept Error");
            else {
              FD_SET(comm_fd, &Master);       // Add new connection to master set
              if (comm_fd > FD_Max) {         // Tracking the max
                FD_Max = comm_fd;
              }
              client_count++;
              //SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
              //SBCP_Message_to_Client->Type = 7;                   // ACK Bonus Feature
              //if (client_count == 1)
              //  strcpy (names_display,"Welcome to the chat room! No one is currently online. ");
              //else
              //  strcpy (names_display,"Welcome to the chat room! User(s) currently online: ");
	      //for(b=4; b<=FD_Max; b++){
	      //  if(b != i && b != sockfd && client_count != 1){
	      //  strcat (names_display, usernames[b]);
	      //  strcat (names_display,", ");
	      //  }
	      //}
	      //strcat (names_display,".");
	      //strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, names_display); // Welcome message plus online user list
              //printf ("SERVER: ACK message sent to client: %s with type: %d\n", SBCP_Message_to_Client->SBCP_Attributes.Payload, SBCP_Message_to_Client->Type);
              //write(i, SBCP_Message_to_Client, sizeof(struct SBCP_Message_Format));
              printf ("\nSERVER: Allowing client to join the chat room \n");
            }
          }
        }  
        else {
          SBCP_Message_from_Client = malloc(sizeof(struct SBCP_Message_Format));
          printf ("SERVER: Memory allocated for client message \n");
          ret_val = read(i, SBCP_Message_from_Client, sizeof(struct SBCP_Message_Format));   // Reading SBCP message from connected client
          printf ("SERVER: Reading message from client \n");
          if (ret_val > 0) {
            if (SBCP_Message_from_Client->Type == 2 && SBCP_Message_from_Client->SBCP_Attributes.Type == 2) {                 // JOIN SBCP Message
              //free(SBCP_Message_from_Client);
              //SBCP_Message_from_Client = malloc(sizeof(struct SBCP_Message_Format));
              //read(i, SBCP_Message_from_Client, sizeof(struct SBCP_Message_Format));
              //printf ("SERVER: Message received from client: %s \n", SBCP_Message_from_Client->SBCP_Attributes.Payload);
              if (client_count <= max_num_active_clients) {                      // Check if max  active client limit is reached
	        int flag = 1;
                for(j=4; j<=FD_Max; j++){          
	          if(strcmp(SBCP_Message_from_Client->SBCP_Attributes.Payload,usernames[j])==0){                                // username already in use
	            flag=0;            
                    SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
                    SBCP_Message_to_Client->Type = 5;                   // NACK Bonus Feature
                    SBCP_Message_to_Client->SBCP_Attributes.Type = 1;   // NACK Reason
                    strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, "Username already in use. Please choose another user name and try again."); 
                    write(i, SBCP_Message_to_Client, sizeof(struct SBCP_Message_Format));
	            client_count--;
	            close(i);             // Close the connection 
	            FD_CLR(i, &Master);   // Clear Master set
	            break;
	          }
	        }
	        if (flag==1) {             // Acceptable username
	          sprintf(usernames[i],"%s",SBCP_Message_from_Client->SBCP_Attributes.Payload);
                  SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
                  SBCP_Message_to_Client->Type = 7;                   // ACK Bonus Feature
                  if (client_count == 1)
                    strcpy (names_display,"Welcome to the chat room! No one is currently online. ");
                  else
                    strcpy (names_display,"Welcome to the chat room! User(s) currently online: ");
	          for(b=4; b<=FD_Max; b++){
	            if(b != i && b != sockfd && client_count != 1){
	              strcat (names_display, usernames[b]);
	              strcat (names_display," ");
	            }
	          }
	          strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, names_display); // Welcome message plus online user list
                  printf ("SERVER: ACK message sent to client: %s with type: %d\n", SBCP_Message_to_Client->SBCP_Attributes.Payload, SBCP_Message_to_Client->Type);
                  write(i, SBCP_Message_to_Client, sizeof(struct SBCP_Message_Format));
	        }
	      }
	      else{           // Max active client count that chat room is programmed to support is reached
                SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
                SBCP_Message_to_Client->Type = 5;                   // NACK Bonus Feature
                SBCP_Message_to_Client->SBCP_Attributes.Type = 1;   // NACK Reason
                strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, "Sorry chat room at max capacity :( Please try again later. "); 
                write(i, SBCP_Message_to_Client, sizeof(struct SBCP_Message_Format));
                //printf ("SERVER: NACKING JOIN request of %s as chat room has reached max capacity \n", usernames[i]);
                client_count--;
                close(i);                    // Close the connection
                FD_CLR(i, &Master);          // Clear Master set
	      }
            }
            if (SBCP_Message_from_Client->Type == 4 && SBCP_Message_from_Client->SBCP_Attributes.Type == 4) {                 // SEND SBCP Message
	      sprintf(Message_from_Client,"%s: %s", usernames[i], SBCP_Message_from_Client->SBCP_Attributes.Payload);
	      for (b=0; b<=FD_Max; b++) {
	        if (FD_ISSET(b, &Master)) {
		  if (b != i && b != sockfd) {
                    SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
                    SBCP_Message_to_Client->Type = 3;                   // FWD SBCP Message
                    SBCP_Message_to_Client->SBCP_Attributes.Type = 4;   // Message SBCP Attribute 
	      	    strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, Message_from_Client);
                    write(b, SBCP_Message_to_Client, sizeof(struct SBCP_Message_Format));
		  }    
		}
	      }
	    }
            free(SBCP_Message_from_Client);
          }
          else {  // read return value <= 0
            if (ret_val == 0) {                 // connection closed
              sprintf(exit_status,"%s has left the session",usernames[i]);
	     //sprintf(SBCP_Message_to_Client->SBCP_Attributes.Payload, "client %s has exited", usernames[i]);
              printf ("SERVER: Client closed connection \n");
              usernames[i][0]='\0';             // Make username reusable
              // Update other clients about exit
              for(g = 0; g <= FD_Max; g++){
                if (FD_ISSET(g, &Master)){
                  if (g != i && g != sockfd){ // except the sockfd and the exited client
                    SBCP_Message_to_Client = malloc(sizeof(struct SBCP_Message_Format));
                    strcpy(SBCP_Message_to_Client->SBCP_Attributes.Payload, exit_status);
                    if (write(g, SBCP_Message_to_Client, sizeof (struct SBCP_Message_Format)) == -1){
                      err_sys ("ERR: Write (Multicast) Error");
                    }
                  }
                }
              }
            } 
	    else {
              err_sys ("ERR: Read Error");
            }
            client_count--;
            close(i);                    // Close the connection
            FD_CLR(i, &Master);          // Clear Master set
          }
        }
      }
    }
  }  
}


