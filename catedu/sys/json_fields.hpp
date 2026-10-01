#pragma once
#include <string>

// Minimal readers for the one-line JSON commands the headless servers accept.
// They find a key anywhere in the line and don't handle nesting or escapes, so
// string values can't contain quotes or backslashes.
bool json_match_str(const char *json, const char *key, char *out, int out_size);
bool json_match_double(const char *json, const char *key, double *out);
bool json_match_bool(const char *json, const char *key, bool *out);

// Appends printf-style formatted text.
void appendf(std::string &out, const char *fmt, ...);
// Appends `str` as a quoted, escaped JSON string.
void append_json_string(std::string &out, const char *str);
