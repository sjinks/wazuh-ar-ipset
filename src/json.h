#ifndef D85BB96D_F64B_4EA1_B406_2A6E9FFB5A4B
#define D85BB96D_F64B_4EA1_B406_2A6E9FFB5A4B

#include <cjson/cJSON.h>

[[gnu::nonnull(1)]] cJSON* json_parse_str(const char* input);
[[gnu::nonnull(1)]] const char* json_get_command(const cJSON* input);
[[gnu::nonnull(1)]] const char* json_get_srcip(const cJSON* input);
[[gnu::nonnull(1)]] const cJSON* json_get_extra_args(const cJSON* input);
[[gnu::nonnull(1, 2)]] char* json_build_control_message(const char* program_name, cJSON* keys);

#endif /* D85BB96D_F64B_4EA1_B406_2A6E9FFB5A4B */
