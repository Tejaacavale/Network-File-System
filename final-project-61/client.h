#ifndef CLIENT_H
#define CLIENT_H
#include "headers.h"

int establish_connection_with_naming_server();
Server_Info send_file_request_to_nm(File_Request *request);
void receive_file_for_read(int socket);
void communicate_with_ss(Server_Info *ss_info, File_Request *request, char *write_data);
int create_socket_for_comm(Server_Info *ss_info);
void client_send(int sock, void *message, int data_type);
void *client_receive(int socket, int data_type);
Server_Info *get_ss_path_from_nm();
void send_request_to_ss(File_Request *request, Server_Info *ss_info);

#endif
