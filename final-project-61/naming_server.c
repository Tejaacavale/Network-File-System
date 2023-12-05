#include "headers.h"
#include "naming_server.h"
#include "path_conversion.h"

pthread_mutex_t lock_list_of_ss = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_num_cl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_connected = PTHREAD_MUTEX_INITIALIZER;
int num_ss;
int num_cl;

// VARIABLE TO HELP IN REDUNDANCY
int max_ss_till_now;

Server_Init_Info list_of_ss[MAX_SS];
int list_of_ss_fd[MAX_SS];
int connected[MAX_SS];

pthread_t list_of_ss_threads[MAX_SS];
pthread_t list_of_cl_threads[MAX_SS];
pthread_t list_of_failure_threads[MAX_SS];
struct TrieNode *Trieroot;
LRUCache *Cache;
void concatenateStrings(char *dest, const char *src)
{
    int i = 0;
    while (dest[i] != '\0')
    {
        i++;
    }

    int j = 0;
    while (src[j] != '\0')
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0'; // Add null terminator at the end
}
int ends_with_txt(const char *str)
{
    size_t len = strlen(str);
    const char *suffix = ".txt";
    size_t suffix_len = strlen(suffix);

    if (len >= suffix_len && strcmp(str + len - suffix_len, suffix) == 0)
    {
        return 1; // The string ends with ".txt"
    }

    return 0; // The string does not end with ".txt"
}
void receive_directory_names_dir_Trie(int socket, char *to_path, int ss)
{
    char *received_dir_name = malloc(2 * BUFFER_SIZE);
    int bytes_received;

    while (1)
    {
        bytes_received = recv(socket, received_dir_name, BUFFER_SIZE, 0);
        // printf("%s\n",received_dir_name);
        if (bytes_received <= 0)
        {
            perror("Failed to receive directory name");
            break;
        }

        received_dir_name[bytes_received] = '\0';
        create_Log(socket, STRING, (void **)&received_dir_name);
        if (strcmp(received_dir_name, "./END_TRANSMISSION") == 0)
        {
            // Termination signal received from client
            break;
        }
        char to_path_2[BUFFER_SIZE];
        strcpy(to_path_2, to_path);
        concatenateStrings(to_path_2, "/");
        concatenateStrings(to_path_2, received_dir_name);
        printf("%s\n", to_path_2);
        char str[BUFFER_SIZE];
        sprintf(str, "%d", ss);
        Trieroot = insert_key(Trieroot, to_path_2, str);
        // Handle the received directory name (you can perform your operations here)
        // printf("Received Directory: %s\n", received_dir_name);
    }
}

void *failure_detection_thread(void *args)
{
    ss_args arg = *(ss_args *)args;
    int sockfd = arg.socket_fd;
    int num = arg.index;
    // for (int i = 0; i < num_ss; i++)
    // {
    //     if (list_of_ss_fd[i] == sockfd)
    //     {
    //         num = i;
    //         break;
    //     }
    // }
    char buffer[1024];
    // Infinite loop for continuous failure detection.
    while (1)
    {
        // Send a "PING" message to the specified socket.
        if (send(sockfd, "PING", 4, 0) <= 0)
        {
            pthread_mutex_lock(&lock_connected);
            printf("Server %d disconnected\n", num + 1);
            connected[num] = 0;
            pthread_mutex_unlock(&lock_connected);
            pthread_exit(NULL);
        }

        // Check for the acknowledgement from storage server
        if (recv(sockfd, buffer, 1024, 0) <= 0)
        {
            pthread_mutex_lock(&lock_connected);
            printf("Server %d disconnected\n", num + 1);
            connected[num] = 0;
            pthread_mutex_unlock(&lock_connected);
            pthread_exit(NULL);
        }

        // Wait for 2 seconds before the next iteration.
        sleep(2);
    }
}

// THIS FUNCTION GIVES THE SS NUMBER WHERE THE CURRENT PATH NEEDS TO BE INSERTED
char *get_ss_for_insertion(char *path)
{
    int cut_index = 0;
    int i = 0;

    /////////////////////////     REASON TO CONVERT TO prefix     //////////////////////////////////////
    // THIS IS BEING DONE TO HANDLE THE FOLLOWING EDGE CASE:                                          //
    // IF CREATE dir/file IS ENTERRED BY THE USER THEN PARENT PATH IS dir                             //
    // IF CREATE dir IS ENTERRED THEN WE WILL HAVE A BUG                                              //
    // HENCE, WE FIRST CONVERT dir TO ./dir AND THEN FIND THE PARENT PATH                             //
    // PARENT PATH OBTAINED WILL BE .                                                                 //
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    char *path2 = prefix(path);
    while (path2[i] != '\0')
    {
        if (path2[i] == '/' || path2[i] == '\\')
        {
            cut_index = i;
        }
        i++;
    }
    char *parent_path = strdup(path2);
    parent_path[cut_index] = '\0';
    char *ss_i = Get_value(Trieroot, prefix(parent_path), Cache);
    free(parent_path);

    return ss_i;
}

int send_to_ss_for_copy(File_Request *Request, int ss_source, int ss_destination)
{
    // Create a socket
    int source_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (source_sock < 0)
    {
        perror("socket error");
        return -1;
    }

    // Set up the server address structure
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(list_of_ss[ss_source].nm_port);
    addr.sin_addr.s_addr = inet_addr(list_of_ss[ss_source].ip_address);

    // Connect to the server
    if (connect(source_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Connection error");
        return -1;
    }

    // Create a socket
    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock < 0)
    {
        perror("socket error");
        return -1;
    }

    // Set up the server address structure
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(list_of_ss[ss_destination].nm_port);
    addr.sin_addr.s_addr = inet_addr(list_of_ss[ss_destination].ip_address);

    // Connect to the server
    if (connect(dest_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Connection error");
        return -1;
    }

    printf("l206 source = %s, dest = %s\n", Request->path, Request->destination_path);

    send(dest_sock, Request, sizeof(File_Request), 0);
    send(source_sock, Request, sizeof(File_Request), 0);

    char buffer[BUFFER_SIZE];
    strcpy(buffer, "INIT.RECEIVE");
    send(dest_sock, buffer, BUFFER_SIZE, 0);
    strcpy(buffer, "INIT.SEND");
    send(source_sock, buffer, BUFFER_SIZE, 0);
    // SEND DATA TO STORAGE SERVER j
    int bytes_received;

    while (1)
    {
        printf("l216\n");
        char linking[BUFFER_SIZE];
        bytes_received = recv(source_sock, linking, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            break;
        }
        if (strcmp(linking, "TRANSFER_DONE") == 0)
        {
            break;
        }
        printf("sending '%s'\n", linking);
        send(dest_sock, linking, bytes_received, 0);
    }
    close(source_sock);
    close(dest_sock);
}
// FUNCTION IS CALLED FOR "CREATE" AND "DELETE"
int send_to_ss_for_delete_and_create(File_Request *Request, int ss_num, int backup_flag)
{
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket error");
        return -1;
    }

    // Set up the server address structure
    struct sockaddr_in addr;
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(list_of_ss[ss_num].nm_port);
    addr.sin_addr.s_addr = inet_addr(list_of_ss[ss_num].ip_address);

    // printf("l228 nm_port = %d\n", list_of_ss[ss_num].nm_port);
    // printf("l230 ip_address = %s\n\n", list_of_ss[ss_num].ip_address);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Connection error");
        return -1;
    }

    // Send the file request to the server
    nm_send(sock, (void *)Request, 1);

    if (backup_flag)
    {
        int ERRNO;
        if (recv(sock, &ERRNO, sizeof(int), 0) < 0)
        {
            perror("Could not receive the response from SS for CREATE or DELETE");
            return SERVER_DOWN;
        }
        printf("l243 ERRNO = %d\n", ERRNO);
        if (ERRNO != OK)
        {
            perror("Could not CREATE RECOVERY FOLDER");
            return ERRNO;
        }
        close(sock);
        return ERRNO;
    }

    // Receive the response (error code) from the server
    int ERRNO;
    if (recv(sock, &ERRNO, sizeof(int), 0) < 0)
    {
        perror("Could not receive the response from SS for CREATE or DELETE");
        return SERVER_DOWN;
    }
    printf("l243 ERRNO = %d\n", ERRNO);
    if (ERRNO != OK)
    {
        perror("Could not CREATE or DELETE");
        return ERRNO;
    }
    // IF PATH IS CREATED SUCCESSFULLY, INSERT INTO TRIE
    if (ERRNO == OK && strcmp(Request->operation, "CREATE") == 0)
    {
        char *ss_i = get_ss_for_insertion(Request->path);
        printf("l248 ss_i = %s\n", ss_i);

        // INSERTS ./DIR INSTEAD OF DIR
        Trieroot = insert_key(Trieroot, Request->path, ss_i);
    }
    // IF PATH IS REMOVED SUCCESSFULLY, REMOVE FROM TRIE
    if (ERRNO == OK && strcmp(Request->operation, "DELETE") == 0)
    {
        // REMOVES ./DIR INSTEAD OF DIR
        Delete_key(Trieroot, prefix(Request->path), Cache);
    }
    // Close the socket
    close(sock);

    // Return the received error code
    return ERRNO;
}

// THIS THREAD IS CREATED FOR EACH SS WHICH CONNECTS TO THE NM
// IN THIS THREAD, WE RECEIVE THE DATA WHICH WE STORE IN THE list_of_ss[]
void *ss_thread_function(void *args)
{
    ss_args _args = *(ss_args *)args;
    int new_socket = _args.socket_fd;
    int index = _args.index;

    // RECEIVING THE DATA
    Server_Init_Info ss_info = *(Server_Init_Info *)nm_receive(new_socket, 2);

    pthread_mutex_lock(&lock_list_of_ss);
    int flag = 0;
    for (int i = 0; i < num_ss; i++)
    {
        // IF THIS SS HAS COME BEFORE OR NOT
        if (list_of_ss[i].nm_port == ss_info.nm_port && list_of_ss[i].client_port == ss_info.client_port)
        {
            pthread_mutex_lock(&lock_connected);
            connected[i] = 1;
            pthread_mutex_unlock(&lock_connected);
            printf("Server %d back online\n", i + 1);
            pthread_create(&list_of_failure_threads[i], NULL, failure_detection_thread, (void *)&new_socket);
            flag = 1;
            break;
        }
    }
    pthread_mutex_unlock(&lock_list_of_ss);
    if (flag == 0)
    {
        connected[index] = 1;
        printf("index = %d\n", index);
        printf("l110 np_port = %d\n", ss_info.nm_port);
        printf("l111 cl_port = %d\n", ss_info.client_port);
        printf("l112 ip_addr = %s\n", ss_info.ip_address);
        printf("\n");

        // ADDING THE DATA TO THE list_of_ss[]
        pthread_mutex_lock(&lock_list_of_ss);

        list_of_ss[index].client_port = ss_info.client_port;
        list_of_ss[index].nm_port = ss_info.nm_port;
        strcpy(list_of_ss[index].ip_address, ss_info.ip_address);
        list_of_ss[index].Paths = ss_info.Paths;

        printf("l127 ip_addr = %s\n", list_of_ss[index].ip_address);
        pthread_create(&list_of_failure_threads[index], NULL, failure_detection_thread, (void *)&new_socket);
        num_ss++;
        pthread_mutex_unlock(&lock_list_of_ss);
    }
    pthread_mutex_lock(&lock_list_of_ss);

    printf("list_of_ss[]:  \n");
    for (int i = 0; i < num_ss; i++)
    {
        printf("%d %s %d\n", list_of_ss[i].nm_port, list_of_ss[i].ip_address, list_of_ss[i].client_port);
    }
    printf("___________\n");

    pthread_mutex_unlock(&lock_list_of_ss);

    return NULL;
}

void *cl_thread_function(void *args)
{
    int new_socket = *(int *)args;

    // Handle client requests
    while (1)
    {
        File_Request *reqptr;
        // memset(&req, 0, sizeof(File_Request));

        // Receive the File_Request from the client
        // recv(new_socket, &req, sizeof(File_Request), 0);
        reqptr = (File_Request *)nm_receive(new_socket, FILE_REQUEST);
        if (reqptr == NULL)
        {
            printf("Client Disconnecting...\n");
            return NULL;
        }
        File_Request req = *reqptr;
        printf("l32\n");

        // CLIENT INTERACTION FLAG
        int client_interaction_flag = 0;
        if (strcmp(req.operation, "READ") == 0 || strcmp(req.operation, "WRITE") == 0 || strcmp(req.operation, "GET_DETAILS"))
        {
            client_interaction_flag = 1;
        }
        printf("operation : %s\n", req.operation);
        // Check if the client is exiting
        if (strcmp(req.operation, "EXIT") == 0)
        {
            printf("l37 Client Exiting...\n");
            return NULL;
        }

        else if (strcmp(req.operation, "CREATE") == 0 || strcmp(req.operation, "DELETE") == 0)
        {
            // VALUE STORES THE SS OF THE PARENT FOLDER OF THE OBJECT TO BE CREATED
            char *value = get_ss_for_insertion(req.path);
            // THIS LOCATION DOES NOT EXIST IN ANY OF THE SS
            if (value == NULL)
            {
                printf("Path does not exist in any Storage Server: Could not %s\n", req.operation);
                // WE RETURN BACK THE ERROR SERVER INFO
                Server_Info ss_info;
                ss_info.port = -1;
                if (send(new_socket, &ss_info, sizeof(Server_Info), 0) == -1)
                {
                    printf("Client Disconnecting...\n");
                    return NULL;
                }
                // WAIT FOR THE NEXT COMMAND OF THE USER
                continue;
            }
            // REQUIRED STORAGE SERVER IS OBTAINED
            int ss_i = atoi(value);
            send_to_ss_for_delete_and_create(&req, ss_i, 0);
        }
        else if (strcmp(req.operation, "COPY") == 0)
        {
            printf("l264 source_path = %s\n", req.path);
            printf("l265 destination_path = %s\n", req.destination_path);

            char *value1 = Get_value(Trieroot, req.path, Cache);
            char *value2 = Get_value(Trieroot, req.destination_path, Cache);

            if (value1 == NULL)
            {
                printf("Source path does not exist in any Storage Server: Could not %s\n", req.operation);
                // WE RETURN BACK THE ERROR SERVER INFO
                Server_Info ss_info;
                ss_info.port = -1;
                if (send(new_socket, &ss_info, sizeof(Server_Info), 0) == -1)
                {
                    printf("Client Disconnecting...\n");
                    return NULL;
                }
                // WAIT FOR THE NEXT COMMAND OF THE USER
                continue;
            }
            if (value2 == NULL)
            {
                printf("Destination path does not exist in any Storage Server: Could not %s\n", req.operation);

                // WE RETURN BACK THE ERROR SERVER INFO
                Server_Info ss_info;
                ss_info.port = -1;
                if (send(new_socket, &ss_info, sizeof(Server_Info), 0) == -1)
                {
                    printf("Client Disconnecting...\n");
                    return NULL;
                }
                // WAIT FOR THE NEXT COMMAND OF THE USER
                continue;
            }
            // GET SSi and SSj FROM TRIE
            int ss_source = atoi(value1);
            int ss_destination = atoi(value2);

            printf("l459 ssi = %d, ssj = %d\n", ss_source, ss_destination);
            send_to_ss_for_copy(&req, ss_source, ss_destination);
        }

        // Print the received File_Request
        // GET THE SERVER STRUCT FROM TRIE AND SEND IT TO CLIENT
        char *value = Get_value(Trieroot, req.path, Cache);
        Server_Info ss_info;
        // PATH DOES NOT EXIST
        if (value == NULL)
        {
            printf("Given path not found in Trie\n");
            ss_info.port = -1;
        }
        // PATH EXISTS
        else
        {
            int index = atoi(value);

            // FINDING THE RIGHT SS (REDUNDANCY)
            if (connected[index] == 0)
            {
                // ss1 is connected
                if (connected[0] == 1)
                {
                    index = 1;
                }
                // ss1 is down but ss2 is connected
                else if (connected[1] == 1)
                {
                    index = 0;
                }
                // can't get data from any ss
                else
                {
                    index = -1;
                }
            }

            pthread_mutex_lock(&lock_list_of_ss);
            if (index != -1)
            {
                ss_info.port = list_of_ss[index].client_port;
                strcpy(ss_info.ip_address, list_of_ss[index].ip_address);
            }
            else
            {
                ss_info.port = -1;
            }
            pthread_mutex_unlock(&lock_list_of_ss);
            printf("l510\n");
            struct TrieNode *Node = get_node(Trieroot, req.path);
            printf("%s\n", Node->value);
            if (strcmp(req.operation, "READ") == 0)
            {
                acquire_readlock(get_node(Trieroot, req.path));
            }
            if (strcmp(req.operation, "WRITE") == 0)
            {
                acquire_writelock(get_node(Trieroot, req.path));
            }
        }
        printf("l379\n");
        if (send(new_socket, &ss_info, sizeof(Server_Info), 0) == -1)
        {
            printf("Client Disconnecting...\n");
            return NULL;
        }
        if (strcmp(req.operation, "READ") == 0 || strcmp(req.operation, "WRITE") == 0)
        {
            int ack;
            recv(new_socket, &ack, sizeof(int), 0);
            if (strcmp(req.operation, "READ") == 0)
            {
                release_readlock(get_node(Trieroot, req.path));
            }
            else
            {
                release_writelock(get_node(Trieroot, req.path));
            }
        }
    }

    // UNMARK CLIENT FROM THE CLIENT ARRAY
    // HANDLE CLIENT EXITING
    pthread_mutex_lock(&lock_num_cl);
    num_cl--;
    pthread_mutex_unlock(&lock_num_cl);

    return NULL;
}

void *alt_listen_ss_function()
{

    // Set up a socket for incoming ss connections
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        return NULL;
    }
    struct sockaddr_in server_address;
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(NM_SS_PORT);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("SS_Binding failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("NM listening for SS on NM_SS_PORT\n");

    pthread_t thread_for_each_ss;
    while (1)
    {
        int ss_socket;
        struct sockaddr_in ss_address;
        socklen_t ss_addr_len = sizeof(ss_address);

        // DON'T ALLOW FOR CONNECTIONS IN THIS CASE
        if (num_ss == MAX_SS)
        {
            continue;
        }
        printf("l542\n");
        // Accept a connection from the ss
        if ((ss_socket = accept(server_socket, (struct sockaddr *)&ss_address, &ss_addr_len)) == -1)
        {
            perror("Accept failed");
            close(server_socket);
            continue;
        }

        Server_Init_Info ss_info = *(Server_Init_Info *)nm_receive(ss_socket, SERVER_INIT_INFO);
        //////////////////////////////////////
        /////// FAILURE DETECTION CODE ///////
        ///////////////////////////////////////
        // Check if the details have been received before
        ss_args args;
        pthread_mutex_lock(&lock_list_of_ss);
        int flag = 0;
        for (int i = 0; i < num_ss; i++)
        {
            // IF THIS SS HAS COME BEFORE OR NOT
            if (list_of_ss[i].nm_port == ss_info.nm_port && list_of_ss[i].client_port == ss_info.client_port)
            {
                pthread_mutex_lock(&lock_connected);
                connected[i] = 1;
                pthread_mutex_unlock(&lock_connected);
                printf("Server %d back online\n", i + 1);
                args.index = i;
                args.socket_fd = ss_socket;
                pthread_create(&list_of_failure_threads[i], NULL, failure_detection_thread, (void *)&args);
                flag = 1;
                break;
            }
        }
        pthread_mutex_unlock(&lock_list_of_ss);
        if (flag == 1)
        {
            continue;
        }
        // ANY SS CONNECTED FOR THE FIRST TIME
        max_ss_till_now++;
        printf("Server %d connected\n", num_ss + 1);
        connected[num_ss] = 1;
        int index = num_ss;
        args.index = index;
        args.socket_fd = ss_socket;
        connected[index] = 1;

        // ADDING THE DATA TO THE list_of_ss[]
        pthread_mutex_lock(&lock_list_of_ss);

        list_of_ss[index].client_port = ss_info.client_port;
        list_of_ss[index].nm_port = ss_info.nm_port;
        strcpy(list_of_ss[index].ip_address, ss_info.ip_address);
        list_of_ss[index].Paths = ss_info.Paths;

        printf("l127 ip_addr = %s\n", list_of_ss[index].ip_address);
        // Sending data to trie.
        char to_path[BUFFER_SIZE];
        strcpy(to_path, ".");
        printf("l592 The file paths in the connected ss are :\n");
        receive_directory_names_dir_Trie(ss_socket, to_path, num_ss);

        // REDUNDANCY
        // if (max_ss_till_now > 1)
        // {
        //     printf("l622 num_ss = %d\n", index);
        //     File_Request req;
        //     char str[256];
        //     sprintf(str, "./%d", index);
        //     strcpy(req.path, str);
        //     strcpy(req.operation, "CREATE");
        //     req.is_directory = 1;
        //     int err = send_to_ss_for_delete_and_create(&req, 0, 1);
        //     printf("l634 err = %d\n", err);

        //     // printf("l631\n");
        //     // strcpy(req.destination_path, str);
        //     // strcpy(req.path, ".");
        //     // printf("l634\n");
        //     // send_to_ss_for_copy(&req, num_ss, 0);
        // }

        if (index >= 2)
        {
            File_Request req;
            strcpy(req.operation, "COPY");
            strcpy(req.destination_path, ".");
            strcpy(req.path, ".");
            req.is_directory = 1;
            int ret = send_to_ss_for_copy(&req, index, 0);
            send_to_ss_for_copy(&req, index, 1);
            printf("l643 ret = %d\n", ret);
        }

        pthread_create(&list_of_failure_threads[index], NULL, failure_detection_thread, (void *)&args);
        num_ss++;
        pthread_mutex_unlock(&lock_list_of_ss);
    }

    close(server_socket); // Close the server socket (never reached in this example)
    return 0;
}

void *listen_cl_function()
{
    // Set up a socket for incoming ss connections
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        return NULL;
    }
    struct sockaddr_in server_address;
    memset(&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(NM_CL_PORT);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("CL_Binding failed");
        close(server_socket);
        return NULL;
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        return NULL;
    }
    printf("NM listening for CL on NM_CL_PORT\n");

    while (1)
    {
        int cl_socket;
        struct sockaddr_in cl_address;
        socklen_t client_addr_len = sizeof(cl_address);

        // DON'T ALLOW FOR CONNECTIONS IN THIS CASE
        if (num_cl == MAX_CL)
        {
            continue;
        }

        // Accept a connection from the client
        if ((cl_socket = accept(server_socket, (struct sockaddr *)&cl_address, &client_addr_len)) == -1)
        {
            perror("Accept failed");
            close(server_socket);
            return NULL;
        }

        pthread_mutex_lock(&lock_num_cl);

        printf("Client %d connected\n", num_cl + 1);
        pthread_create(&list_of_cl_threads[num_cl], NULL, cl_thread_function, (void *)&cl_socket);
        num_cl++;

        pthread_mutex_unlock(&lock_num_cl);

        // CREATE THREADS HERE
        printf("l148\n");
    }

    close(server_socket); // Close the server socket (never reached in this example)
    return 0;
}

void *nm_receive(int socket, int data_type)
{
    // Variable to store the received data structure
    void *receiving_data_structure;

    // Determine the data type and allocate memory accordingly
    switch (data_type)
    {
    case STRING: // String data type
        // Allocate memory for a string of 1024 characters
        receiving_data_structure = malloc(sizeof(char) * (BUFFER + 1));
        // Receive data from the server
        int bytes_received = recv(socket, receiving_data_structure, BUFFER, 0);
        if (bytes_received > 0)
        {
            // Ensure proper null-termination of the received string
            ((char *)receiving_data_structure)[bytes_received] = '\0';
        }
        else
        {
            perror("l24 recv failed");
            return NULL;
        }
        break;

    case FILE_REQUEST: // File_Request data type
        // Allocate memory for a File_Request structure
        receiving_data_structure = malloc(sizeof(File_Request));
        // Receive data from the server
        if (recv(socket, receiving_data_structure, sizeof(File_Request), 0) <= 0)
        {
            return NULL;
        }
        break;
    case SERVER_INIT_INFO: // Server_Init_Info data type
        // Allocate memory for a Server_Init_Info structure
        receiving_data_structure = malloc(sizeof(Server_Init_Info));
        // Receive data from the server
        if (recv(socket, receiving_data_structure, sizeof(Server_Init_Info), 0) <= 0)
        {
            return NULL;
        }
        break;
    case SERVER_INFO:
        // Allocate memory for a Server_Init_Info structure
        receiving_data_structure = malloc(sizeof(Server_Info));
        // Receive data from the server
        if (recv(socket, receiving_data_structure, sizeof(Server_Info), 0) <= 0)
        {
            return NULL;
        }
        break;
    }
    create_Log(socket, data_type, &receiving_data_structure);
    return receiving_data_structure;
}

void nm_send(int sock, void *message, int data_type)
{
    // Calculate the size of the message (including the null terminator)
    int message_size;
    switch (data_type)
    {
    case (0):
        message_size = strlen(message) + 1;
        break;
    case (1):
        message_size = sizeof(File_Request);
        break;
    }

    // Send the path to the server
    if (send(sock, message, message_size, 0) == -1)
    {
        perror("send failed");
        return;
    }
}

// CREATES SOCKET GIVEN THE PORT AND IP OF SS
int create_socket_for_comm(Server_Info *ss_info)
{
    int port = ss_info->port;
    char *ip_address = strdup(ss_info->ip_address);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0)
    {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // Keep trying to establish a connection until successful
    while (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connection failed");
        sleep(5);
    }
    return sock;
}

void create_Log(int socket, int data_type, void **data_structure)
{
    void *message_received = *data_structure;
    char ip_address[INET_ADDRSTRLEN];
    int port;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(socket, (struct sockaddr *)&addr, &len) < 0)
    {
        perror("getsockname");
        return;
    }
    inet_ntop(AF_INET, &addr.sin_addr, ip_address, INET_ADDRSTRLEN);
    port = ntohs(addr.sin_port);
    FILE *fptr = fopen("logs.txt", "a+");
    fprintf(fptr, "Received from ip:%s port:%d\n", ip_address, port);
    if (data_type == STRING)
    {
        char *printhead = (char *)message_received;
        fprintf(fptr, "%s\n", printhead);
    }
    if (data_type == FILE_REQUEST)
    {
        File_Request *printhead = (File_Request *)message_received;
        fprintf(fptr, "%s %s\n", printhead->operation, printhead->path);
    }
    if (data_type == SERVER_INIT_INFO)
    {

        Server_Init_Info *printhead = (Server_Init_Info *)message_received;
        fprintf(fptr, "ip_address:%s cl_port:%d nm_port:%d\n", printhead->ip_address, printhead->client_port, printhead->nm_port);
    }
    fclose(fptr);
}

int main()
{
    Cache = createLRUCache(10);
    // INITIATE SS LIST
    for (int i = 0; i < MAX_SS; i++)
    {
        list_of_ss[i].nm_port = -1;
        connected[i] = 0;
        list_of_ss[i].is_backed_up = 0;
    }
    num_ss = 0;
    num_cl = 0;
    max_ss_till_now = 0;

    // Intialising Trie.
    Trieroot = new_object();
    // LISTENS FOR SS CONNECTIONS
    pthread_t listen_ss;
    if (pthread_create(&listen_ss, NULL, alt_listen_ss_function, NULL) != 0)
    {
        perror("listen_ss_function: Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // LISTENS FOR CL CONNECTIONS
    pthread_t listen_cl;
    if (pthread_create(&listen_cl, NULL, listen_cl_function, NULL) != 0)
    {
        perror("listen_cl_function: Thread creation failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(listen_cl, NULL);
    return 0;
}
