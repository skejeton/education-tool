#ifndef HPP_CATEDU_CORE_PATCHER_PATCH_DEBUGGER
#define HPP_CATEDU_CORE_PATCHER_PATCH_DEBUGGER

#include "patch_explorer.hpp"

/**
 * @brief For reading patches.
 */
struct PatchDebugger
{
    PatchExplorer explorer;

    void section(const char *section);
};

#endif
