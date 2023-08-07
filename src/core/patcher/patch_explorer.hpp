#ifndef HPP_CATEDU_CORE_PATCHER_PATCH_EXPLORER
#define HPP_CATEDU_CORE_PATCHER_PATCH_EXPLORER

#include <vector>
#include "patch.hpp"

/**
 * @brief For reading patches.
 */
struct PatchExplorer {
    Patch *patch;

    Buffer get_section(const char *section);
};

#endif
