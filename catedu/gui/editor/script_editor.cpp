#include "script_editor.hpp"
#include "catedu/ui/layout/autolayout.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"
#include <functional>

struct ScriptCard
{
    const char *title;
    Color color;
    bool embedded;
};

void show_script_card_btn(UiPass &user, ScriptCard card,
                          std::function<void()> cb, std::function<void()> press)
{
    AutoLayoutElement el = {};
    if (card.embedded)
    {
        el.width = {AutoLayoutDimension::pixel, 79};
    }
    else
    {
        el.width = {AutoLayoutDimension::pixel, 186};
    }
    el.height = {AutoLayoutDimension::autom};
    el.align_width = 0.5;
    el.margin = {2, 2, 2, 2};
    el.padding = {4, 4, 4, 4};
    el.border = {1, 1, 1, 1};

    begin_button_frame(user, card.title, el, card.color.to_vector4(), 0.9);

    label(user, card.title, {1, 1}, UiMakeBrush::make_solid(0xFFFFFFBB));

    cb();

    if (end_button_frame(user))
    {
        press();
    }
}

struct ScriptCardData
{
    bool dragging;
    Vector2 drag_offset;
    Vector2 drag_peg;
};

void show_script_card(size_t id, UiPass &user, ScriptCard card,
                      std::function<void()> cb)
{
    AutoLayoutElement el = {};
    if (card.embedded)
    {
        el.width = {AutoLayoutDimension::pixel, 79};
    }
    else
    {
        el.width = {AutoLayoutDimension::pixel, 186};
    }
    el.height = {AutoLayoutDimension::autom};
    el.margin = {2, 2, 2, 2};
    el.padding = {4, 4, 4, 4};
    el.border = {1, 1, 1, 1};

    UiBrush background = UiMakeBrush::make_plain_brush()
                             .with_gradient(card.color, card.color.darken(0.5))
                             .squircle(0.9, 0.5)
                             .build();
    UiBrush border = UiMakeBrush::make_plain_brush()
                         .with_solid(0x00000022)
                         .squircle(0.9, 0.5)
                         .build();

    user.push_id(id);
    user.state->element_storage.push("Card");
    auto val = user.state->element_storage.value();

    if (val->userdata == nullptr)
    {
        val->userdata = ALLOCATOR_MALLOC.alloc(sizeof(ScriptCardData));
    }
    auto data = (ScriptCardData *)val->userdata;

    if (data->dragging)
    {
        el.position = AutoLayoutPosition::absolute;
        el.offset = data->drag_offset;
        el.pop = true;
    }

    user.begin_generic(el, background, border,
                       user.state->element_storage.id());

    if (user.actively_hovered() && user.state->input.k[INPUT_MB_LEFT].pressed)
    {
        data->dragging = true;
        data->drag_peg = user.state->input.mouse_pos / user.state->dpi_scale -
                         val->border_box.pos / user.state->dpi_scale;
    }

    if (data->dragging)
    {
        data->drag_offset =
            user.state->input.mouse_pos / user.state->dpi_scale -
            data->drag_peg;
    }

    if (user.state->input.k[INPUT_MB_LEFT].released)
    {
        data->dragging = false;
    }

    label(user, card.title, {1, 1}, UiMakeBrush::make_solid(0xFFFFFFBB));

    cb();

    user.end_generic();
    user.state->element_storage.pop();
    user.pop_id();
}

void ScriptEditor::show(UiPass &user)
{
    if (this->current->parent)
    {
        show_script_card_btn(
            user, {"Back", 0xCCCCCC99}, [&] {},
            [&] {
                if (this->current->parent->type == ScriptNode::Type::event)
                {
                    this->current = this->current->parent;
                }
                else
                {
                    this->current = this->current->parent->parent;
                }
            });
    }

    int i = 0;
    ScriptNode *parent = nullptr;
    bool yes;
    ScriptNode::Type type;

    ScriptNode *node = this->current;
    while (node)
    {
        ScriptNode &s = *node;
        user.push_id(i++);

        switch (s.type)
        {
        case ScriptNode::Type::event:
            show_script_card(size_t(&s), user, {"On...", 0xCCCC0099}, [&] {
                const char *name = "";
                switch (s.event)
                {
                case ScriptNode::EventType::start:
                    name = "Start";
                    break;
                case ScriptNode::EventType::yes:
                    name = "Yes";
                    break;
                case ScriptNode::EventType::no:
                    name = "No";
                    break;
                }
                label(user, name, {2, 2}, UiMakeBrush::make_solid(0xFFFFFFFF));
            });
            break;
        case ScriptNode::Type::say:
            show_script_card(size_t(&s), user, {"Say...", 0x0000CC99},
                             [&] { input(user, "Message", s.say, 256); });
            break;
        case ScriptNode::Type::yesno:
            show_script_card(size_t(&s), user, {"Ask...", 0x0000CC99}, [&] {
                input(user, "Message", s.yesno.question, 256);
                AutoLayoutElement el = {};
                el.layout.type = AutoLayout::row;

                user.begin_generic(el, {}, {});
                show_script_card_btn(
                    user, {"Yes", 0xCCCC0099, true}, [&] {},
                    [&] {
                        parent = &s;
                        yes = true;
                        type = ScriptNode::Type::event;
                    });
                show_script_card_btn(
                    user, {"No", 0xCCCC0099, true}, [&] {},
                    [&] {
                        parent = &s;
                        yes = false;
                        type = ScriptNode::Type::event;
                    });
                user.end_generic();
            });
            break;
        }

        user.pop_id();
        node = node->next;
    };

    if (parent)
    {
        if (yes)
        {
            if (!parent->yesno.yes)
            {
                parent->yesno.yes =
                    script->append_node(ScriptNode::Type::event, parent);
            }
            current = parent->yesno.yes;
            parent->yesno.yes->event = ScriptNode::EventType::yes;
        }
        else
        {
            if (!parent->yesno.no)
            {
                parent->yesno.no =
                    script->append_node(ScriptNode::Type::event, parent);
                parent->yesno.no->event = ScriptNode::EventType::no;
            }
            current = parent->yesno.no;
        }
    }

    show_script_card_btn(
        user, {"Add action 'Say'", 0x33FF3399},
        [&] {
            user.bold = true;
            label(user, "+", {3, 3}, UiMakeBrush::make_solid(0xEEFFEEFF));
            user.bold = false;
        },
        [&] { script->append_node(ScriptNode::Type::say, current); });

    show_script_card_btn(
        user, {"Add action 'Ask'", 0x33FF3399},
        [&] {
            user.bold = true;
            label(user, "+", {3, 3}, UiMakeBrush::make_solid(0xEEFFEEFF));
            user.bold = false;
        },
        [&] { script->append_node(ScriptNode::Type::yesno, current); });
}

ScriptEditor ScriptEditor::create(Script *script)
{
    ScriptEditor result = {};

    result.script = script;
    if (result.script->root == nullptr)
    {
        result.script->root = result.script->nodes.alloc();
        *result.script->root = {};
        result.script->root->type = ScriptNode::Type::event;
    }
    result.current = script->root;

    return result;
}
