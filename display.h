#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct syst_info {
    char username[1024];
    char system[1024];
} syst_info;

syst_info* start();

char *updatepath(char *oldpath);

#endif 