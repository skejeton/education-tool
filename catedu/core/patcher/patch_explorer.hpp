#pragma once
#include "patch.hpp"

/**
 * @brief For reading patches.
 */
struct PatchExplorer
{
    Patch *patch;

    static PatchExplorer from_patch(Patch *patch);

    bool enter_section(const char *section);
    bool leave_section();

    PatchSection get_section(const char *section);
};
