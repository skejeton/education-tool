#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/rendering/3d/model.hpp"
#include "catedu/sys/console.hpp"
#include "catedu/sys/sokol_setup.hpp"

using namespace catedu;

struct TestEntry : SokolSetup
{
    RawModel raw_model;
    Model model;

    void init()
    {
        console_create_or_bind_existing();

        if (RawModel::load_gltf("assets/models/barrel.gltf", raw_model))
        {
            fprintf(stderr, "Loaded gltf model!\n");
        }
        else
        {
            fprintf(stderr, "Failed to load gltf model!\n");
            return;
        }

        print_info(raw_model);

        if (Model::load_from_raw(raw_model, model))
        {
            fprintf(stderr, "Loaded model!\n");
        }
        else
        {
            fprintf(stderr, "Failed to load model!\n");
            return;
        }
    }

    void frame()
    {
    }

    void cleanup()
    {
    }

    void input(const sapp_event *event)
    {
    }
};

SOKOL_SETUP_QUICK_ENTRYPOINT(TestEntry, "PBR Test");
