#include "commands.h"

#include "ipset.h"
#include "json.h"
#include "protocol.h"
#include "utils.h"
#include "wazuh.h"

int process_add_command(struct ipset* handle, const char* ip, const char* set_name)
{
    send_control_message(ip);
    cJSON* confirmation = read_and_parse();

    const char* command = json_get_command(confirmation);
    enum ar_command cmd = utils_parse_command(command);

    int exit_code;
    if (cmd == AR_COMMAND_CONTINUE) {
        exit_code = ipset_add_ip(handle, set_name, ip) == 0 ? OS_SUCCESS : OS_FAILURE;
    }
    else if (cmd == AR_COMMAND_ABORT) {
        wazuh_log_message("Aborted");
        exit_code = OS_SUCCESS;
    }
    else {
        wazuh_log_message("Unknown confirmation command: %s", command);
        exit_code = OS_FAILURE;
    }

    cJSON_Delete(confirmation);
    return exit_code;
}

int process_delete_command(struct ipset* handle, const char* ip, const char* set_name)
{
    return ipset_del_ip(handle, set_name, ip) == 0 ? OS_SUCCESS : OS_FAILURE;
}
