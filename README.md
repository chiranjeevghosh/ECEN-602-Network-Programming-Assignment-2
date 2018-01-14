# ECEN-602-Network-Programming-Assignment 2
A simple chat server implementation

The Simple Broadcast Chat Protocol (SBCP) is a protocol that allows clients to join and leave a global chat session, view members of the session, and send and receive messages. I used the SBCP protocol to develop the chat server and client instances using C. An instance of the server provides a single “chat room,” which can only handle a finite number of clients. Clients explicitly JOIN the session. A client receives a list of the connected members of the chat session once they complete the JOIN transaction. Clients use SEND messages to carry chat text, and clients receive chat text from the server using the FWD message. Clients may exit unceremoniously at any time during the
chat session. The server should detect a client exit, cleanup the resources allocated to that client and notify the other clients. Additionally, the client program will be able to detect idle clients and notify the server.

Package content:
Server.c
Client.c
utils.h
Makefile


Usage:
'make clean' to remove all previously created object files.
'make server' to compile the server source code in the package.
'make client' to compile the client source code in the package.

./server Port_Number Max_clients
./client Username Server_IP_Address Port_Number

Tests:
Normal operation of the chat server with 3 clients connected tested OK.
Client with username presently in use denied permission to enter chat room.
Previously used username (not currently in chat room) allowed to be used for new client.
Only Max_clients number of clients allowed in the chat room.
IP version independent implementation.
