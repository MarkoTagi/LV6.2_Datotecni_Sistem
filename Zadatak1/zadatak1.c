#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

int containsSubstring(const char*, const char*);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    DIR* directoryPointer = opendir(argv[1]);
    if (directoryPointer == NULL) {
        perror("Could not get the directory pointer!\nReason");
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry;
    errno = 0;
    printf("Entries containing %s:\n", argv[2]);
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        if (containsSubstring(directoryEntry->d_name, argv[2]) == 1) printf("\t%s\n", directoryEntry->d_name);
    }
    if (errno != 0) {
        perror("Could not read the directory!\nReason");
        exit(EXIT_FAILURE);
    }
    closedir(directoryPointer);
    return 0;
}

int containsSubstring(const char* text, const char* sample) {
    int textLength = strlen(text); int sampleLength = strlen(sample);
    int currentPosition = 0;
    while (text[currentPosition] != '\0') {
        int samplePosition = 1;
        if ((textLength - currentPosition) < sampleLength) return 0;
        if (text[currentPosition] == sample[0]) {
            while (text[++currentPosition] == sample[samplePosition++]) ;
            if (sample[samplePosition - 1] == '\0') return 1;
        }
        currentPosition += samplePosition;
    }
    return 0;
}