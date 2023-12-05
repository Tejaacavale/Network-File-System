#include "headers.h"
#include "storage_server.h"
#include "path_conversion.h"
// pthread_t handle_client_connection_thread;
// int client_sock[3];
int numcl = 0;
void removeFirstTwoDigits(char arr[100], char to_arr[100])
{
    // Move the pointer two positions ahead
    for (int i = 0; i < strlen(arr) - 2; i++)
    {

        to_arr[i] = arr[i + 2];
    }
    to_arr[strlen(arr) - 2] = '\0';
}

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
void receive_directory_names_dir(int socket, char *to_path)
{
    char received_dir_name[2 * BUFFER_SIZE];
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
        // Handle the received directory name (you can perform your operations here)
        // printf("Received Directory: %s\n", received_dir_name);
        if (mkdir(to_path_2, 0777) == -1)
        {
            if (errno == EEXIST)
            {
                // Handle case where directory already exists
                printf("Directory already exists: %s\n", received_dir_name);
                // You can choose to skip or handle this situation differently
            }
            else
            {
                perror("mkdir");
                // printf("MKDIRERROR\n");
                // Handle other errors if necessary
            }
        }
        else
        {
            printf("Directory created:%s\n", to_path);
        }
    }
}

int i;
void receive_file_contents(int socket, const char *file_path)
{
    int file_fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    printf("%s\n", file_path);

    if (file_fd == -1)
    {
        perror("open");
        return;
    }
    char buffer[2 * BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0)
    {

        if (strcmp(buffer, "$%^") == 0)
        {
            break;
        }
        buffer[bytes_received] = '\0';
        // printf("%lu\n",strlen(buffer));
        if (write(file_fd, buffer, strlen(buffer)) != strlen(buffer))
        {
            perror("write");
            break;
        }
    }
    close(file_fd);
}
int i;
void receive_directory_names_text(int socket, char *to_path)
{
    char received_dir_name[2 * BUFFER_SIZE];
    int bytes_received;
    while (1)
    {
        bytes_received = recv(socket, received_dir_name, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            perror("Failed to receive file name");
            break;
        }

        received_dir_name[bytes_received] = '\0';
        // printf("%s\n",received_dir_name);
        if (strcmp(received_dir_name, "./END_TRANSMISSION") == 0)
        {
            // Termination signal received from client
            break;
        }
        char to_path_2[BUFFER_SIZE];
        strcpy(to_path_2, to_path);
        // received_dir_name[bytes_received] = '\0';
        // printf("%s\n",to_path);
        strcat(to_path_2, "/");
        strcat(to_path_2, received_dir_name);
        // printf("%s\n",to_path);
        receive_file_contents(socket, to_path_2);
        printf("Received file: %s\n", to_path_2);

        // Check if it is a file or directory
        // struct stat file_stat;
        // stat(received_dir_name, &file_stat);
        // if (S_ISREG(file_stat.st_mode)) {
        //     receive_file_contents(socket, received_dir_name);
        // // } else {
        //     printf("Received file: %s\n", received_dir_name);
        //     mkdir(received_dir_name, 0777); // Create directory
        // }
    }
}
void recieve_file(int socket, char *to_path)
{
    int file_fd = open(to_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (file_fd == -1)
    {
        perror("open");
        return;
    }
    char buffer[2 * BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0)
    {

        if (strcmp(buffer, "$%^") == 0)
        {
            break;
        }
        buffer[bytes_received] = '\0';
        // printf("%lu\n",strlen(buffer));
        if (write(file_fd, buffer, strlen(buffer)) != strlen(buffer))
        {
            perror("write");
            break;
        }
    }
    close(file_fd);
}

void send_directory_name(int socket, const char *dir_name, const char *current_path)
{
    char full_path[2 * BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    // char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // send(socket,start,BUFFER_SIZE,0);
    removeFirstTwoDigits(full_path, full_path);
    printf("Sending Directory: %s\n", full_path);
    send(socket, full_path, BUFFER_SIZE, 0);
}
void send_directory_name_txt(int socket, const char *dir_name, const char *current_path)
{
    char full_path[2 * BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // printf("%s\n",full_path);
    // send(socket,start,BUFFER_SIZE,0);
    removeFirstTwoDigits(full_path, full_path);
    printf("Sending File :%s\n", full_path);
    send(socket, full_path, BUFFER_SIZE, 0);
}
void send_endtransmission(int socket, const char *dir_name, const char *current_path)
{
    char full_path[2 * BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    // char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // send(socket,start,BUFFER_SIZE,0);

    send(socket, full_path, BUFFER_SIZE, 0);
}
void seek_recursive_send_dir(int socket, const char *path, const char *current_path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            // Send directory name to server
            send_directory_name(socket, entry->d_name, current_path);
            printf("Sent Directory :%s\n", entry->d_name);
            // Recursive call to send contents of subdirectory
            char new_current_path[2 * BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", path, entry->d_name);
            seek_recursive_send_dir(socket, full_path, new_current_path);
        }
        else if (ends_with_txt(full_path))
        {

            send_directory_name(socket, entry->d_name, current_path);
            printf("Sent Files :%s\n", entry->d_name);
        }
    }

    closedir(dir);
}
void seek_recursive_send_dir_2(int socket, const char *path, const char *current_path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            // Send directory name to server
            send_directory_name(socket, entry->d_name, current_path);
            printf("Sent Directory :%s\n", entry->d_name);
            // Recursive call to send contents of subdirectory
            char new_current_path[2 * BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", path, entry->d_name);
            seek_recursive_send_dir_2(socket, full_path, new_current_path);
        }
    }

    closedir(dir);
}
void seek_recursive_send_dir_trie(int socket, const char *path, const char *current_path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            // Send directory name to server
            send_directory_name(socket, entry->d_name, current_path);
            printf("Sent Directory :%s\n", entry->d_name);
            // Recursive call to send contents of subdirectory
            char new_current_path[2 * BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", path, entry->d_name);
            seek_recursive_send_dir_trie(socket, full_path, new_current_path);
        }
        else if (ends_with_txt(full_path))
        {

            send_directory_name(socket, entry->d_name, current_path);
            printf("Sent Files :%s\n", entry->d_name);
        }
    }

    closedir(dir);
}
// void send_file_contents(int socket, const char *file_path) {
//     FILE *file = fopen(file_path, "r");
//     if (file == NULL) {
//         perror("fopen");
//         return;
//     }

//     char buffer[BUFFER_SIZE];
//     size_t bytes_read;
//     while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
//         printf("%s\n",buffer);
//         if (send(socket, buffer, bytes_read, 0) == -1) {
//             perror("send file contents");
//             break;
//         }
//     }
//     strcpy(buffer,"$%^");
//     if (send(socket, buffer, bytes_read, 0) == -1) {
//             perror("send file contents");
//         }

//     fclose(file);
// }

void seek_recursive_send_text(int socket, const char *path, const char *current_path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            // send_directory_name(socket, entry->d_name, current_path);
            char new_current_path[BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", current_path, entry->d_name);
            seek_recursive_send_text(socket, full_path, new_current_path);
        }
        else if (S_ISREG(file_stat.st_mode))
        {
            if (ends_with_txt(full_path))
            {
                send_directory_name_txt(socket, entry->d_name, current_path);
                send_file_contents(socket, full_path);
                char ending_symbol[BUFFER_SIZE];
                strcpy(ending_symbol, "$%^");
                send(socket, ending_symbol, BUFFER_SIZE, 0);
                printf("Sent File: %s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
}
void transfer_file(int socket, char *from)
{
    int file_fd = open(from, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open");
        return;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        close(file_fd);
        return;
    }

    // Return to the beginning of the file
    if (lseek(file_fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        close(file_fd);
        return;
    }

    char buffer[2 * BUFFER_SIZE];
    size_t bytes_read;
    int i = 0;

    while (file_size > 0)
    {
        size_t remaining_bytes = file_size > BUFFER_SIZE ? BUFFER_SIZE : file_size;
        i++;
        bytes_read = read(file_fd, buffer, remaining_bytes);
        if (bytes_read <= 0)
        {
            if (bytes_read == -1)
            {
                perror("read");
            }
            break;
        }
        if (file_size < BUFFER_SIZE)
        {
            buffer[bytes_read] = '\0';
        }
        ssize_t bytes_sent = send(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_sent == -1)
        {
            perror("send file contents");
            break;
        }

        file_size -= bytes_read;
    }
    char ending_symbol[BUFFER_SIZE];
    strcpy(ending_symbol, "$%^");
    send(socket, ending_symbol, BUFFER_SIZE, 0);
    close(file_fd);
}
void *failure_detection_thread(void *args)
{
    int sockfd = *((int *)args);
    while (1)
    {
        char message[1024];
        if (recv(sockfd, message, 1024, 0) > 0)
        {
            send(sockfd, "ACK", 3, 0);
        }
    }
}

// Function to establish connection with naming server
// HERE, SS IS THE SUB
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
    server_address.sin_port = htons(NM_SS_PORT);
    inet_pton(AF_INET, NM_SERVER_IP, &(server_address.sin_addr));

    // Connect to the Naming Server
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection to Naming Server failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to Naming Server.\n");

    // Send the server information to the Naming Server
    // Server_Init_Info send_ss_info;
    // send_ss_info.nm_port = ss_info.nm_port;
    // send_ss_info.client_port = ss_info.client_port;
    // strcpy(send_ss_info.ip_address, "");
    ss_info.Paths = NULL;

    fprintf(stdout, "l54 np_port = %d\n", ss_info.nm_port);
    fprintf(stdout, "l55 cl_port = %d\n", ss_info.client_port);
    fprintf(stdout, "\n");

    ss_send(socket_fd, (void *)&ss_info, 2);

    // N ACCESSIBLE PATHS
    int num_accessible_paths;
    printf("Enter number of accessible paths: ");
    scanf("%d", &num_accessible_paths);
    for (int i = 0; i < num_accessible_paths; i++)
    {
        char accessible_path[BUFFER_SIZE];
        scanf("%s", accessible_path);
        
        send(socket_fd, no_prefix(accessible_path), BUFFER_SIZE, 0);
    }

    // Sending all the file info and structure to NS.
    // Sending all the directory details
    // char starting_directory[4096] = ".";
    // seek_recursive_send_dir_trie(socket_fd, starting_directory, ".");

    send_endtransmission(socket_fd, "END_TRANSMISSION", ".");
    pthread_t failure_thread;
    pthread_create(&failure_thread, NULL, failure_detection_thread, (void *)&socket_fd);

    return socket_fd;
}

// handle all types of requests
int handle_request(int socket, File_Request *Request)
{
    printf("Req->operation = %s\n", Request->operation);
    printf("req-> = %s\n", Request->path);

    // If unsuccessful, return the error code
    if (strcmp(Request->operation, "READ") == 0)
    {
        // Read
        int ERRNO = read_file(socket, Request->path);
        printf("l76\n");
        return ERRNO;
    }
    if (strcmp(Request->operation, "WRITE") == 0)
    {
        // Write
        printf("l81\n");
        char *data = (char *)ss_receive(socket, 0);
        printf("data = %s\n", data);
        write_file(Request->path, data);
    }
    if (strcmp(Request->operation, "CREATE") == 0)
    {
        // Create
        int ERRNO;
        ERRNO = create_empty_file_or_directory(Request->path, Request->is_directory);
        printf("OK");
        return ERRNO;
    }
    if (strcmp(Request->operation, "DELETE") == 0)
    {
        // Delete
        int ERRNO = delete_file_or_directory(Request->path, Request->is_directory);
        return ERRNO;
    }
    if (strcmp(Request->operation, "COPY") == 0)
    {
        // Copy
        // TEJAS CAVALEEEEE (:
        printf("l563\n");
        int ERRNO = copy_files_or_directories(socket, Request->path, Request->destination_path, Request->is_directory);
        printf("l565\n");
        return ERRNO;
    }
    if (strcmp(Request->operation, "GET_DETAILS") == 0)
    {
        // Get Size and Permissions
        int ERRNO = get_size_and_permissions(socket, Request->path);
        return ERRNO;
    }
    // Return 0 on success
    return OK;
}

// Handles requests received directly from the naming server- CREATE and DELETE
int global_error;
void *handle_nm_connection(void *args)
{
    int nm_socket = *(int *)args;
    File_Request Request = *(File_Request *)ss_receive(nm_socket, 1);
    printf("l582\n");
    global_error = handle_request(nm_socket, &Request);
    printf("l584\n");
    if (send(nm_socket, &global_error, sizeof(int), 0) == -1)
    {
        perror("send");
    }
    printf("l588\n");
}

void *nm_listen_thread_function()
{
    // Create socket for listening to nm
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        perror("Couldn't create socket");
        return NULL;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ss_info.nm_port); // Listen on nm port
    server_addr.sin_addr.s_addr = INADDR_ANY;

    inet_ntop(AF_INET, &(server_addr.sin_addr), ss_info.ip_address, INET_ADDRSTRLEN);

    // Bind the socket
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(listen_socket);
        return NULL;
    }
    // Listen for incoming connections
    if (listen(listen_socket, BACKLOG) == -1)
    {
        perror("Listen failed");
        close(listen_socket);
        return NULL;
    }

    printf("Listening for instructions from Naming Server\n");

    while (1)
    {
        // Accept a connection
        printf("l626 listening on port %d\n", ss_info.nm_port);
        int nm_socket = accept(listen_socket, NULL, NULL);
        if (nm_socket < 0)
        {
            perror("Accept failed");
            close(listen_socket);
            continue;
        }
        printf("l634\n");
        // Handle the connection
        // Intialise it
        // Create thread
        pthread_t handle_connection_thread;
        pthread_create(&handle_connection_thread, NULL, handle_nm_connection, (void *)&nm_socket);
        printf("l637\n");
        // close the socket
        // close(nm_socket);
    }

    // Close the listen socket (this part is unreachable in this example)
    close(listen_socket);

    return NULL;
}

// FUNCTION TO WHICH WORKS AFTER THE CONNECTION IS ESTABLISHED WITH THE CLIENT
void *handle_client_connection(void *args)
{
    int client_socket = *(int *)args;
    File_Request *req = (File_Request *)ss_receive(client_socket, 1);
    printf("l175 operation = %s, path = %s\n", req->operation, req->path);
    int ERRNO = handle_request(client_socket, req);
    printf("l188\n");
    if (strcmp(req->operation, "READ") != 0)
    {
        printf("l191\n");
        if (send(client_socket, &ERRNO, sizeof(int), 0) <= 0)
        {
            perror("l195 send");
        }
    }
    close(client_socket);
    printf("l177\n");
    return NULL;
}

// HERE, SS IS THE DOM
void *client_listen_thread_function()
{
    // Create a socket for client connections
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        perror("Socket creation failed");
        return NULL;
    }

    // Set up server_address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;          // Listen on all available interfaces
    server_address.sin_port = htons(ss_info.client_port); // Listen on client port

    // Bind the socket
    if (bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Bind failed");
        close(listen_socket);
        return NULL;
    }

    // Listen for incoming connections
    if (listen(listen_socket, BACKLOG) == -1)
    {
        perror("Listen failed");
        close(listen_socket);
        return NULL;
    }

    printf("Storage Server is listening for client connections...\n");
    // Accept and handle incoming connections
    while (1)
    {
        int client_socket;
        // Accept a connection
        printf("l216\n");

        client_socket = accept(listen_socket, NULL, NULL);

        printf("l218\n");
        if (client_socket < 0)
        {
            perror("Accept failed");
            close(listen_socket);
            continue;
        }
        // Handle the client connection (you may want to create a separate thread or process for this)
        printf("l225 enterring handle_client_connection()\n");
        pthread_t handle_client_connection_thread;
        pthread_create(&handle_client_connection_thread, NULL, handle_client_connection, (void *)&client_socket);
        numcl++;
        // handle_client_connection((void*)&client_socket);
        printf("l227\n");

        // if (close(client_socket) == -1)
        // {
        //     perror("close");

        // }
    }

    // Close the listen socket (this part is unreachable in this example)
    if (close(listen_socket) == -1)
    {
        perror("close");
    }

    return NULL;
}
// Function to create an empty file or directory in the current working directory
int create_empty_file_or_directory(const char *name, int is_directory)
{
    // Get the current working directory
    char current_directory[256];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
        perror("getcwd() error");
        return -1;
    }

    // Concatenate the file/directory name to the current working directory
    char full_path[512];
    char *name2 = strdup(name);
    char *name3 = no_prefix(name2);
    snprintf(full_path, sizeof(full_path), "%s/%s", current_directory, name3);

    printf("full_path = %s\n", full_path);

    // Create an empty file or directory
    if (is_directory)
    {
        if (mkdir(full_path, 0777) == -1)
        {
            perror("mkdir() error");
            return PERMISSION_DENIED;
        }
    }
    else
    {
        // Use open instead of fopen
        int file_descriptor = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        // int file_descriptor = open(full_path, O_WRONLY | O_CREAT | O_TRUNC ,);

        if (file_descriptor == -1)
        {
            perror("open() error");
            return PERMISSION_DENIED;
        }

        // Use close instead of fclose
        if (close(file_descriptor) == -1)
        {
            perror("close() error");
            return -1;
        }
    }

    printf("Created %s '%s' successfully in '%s'.\n", (is_directory ? "directory" : "file"), name, current_directory);
    return OK;
}

// Function to delete a file or directory in the current working directory
int delete_file_or_directory(const char *name, int is_directory)
{
    // Get the current working directory
    char current_directory[256];
    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
        perror("getcwd() error");
        return -1;
    }

    // Concatenate the file/directory name to the current working directory
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_directory, name);

    // Delete the file or directory
    if (is_directory)
    {
        // if (rmdir(full_path) == -1)
        // {
        //     perror("rmdir() error");
        //     return PERMISSION_DENIED;
        // }
        // Build the command and arguments
        char *const args[] = {"rm", "-rf", full_path, NULL};

        int ret = fork();
        if (ret == 0)
        {
            // Execute the command using execvp
            if (execvp("rm", args) == -1)
            {
                perror("execvp");
            }
        }

        // This code will only be reached if execvp fails
        printf("Error: execvp failed to execute the command.\n");
    }
    else
    {
        if (remove(full_path) == -1)
        {
            perror("remove() error");
            return PERMISSION_DENIED;
        }
    }

    printf("Deleted %s '%s' successfully from '%s'.\n", (is_directory ? "directory" : "file"), name, current_directory);
}

int copy_files(int socket, const char *source_path, const char *destination_path)
{
    char buffer[2 * BUFFER_SIZE];
    int bytes_recieved = recv(socket, buffer, BUFFER_SIZE, 0);
    buffer[bytes_recieved] = '\0';

    // printf("l820\n");
    if (strcmp(buffer, "INIT.RECEIVE") == 0)
    {
        receive_file_contents(socket, destination_path);
        printf("COPIED_FILE\n");
    }
    if (strcmp(buffer, "INIT.SEND") == 0)
    {
        send_file_contents(socket, source_path);
        char buffer[BUFFER_SIZE];
        strcpy(buffer, "$%^");
        send(socket, buffer, BUFFER_SIZE, 0);

        strcpy(buffer, "TRANSFER_DONE");
        send(socket, buffer, BUFFER_SIZE, 0);
    }
    // printf("l838\n");
    return OK;
}

int copy_directories(int socket, const char *source_path, const char *destination_path)
{
    char buffer[2 * BUFFER_SIZE];
    int bytes_recieved = recv(socket, buffer, BUFFER_SIZE, 0);
    buffer[bytes_recieved] = '\0';

    // printf("l820\n");
    if (strcmp(buffer, "INIT.RECEIVE") == 0)
    {
        char use_path[BUFFER_SIZE];
        strcpy(use_path, destination_path);
        receive_directory_names_dir(socket, use_path);
        strcpy(use_path, destination_path);
        printf("Directories received successfully.\n");
        strcpy(use_path, destination_path);
        // strcpy(root,".");
        receive_directory_names_text(socket, use_path);
        printf("COPIED_FOLDER\n");
    }
    if (strcmp(buffer, "INIT.SEND") == 0)
    {
        seek_recursive_send_dir_2(socket, source_path, ".");
        printf("Directory names sent successfully.\n");
        send_endtransmission(socket, "END_TRANSMISSION", ".");
        seek_recursive_send_text(socket, source_path, ".");
        send_endtransmission(socket, "END_TRANSMISSION", ".");
        printf("Files sent successfully.\n");
        strcpy(buffer, "TRANSFER_DONE");
        send(socket, buffer, BUFFER_SIZE, 0);
    }
    return OK;
}

// Function to copy files or directories between Storage Servers
int copy_files_or_directories(int socket, const char *source_path, const char *destination_path, int is_directory)
{
    printf("l847\n");
    // printf("%")
    if (is_directory)
    {
        return copy_directories(socket, source_path, destination_path);
    }
    else
    {
        return copy_files(socket, source_path, destination_path);
    }
    return OK;
}

// Function to read a file
// CAVALE
int read_file(int socket, const char *file_name)
{
    printf("l350\n");
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open");
        return -1;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        close(file_fd);
        return -1;
    }

    // Return to the beginning of the file
    if (lseek(file_fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        close(file_fd);
        return -1;
    }

    char buffer[2 * BUFFER_SIZE];
    ssize_t bytes_read;
    int i = 0;

    while (file_size > 0)
    {
        size_t remaining_bytes = file_size > BUFFER_SIZE ? BUFFER_SIZE : file_size;
        i++;
        bytes_read = read(file_fd, buffer, remaining_bytes);
        if (bytes_read <= 0)
        {
            if (bytes_read == -1)
            {
                perror("read");
            }
            break;
        }
        if (file_size < BUFFER_SIZE)
        {
            buffer[bytes_read] = '\0';
        }
        ssize_t bytes_sent = send(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_sent == -1)
        {
            perror("send file contents");
            break;
        }
        printf("l400\n");
        file_size -= bytes_read;
        usleep(5000);
    }
    printf("404\n");
    close(file_fd);
    char ending_symbol[BUFFER_SIZE];
    strcpy(ending_symbol, "$%^");
    printf("408\n");
    if (send(socket, ending_symbol, BUFFER_SIZE, 0) == -1)
    {
        perror("l418 send");
    }
    printf("410\n");
    printf("HOGAYA\n");
    return 0;
}

void send_file_contents(int socket, const char *file_path)
{
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open");
        return;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        close(file_fd);
        return;
    }

    // Return to the beginning of the file
    if (lseek(file_fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        close(file_fd);
        return;
    }

    char buffer[2 * BUFFER_SIZE];
    ssize_t bytes_read;
    int i = 0;

    while (file_size > 0)
    {
        size_t remaining_bytes = file_size > BUFFER_SIZE ? BUFFER_SIZE : file_size;
        i++;
        bytes_read = read(file_fd, buffer, remaining_bytes);
        if (bytes_read <= 0)
        {
            if (bytes_read == -1)
            {
                perror("read");
            }
            break;
        }
        if (file_size < BUFFER_SIZE)
        {
            buffer[bytes_read] = '\0';
        }
        ssize_t bytes_sent = send(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_sent == -1)
        {
            perror("send file contents");
            break;
        }

        file_size -= bytes_read;
        usleep(5000);
    }

    close(file_fd);
}

// Function to write to a file
int write_file(const char *file_name, const char *data)
{
    // Open the file for writing (create if it doesn't exist, overwrite if it does)
    FILE *file = fopen(file_name, "w");

    // Check if the file was opened successfully
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // Write data to the file
    if (fputs(data, file) == EOF)
    {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    // Close the file
    fclose(file);
    return 0;
}

// Function to get size and permissions
int get_size_and_permissions(int socket, const char *file_name)
{
    // Use stat function to retrieve file information
    struct stat file_stat;
    if (stat(file_name, &file_stat) == -1)
    {
        perror("Error getting file information");
        return -1;
    }
    // STRUCT TO BE RETURNED
    struct FileInfo file_info;

    // Copy file_name to the structure
    snprintf(file_info.file_name, sizeof(file_info.file_name), "%s", file_name);

    // Set size in the structure
    file_info.size = (long long)file_stat.st_size;

    // Set permissions in the structure
    snprintf(file_info.permissions, sizeof(file_info.permissions), "%c%c%c%c%c%c%c%c%c",
             (file_stat.st_mode & S_IRUSR) ? 'r' : '-',
             (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
             (file_stat.st_mode & S_IXUSR) ? 'x' : '-',
             (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
             (file_stat.st_mode & S_IWGRP) ? 'w' : '-',
             (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
             (file_stat.st_mode & S_IROTH) ? 'r' : '-',
             (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
             (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

    ss_send(socket, (void *)&file_info, 3);
    return 0; // Return 0 to indicate success
}

int main(int argc, char *argv[])
{
    ip_address = strdup("127.0.1.1");
    if (argc != 3)
    {
        printf("Usage: %s <server port> <client port>\n", argv[0]);
        exit(0);
    }
    // server_connection_port = atoi(argv[1]);
    ss_info.nm_port = atoi(argv[1]);
    ss_info.client_port = atoi(argv[2]);

    // Create thread for listening on nm_port
    pthread_t nm_listen_thread;
    pthread_create(&nm_listen_thread, NULL, nm_listen_thread_function, NULL);
    // Establish connection with naming server
    /// BEFORE RUNNING THIS FUNCTION, ADD SIGNAL IN THE PREVIOUS THREAD FUNCTION
    // ENSURE THAT ss_info.ip_address IS OBTAINED BEFORE RUNNING THE NEXT FUNCTION
    int sock_fd = establish_connection_with_naming_server();

    client_listen_thread_function();
    pthread_join(nm_listen_thread, NULL);
    return 0;
}

void ss_send(int sock, void *message, int data_type)
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
    case (2):
        message_size = sizeof(Server_Init_Info);
        break;
    case (3):
        message_size = sizeof(FileInfo);
    }

    // Send the path to the server
    if (send(sock, message, message_size, 0) == -1)
    {
        perror("send failed");
        return;
    }
}

void *ss_receive(int socket, int data_type)
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
            return NULL;
        }
        break;

    case 1: // File_Request data type
        // Allocate memory for a File_Request structure
        receiving_data_structure = malloc(sizeof(File_Request));
        // Receive data from the server
        if (recv(socket, receiving_data_structure, sizeof(File_Request), 0) == -1)
        {
            perror("recv failed");
            return NULL;
        }
        break;
    }
    return receiving_data_structure;
}