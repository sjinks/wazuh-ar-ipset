#ifndef E3DBFFEA_EAB7_41DB_BEA4_D4593BF9EA05
#define E3DBFFEA_EAB7_41DB_BEA4_D4593BF9EA05

struct cJSON;

enum ar_command {
    AR_COMMAND_INVALID = -1,
    AR_COMMAND_ADD,
    AR_COMMAND_DELETE,
    AR_COMMAND_CONTINUE,
    AR_COMMAND_ABORT
};

[[gnu::nonnull(1)]] int utils_get_ip_family(const char* ip_str);
[[gnu::nonnull(1)]] const char* utils_get_list_name(const struct cJSON* input, int family);
[[gnu::nonnull(1)]] enum ar_command utils_parse_command(const char* command);

[[nodiscard]] char* utils_strdup(const char* str);

#endif /* E3DBFFEA_EAB7_41DB_BEA4_D4593BF9EA05 */
