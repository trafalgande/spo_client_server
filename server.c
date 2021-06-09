#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cmd_parser.h"
#include <json-c/json.h>

#define HOST "127.0.0.1"
#define PORT 4444

#define BUFFER_SIZE 1024
#define RESPONSE_SIZE 1024

typedef enum {
    CREATE,
    READ,
    UPDATE,
    DELETE
} COMMAND_TYPE;

struct Api {
    COMMAND_TYPE command;
    const char* params;
    const char* path;
    const char* condition;
};

typedef struct Api Api;

int main() {

    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    char buffer[BUFFER_SIZE];
    char response[RESPONSE_SIZE];

    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("[-] Error in connection.\n");
        exit(1);
    }
    printf("[+] Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(HOST);

    ret = bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("[-] Error in binding.\n");
        exit(1);
    }
    printf("[+] Bind to port %d\n", PORT);

    if (listen(sockfd, 10) == 0) {
        printf("[+] Listening....\n");
    } else {
        printf("[-] Error in binding.\n");
    }

    while (1) {
        newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);
        if (newSocket < 0) {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0) {
            close(sockfd);

            while (1) {
                bzero(buffer, BUFFER_SIZE);
                recv(newSocket, buffer, BUFFER_SIZE, 0);
                if (strcmp(buffer, ":exit") == 0) {
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    break;
                }

                if (strcmp(buffer, "\n") == 0) {
                    strcpy(buffer, "Wrong behavior");
                    printf("<<: %s\n", buffer);
                    send(newSocket, buffer, strlen(buffer), 0);
                }

                printf("<<: %s\n", buffer);
                json_object *jobj = parse_str_to_json_obj(buffer);
                if (jobj) {
                    COMMAND_TYPE command;
                    const char* params;
                    const char* path;
                    const char* condition;
                    json_object_object_foreach(jobj, key, val) {
                        if (strcmp(key, "cmd") == 0) {
                            if (strcmp(json_object_get_string(val), "create") == 0)
                                command = CREATE;
                            if (strcmp(json_object_get_string(val), "read") == 0)
                                command = READ;
                            if (strcmp(json_object_get_string(val), "update") == 0)
                                command = UPDATE;
                            if (strcmp(json_object_get_string(val), "delete") == 0)
                                command = DELETE;
                        }
                        if (strcmp(key, "params") == 0)
                            params = json_object_get_string(val);
                        if (strcmp(key, "path") == 0)
                            path = json_object_get_string(val);
                        if (strcmp(key, "cond") == 0)
                            condition = json_object_get_string(val);
                    }
                    struct Api api_struct = {command, params, path, condition};

                    switch (command) {
                        case CREATE: {
                            //api_create call
                            break;
                        }
                        case READ: {
                            //api_read call
                            break;
                        }
                        case UPDATE: {
                            //api_update call
                            break;
                        }
                        case DELETE: {
                            //api_delete call
                            break;
                        }
                        default: {
                            //ignore
                            break;
                        }
                    }
                }
                send(newSocket, buffer, strlen(buffer), 0);
            }
        }
    }
    close(newSocket);
    return 0;
}