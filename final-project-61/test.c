#include <stdio.h>

void extractFileName(const char *path) {
    char fileName[100]; // Assuming maximum file name length of 99 characters
    int pos = -1;

    // Find the last occurrence of '/' in the path
    sscanf(path, "%*[^/]%n", &pos);

    if (pos >= 0 && path[pos + 1] != '\0') {
        // Copy the file name after the last '/'
        sscanf(path + pos + 1, "%99[^.]", fileName);
        printf("File name without extension: %s\n", fileName);
    } else {
        printf("Invalid path format or file name not found.\n");
    }
}

int main() {
    char filePath[100]; // Assuming maximum file path length of 99 characters

    printf("Enter file path: ");
    scanf("%99s", filePath); // Read the file path from user input

    extractFileName(filePath);

    return 0;
}
