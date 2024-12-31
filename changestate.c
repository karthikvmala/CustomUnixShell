#include "standard.h"
#include "changestate.h"
#include "signal.h"
#include "activity.h"

void background_forground(int pid) {
    bool found = false;
    int present = -1;
    for (int i = 0; i < currentbackgroundprocess; i++) {
        if (bg[i].pidofprocess == pid || check_status(bg[i].pidofprocess) != 2) {
            found = true;
            present = i;
            break;
        }
    }
    if (!found) {
        printf(red "Invalid command: No such process in background.\n" reset);
        return;
    }

    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    int p_grouppid = getpgid(pid);
    if (p_grouppid == -1) {
        perror("Error getting process group ID");
        return;
    }
    int curpid = getpgid(0);

    if (tcsetpgrp(0, p_grouppid) == -1) {
        perror("Error moving process to foreground");
        return;
    }
    
    printf("Process with pid %d continuing in foreground.\n", pid);

    if (kill(pid, SIGCONT) == -1) {
        perror("Error executing process in foreground");
        return;
    }

    int status;
    int result = waitpid(pid, &status, WUNTRACED);
    if (result == -1) {
        perror("Error waiting for process");
        return;
    }

    signal(SIGTTOU, SIG_IGN);
    if (tcsetpgrp(0, curpid) == -1) {
        perror("Error passing control back to shell");
        return;
    }

    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
}

void background_continue(int pid) {
    pid_t temp = (pid_t)pid;
    send_signal(temp, 18, 1);
}
