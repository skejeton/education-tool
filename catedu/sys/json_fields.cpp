#include "json_fields.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool json_match_str(const char *json, const char *key, char *out,
                           int out_size)
{
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(json, pattern);
    if (!p)
        return false;
    p += strlen(pattern);
    while (*p == ' ' || *p == ':' || *p == '\t')
        p++;
    if (*p == '"')
    {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < out_size - 1)
        {
            out[i++] = *p++;
        }
        out[i] = '\0';
        return true;
    }
    return false;
}

bool json_match_double(const char *json, const char *key, double *out)
{
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(json, pattern);
    if (!p)
        return false;
    p += strlen(pattern);
    while (*p == ' ' || *p == ':' || *p == '\t')
        p++;
    char *end;
    *out = strtod(p, &end);
    return end != p;
}

bool json_match_bool(const char *json, const char *key, bool *out)
{
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *p = strstr(json, pattern);
    if (!p)
        return false;
    p += strlen(pattern);
    while (*p == ' ' || *p == ':' || *p == '\t')
        p++;
    if (strncmp(p, "true", 4) == 0)
    {
        *out = true;
        return true;
    }
    if (strncmp(p, "false", 5) == 0)
    {
        *out = false;
        return true;
    }
    return false;
}

void appendf(std::string &out, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int size = vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);

    if (size > 0)
    {
        size_t at = out.size();
        out.resize(at + size + 1);
        vsnprintf(&out[at], size + 1, fmt, args);
        out.resize(at + size);
    }
    va_end(args);
}

void append_json_string(std::string &out, const char *str)
{
    out += '"';
    for (const char *c = str; *c; c++)
    {
        if (*c == '"' || *c == '\\')
        {
            out += '\\';
            out += *c;
        }
        else if ((unsigned char)*c < 0x20)
        {
            appendf(out, "\\u%04x", *c);
        }
        else
        {
            out += *c;
        }
    }
    out += '"';
}
