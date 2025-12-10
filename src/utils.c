#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "json.h"

int utils_get_ip_family(const char* ip_str)
{
    if (strchr(ip_str, ':') != nullptr) {
        struct in6_addr addr6;
        return inet_pton(AF_INET6, ip_str, &addr6) == 1 ? 6 : -1;
    }

    struct in_addr addr4;
    return inet_pton(AF_INET, ip_str, &addr4) == 1 ? 4 : -1;
}

const char* utils_get_list_name(const struct cJSON* input, int family)
{
    const cJSON* extra_args = json_get_extra_args(input);

    // Will return 0 if extra_args is NULL
    int size = cJSON_GetArraySize(extra_args);
    if (size != 2) {
        return nullptr;
    }

    cJSON* value = nullptr;
    if (family == 4) {
        value = cJSON_GetArrayItem(extra_args, 0);
    }
    else if (family == 6) {
        value = cJSON_GetArrayItem(extra_args, 1);
    }

    // Will return NULL if value is NULL or is not a string
    return cJSON_GetStringValue(value);
}

enum ar_command utils_parse_command(const char* command)
{
    if (strcmp(command, "add") == 0) {
        return AR_COMMAND_ADD;
    }

    if (strcmp(command, "delete") == 0) {
        return AR_COMMAND_DELETE;
    }

    if (strcmp(command, "continue") == 0) {
        return AR_COMMAND_CONTINUE;
    }

    if (strcmp(command, "abort") == 0) {
        return AR_COMMAND_ABORT;
    }

    return AR_COMMAND_INVALID;
}

char* utils_strdup(const char* str)
{
    if (str == nullptr) {
        return nullptr;
    }

    size_t len = strlen(str);
    char* dup  = (char*)calloc(1, len + 1);
    return dup != nullptr ? (char*)memcpy(dup, str, len) : nullptr;
}
