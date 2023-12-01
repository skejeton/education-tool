#ifndef HPP_CATEDU_CORE_PATCHER_USER
#define HPP_CATEDU_CORE_PATCHER_USER

#include "patch_applier.hpp"
#include "patch_creator.hpp"
#include "patch_debugger.hpp"

struct PatchingMode
{
    enum Mode
    {
        None,
        Create,
        Apply,
        Debug
    } mode;
    union {
        PatchCreator creator;
        PatchApplier applier;
        PatchDebugger debugger;
    };

    void begin(Mode mode, Patch *patch);
    void pass_string(const char *name, char **string);
    void pass_value(const char *name, Buffer buffer);
    void end();
};

struct Patcher
{
    using Callback = void (*)(Patcher *patcher, void *userdata);
    PatchingMode mode;

    Patch create_patch(Callback callback, void *data);
    void apply_patch(Callback callback, void *data, Patch *patch);
    void debug_patch(Callback callback, Patch *patch);

    template <class T> void value(const char *name, T *value)
    {
        mode.pass_value(name, {sizeof *value, value});
    }

    void string(const char *name, char **string);
};

#endif
