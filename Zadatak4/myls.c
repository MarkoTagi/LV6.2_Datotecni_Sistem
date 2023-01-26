#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

void getDirectorySize(const char* directoryPath, int* directorySize, int* fileCount);

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char* directoryPath = argv[1];
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    errno = 0;
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Could not get file statistics[%s].\n\tReason", newPath); fflush(stderr);
            perror("");
            exit(EXIT_FAILURE);
        }
        int notSelfOrParent = (strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0);
        if (S_ISDIR(fileStats->st_mode) && notSelfOrParent) {
            printf("Directory: [%s]", directoryEntry->d_name);
            int fileCount = 0, directorySize = 0;
            getDirectorySize(newPath, &directorySize, &fileCount);
            printf(", %dB ( %d files ).\n", directorySize, fileCount);
        }
    }
    free(fileStats);
    free(directoryEntry);
    return 0;
}

void getDirectorySize(const char* directoryPath, int* directorySize, int* fileCount) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Could not get file statistics[%s].\n\tReason", newPath); fflush(stderr);
            perror("");
            exit(EXIT_FAILURE);
        }
        int notSelfOrParent = (strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0);
        if (S_ISDIR(fileStats->st_mode) && notSelfOrParent) getDirectorySize(newPath, directorySize, fileCount);
        if (S_ISREG(fileStats->st_mode)) ++(*fileCount);
        if (notSelfOrParent) *directorySize += fileStats->st_size;
    }
    free(fileStats);
    free(directoryEntry);
}