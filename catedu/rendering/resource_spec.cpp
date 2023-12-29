#include "resource_spec.hpp"

void ResourceSpec::deinit()
{
    tileset.deinit();
    models.deinit();
    tiles.deinit();
}

TableId ResourceSpec::find_model_by_name(const char *name)
{
    for (auto [id, model] : iter(models))
    {
        if (strcmp(name, model.name) == 0)
        {
            return id;
        }
    }

    return {};
}
