#pragma once
#include "patch.hpp"

/**
 * @brief For creating patches.
 */
struct PatchCreator
{
    Patch *patch;
    size_t offs;
    size_t cap;

    void value(const char *section, Buffer data);
    void string(const char *section, const char *value);
};
