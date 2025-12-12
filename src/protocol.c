#include "protocol.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "json.h"
#include "wazuh.h"

static char buf[MAX_BUF_SIZE];

cJSON* read_and_parse()
{
    memset(buf, 0, sizeof(buf));
    if (fgets(buf, MAX_BUF_SIZE, stdin) == NULL) {
        wazuh_log_message("Failed to read input from stdin");
        exit(OS_FAILURE);
    }

    size_t len = strlen(buf);
    if (len == MAX_BUF_SIZE - 1 && buf[len - 1] != '\n') {
        wazuh_log_message("Input exceeds maximum buffer size");
        exit(OS_FAILURE);
    }

    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    wazuh_log_message("%s", buf);
    cJSON* json = json_parse_str(buf);
    if (json == NULL) {
        wazuh_log_message("Failed to parse JSON input");
        exit(OS_FAILURE);
    }

    return json;
}

void send_control_message(const char* srcip)
{
    cJSON* keys = cJSON_CreateStringArray(&srcip, 1);
    if (keys == NULL) {
        wazuh_log_message("Failed to create control message keys (out of memory?)");
        exit(OS_FAILURE);
    }

    char* message = json_build_control_message(g_program_name, keys);
    if (message == NULL) {
        wazuh_log_message("Failed to build control message (out of memory?)");
        cJSON_Delete(keys);
        exit(OS_FAILURE);
    }

    // `keys` are deleted by `json_build_control_message()`

    printf("%s\n", message);
    free(message);
}
