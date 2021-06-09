#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <json-c/json.h>

const char *create_json_from_args(const char *command, const char *params,
                                  const char *path, const char *condition) {
    struct json_object *jobj;
    jobj = json_object_new_object();
    json_object_object_add(jobj, "cmd", json_object_new_string(command));
    json_object_object_add(jobj, "params", json_object_new_string(params));
    json_object_object_add(jobj, "path", json_object_new_string(path));
    json_object_object_add(jobj, "cond", json_object_new_string(condition));

    return json_object_to_json_string(jobj);
}

const char *parse_cmd_to_json(char *cmd) {
    int i;
    int count = 0;
    bool flag = false;
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ')
            count++;
        if (cmd[i] == '[')
            flag = true;
    }


    if (count == 2) {
        char *cmd_token = strtok(cmd, " ");
        char *param_token = strtok(NULL, " ");
        char *path_token = strtok(NULL, " ");
        if (flag) {
            strtok(path_token, "[");
            char *cond_token = strtok(NULL, "[");
            cond_token[strcspn(cond_token, "]")] = 0;
            return create_json_from_args(cmd_token, param_token, path_token, cond_token);
        } else {
            return create_json_from_args(cmd_token, param_token, path_token, "");
        }
    } else {
        return NULL;
    }
}

json_object* parse_str_to_json_obj(const char* in) {
    return json_tokener_parse(in);
}