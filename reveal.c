#include "reveal.h"
#include "standard.h"

void print_permissions(mode_t mode) {
    char type = '?';
    if (S_ISREG(mode)) type = '-';
    else if (S_ISDIR(mode)) type = 'd';
    else if (S_ISLNK(mode)) type = 'l';

    printf("%c", type); 
    printf("%c", (mode & S_IRUSR) ? 'r' : '-'); 
    printf("%c", (mode & S_IWUSR) ? 'w' : '-'); 
    printf("%c", (mode & S_IXUSR) ? 'x' : '-'); 
    printf("%c", (mode & S_IRGRP) ? 'r' : '-'); 
    printf("%c", (mode & S_IWGRP) ? 'w' : '-'); 
    printf("%c", (mode & S_IXGRP) ? 'x' : '-'); 
    printf("%c", (mode & S_IROTH) ? 'r' : '-'); 
    printf("%c", (mode & S_IWOTH) ? 'w' : '-'); 
    printf("%c", (mode & S_IXOTH) ? 'x' : '-'); 
}

int mysort(const struct dirent** a, const struct dirent** b) {
    char first_char_a = (*a)->d_name[0];
    char first_char_b = (*b)->d_name[0];
    if (isupper(first_char_a)) {
        first_char_a = tolower(first_char_a);
    }
    if (isupper(first_char_b)) {
        first_char_b = tolower(first_char_b);
    }
    if (first_char_a < first_char_b) {
        return -1;
    } else if (first_char_a > first_char_b) {
        return 1;
    } else {
        return 0;
    }
}
