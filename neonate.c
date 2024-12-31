#include "neonate.h"
#include "standard.h"
#include <termios.h>

#define MAX_INPUT_SIZE 1000

void neonate(int wtime) {   
    struct termios orig_termios;    
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw_termios = orig_termios;
    raw_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);

    while(1) {
        FILE *file = fopen("/proc/loadavg", "r");
        if (file == NULL) {
            perror("Failed to open /proc/loadavg file");
            return;
        }

        float loadavg[5];
        char waste[5];
        int process = 0;
        
       char buffer[MAX_INPUT_SIZE];
        if (fgets(buffer, MAX_INPUT_SIZE, file) == NULL) {
            perror("Failed to read loadavg values");
            fclose(file);
            return;
        }

        int count = sscanf(buffer, "%f %f %f %s %d", &loadavg[0], &loadavg[1], &loadavg[2], waste, &process);
        if (count != 5) {
            perror("Failed to parse loadavg values");
            fclose(file);
            return;
        }

        printf("%d\n", process);
        
        fclose(file);

        // printf("HERE\n");
        // printf("%d\n", wtime);

        time_t start_time;
        time(&start_time);
        time_t current_time;
        time(&current_time);
        int flag = 0;
        while (difftime(current_time, start_time) < wtime) {
            time(&current_time);
            struct timeval tv = {0L, 0L};
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            int result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
            if (result > 0) {
                char ch;
                read(STDIN_FILENO, &ch, sizeof(ch));
                if (ch == 'x') {
                    flag = 1;
                    break;
                }
            }
        }
        
        if (flag) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
            return;
        }
    }

}