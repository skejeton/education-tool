#ifndef HPP_CATEDU_CORE_PATCHER_PATCH_APPLIER
#define HPP_CATEDU_CORE_PATCHER_PATCH_APPLIER

#include "catedu/core/memory/buffer.hpp"
#include "patch.hpp"
#include "patch_explorer.hpp"
#include <vector>

/**
 * @brief For applying patches.
 */
struct PatchApplier
{
    PatchExplorer explorer;

    void value(const char* section, Buffer data);
    void string(const char* section, char** value);
};

#endif
