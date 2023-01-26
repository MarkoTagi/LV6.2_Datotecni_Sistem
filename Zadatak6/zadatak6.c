#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Cannot open directory [%s].\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    errno = 0;
    int largestFileSize = 0;
    char largestFileName[PATH_MAX];
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Cannot get file statistics [%s].\n\tReason", newPath); fflush(stderr);
            perror("");
            exit(EXIT_FAILURE);
        }
        if (S_ISREG(fileStats->st_mode)) {
            if (fileStats->st_size > largestFileSize) {
                largestFileSize = fileStats->st_size;
                strcpy(largestFileName, directoryEntry->d_name);
            }    
        }
    }
    if (errno != 0) {
        fprintf(stderr, "Error! Could not read the directory [%s].\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    printf("The largest file in the directory [%s] was [%s, %dB]\n", directoryPath, largestFileName, largestFileSize);
    closedir(directoryPointer);
    free(fileStats);
    free(directoryEntry);
}