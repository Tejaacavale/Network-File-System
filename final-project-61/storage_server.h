#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H

#define BACKLOG 5
int server_connection_port;
char *ip_address;
Server_Init_Info ss_info;

void *failure_detection_thread(void *args);
int establish_connection_with_naming_server();
int handle_request(int socket, File_Request *Request);
void *handle_nm_connection(void *args);
void *nm_listen_thread_function();
void *handle_client_connection(void *args);
void *client_listen_thread_function();
int create_empty_file_or_directory(const char *name, int is_directory);
int delete_file_or_directory(const char *name, int is_directory);
int copy_files(int socket, const char *source_path, const char *destination_path);
int copy_directories(int socket, const char *source_path, const char *destination_path);
int copy_files_or_directories(int socket, const char *source_path, const char *destination_path, int is_directory);
int read_file(int socket, const char *file_name);
void send_file_contents(int socket, const char *file_path);
int write_file(const char *file_name, const char *data);
int get_size_and_permissions(int socket, const char *file_name);
void ss_send(int sock, void *message, int data_type);
void *ss_receive(int socket, int data_type);

#endif
