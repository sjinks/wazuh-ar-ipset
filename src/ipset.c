#include "ipset.h"

#include <stdlib.h>
#include <string.h>
#include <libipset/ipset.h>
#include <libipset/session.h>
#include <libipset/types.h>

#include "utils.h"
#include "wazuh.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static int standard_error(struct ipset* ipset, void* p)
{
    struct ipset_session* session = ipset_session(ipset);
    enum ipset_err_type err_type  = ipset_session_report_type(session);

    const char* msg = ipset_session_report_msg(session);
    char* message   = msg != nullptr ? utils_strdup(msg) : nullptr;
    if (message != NULL) {
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }
    }

    switch (err_type) {
        case IPSET_NO_ERROR:
            break;
        case IPSET_WARNING:
            wazuh_log_message("Warning: %s", message != nullptr && *message != '\0' ? message : "Unknown warning");
            break;
        case IPSET_NOTICE:
            wazuh_log_message("Notice: %s", message != nullptr && *message != '\0' ? message : "Unknown notice");
            break;
        case IPSET_ERROR:
            wazuh_log_message("Error: %s", message != nullptr && *message != '\0' ? message : "Unknown error");
            break;
    }

    free(message);

    ipset_session_report_reset(session);
    return err_type < IPSET_NOTICE ? 1 : -1;
}

struct ipset* ipset_initialize()
{
    ipset_load_types();
    struct ipset* h = ipset_init();
    if (h != nullptr) {
        ipset_custom_printf(
            h,
            nullptr,
            standard_error,
            nullptr,
            nullptr
        );
    }
    else {
        wazuh_log_message("Failed to initialize ipset library");
    }

    return h;
}

void ipset_finalize(struct ipset* h)
{
    if (h != NULL) {
        ipset_fini(h);
    }
}

int ipset_add_ip(struct ipset* h, const char* set_name, const char* ip)
{
    char* data[] = {
        "",
        "add",
        (char*)set_name,
        (char*)ip,
        "-exist",
    };

    return ipset_parse_argv(h, ARRAY_SIZE(data), data);
}

int ipset_del_ip(struct ipset* h, const char* set_name, const char* ip)
{
    char* data[] = {
        "",
        "del",
        (char*)set_name,
        (char*)ip,
        "-exist",
    };

    return ipset_parse_argv(h, ARRAY_SIZE(data), data);
}
