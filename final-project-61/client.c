#include "client.h"
#include "headers.h"
#include "path_conversion.h"

int socket_fd;

// CONNECTS TO THE NM_CL_PORT
int establish_connection_with_naming_server()
{
    int socket_fd;
    struct sockaddr_in server_address;

    // Create a socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server_address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(NM_CL_PORT);
    inet_pton(AF_INET, NM_SERVER_IP, &(server_address.sin_addr));

    // Connect to the Naming Server
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection to Naming Server failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to Naming Server.\n");

    return socket_fd;
}
// Function to send a file request to the Naming Server (NM)
Server_Info send_file_request_to_nm(File_Request *request)
{
    // Implementation for sending file requests to the NM
    Server_Info ss_info;

    // Send the file request to the Naming Server
    send(socket_fd, request, sizeof(File_Request), 0);

    // Receive the Server_Info structure from the Naming Server
    recv(socket_fd, &ss_info, sizeof(Server_Info), 0);

    // Return the Server_Info structure containing IP address and port of the relevant Storage Server
    return ss_info;
}

// RETURNS THE CONTENT SENT FROM THE SS
// FOR READ AND GET_DETAILS
// CAVALE
void receive_file_for_read(int socket)
{

    char buffer[2 * BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0)
    {

        if (strcmp(buffer, "$%^") == 0)
        {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    printf("\nREAD DONE\n");
}
// Function to communicate with the designated Storage Server (SS)
void communicate_with_ss(Server_Info *ss_info, File_Request *request, char *write_data)
{
    int port = ss_info->port;
    // CREATING THE SOCKET
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Could not create socket!");
        return;
    }

    struct sockaddr_in server_socket_address;
    server_socket_address.sin_port = htons(port);
    server_socket_address.sin_family = AF_INET;
    inet_pton(AF_INET, ss_info->ip_address, &server_socket_address.sin_addr);

    printf("l70 port = %d\n", port);
    printf("l71 ip_address = %s\n", ss_info->ip_address);
    printf("l71 client_socket = %d\n", client_socket);
    // CONNECTING THE CLIENT TO THE SERVER
    if (connect(client_socket, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address)) < 0)
    {
        perror("Could not connect");
        return;
    }
    printf("l114\n");
    client_send(client_socket, (void *)request, 1);

    if (strcmp(request->operation, "WRITE") == 0)
    {
        // write data
        client_send(client_socket, write_data, 0);
    }
    else if (strcmp(request->operation, "READ") == 0)
    {
        printf("l123\n");
        // FOR READ
        receive_file_for_read(client_socket);
        printf("l127\n");
    }
    else
    {
        printf("l133\n");

        FileInfo *file_info = client_receive(client_socket, 2);
        printf("Size = %lld\n", file_info->size);
        printf("Permissions = %s\n", file_info->permissions);
    }
}

int create_socket_for_comm(Server_Info *ss_info)
{
    int port = ss_info->port;
    char *ip_address = ss_info->ip_address;
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

// Send path of file to naming server
void client_send(int sock, void *message, int data_type)
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
        exit(EXIT_FAILURE);
    }
}

void *client_receive(int socket, int data_type)
{
    // Variable to store the received data structure
    void *receiving_data_structure;

    // Determine the data type and allocate memory accordingly
    switch (data_type)
    {
    case 0: // String data type
        // Allocate memory for a string of 1024 characters
        receiving_data_structure = malloc(sizeof(char) * 1025);
        // Receive data from the server
        int bytes_received = recv(socket, receiving_data_structure, 1024, 0);
        if (bytes_received > 0)
        {
            // Ensure proper null-termination of the received string
            ((char *)receiving_data_structure)[bytes_received] = '\0';
        }
        else
        {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        break;

    case 1: // File_Request data type
        // Allocate memory for a File_Request structure
        receiving_data_structure = malloc(sizeof(Server_Info));
        // Receive data from the server
        if (recv(socket, receiving_data_structure, sizeof(Server_Info), 0) == -1)
        {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        receiving_data_structure = malloc(sizeof(FileInfo));
        if (recv(socket, receiving_data_structure, sizeof(FileInfo), 0) == -1)
        {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
    }
    return receiving_data_structure;
}

Server_Info *get_ss_path_from_nm()
{
    Server_Info NM_SERVER_INFO;
    NM_SERVER_INFO.port = NM_SS_PORT;
    strcpy(NM_SERVER_INFO.ip_address, NM_SERVER_IP);
    int nm_socket = create_socket_for_comm(&NM_SERVER_INFO);
    client_send(nm_socket, "GET_SS_PATH", 0); // What is the actual request for this ?
    Server_Info *ss_path = (Server_Info *)client_receive(nm_socket, 1);
    close(nm_socket);
    return ss_path;
}

void send_request_to_ss(File_Request *request, Server_Info *ss_info)
{
    int ss_socket = create_socket_for_comm(ss_info);
    client_send(ss_socket, request, 1);
    char *response = (char *)client_receive(ss_socket, 0);
    // If read - print data as well
    close(ss_socket);
}

int main()
{
    // ESTABLISH CONNECTION WITH NAMING SERVER FIRST
    socket_fd = establish_connection_with_naming_server();

    // Variables for user input
    char operation[20];
    char path[256];
    char destination_path[256];

    // Infinite loop for user interaction
    while (1)
    {
        int client_interaction_flag = 0;
        // Prompt for user input
        printf("Enter file operation (e.g., READ, WRITE, CREATE, DELETE, COPY, GET_DETAILS): ");
        scanf("%s", operation);

        // Check for exit condition
        if(strcmp("READ",operation)!=0 && strcmp("WRITE",operation)!=0 && strcmp("CREATE",operation)!=0 && strcmp("DELETE",operation)!=0 
        && strcmp("COPY",operation)!=0 &&
        strcmp("GET_DETAILS",operation)!=0){
            printf("Invalid Operation\n");
            continue;
        }
        if (strcmp(operation, "EXIT") == 0)
        {
            printf("Exiting client...\n");
            File_Request file_request;
            strcpy(file_request.operation,operation);
            client_send(socket_fd,(void*)&file_request,1);
            break; // Exit the loop
        }
        // SET THE CLIENT INTERACTION FLAG
        if (strcmp(operation, "READ") == 0 || strcmp(operation, "WRITE") == 0 || strcmp(operation, "GET_DETAILS") == 0)
        {
            client_interaction_flag = 1;
        }
        int is_directory = 0;

        // FILE OR DIRECTORY FOR CREATE, DELETE AND COPY
        if (strcmp(operation, "CREATE") == 0 || strcmp(operation, "DELETE") == 0 || strcmp(operation, "COPY") == 0)
        {
            printf("Enter 1 for file, 2 for directory: ");
            scanf("%d", &is_directory);
            is_directory--;
        }
        // TWO FILE PATHS FOR COPY OPERATION
        if (strcmp(operation, "COPY") == 0)
        {
            printf("Enter source path: ");
            scanf("%s", path);
            printf("Enter destination path: ");
            scanf("%s", destination_path);
        }
        // GET ONE FILE PATH FOR OTHER OPERATIONS
        else if (strcmp(operation, "COPY") != 0)
        {
            printf("Enter %s path: ", (is_directory ? "directory" : "file"));
            scanf("%s", path);
        }

        char write_data[BUFFER_SIZE];
        if (strcmp(operation, "WRITE") == 0)
        {
            printf("Enter data to write: ");

            char garbage;
            scanf("%c", &garbage);

            fgets(write_data, sizeof(write_data), stdin);
        }
        // Create a File_Request structure
        File_Request file_request;
        strcpy(file_request.operation, operation);
        strcpy(file_request.path, prefix(path));
        strcpy(file_request.destination_path, prefix(destination_path));
        file_request.is_directory = is_directory;

        // Send file request to the Naming Server
        Server_Info ss_info = send_file_request_to_nm(&file_request);
        printf("l318\n");
        if (client_interaction_flag == 1)
        {
            printf("ss_info.port = %d\n", ss_info.port);
            // FILE NOT FOUND
            if (ss_info.port == -1)
            {
                printf("Could not execute command: Path not found\n");
            }
            // Communicate with the designated Storage Server
            else
            {
                communicate_with_ss(&ss_info, &file_request, write_data);
            }
        }
        if(strcmp(file_request.operation,"READ")==0 || strcmp(file_request.operation,"WRITE")==0){
            int ack;
            int size=sizeof(int);
            send(socket_fd,&ack,size,0);
        }

        memset(operation, 0, sizeof(operation));
        memset(path, 0, sizeof(path));
        memset(destination_path, 0, sizeof(destination_path));
    }
    // Close the socket
    close(socket_fd);

    return 0;
}
