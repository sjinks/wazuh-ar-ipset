#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

#include "commands.h"
#include "ipset.h"
#include "json.h"
#include "protocol.h"
#include "utils.h"
#include "wazuh.h"

static struct ipset* ipset_handle = NULL;
static cJSON* input = NULL;

char* g_program_name = "";

static void exit_func()
{
    // cJSON_Delete() will handle NULL pointers
    cJSON_Delete(input);
    input = NULL;

    ipset_finalize(ipset_handle);
    wazuh_finalize();
}

int main(int argc, char** argv)
{
    g_program_name = argv[0];

    atexit(exit_func);
    wazuh_initialize();
    ipset_handle = ipset_initialize();
    if (ipset_handle == NULL) {
        return OS_FAILURE;
    }

    input = read_and_parse();

    const char* srcip = json_get_srcip(input);
    if (srcip == NULL) {
        wazuh_log_message("Missing srcip in input JSON");
        return OS_FAILURE;
    }

    int family = utils_get_ip_family(srcip);
    if (family == -1) {
        wazuh_log_message("Invalid IP address: %s", srcip);
        return OS_FAILURE;
    }

    const char* list_name = utils_get_list_name(input, family);
    if (list_name == NULL) {
        wazuh_log_message("Missing list name for IPv%d", family);
        return OS_FAILURE;
    }

    const char* command = json_get_command(input);
    if (command == NULL) {
        wazuh_log_message("Malformed JSON input");
        return OS_FAILURE;
    }

    enum ar_command cmd = utils_parse_command(command);

    if (cmd != AR_COMMAND_ADD && cmd != AR_COMMAND_DELETE) {
        wazuh_log_message("Unknown command: %s", command);
        return OS_FAILURE;
    }

    if (cmd == AR_COMMAND_ADD) {
        return process_add_command(ipset_handle, srcip, list_name);
    }

    if (cmd == AR_COMMAND_DELETE) {
        return process_delete_command(ipset_handle, srcip, list_name);
    }

    wazuh_log_message("Unknown command: %s", command);
    return OS_FAILURE;
}
