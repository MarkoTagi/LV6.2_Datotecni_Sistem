#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#define MAX_LENGTH 256

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char directoryPath[MAX_LENGTH];
    strcpy(directoryPath, argv[1]);
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        perror("Failed to get directory pointer!\nReason");
        exit(EXIT_FAILURE);
    }
    struct stat* fileStatistics = (struct stat*)malloc(sizeof(struct stat));
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    int subdirectoryCount = 0, fileCount = 0, linkCount = 0;
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        if (strcmp(directoryPath, "/") != 0) strcat(directoryPath, "/");
        strcat(directoryPath, directoryEntry->d_name);
        if (stat(directoryPath, fileStatistics) == -1) {
            printf("[%s] ", directoryEntry->d_name); fflush(stdout);
            perror("Failed to gather file statistics!\n\tReason");
        }
        // else printf("File path: [%s]\n", directoryPath);
        if (S_ISDIR(fileStatistics->st_mode)) ++subdirectoryCount;
        if (S_ISREG(fileStatistics->st_mode)) ++fileCount;
        if (S_ISLNK(fileStatistics->st_mode)) ++linkCount;
        strcpy(directoryPath, argv[1]);
    }
    printf("Directory statistics:\n");
    printf("\tNumber of subdirectories: %d\n", subdirectoryCount);
    printf("\tNumber of files: %d\n", fileCount);
    printf("\tNumber of links: %d\n", linkCount);
    free(directoryEntry);
    free(fileStatistics);
    closedir(directoryPointer);
    return 0;
}