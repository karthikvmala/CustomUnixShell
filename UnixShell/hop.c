#include "hop.h"
#include "standard.h"

extern bool ok;

void jump_to(char* file, char* start) {
    char* prev = (char*)malloc(max_path * sizeof(char));
    getcwd(prev, sizeof(prev));
    if (file == NULL) {
        if (chdir(start) == -1) {
            perror(red "Error in hop" reset);
            ok = false;
        }
    } else if (file[0] == '~') {
        char* newStart = (char*)malloc(max_path * sizeof(char));
        strcpy(newStart, add_string(start, file, 1));
        // printf("HERE %s\n", newStart);
        if (chdir(newStart) == -1) {
            perror(red "Error in hop" reset);
            ok = false;
        }
        free(newStart); // Free the allocated memory
    } else if (chdir(file) == -1) {
        perror(red "Error in hop" reset);
        ok = false;
    }
}