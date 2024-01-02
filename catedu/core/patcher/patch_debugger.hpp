#pragma once
#include "patch_explorer.hpp"

/**
 * @brief For reading patches.
 */
struct PatchDebugger
{
    PatchExplorer explorer;

    void section(const char *section);
};
