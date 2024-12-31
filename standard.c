#include "standard.h"

int min(int a, int b) {
    return a < b ? a : b;
}

char* add_string(char* b, char* a, int offset) {
    memmove(a, a + offset, strlen(a));
    int newStartLength = strlen(b) + strlen(a) + 1; 
    char* newStart = (char*)malloc(newStartLength * sizeof(char)); 
    strcpy(newStart, b); 
    strcat(newStart, a); 
    newStart[newStartLength - 1] = '\0'; 
    return newStart;
}

void set_word(char* word) {
    int start = 0, end = strlen(word);
    while(word[end - 1] == '\n' || word[end- 1] == ' ') {
        end--;
    }
    while (word[start] == '\n' || word[start] == ' ') {
        start++;
    }
    word[end] = '\0';
    memmove(word, word + start, strlen(word));
}

void tokenize(char** buffer, int* n, char* input, const char* delim) {
    char* token = strtok(input, delim);
    int start = 0;
    while(token != NULL) {
        buffer[start] = (char*)malloc(strlen(token) + 1);
        strcpy(buffer[start], token);
        set_word(buffer[start]);
        start++;
        token = strtok(NULL, delim);
    }
    buffer[start] = NULL;
    *n = start;
}
