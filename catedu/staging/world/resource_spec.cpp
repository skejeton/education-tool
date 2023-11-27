#include "resource_spec.hpp"

void ResourceSpec::deinit()
{
    tileset.deinit();
    models.deinit();
    tiles.deinit();
}

TableId ResourceSpec::find_model_by_name(const char *name)
{
    auto it = TableIterator<SpecModel>::init(&models);

    for (; it.going(); it.next())
    {
        SpecModel &model = it.table->get_assert(it.id);
        if (strcmp(name, model.name) == 0)
        {
            return it.id;
        }
    }

    return {};
}
