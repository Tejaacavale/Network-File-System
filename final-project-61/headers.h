#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "errorcodes.h"
#include "Caching.h"

#define NM_SS_PORT 8080
#define NM_CL_PORT 8081
#define NM_SERVER_IP "127.0.0.1"
#define MAX_SS 100
#define MAX_CL 100
#define BUFFER 5
#define BUFFER_SIZE 4096
// Structure to represent a file request
typedef struct
{
    char operation[20];         // e.g., "READ", "WRITE", "CREATE", "DELETE", "COPY"
    char path[256];             // File path
    char destination_path[256]; // USED ONLY FOR COPY
    int is_directory;           // FLAG USED ONLY FOR CREATING FILE/DIRECTORY
} File_Request;

// Structure to represent server information (IP address and port)
typedef struct
{
    char ip_address[16];
    int port;
} Server_Info;

typedef struct Server_Info Client_Info;

typedef char **List; // Tries or Array
typedef struct
{
    char ip_address[16];
    int nm_port;
    int client_port;
    List Paths;

    // FOR REDUNDANCY
    int is_backed_up;

} Server_Init_Info;

// USED FOR GET_DETAILS
// Structure to store file information
typedef struct FileInfo FileInfo;
struct FileInfo
{
    char file_name[256];
    long long size;
    char permissions[10]; // Assuming RWX format for simplicity
};

#define STRING 0
#define FILE_REQUEST 1
#define SERVER_INIT_INFO 2
#define SERVER_INFO 3

#endif