#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#define PORT 8080
#define BUFFER_SIZE 4096
int ends_with_txt(const char *str) {
    size_t len = strlen(str);
    const char *suffix = ".txt";
    size_t suffix_len = strlen(suffix);

    if (len >= suffix_len && strcmp(str + len - suffix_len, suffix) == 0) {
        return 1; // The string ends with ".txt"
    }

    return 0; // The string does not end with ".txt"
}
void receive_directory_names_dir(int socket) {
    char received_dir_name[2*BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(socket, received_dir_name, BUFFER_SIZE, 0);

        if (bytes_received <= 0) {
            perror("Failed to receive directory name");
            break;
        }

        received_dir_name[bytes_received] = '\0';

        if (strcmp(received_dir_name, "./END_TRANSMISSION") == 0) {
            // Termination signal received from client
            break;
        }

        // Handle the received directory name (you can perform your operations here)
        // printf("Received Directory: %s\n", received_dir_name);
        if (mkdir(received_dir_name, 0777) == -1) {
            if (errno == EEXIST) {
                // Handle case where directory already exists
                printf("Directory already exists: %s\n", received_dir_name);
                // You can choose to skip or handle this situation differently
            } else {
                // perror("mkdir");
                // Handle other errors if necessary
            }
        }
        
    }
}
// void receive_directory_names_text(int socket) {
//     char received_dir_name[2*BUFFER_SIZE];
//     int bytes_received;
//     while (1) {
        
//         bytes_received = recv(socket, received_dir_name, BUFFER_SIZE, 0);
//         // printf("%s\n",received_dir_name);
//         if (bytes_received <= 0) {
//             perror("Failed to receive directory name");
//             break;
//         }

//         received_dir_name[bytes_received] = '\0';

//         if (strcmp(received_dir_name, "./END_TRANSMISSION") == 0) {
//             // Termination signal received from client
//             break;
//         }
//         // printf("%s\n",received_dir_name);
//         // Handle the received directory name (you can perform your operations here)
//         // printf("Received Directory: %s\n", received_dir_name);
//         // if (mkdir(received_dir_name, 0777) == -1) {
//         //     if (errno == EEXIST) {
//         //         // Handle case where directory already exists
//         //         printf("Directory already exists: %s\n", received_dir_name);
//         //         // You can choose to skip or handle this situation differently
//         //     } else {
//         //         // perror("mkdir");
//         //         // Handle other errors if necessary
//         //     }
//         // }
        
//     }
// }
// void receive_file_contents(int socket, const char *file_path) {
//     FILE *file = fopen(file_path, "w");
//     if (file == NULL) {
//         perror("fopen");
//         return;
//     }

//     char buffer[BUFFER_SIZE];
//     int bytes_received;
//     while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
//         printf("%s",buffer);
//         if(strcmp(buffer,"$%^")){//TO SYMBOLISE THE END.
//             break;
//         }
//         fwrite(buffer, 1, bytes_received, file);
//     }

//     fclose(file);
// }
void receive_file_contents(int socket, const char *file_path) {
    int file_fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd == -1) {
        perror("open");
        return;
    }

    char buffer[2*BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {

        if(strcmp(buffer,"$%^")==0){
            break;
        }
        buffer[bytes_received]='\0';
        printf("%lu\n",strlen(buffer));
        if (write(file_fd, buffer, strlen(buffer)) != strlen(buffer)) {
            perror("write");
            break;
        }
    }
    close(file_fd);
}
void receive_directory_names_text(int socket) {
    char received_dir_name[2 * BUFFER_SIZE];
    int bytes_received;
    while (1) {
        bytes_received = recv(socket, received_dir_name, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            perror("Failed to receive file name");
            break;
        }

        received_dir_name[bytes_received] = '\0';
        printf("%s\n",received_dir_name);
        if (strcmp(received_dir_name, "./END_TRANSMISSION") == 0) {
            // Termination signal received from client
            break;
        }
        received_dir_name[bytes_received] = '\0';
        receive_file_contents(socket, received_dir_name);
        printf("Received file: %s\n", received_dir_name);
        
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

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(server_addr);

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for incoming connections...\n");

    // Accept incoming connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from client!\n");

    // Receive directory names from client
    receive_directory_names_dir(client_socket);
    printf("Directory names received successfully.\n");
    receive_directory_names_text(client_socket);
    printf("Textfiles names received successfully.\n");

    close(client_socket);
    close(server_socket);

    return 0;
}
