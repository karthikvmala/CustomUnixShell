#include "standard.h"
#include "iman.h"

#define PORT 80
#define BUFFER_SIZE 15000

void iman(char* command_name) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[1024];
    char response[BUFFER_SIZE];
    int bytes_received;
    char *response_body;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname("man.he.net");
    if (server == NULL) {
        fprintf(stderr, "Error: no such host\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    snprintf(request, sizeof(request),
        "GET /?topic=%s&section=all HTTP/1.1\r\n"
        "Host: man.he.net\r\n"
        "Connection: close\r\n\r\n", command_name);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    bzero(response, sizeof(response));
    bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    close(sockfd);

    response[bytes_received] = '\0'; 
    char *header_end = strstr(response, "\n\n");
    if (header_end == NULL) {
        fprintf(stderr, "Error: No header end found\n");
        exit(EXIT_FAILURE);
    }

    response_body = header_end + 2;

    printf("\n%s\n", response_body);
}