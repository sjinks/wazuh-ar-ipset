#include "wazuh.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "globals.h"
#include "utils.h"

static char* log_file_name = nullptr;

static char* get_timestamp(time_t time, size_t buf_size, char (*buf)[buf_size])
{
    struct tm* localtm = localtime(&time);

    snprintf(
        *buf, buf_size,
        "%d/%02d/%02d %02d:%02d:%02d",
        localtm->tm_year + 1900, localtm->tm_mon + 1, localtm->tm_mday,
        localtm->tm_hour, localtm->tm_min, localtm->tm_sec
    );

    return *buf;
}

static char* wazuh_get_home_dir()
{
    const struct passwd* pw = getpwnam("wazuh");
    char* path = nullptr;
    if (pw != nullptr && pw->pw_dir != nullptr) {
        path = pw->pw_dir;
    }
    else {
        path = getenv("WAZUH_HOME");
    }

    if (path == nullptr || path[0] == '\0') {
        path = "/var/ossec";
    }

    struct stat sb;
    int rc = stat(path, &sb);
    if (rc != 0 || !S_ISDIR(sb.st_mode)) {
        return nullptr;
    }

    return utils_strdup(path);
}

static char* get_wazuh_log_file()
{
    char* home_dir = wazuh_get_home_dir();
    if (home_dir == nullptr) {
        return nullptr;
    }

    char* log_path = calloc(1, PATH_MAX);
    if (log_path == nullptr) {
        free(home_dir);
        return nullptr;
    }

    snprintf(log_path, PATH_MAX, "%s/logs/active-responses.log", home_dir);
    free(home_dir);
    return log_path;
}

void wazuh_initialize()
{
    if (log_file_name == nullptr) {
        log_file_name = get_wazuh_log_file();
    }
}

void wazuh_finalize()
{
    if (log_file_name != nullptr) {
        free(log_file_name);
        log_file_name = nullptr;
    }
}

void wazuh_log_message(const char* format, ...)
{
    char timestamp[sizeof("YYYY/MM/DD HH:MM:SS")];
    const time_t now = time(nullptr);
    get_timestamp(now, sizeof(timestamp), &timestamp);

    FILE* f = nullptr;
    if (log_file_name != nullptr) {
        f = fopen(log_file_name, "a");
    }

    FILE* stream = f != nullptr ? f : stderr;

    fprintf(stream, "%s %s: ", timestamp, g_program_name);

    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    fputc('\n', stream);

    fflush(stream);

    if (f != nullptr) {
        fclose(f);
    }
}
