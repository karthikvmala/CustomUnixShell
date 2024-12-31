#include "standard.h"
#include "history.h"

extern bool ok;

void read_history(char* path) {
    FILE* hisfile;
    char* hispath = (char*)malloc(max_path* sizeof(char));
    strcpy(hispath, path);
    hisfile = fopen(hispath, "r");
    char buffer[max_path];
    int cnt = 0;
    char lines[his_no][max_path];
    while (fgets(buffer, max_path, hisfile) && cnt < his_no) {
        strcpy(lines[cnt], buffer);
        cnt++;
    }
    fclose(hisfile);
    for (int i = cnt - 1; i >= 0; i--) {
        printf("%s", lines[i]);
    }
}

void write_history(char* path, char* data) {
    FILE* hisfile1;
    FILE* hisfile2;
    char* hispath1 = (char*)malloc(max_path* sizeof(char));
    char* hispath2 = (char*)malloc(max_path* sizeof(char));
    strcpy(hispath1, path);
    strcpy(hispath2, path);
    strcat(hispath1, "/historyfile.txt");
    strcat(hispath2, "/historyfile2.txt");
    hisfile1 = fopen(hispath1, "r");
    hisfile2 = fopen(hispath2, "w+"); 
    if (hisfile1 == NULL || hisfile2 == NULL) {
        printf("Error opening files\n");
        ok = false;
        return;
    }
    char buffer[max_path];
    int cnt = 0;
    fputs(data, hisfile2);
    while (fgets(buffer, max_path, hisfile1) && cnt < his_no - 1) {
        fputs(buffer, hisfile2);
        cnt++;
    }
    fclose(hisfile1);
    fclose(hisfile2);
    remove(hispath1);
    rename(hispath2, hispath1);
}


