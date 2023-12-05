#ifndef NAMING_SERVER_H
#define NAMING_SERVER_H

int number_of_ss;

typedef struct ss_args ss_args;
struct ss_args
{
    int socket_fd;
    int index;
};

typedef struct stLogNode
{
    void *message_received;
    int port;
    int data_type;
    char ip_address[INET_ADDRSTRLEN];
    int client_type; // 0 for client, 1 for ss
    struct stLogNode *Next;
} stLogNode;

typedef stLogNode *LogNode;
typedef struct
{
    LogNode Front;
    LogNode Rear;
} stLogger;

typedef stLogger *Logger;

void *failure_detection_thread(void *args);
int send_to_ss_for_copy(File_Request *Request, int ss_source, int ss_destination);
int send_to_ss_for_delete_and_create(File_Request *Request, int ss_num, int backup_flag);
void *ss_thread_function(void *args);
void *cl_thread_function(void *args);
void *listen_ss_function();
void *alt_listen_ss_function();
void *listen_cl_function();
void *nm_receive(int socket, int data_type);
void create_Log(int socket, int data_type, void **data_structure);
void nm_send(int sock, void *message, int data_type);
int create_socket_for_comm(Server_Info *ss_info);

#endif