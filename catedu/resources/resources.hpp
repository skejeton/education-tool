#pragma once
#include "catedu/rendering/resource_spec.hpp"

ResourceSpec load_resource_spec();

// A resource spec with every model's name but no GPU data, for exporting the
// world without a graphics context. Free it with `models.deinit()`, not
// `deinit()`, which would destroy GPU buffers that were never created.
ResourceSpec load_resource_names();

// Finds the glTF file a model in the resource spec is loaded from, and which
// of its submodels it is.
bool find_model_source(const char *name, const char **path, int *submodel);
