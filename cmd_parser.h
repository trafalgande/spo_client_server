#ifndef TEST_CMD_PARSER_H
#define TEST_CMD_PARSER_H

#include <json-c/json_object.h>

const char* create_json_from_args(const char *command, const char *path, const char *condition);
const char* parse_cmd_to_json(char* cmd);
json_object* parse_str_to_json_obj(const char* in);
#endif //TEST_CMD_PARSER_H
