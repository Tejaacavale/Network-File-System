#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8080
#define BUFFER_SIZE 4096
// #define int long long
int ends_with_txt(const char *str) {
    size_t len = strlen(str);
    const char *suffix = ".txt";
    size_t suffix_len = strlen(suffix);

    if (len >= suffix_len && strcmp(str + len - suffix_len, suffix) == 0) {
        return 1; // The string ends with ".txt"
    }

    return 0; // The string does not end with ".txt"
}
void send_directory_name(int socket, const char *dir_name, const char *current_path) {
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    // char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // send(socket,start,BUFFER_SIZE,0);
    send(socket, full_path, BUFFER_SIZE, 0);
}
void send_directory_name_txt(int socket, const char *dir_name, const char *current_path) {
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // printf("%s\n",full_path);
    // send(socket,start,BUFFER_SIZE,0);
    send(socket, full_path, BUFFER_SIZE, 0);
}
void send_endtransmission(int socket, const char *dir_name, const char *current_path) {
    char full_path[2*BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", current_path, dir_name);
    // printf("%s\n",full_path);
    // char start[BUFFER_SIZE];
    // strcpy(start,"START");
    // send(socket,start,BUFFER_SIZE,0);
    send(socket, full_path, BUFFER_SIZE, 0);
}
void seek_recursive_send_dir(int socket, const char *path, const char *current_path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            // Send directory name to server
            send_directory_name(socket, entry->d_name, current_path);

            // Recursive call to send contents of subdirectory
            char new_current_path[2 * BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", path, entry->d_name);
            seek_recursive_send_dir(socket, full_path, new_current_path);
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

void send_file_contents(int socket, const char *file_path) {
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd == -1) {
        perror("open");
        return;
    }

    off_t file_size = lseek(file_fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("lseek");
        close(file_fd);
        return;
    }

    // Return to the beginning of the file
    if (lseek(file_fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(file_fd);
        return;
    }

    char buffer[2*BUFFER_SIZE];
    ssize_t bytes_read;
    int i = 0 ;

    while (file_size > 0) {
        size_t remaining_bytes = file_size > BUFFER_SIZE ? BUFFER_SIZE : file_size;
        i++;
        bytes_read = read(file_fd, buffer, remaining_bytes);
        if (bytes_read <= 0) {
            if (bytes_read == -1) {
                perror("read");
            }
            break;
        }
        if(file_size < BUFFER_SIZE){
            buffer[bytes_read]='\0';
        }
        ssize_t bytes_sent = send(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_sent == -1) {
            perror("send file contents");
            break;
        }
        

        file_size -= bytes_read;
    }

    close(file_fd);
}


      
        
void seek_recursive_send_text(int socket, const char *path, const char *current_path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue; // Skip hidden files/directories
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            // send_directory_name(socket, entry->d_name, current_path);
            char new_current_path[BUFFER_SIZE];
            snprintf(new_current_path, sizeof(new_current_path), "%s/%s", current_path, entry->d_name);
            seek_recursive_send_text(socket, full_path, new_current_path);
        } else if (S_ISREG(file_stat.st_mode)) {
            if(ends_with_txt(full_path)){
            send_directory_name_txt(socket, entry->d_name, current_path);
            send_file_contents(socket, full_path);
            char ending_symbol[BUFFER_SIZE];
            strcpy(ending_symbol,"$%^");
            send(socket, ending_symbol, BUFFER_SIZE, 0);
            }
        }
    }

    closedir(dir);
}
int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Replace SERVER_IP_ADDRESS with the actual IP address of the server

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Start sending directory names from a specific path (e.g., current directory)
    char starting_directory[4096] = "./client"; // Replace this with the desired starting directory
    seek_recursive_send_dir(client_socket, starting_directory, ".");

    // Inform server that all directory names have been sent (you can define your own termination signal)
    send_endtransmission(client_socket, "END_TRANSMISSION", ".");

    printf("Directory names sent successfully.\n");
    seek_recursive_send_text(client_socket,starting_directory,".");
    send_endtransmission(client_socket, "END_TRANSMISSION", ".");
    printf("Files sent successfully.\n");
    close(client_socket);

    return 0;
}
