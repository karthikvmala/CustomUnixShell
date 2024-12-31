#include "standard.h"
#include "signal.h"
#include "display.h"
#include "hop.h"
#include "history.h"
#include "reveal.h"
#include "seek.h"
#include "changestate.h"
#include "activity.h"
#include "neonate.h"
#include "iman.h"
#include <strings.h>

int currentbackgroundprocess = 0;
backgroundProcess* bg = NULL;
int currentpid = -2;
int seekcnt = 0;
bool d = false;
bool f = false;
bool e = false, isdir = false;
char e_path[max_path];
char command[max_path];
char prevdir[max_path];
char shrcfile[max_path];
int execution_time = 0;
bool ok = true; 
int maincnt = 0;

char prevcwdirec[max_path];

void execute_command(char* input, char* cwdirec, char* startpath, char* history) {
    ok = true;
    int n = 0, m = 0, o = 0, pi = 0, fd = 0;
    char* buffer1[max_path];
    char* buffer2[max_path];
    char* buffer3[max_path];
    char* pipebuffer[max_path];
    char currentpath[max_path];
    tokenize(buffer1, &n, input, ";"); 
    
    for (int i = 0; i < n; i++) {
        if (!strcmp(buffer1[i], "log")) {
            ok = false;
        }
    }

    for(int i = 0; i < n; i++) {
        set_word(buffer1[i]);
        bool background = false;
        if (buffer1[i][strlen(buffer1[i]) - 1] == '&') {
            background = true;
        }

        bool pipepresent = false;
        for (int j = 0; j < strlen(buffer1[i]); j++) {
            if (buffer1[i][j] == '|') {
                pipepresent = true;
            }
        }

        char temp[max_path];
        strcpy(temp, buffer1[i]);
        if (pipepresent) {
            tokenize(pipebuffer, &pi, temp, "|");
            int fd = dup(1);
            int fd1 = dup(0);
            int pipe1[2];
            int pipe2[2];

            if (pipe(pipe1) == -1) {
                perror(red "pipe error" reset);
            }
            dup2(pipe1[1], 1);
            execute_command(pipebuffer[0], cwdirec, startpath, history);
            close(pipe1[1]);

            for (int iter = 1; iter < pi - 1; iter++) {
                pipe(pipe2);

                dup2(pipe1[0], 0);
                dup2(pipe2[1], 1);
                execute_command(pipebuffer[iter], cwdirec, startpath, history);
                close(pipe1[0]);

                pipe1[0] = pipe2[0];
                close(pipe2[1]);
            }
            
            dup2(pipe1[0], 0);
            dup2(fd, STDOUT_FILENO);
            execute_command(pipebuffer[pi - 1], cwdirec, startpath, history);
            dup2(fd1, STDIN_FILENO);
            close(pipe1[0]);
        }

        if (pipepresent) {
            continue;
        }

        int size = 0, final_size = 0, cnt = 0, tempsize = 0;
        int mp[2] = {0, 0};
        char temp_string[max_path];
        char finalcommand[max_path];
        char* temp_buffer[max_path];
        char* final_output[max_path];
        strcpy(temp_string, buffer1[i]);
        tokenize(temp_buffer, &size, temp_string, " ");
        strcpy(finalcommand, buffer1[i]);

        bool append = false;
        for (int j = 0; j < size; j++) {
            if (strcmp(temp_buffer[j], "<") == 0) {
                mp[0] = ++cnt;
            } else if (strcmp(temp_buffer[j], ">>") == 0) {
                mp[1] = ++cnt;
                append = true;
            } else if (strcmp(temp_buffer[j], ">") == 0) {
                mp[1] = ++cnt;
            }
        }
        if (cnt == 2) {
            for(int k = 0; k < 2; k++) {
                if (mp[k] == 2) {
                    if (k == 1) {
                        if (append) {
                            freopen(temp_buffer[size - 1], "a+", stdout);
                        } else {
                            freopen(temp_buffer[size - 1], "w", stdout);
                        }

                        char* temp_buffer2[max_path];
                        char* temp_buffer3[max_path];
                        tokenize(temp_buffer2, &final_size, buffer1[i], ">");
                        tokenize(temp_buffer3, &final_size, temp_buffer2[0], "<");
                        freopen(temp_buffer3[final_size - 1], "r", stdin);
                        strcpy(finalcommand, temp_buffer3[0]);
                    } else {
                        freopen(temp_buffer[size - 1], "r", stdin);

                        char* temp_buffer2[max_path];
                        char* temp_buffer3[max_path];
                        tokenize(temp_buffer2, &final_size, buffer1[i], ">");
                        tokenize(temp_buffer3, &final_size, temp_buffer2[0], ">");

                        if (append) {
                            freopen(final_output[final_size - 1], "a+", stdout);
                        } else {
                            freopen(final_output[final_size - 1], "w", stdout);
                        }
                        strcpy(finalcommand, temp_buffer3[0]);
                    }
                } 
            }
        } else if (cnt == 1) {
            for (int k = 0; k < 2; k++) {
                if (mp[k] == 1) {
                    if (k == 1) {
                        if (append) {
                            freopen(temp_buffer[size - 1], "a+", stdout);
                        } else {
                            freopen(temp_buffer[size - 1], "w", stdout);
                        }
                        char* temp_buffer2[max_path];
                        tokenize(temp_buffer2, &tempsize, buffer1[i], ">");
                        strcpy(finalcommand, temp_buffer2[0]);
                    } else {
                        printf("%s\n", temp_buffer[size - 1]);
                        if (freopen(temp_buffer[size - 1], "r", stdin) == NULL) {
                            perror(red "Failed to redirect stdin" reset);
                            exit(EXIT_FAILURE);
                        }
                        char* temp_buffer2[max_path];
                        tokenize(temp_buffer2, &tempsize, buffer1[i], "<");
                        strcpy(finalcommand, temp_buffer2[0]);
                    }
                } 
            }
        }

        tokenize(buffer2, &m, finalcommand, "&"); // Tokenize based on ampersands
        for (int s = 0; s < m; s++) { 
            char full_command[max_path];
            strcpy(full_command, buffer2[s]);
            tokenize(buffer3, &o, buffer2[s], " "); // Tokenize based on spaces
            if (s != m - 1 || background == true) {   // Handle background processes
                buffer3[o + 1] = NULL;
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    perror(red "pipe error" reset);
                }
                pid_t pid = fork();
                if (pid == -1) {
                    perror( red "fork" reset);
                    ok = false;
                } else if (pid == 0) {
                    setpgrp();
                    int flag = execvp(buffer3[0], buffer3);
                    if (flag == -1) {
                        printf(red "Error executing %s as a system command!\n" reset, full_command);
                        exit(1);
                    }
                } else {
                    printf("Process %s with pid %d started\n", full_command, pid);
                    signal(SIGCHLD, sighandler);
                    bg[currentbackgroundprocess].pidofprocess = pid;
                    strcpy(bg[currentbackgroundprocess].name, full_command);
                    currentbackgroundprocess++;
                }
            } 
        }

        
        FILE* myshrc = fopen(shrcfile, "r");
        if (myshrc == NULL) {
            perror(red "Failed to open file.myshrc" reset);
        } else {
            char line[max_path];
            while (fgets(line, sizeof(line), myshrc)) {
                char* left = strtok(line, "=");
                char* right = strtok(NULL, "=");
                set_word(left);
                set_word(right);
                if (left != NULL && right != NULL && strcmp(left, buffer3[0]) == 0) {
                    strcpy(buffer3[0], right);
                    break;
                }
            }
        }
        fclose(myshrc);
        
        if (!strcmp(buffer3[0], "activities")) {
            findactivites();
        } else if (!strcmp(buffer3[0], "hop")) {
            if (o == 1) {
                jump_to(startpath, startpath);
                printf("%s\n", getcwd(currentpath, sizeof(currentpath)));
            }
            for (int i = 1; i < o; i++) {
                if (buffer3[i][0] == '-') {
                    char temp[max_path];
                    getcwd(temp, sizeof(temp));
                    jump_to(prevcwdirec, startpath);
                    strcpy(prevcwdirec, temp);
                } else {
                    getcwd(prevcwdirec, sizeof(prevcwdirec));
                    jump_to(buffer3[i], startpath);
                }
                printf("%s\n", getcwd(currentpath, sizeof(currentpath)));
            }
        }
        else if (!strcmp(buffer3[0], "neonate")) {
            int time = atoi(buffer3[2]);
            neonate(time);
        }
        else if (!strcmp(buffer3[0], "reveal")) {
            char temp1[max_path];
            getcwd(temp1, sizeof(temp1));
            bool l = false;
            bool a = false;
            bool abspath = false;
            bool relpath = false;
            bool reverse = false;
            if (o != 1 && buffer3[o - 1][0] == '/') {
                abspath = true;
            } else if (o != 1 && buffer3[o - 1][0] != '-') {
                relpath = true;
            } else if (buffer3[o - 1][0] == '-' && strlen(buffer3[o - 1]) == 1) {
                reverse = true;
            }

            for (int j = 1; j < ((abspath == true || relpath == true || reverse == true) ? o - 1 : o); j++) {
                for (int k = 0; k < strlen(buffer3[j]); k++) {
                    if (buffer3[j][k] == '-') {
                        continue;
                    }
                    else {
                        if (buffer3[j][k] == 'l') {
                            l = true;
                        } else if (buffer3[j][k] == 'a') {
                            a = true;
                        }
                    }
                }
            }
            char temp[max_path];
            strcpy(temp, temp1);

            if (abspath) {
                strcpy(temp, buffer3[o - 1]);
            } else if (relpath) {
                if (buffer3[o - 1][0] != '~') {
                    strcat(temp, "/");
                    strcpy(temp, add_string(temp, buffer3[o - 1], 0));
                } else {
                    strcpy(temp, add_string(startpath, buffer3[o - 1], 1));
                }
            } else if (reverse) {
                strcpy(temp, prevcwdirec);
            }

            DIR* dir = opendir(temp);
            if (dir == NULL) {
                perror(red "Failed to open directory" reset);
                ok = false;
            }
            struct stat file_stat;
            struct dirent** entries;
            int num_entries = scandir(temp, &entries, NULL, mysort);
            if (num_entries == -1) {
                perror( red "scandir" reset);
                ok = false;
            }
            if (l) {
                long sum = 0;
                for (int j = 0; j < num_entries; j++) {
                struct dirent* entry1 = entries[j];
                struct stat file_stat1;
                char file_path[5000];
                snprintf(file_path, sizeof(file_path), "%s/%s", temp, entry1->d_name);
                if (stat(file_path, &file_stat1) == -1) {
                    perror(red "stat" reset);
                    ok = false;
                }
                if (!a && entry1->d_name[0] == '.') {
                    continue;
                }
                sum += file_stat1.st_blocks;
                }
                printf("Total Blocks: %ld\n", sum / 2);
            }

            for (int i = 0; i < num_entries; i++) {
                struct dirent* entry = entries[i];
                struct stat file_stat;
                char file_path[5000];
                snprintf(file_path, sizeof(file_path), "%s/%s", temp, entry->d_name);
                if (stat(file_path, &file_stat) == -1) {
                    perror(red "stat" reset);
                    ok = false;
                }
                if (!a && entry->d_name[0] == '.') {
                    continue;
                } else if (l) {
                    char file_path[5000];
                    snprintf(file_path, sizeof(file_path), "%s/%s", temp, entry->d_name);
                    if (stat(file_path, &file_stat) == -1) {
                        perror( red "stat" reset);
                        ok = false;
                    }
                    
                    print_permissions(file_stat.st_mode);
                    printf(" %3lu ", (unsigned long)file_stat.st_nlink);
                    printf("%-7s ", getpwuid(file_stat.st_uid)->pw_name);
                    printf("%-7s ", getpwuid(file_stat.st_gid)->pw_name);
                    printf("%8lu ", (unsigned long)file_stat.st_size);


                    time_t modified_time = file_stat.st_mtime;
                    struct tm *timeinfo = localtime(&modified_time);
                    char time_buffer[80];
                    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M", timeinfo);
                    printf("%s ", time_buffer);
                }
                if (S_ISDIR(file_stat.st_mode)) {
                    printf(blue);
                } else if (file_stat.st_mode & S_IXUSR) {
                    printf(green); 
                } else if (S_ISREG(file_stat.st_mode)) {
                    printf(white);
                }
                printf("%-20s\n", entry->d_name);
                printf(reset); 
                free(entry);
            }
            free(entries);
            closedir(dir);
        }

        else if (!strcmp(buffer3[0], "bg")) {
            int process = atoi(buffer3[1]);
            background_continue(process);
        }

        else if (!strcmp(buffer3[0], "fg")) {
            int process = atoi(buffer3[1]);
            background_forground(process);
        }
        else if (!strcmp(buffer3[0], "log")) {
            if (o == 1) {
                read_history(history);
                continue;
            } else if (o == 2 && !strcmp(buffer3[1], "purge")) {
                FILE* history_file = fopen(history, "w");
                if (history_file == NULL) {
                    perror( red "Failed to open history file" reset);
                    ok = false;
                }
                fclose(history_file);
                continue;
            } else if (o == 3 && !strcmp(buffer3[1], "execute")) {
                int value = atoi(buffer3[2]);
                char* result = (char*)malloc(max_path * sizeof(char));
                FILE* history_file = fopen(history, "r");
                if (history_file == NULL) {
                    perror( red "Failed to open history file" reset);
                    ok = false;
                }

                strcpy(result, "");
                char line[max_path];
                int line_number = 1;
                while (fgets(line, sizeof(line), history_file)) {
                    if (line_number == value) {
                        strcpy(result, line);
                    }
                    line_number++;
                }

                if (strlen(result) == 0) {
                    perror(red "Out of range" reset);
                }

                fclose(history_file);
                getcwd(currentpath, sizeof(currentpath));
                execute_command(result, currentpath, startpath, history);
                continue;
            } else {
                perror(red "Invalid log command\n" reset);
            }
        }
        else if (!strcmp(buffer3[0], "ping")) {
            if (o != 3) {
                printf(red "invalid number of commands!\n" reset);
                return;
            } else {
                int to_ping = atoi(buffer3[1]);
                int signal =atoi(buffer3[2]);
                send_signal(to_ping, signal, 0);
            }
        }

        else if (!strcmp(buffer3[0], "proclore")) {
            int pid;
            if (o == 1) {
                pid = getpid();
            } else {
                pid = atoi(buffer3[1]);
            }

            char path[max_path];
            FILE *file;
            char line[max_path];
            
            snprintf(path, sizeof(path), "/proc/%d/status", pid);
            file = fopen(path, "r");
            if (file == NULL) {
                perror( red "fopen" reset);
                ok = false;
            }

            char comm[256] = {0};  
            char state[256] = {0}; 
            unsigned long vm_size = 0; 
            int pgrp = 0; 
            
            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, "State:", 6) == 0) {
                    sscanf(line, "State:\t%c", state);
                } else if (strncmp(line, "VmSize:", 7) == 0) {
                    sscanf(line, "VmSize:\t%lu kB", &vm_size);
                } else if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line, "Name:\t%255s", comm);
                }
            }
            fclose(file);

            printf(magenta "PID:" reset " %d\n", pid);
            printf(magenta "Process Status:" reset " %s\n", state);
            
            pgrp = getpgid(pid);
            printf(magenta "Process Group:" reset " %d\n", pgrp);
            
            printf(magenta "Virtual Memory:" reset " %lu kB\n", vm_size);
            
            snprintf(path, sizeof(path), "/proc/%d/exe", pid);
            char exe_path[256];
            ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
            if (len != -1) {
                exe_path[len] = '\0';
                printf(magenta "Executable Path: " reset "%s\n", exe_path);
            } else {
                perror(red "readlink" reset);
                ok = false;
            }

        }
        else if (!strcmp(buffer3[0], "iMan")) {
            iman(buffer3[1]);
        }

        else if (!strcmp(buffer3[0], "seek")) {
            char temp1[max_path];
            getcwd(temp1, sizeof(temp1));
            seekcnt = 0;
            isdir = false;
            d = false;
            f = false;
            bool path = false;
            if (o != 1 && buffer3[o - 1][0] == '.') {
                path = true;
            }
            for (int j = 1; j < (path == true ? o - 1 : o); j++) {
                for (int k = 0; k < strlen(buffer3[j]); k++) {
                    if (buffer3[j][0] != '-') {
                        continue;
                    }
                    else {
                        if (buffer3[j][k] == 'd') {
                            d = true;
                        } else if (buffer3[j][k] == 'f') {
                            f = true;
                        } else if (buffer3[j][k] == 'e') {
                            e = true;
                        }
                    }
                }
            }
            if (d && f) {
                printf(red "Invalid Flags\n" reset);
                continue;
            }
            if (!d && !f) {
                d = true;
                f = true;
            }
            char temp[max_path];
            strcpy(temp, temp1);
            if (path) {
                strcpy(temp, add_string(temp, buffer3[o - 1], 1));
            }
            searchf(temp, buffer3[o - (path ? 2 : 1)], temp, &seekcnt);
            if (seekcnt == 0) {
                printf(yellow "No files/dictionaires found\n" reset);
            } else if (e && isdir) {
                chdir(e_path);
            } else if (e && !isdir) {
                FILE* file = fopen(e_path, "r");
                if (file == NULL) {
                    perror(red "Failed to open file" reset);
                    ok = false;
                } else {
                    char line[max_path];
                    while (fgets(line, sizeof(line), file)) {
                        printf("%s", line);
                    }
                    fclose(file);
                }
            }
        }

        else if (!background && !pipepresent) {
            strcpy(command, buffer3[0]);
            pid_t pid = fork();
            if (pid == -1) {
                perror(red "fork" reset);
                ok = false;
            } else if (pid == 0) {
                buffer3[o + 1] = NULL;
                signal(SIGINT, SIG_DFL);
                if (execvp(buffer3[0], buffer3) == -1) {
                    perror(red "execvp" reset);
                    printf(red "%s is not a valid command\n" reset, command);
                    ok = false;
                }
            } else {
                currentpid = pid;
                time_t start_time = time(NULL);
                waitpid(pid, NULL, WUNTRACED);
                time_t end_time = time(NULL);
                execution_time = (int)difftime(end_time, start_time);
            }
        }
    }
}

char current_fg[max_path];

void zsignal() {
    if (currentpid != -2) {
        bg[currentbackgroundprocess].pidofprocess = currentpid;
        strtok(current_fg, "\n");
        strcpy(bg[currentbackgroundprocess].name, current_fg);
        currentbackgroundprocess++;

        setpgid(currentpid, currentpid);
        kill(currentpid, SIGTSTP);
        printf("Process with PID %d stopped.\n", currentpid);
        currentpid = -2;
    }
}

int main() {
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, zsignal);
    syst_info* t = start();
    char startpath[max_path];
    char currentpath[max_path];
    char prevpath[max_path];
    char input[max_path];
    getcwd(startpath, sizeof(startpath));
    getcwd(prevpath, sizeof(prevpath));
    
    strcpy(shrcfile, startpath);
    strcat(shrcfile, "/file.myshrc");
    char history[max_path];
    strcpy(history, startpath);
    strcat(history, "/historyfile.txt");
    FILE* history_file = fopen(history, "a");
    if (history_file == NULL) {
        perror(red "Failed to create history file" reset);
        ok = false;
    }
    fclose(history_file);

    bg = (backgroundProcess*)malloc(sizeof(backgroundProcess) * 500);

    while(1) {
        printf(green "<%s@%s:%s>" reset, t->username, t->system, updatepath(startpath));
        if (execution_time > 2) {
            printf(blue "%s: %d seconds " reset, command, execution_time);
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            fflush(stdout);
            dsignal();
        }

        strcpy(current_fg, input);
    
        if (feof(stdin)) { 
            break;
        }

        getcwd(prevpath, sizeof(prevpath));

        char* mainbuffer[max_path];
        char maintemp[max_path];
        strcpy(maintemp, input);
        tokenize(mainbuffer, &maincnt, maintemp, " \t");

        for (int i = 0; i < maincnt; i++) {
            if (strcmp(mainbuffer[i], "log") == 0) {
                ok = false;
            }
        }

        if (ok) {
            FILE* history_file = fopen(history, "a");
            if (history_file == NULL) {
                perror(red "Failed to open history file" reset);
                ok = false;
            }

            char first_line[max_path];
            history_file = fopen(history, "r");
            if (history_file == NULL) {
                perror(red"Failed to open history file" reset);
                ok = false;
            }
            fgets(first_line, sizeof(first_line), history_file);
            fclose(history_file);

            if (strcmp(first_line, input) != 0) {
                write_history(startpath, input);
            }
        }
        
        execute_command(input, prevpath, startpath, history);
        freopen("/dev/tty", "w", stdout);
        freopen("/dev/tty", "r", stdin);
    }
}