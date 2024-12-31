#include "seek.h"
#include "standard.h"

extern bool d, f, e, isdir;
extern char e_path[max_path];

void searchf(char* direc, char* filename, char* startdirec, int* seekcnt) {
    DIR* dir;
    struct dirent* entry;
    char path[max_path];

    dir = opendir(direc);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", direc, entry->d_name);

        if (entry->d_type == DT_DIR) {
            if (d && strcmp(entry->d_name, filename) == 0) {
                *seekcnt = *seekcnt + 1;
                strcpy(e_path, path);
                isdir = true;
                char new_path[max_path];
                snprintf(new_path, sizeof(new_path), "%s/%s", direc, entry->d_name);
                size_t startdirec_len = strlen(startdirec);
                size_t path_len = strlen(new_path);
                if (path_len >= startdirec_len) {
                    memmove(new_path, new_path + startdirec_len, path_len - startdirec_len + 1);
                }
                printf(blue "%s\n" reset, new_path);
            }
            searchf(path, filename, startdirec, seekcnt);
        }

        if (entry->d_type == DT_REG) {
            char* file_name = strtok(entry->d_name, ".");
            if (f && strcmp(file_name, filename) == 0) {
                *seekcnt = *seekcnt + 1;
                strcpy(e_path, path);
                size_t startdirec_len = strlen(startdirec);
                size_t path_len = strlen(path);
                if (path_len >= startdirec_len) {
                    memmove(path, path + startdirec_len, path_len - startdirec_len + 1);
                }
                printf(green "%s\n" reset, path);
            }
        }
    }

    if (*seekcnt > 1) {
        e = false;
    }

    closedir(dir);
}