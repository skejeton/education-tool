#include "user.hpp"
#include <assert.h>

void
PatchingMode::begin(Mode mode, Patch* patch)
{
    // (Assert we aren't using `begin` to end patching mode)
    assert(mode != None);

    this->mode = mode;
    switch (mode) {
        case Create:
            this->creator = { patch };
            break;
        case Apply:
            this->applier = { patch };
            break;
        case Debug:
            this->debugger = { patch };
            break;
    }
}

void
PatchingMode::pass_string(const char* name, char** string)
{
    assert(name);
    assert(this->mode != None);

    switch (mode) {
        case Create:
            this->creator.string(name, *string);
            break;
        case Apply:
            this->applier.string(name, string);
            break;
        case Debug:
            this->debugger.section(name);
            break;
    }
}

void
PatchingMode::pass_value(const char* name, Buffer buffer)
{
    assert(name);
    assert(this->mode != None);

    switch (mode) {
        case Create:
            this->creator.value(name, buffer);
            break;
        case Apply:
            this->applier.value(name, buffer);
            break;
        case Debug:
            this->debugger.section(name);
            break;
    }
}

void
PatchingMode::end()
{
    this->mode = None;
}

Patch
Patcher::create_patch(Callback callback, void* data)
{
    Patch patch = {};
    this->mode.begin(PatchingMode::Create, &patch);
    callback(this, data);
    this->mode.end();

    return patch;
}

void
Patcher::apply_patch(Callback callback, void* data, Patch* patch)
{
    this->mode.begin(PatchingMode::Apply, patch);
    callback(this, data);
    this->mode.end();
}

void
Patcher::debug_patch(Callback callback, Patch* patch)
{
    this->mode.begin(PatchingMode::Debug, patch);
    callback(this, nullptr);
    this->mode.end();
}

void
Patcher::string(const char* name, char** string)
{
    this->mode.pass_string(name, string);
}
