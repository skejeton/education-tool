#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/model.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/sys/console.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/sys/sokol_setup.hpp"

enum class ModelTypes
{
    tile,
    wall,
    barrel,
    skybox,
    counter,
    car,
    count_
};

using namespace catedu;

char *layer1 = "LTTTTTTTTR"
               "L    C   R"
               "L    C   R"
               "L    CCCCR"
               "L        R"
               "L        R"
               "L        R"
               "L        R"
               "L        R"
               "L        R"
               "LBBBBBBBBR";

char *layer2 = "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________"
               "__________";

char *layer3 = "T    C   T"
               "  bb      "
               "          "
               "     b   C"
               "          "
               "          "
               "      c   "
               "          "
               "          "
               "          "
               "B        B";

char *layers[] = {layer1, layer2, layer3};

void render_tile(char tile, int x, int y, pbr::Renderer &renderer,
                 Camera camera, Model *models)
{
    pbr_vs_params_t vs_params = {};
    vs_params.viewproj = camera.vp;
    vs_params.model = Matrix4::translate({(float)x, 0, (float)y});
    vs_params.color_mul = {1, 1, 1, 1};

    switch (tile)
    {
    case 'T':
        vs_params.model *= Matrix4::rotate_y(270 * (MATH_TAU / 360));
        goto render;
    case 'B':
        vs_params.model *= Matrix4::rotate_y(90 * (MATH_TAU / 360));
        goto render;
    case 'L':
        vs_params.model *= Matrix4::rotate_y(180 * (MATH_TAU / 360));
        goto render;
    case 'R':
    render:
        renderer.render_model(models[(int)ModelTypes::wall], vs_params);
        break;
    case 'C':
        renderer.render_model(models[(int)ModelTypes::counter], vs_params);
        break;
    case 'b':
        renderer.render_model(models[(int)ModelTypes::barrel], vs_params);
        break;
    case '_':
        renderer.render_model(models[(int)ModelTypes::tile], vs_params);
        break;
    case 'c':
        renderer.render_model(models[(int)ModelTypes::car], vs_params);
        break;
    default:
        break;
    }
}

void render_layer(char *layer, int w, int h, pbr::Renderer &renderer,
                  Camera camera, Model *models)
{
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            render_tile(layer[y * w + x], x, y, renderer, camera, models);
        }
    }
}

Model load_model(const char *path)
{
    RawModel raw_model;

    if (RawModel::load_gltf(path, raw_model))
    {
        fprintf(stderr, "Loaded gltf model!\n");
    }
    else
    {
        fprintf(stderr, "Failed to load gltf model!\n");
        exit(-1);
    }

    print_info(raw_model);

    Model model;

    if (Model::load_from_raw(raw_model, model))
    {
        fprintf(stderr, "Loaded model!\n");
    }
    else
    {
        fprintf(stderr, "Failed to load model!\n");
        exit(-1);
    }

    raw_model.deinit();

    return model;
}

struct TestEntry : SokolSetup
{
    RawModel raw_model;

    Model models[(int)ModelTypes::count_];
    Model model;

    Camera camera;
    Input input_state;

    pbr::Renderer renderer;

    void init()
    {
        console_create_or_bind_existing();

        models[(int)ModelTypes::tile] = load_model("assets/models/tile.gltf");
        models[(int)ModelTypes::wall] =
            load_model("assets/models/wall_edge.gltf");
        models[(int)ModelTypes::barrel] =
            load_model("assets/models/barrel.gltf");
        models[(int)ModelTypes::skybox] =
            load_model("assets/models/skybox.gltf");
        models[(int)ModelTypes::counter] =
            load_model("assets/models/counter.gltf");
        models[(int)ModelTypes::car] = load_model("assets/models/car.gltf");

        renderer = pbr::Renderer::init();
        input_state = input_state.init();
        camera = Camera::init(45);
    }

    void frame()
    {
        camera.set_aspect(sapp_widthf() / sapp_heightf());
        camera_input_apply(&camera, &input_state);
        renderer.begin_pass();

        {
            pbr_vs_params_t vs_params = {};
            vs_params.viewproj = camera.vp;
            vs_params.model = Matrix4::translate(camera.position);
            vs_params.color_mul = {1, 1, 1, 1};
            vs_params.lightness = 1.0;

            renderer.render_model(models[(int)ModelTypes::skybox], vs_params);
        }

        for (int i = 0; i < 3; i++)
        {
            render_layer(layers[i], 10, 11, renderer, camera, models);
        }

        renderer.end_pass();

        sg_commit();

        input_state.update();
    }

    void cleanup()
    {
        renderer.deinit();
        for (int i = 0; i < (int)ModelTypes::count_; i++)
        {
            models[i].deinit();
        }
    }

    void input(const sapp_event *event)
    {
        input_state.pass_event(event);
    }
};

SOKOL_SETUP_QUICK_ENTRYPOINT(TestEntry, "PBR Test");
