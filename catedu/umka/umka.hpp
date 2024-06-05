#pragma once

#include "catedu/ui/user.hpp"
#include <umka_api.h>

struct UmkaBindingsData
{
    UiUser *user;
};

struct UmkaModule
{
    void *umka;
    char *last_source_path;
    UmkaBindingsData *data;

    static UmkaModule create(UmkaBindingsData *data);
    void destroy();

    bool load(const char *path);
    bool reload();

    UmkaError *call(const char *module, const char *function, size_t nparams,
                    UmkaStackSlot *params, UmkaStackSlot *result = nullptr);

    UmkaError *get_error();
};
