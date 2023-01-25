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

char directoryPath[MAX_LENGTH];

int containsString(const char*, const char*);
void findFiles(DIR* directoryPointer, struct dirent* directoryEntry, char* pattern, int tabCount);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    strcpy(directoryPath, argv[1]);
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        perror("Could not get directory pointer!\nReason");
        exit(EXIT_FAILURE);
    }
    findFiles(directoryPointer, readdir(directoryPointer), argv[2], 0);
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

void findFiles(DIR* directoryPointer, struct dirent* directoryEntry, char* pattern, int tabCount) {
    errno = 0;
    if (directoryEntry == NULL) return;
    for (int i = 0; i < tabCount; ++i) printf("-");
    if (containsString(directoryEntry->d_name, pattern)) printf("%s [*]\n", directoryEntry->d_name);
    else if (strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..") != 0) printf("%s\n", directoryEntry->d_name);
    // sleep(2);
    int currentTabCount = tabCount;
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    char tempDirectoryPath[MAX_LENGTH];
    strcpy(tempDirectoryPath, directoryPath);
    strcat(directoryPath, "/"); strcat(directoryPath, directoryEntry->d_name);
    if (stat(directoryPath, fileStats) == -1) perror("Could not get file statistics!\nReason");
    if (S_ISDIR(fileStats->st_mode) && strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..") != 0) {
        DIR* subdirectoryPointer = opendir(directoryPath);
        findFiles(subdirectoryPointer, readdir(subdirectoryPointer), pattern, ++tabCount);
    }
    strcpy(directoryPath, tempDirectoryPath);
    findFiles(directoryPointer, readdir(directoryPointer), pattern, currentTabCount);
    free(fileStats);
}
