#include "standard.h"
#include "activity.h"

int sortactivites(const void* x, const void* y) {
    const backgroundProcess *process1 = (backgroundProcess *)x;
    const backgroundProcess *process2 = (const backgroundProcess *)y;
    return strcmp(process1->name, process2->name);
}

int check_status(int pid) {
    char path[max_path];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return 2;
    }

    char line[max_path];
    char state;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%c", &state);
        }
    }

    if (state == 'T') {
        return 1;
    } else {
        return 0;
    }
}

void findactivites() {
    qsort(bg, currentbackgroundprocess, sizeof(backgroundProcess), sortactivites);
    for (int iter = 0; iter < currentbackgroundprocess; iter++) {
        char cur_status[50];

        if (check_status(bg[iter].pidofprocess) == 1) {
            strcpy(cur_status, "Stopped");
        } else if (check_status(bg[iter].pidofprocess) == 0) {
            strcpy(cur_status, "Running");
        } else if (check_status(bg[iter].pidofprocess) == 2) {
            continue;
        }

        printf("%d : %s - %s\n", bg[iter].pidofprocess, bg[iter].name, cur_status);
    }
}   