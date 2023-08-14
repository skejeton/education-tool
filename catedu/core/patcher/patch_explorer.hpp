#ifndef HPP_CATEDU_CORE_PATCHER_PATCH_EXPLORER
#define HPP_CATEDU_CORE_PATCHER_PATCH_EXPLORER

#include <vector>
#include "patch.hpp"
#include "catedu/core/memory/serial_mem.hpp"

/**
 * @brief For reading patches.
 */
struct PatchExplorer {
    Patch *patch;

    static PatchExplorer from_patch(Patch *patch);

    bool enter_section(const char *section);
    bool leave_section();

    PatchSection get_section(const char *section);
};

#endif
