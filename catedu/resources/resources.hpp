#pragma once
#include "catedu/rendering/resource_spec.hpp"

ResourceSpec load_resource_spec();

// Finds the glTF file a model in the resource spec is loaded from, and which
// of its submodels it is.
bool find_model_source(const char *name, const char **path, int *submodel);
