#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#define MAX_DEPTH 3

void removeFiles(const char* filePath, size_t greaterThan, int currentDepth);

int main() {
    char directoryPath[PATH_MAX];
    printf("Enter directory path: ");
    fgets(directoryPath, PATH_MAX, stdin);
    int directoryPathLength = strlen(directoryPath);
    directoryPath[--directoryPathLength] = '\0';
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Cannot open directory [%s]\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    removeFiles(directoryPath, 10, 0);
    if (errno != 0) {
        fprintf(stderr, "Failed to read directory [%s]\n\tReason", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    closedir(directoryPointer);
    return 0;
}

void removeFiles(const char* filePath, size_t greaterThan, int currentDepth) {
    if (currentDepth > MAX_DEPTH) {
        printf("Maximum depth ( %d )reached. Returning...\n", MAX_DEPTH);
        return;
    }
    DIR* directoryPointer = opendir(filePath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Cannot open directory [%s]\n\tReason", filePath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    errno = 0;
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        char newPath[PATH_MAX]; strcpy(newPath, filePath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Cannot get file statistics [%s].\n\tReason)", newPath); fflush(stderr);
            perror("");
            exit(EXIT_FAILURE);
        }
        bool notSelfOrParent = (strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0);
        if (S_ISDIR(fileStats->st_mode) && notSelfOrParent) removeFiles(newPath, greaterThan, currentDepth + 1);
        if (S_ISREG(fileStats->st_mode) && fileStats->st_size > (greaterThan * 1024)) {
            printf("Removing [%s]...\n\tReason: It's size ( %ldB ) is greater than %ldkB.", directoryEntry->d_name, fileStats->st_size, greaterThan);
            int pid = fork();
            if (pid == -1) {
                perror("Failed to fork process!\n\tReason");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                if (execlp("rm", "rm", newPath, NULL) == -1) {
                    fprintf(stderr, "Failed to remove file [%s].\n\tReason", directoryEntry->d_name);
                    perror("");
                    exit(EXIT_FAILURE);
                }
            }
            else wait(NULL);
        }
    }
    closedir(directoryPointer);
    free(directoryEntry);
}