#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include "api.h"
#include "cmd_parser.h"
#include "util.h"


#define HOST "127.0.0.1"
#define PORT 4444
#define JAVA_CALL "java -classpath /home/losha/IdeaProjects/spo-parser/src/main/java/lib/gson-2.8.7.jar:/home/losha/IdeaProjects/spo-parser/src/main/java/classes CustomParser"

#define BUFFER_SIZE 1024

typedef enum {
    CREATE,
    READ,
    UPDATE,
    DELETE
} COMMAND_TYPE;

struct Api {
    COMMAND_TYPE command;
    char* params;
    char* path;
    char* condition;
    char* sign;
    char* right;
};

typedef struct Api Api;

struct Api parse_json_str_to_api_struct(char* in) {
    json_object *jobj = parse_str_to_json_obj(in);
    COMMAND_TYPE command;
    char *params;
    char *path;
    char *condition;
    char *sign;
    char *right;
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
        if (strcmp(key, "param") == 0)
            params = json_object_get_string(val);
        if (strcmp(key, "path") == 0)
            path = json_object_get_string(val);
        if (strcmp(key, "cond") == 0)
            condition = json_object_get_string(val);
        if (strcmp(key, "sign") == 0)
            sign = json_object_get_string(val);
        if (strcmp(key, "right") == 0)
            right = json_object_get_string(val);

    }
    struct Api api_struct = {command, params, path, condition, sign, right};
    return api_struct;
}

struct Api parse_output(char* cmd) {
    char buf[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    FILE *fp;
    struct Api api;
    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
    }
    while(fgets(buf, BUFFER_SIZE, fp) != NULL) {
        if (index_of(buf, "{") != -1) {
            strcpy(output, buf);
            api = parse_json_str_to_api_struct(output);
        }
    }

    if (pclose(fp)) {
        printf("Command not found or exited with error code\n");
    }

    return api;
}

int main() {

    init_db();
    const char *res = api_read(".","2","3","4","5");
    printf("%s\n", res);

    Api api = parse_output(JAVA_CALL);
    printf("%s\n", api.path);

    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    char buffer[BUFFER_SIZE];
    const char* response;

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
                    char* params;
                    char* path;
                    char* condition;
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
                           response = api_create(api_struct.path, api_struct.params, "", api_struct.condition, "");
                            send(newSocket, response, strlen(response), 0);
                            bzero(&response, strlen(response));
                            break;
                        }
                        case READ: {
                            response = api_read(api_struct.path, api_struct.params, api_struct.condition, "", "");
                            send(newSocket, response, strlen(response), 0);
                            bzero(&response, strlen(response));
                            break;
                        }
                        case UPDATE: {
                            response = api_update(api_struct.path, api_struct.params, api_struct.condition, "", "");
                            send(newSocket, response, strlen(response), 0);
                            bzero(&response, strlen(response));
                            break;
                        }
                        case DELETE: {
                            response = api_delete(api_struct.path, api_struct.params, "", "", "");
                            send(newSocket, response, strlen(response), 0);
                            bzero(&response, strlen(response));
                            break;
                        }
                        default: {
                            response = "{\"error\": \"unexpected_command\"}";
                            send(newSocket, response, strlen(response), 0);
                            bzero(&response, strlen(response));
                            break;
                        }
                    }
                }
            }
        }
    }
    close(newSocket);
    return 0;
}

