#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void runFiles(const char* directoryPath);

int main() {
    char directoryPath[PATH_MAX];
    printf("Enter directory path: ");
    fgets(directoryPath, PATH_MAX, stdin);
    int directoryPathLength = strlen(directoryPath);
    directoryPath[--directoryPathLength] = '\0';
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    if (errno != 0) {
        fprintf(stderr, "Failed to read directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    runFiles(directoryPath);
    closedir(directoryPointer);
    return 0;
}

void runFiles(const char* directoryPath) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror("");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Failed to get file statistics [%s]\n\tReason: ", directoryEntry->d_name); fflush(stderr);
            perror("");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) runFiles(newPath);
        if (fileStats->st_mode & S_IXUSR) {
            printf("Trying to run [%s]...\n", newPath);
            int pid = fork();
            if (pid == -1) {
                perror("Failed to fork process!\n\tReason");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                if (execlp("sh", "sh", newPath, NULL) == -1) {
                    fprintf(stderr, "Failed to execute file [%s]!\n\tReason: ", newPath); fflush(stderr);
                    perror("");
                    exit(EXIT_FAILURE);
                }
            }
            else wait(NULL);
        }
    }
    closedir(directoryPointer);
    free(directoryEntry);
    free(fileStats);
}