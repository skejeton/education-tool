#ifndef HPP_CATEDU_CORE_PATCHER_PATCH_APPLIER
#define HPP_CATEDU_CORE_PATCHER_PATCH_APPLIER

#include <vector>
#include "patch.hpp"
#include "patch_explorer.hpp"

/**
 * @brief For applying patches.
 */
struct PatchApplier {
    PatchExplorer explorer;

    void value(const char *section, Buffer data);
    void string(const char *section, char **value);
};

#endif

