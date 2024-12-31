#include "standard.h"
#include "signal.h"
#include "activity.h"

void send_signal(int process, int signal, int flag) {
    pid_t pid = (pid_t)process;
    if (kill(pid, signal)) {
        perror(red "No such process found\n" reset);
    } else {
        if (!flag) {
            printf(orange "Sent signal %d to process with pid %d" reset "\n", signal, pid);    
        } else {
            printf(magenta "Process with pid %d continued" reset "\n", pid);
        }  
        // if (signal == 9) {
        //     for (int iter = 0; iter < currentbackgroundprocess; iter++) {
        //         if (bg[iter].pidofprocess == process) {
        //                if (bg[iter].pidofprocess == pid) {                }
        //         }
        //         currentbackgroundprocess--;
        //     }
        // }
    }
}

void sighandler() {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        for (int iter = 0; iter < currentbackgroundprocess; iter++) {
            if (bg[iter].pidofprocess == pid) {
                if (WIFEXITED(status)) {
                    if (status == 0) {
                        printf(yellow "Process %s with pid %d exited normally \n" reset, bg[iter].name, pid);
                    } else {
                        printf(red "Process %s with pid %d exited abnormally \n" reset, bg[iter].name, pid);
                    }
                } else if (WIFSIGNALED(status)) {
                    printf(red "Process %s with pid %d exited abnormally \n" reset, bg[iter].name, pid);
                }
                // for (int j = iter; j < currentbackgroundprocess - 1; j++) {
                //     bg[j] = bg[j + 1];
                // }
                // currentbackgroundprocess--;
            }
        }
    }
}

void dsignal() {
    for (int i = 0; i < currentbackgroundprocess; i++) {
        if(check_status(bg[i].pidofprocess) != 2) {
            kill(bg[i].pidofprocess, SIGKILL);
        }
    }
    exit(0);
}
