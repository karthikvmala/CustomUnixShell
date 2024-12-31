#include "display.h"
#include "standard.h"

char curpath[max_path];
char prevpath[max_path];

syst_info* start() {
    syst_info* new_info = (syst_info*)malloc(sizeof(syst_info));
    struct passwd *pw = getpwuid(getuid());
    const char *username = pw->pw_name;
    char host[max_path];
    int gethostname_return = gethostname(host, sizeof(host));

    if (gethostname_return == -1) {
        perror(red "gethostname" reset);
        exit(1);
    }

    if (username != NULL) {
        strncpy(new_info->username, username, sizeof(new_info->username) - 1);
        new_info->username[sizeof(new_info->username) - 1] = '\0';  
    }

    strncpy(new_info->system, host, sizeof(new_info->system) - 1);
    new_info->system[sizeof(new_info->system) - 1] = '\0';
    
    return new_info;
}

char *updatepath(char *oldpath){
    char* newpath = (char*)malloc(max_path * sizeof(char));
    char rel[max_path] = "~";
    getcwd(curpath, sizeof(curpath));

    if (strlen(curpath) < strlen(oldpath)) {
        return curpath;
    } else {
        strcpy(newpath, curpath + strlen(oldpath));
        return strcat(rel, newpath);
    }
}

