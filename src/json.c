#include "json.h"

#include <cjson/cJSON.h>

#define AR_VERSION     1
#define AR_MODULE_NAME "active-response"

cJSON* json_parse_str(const char* input)
{
    cJSON* json = cJSON_Parse(input);
    if (json == NULL) {
        return NULL;
    }

    const cJSON* version    = cJSON_GetObjectItem(json, "version");
    const cJSON* origin     = cJSON_GetObjectItem(json, "origin");
    const cJSON* command    = cJSON_GetObjectItem(json, "command");
    const cJSON* parameters = cJSON_GetObjectItem(json, "parameters");

    if (!cJSON_IsNumber(version) || !cJSON_IsObject(origin) || !cJSON_IsString(command) || !cJSON_IsObject(parameters)) {
        cJSON_Delete(json);
        return NULL;
    }

    return json;
}

const char* json_get_command(const cJSON* input)
{
    const cJSON* command = cJSON_GetObjectItem(input, "command");
    return cJSON_GetStringValue(command);
}

/**
 * @see https://documentation.wazuh.com/current/user-manual/capabilities/active-response/custom-active-response-scripts.html
 */
const char* json_get_srcip(const cJSON* input)
{
    const cJSON* parameters = cJSON_GetObjectItem(input, "parameters");
    if (!cJSON_IsObject(parameters)) {
        return NULL;
    }

    const cJSON* alert = cJSON_GetObjectItem(parameters, "alert");
    if (!cJSON_IsObject(alert)) {
        return NULL;
    }

    const cJSON* data = cJSON_GetObjectItem(alert, "data");
    if (!cJSON_IsObject(data)) {
        return NULL;
    }

    const cJSON* srcip = cJSON_GetObjectItem(data, "srcip");
    return cJSON_GetStringValue(srcip);
}

const cJSON* json_get_extra_args(const cJSON* input)
{
    const cJSON* parameters = cJSON_GetObjectItem(input, "parameters");
    if (!cJSON_IsObject(parameters)) {
        return NULL;
    }

    const cJSON* extra_args = cJSON_GetObjectItem(parameters, "extra_args");
    if (cJSON_IsArray(extra_args)) {
        return extra_args;
    }

    return NULL;
}

/**
 * @see https://documentation.wazuh.com/current/user-manual/capabilities/active-response/custom-active-response-scripts.html#stateful-active-response
 * @note The caller is responsible for freeing the returned string.
 * @note The `keys` parameter is an array of strings. The function takes ownership of it.
 */
char* json_build_control_message(const char* program_name, cJSON* keys)
{
    /*
     * {
     *     "version": 1,
     *     "origin": {
     *         "name": "program-name",
     *         "module": "active-response"
     *     },
     *     "command": "check_keys",
     *     "parameters": {
     *         "keys": ["10.0.0.1"]
     *     }
     * }
    */

    // cJSON_AddXXX correctly handle NULL pointers

    cJSON* origin = cJSON_CreateObject();
    cJSON_AddStringToObject(origin, "name", program_name);
    cJSON_AddStringToObject(origin, "module", AR_MODULE_NAME);

    cJSON* parameters = cJSON_CreateObject();
    cJSON_AddItemToObject(parameters, "keys", keys);

    cJSON* message = cJSON_CreateObject();
    cJSON_AddNumberToObject(message, "version", AR_VERSION);
    cJSON_AddItemToObject(message, "origin", origin);
    cJSON_AddStringToObject(message, "command", "check_keys");
    cJSON_AddItemToObject(message, "parameters", parameters);

    char* msg = message ? cJSON_PrintUnformatted(message) : NULL;
    cJSON_Delete(message);
    return msg;
}
