#pragma once
#include "catedu/core/memory/buffer.hpp"
#include "patch_explorer.hpp"

/**
 * @brief For applying patches.
 */
struct PatchApplier
{
    PatchExplorer explorer;

    void value(const char *section, Buffer data);
    void string(const char *section, char **value);
};
