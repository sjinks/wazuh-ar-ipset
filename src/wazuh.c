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

static char* log_file_name = NULL;

static size_t utf8_sequence_length(const unsigned char* p)
{
    if (*p >= 0xC2 && *p <= 0xDF) {
        return p[1] >= 0x80 && p[1] <= 0xBF ? 2 : 0;
    }

    if (*p >= 0xE0 && *p <= 0xEF) {
        if (p[1] < 0x80 || p[1] > 0xBF || (*p == 0xE0 && p[1] < 0xA0) || (*p == 0xED && p[1] > 0x9F)) {
            return 0;
        }

        return p[2] >= 0x80 && p[2] <= 0xBF ? 3 : 0;
    }

    if (*p >= 0xF0 && *p <= 0xF4) {
        if (p[1] < 0x80 || p[1] > 0xBF || (*p == 0xF0 && p[1] < 0x90) || (*p == 0xF4 && p[1] > 0x8F)) {
            return 0;
        }

        if (p[2] < 0x80 || p[2] > 0xBF) {
            return 0;
        }

        return p[3] >= 0x80 && p[3] <= 0xBF ? 4 : 0;
    }

    return 0;
}

static void write_log_text(FILE* stream, const char* text)
{
    const unsigned char* p = (const unsigned char*)text;
    const unsigned char* run = p;

    while (*p != '\0') {
        size_t escaped = 0;
        if (*p < 0x20 || *p == 0x7F) {
            escaped = 1;
        }
        else if (*p == 0xC2 && p[1] >= 0x80 && p[1] <= 0x9F) {
            escaped = 2;
        }
        else if (*p == 0xE2 && p[1] == 0x80 && (p[2] == 0xA8 || p[2] == 0xA9)) {
            escaped = 3;
        }
        else if (*p >= 0x80) {
            size_t length = utf8_sequence_length(p);
            if (length != 0) {
                p += length;
                continue;
            }

            escaped = 1;
        }

        if (escaped == 0) {
            ++p;
        }
        else {
            fwrite(run, 1, (size_t)(p - run), stream);
            for (size_t i = 0; i < escaped; ++i) {
                fprintf(stream, "\\x%02X", (unsigned int)p[i]);
            }

            p  += escaped;
            run = p;
        }
    }

    fwrite(run, 1, (size_t)(p - run), stream);
}

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
    char* path = NULL;
    if (pw != NULL && pw->pw_dir != NULL) {
        path = pw->pw_dir;
    }
    else {
        path = getenv("WAZUH_HOME");
    }

    if (path == NULL || path[0] == '\0') {
        path = "/var/ossec";
    }

    struct stat sb;
    int rc = stat(path, &sb);
    if (rc != 0 || !S_ISDIR(sb.st_mode)) {
        return NULL;
    }

    return utils_strdup(path);
}

static char* get_wazuh_log_file()
{
    char* home_dir = wazuh_get_home_dir();
    if (home_dir == NULL) {
        return NULL;
    }

    char* log_path = calloc(1, PATH_MAX);
    if (log_path == NULL) {
        free(home_dir);
        return NULL;
    }

    snprintf(log_path, PATH_MAX, "%s/logs/active-responses.log", home_dir);
    free(home_dir);
    return log_path;
}

void wazuh_initialize()
{
    if (log_file_name == NULL) {
        log_file_name = get_wazuh_log_file();
    }
}

void wazuh_finalize()
{
    if (log_file_name != NULL) {
        free(log_file_name);
        log_file_name = NULL;
    }
}

void wazuh_log_message(const char* format, ...)
{
    char timestamp[sizeof("YYYY/MM/DD HH:MM:SS")];
    const time_t now = time(NULL);
    get_timestamp(now, sizeof(timestamp), &timestamp);

    FILE* f = NULL;
    if (log_file_name != NULL) {
        f = fopen(log_file_name, "a");
    }

    FILE* stream = f != NULL ? f : stderr;

    fprintf(stream, "%s ", timestamp);
    write_log_text(stream, g_program_name);
    fputs(": ", stream);

    va_list args;
    va_start(args, format);
    va_list copy;
    va_copy(copy, args);
    int length = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    char* message = length >= 0 ? malloc((size_t)length + 1) : NULL;
    if (message != NULL) {
        vsnprintf(message, (size_t)length + 1, format, args);
        write_log_text(stream, message);
        free(message);
    }
    else {
        fputs("[log message unavailable]", stream);
    }
    va_end(args);

    fputc('\n', stream);

    fflush(stream);

    if (f != NULL) {
        fclose(f);
    }
}
