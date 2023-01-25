#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#define MAX_LENGTH 1024

int containsString(const char*, const char*);
int findPattern(const char* filePath, const char* pattern, int indentCount);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int filesFound = findPattern(argv[1], argv[2], 0);
    if (errno != 0) {
        perror("Failed to find the files!\nReason");
        exit(EXIT_FAILURE);
    }
    printf("Files containing pattern [%s]: %d.\n", argv[2], filesFound);
    return 0;
}

int containsString(const char* text, const char* pattern) {
    int textLength = strlen(text); int patternLength = strlen(pattern);
    int currentPosition = 0;
    while (text[currentPosition] != '\0' && (textLength - currentPosition >= patternLength)) {
        int patternPosition = 0;
        if (text[currentPosition] == pattern[patternPosition]) {
            while (text[++currentPosition] == pattern[++patternPosition]) ;
            if (pattern[patternPosition] == '\0') return 1;
        }
        currentPosition += patternPosition + 1;
    }
    return 0;
}

int findPattern(const char* filePath, const char* pattern, int indentCount) {
    int patternCount = 0;
    DIR* directoryPointer = opendir(filePath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "[%s] ", filePath); fflush(stderr);
        perror("Failed to open directory!\nReason");
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    errno = 0;
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        int notSelfOrParent = (strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0);
        if (notSelfOrParent) {
            for (int i = 0; i < indentCount; ++i) printf("\t");
            if (indentCount > 0) printf("\\->");
        }
        if (containsString(directoryEntry->d_name, pattern)) {
            ++patternCount;
            printf("%s [*]\n", directoryEntry->d_name);
        }
        else if (notSelfOrParent) printf("%s\n", directoryEntry->d_name);
        char newFilePath[MAX_LENGTH]; strcpy(newFilePath, filePath);
        strcat(newFilePath, "/"); strcat(newFilePath, directoryEntry->d_name);
        struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
        if (stat(newFilePath, fileStats) == -1) {
            fprintf(stderr, "[%s] ", newFilePath); fflush(stderr);
            perror("Failed to get file statistics!\nReason");
            exit(EXIT_FAILURE);
        }
        if (notSelfOrParent && S_ISDIR(fileStats->st_mode)) {
            patternCount += findPattern(newFilePath, pattern, ++indentCount);
            --indentCount;
        }
        free(fileStats);
    }
    free(directoryEntry);
    return patternCount;
}