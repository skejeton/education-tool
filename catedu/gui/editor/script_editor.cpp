#include "script_editor.hpp"
#include "catedu/scene/script.hpp"
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

void show_generic_script_card(ScriptCardDragNDrop &dnd, ScriptNode *node,
                              UiPass &user, ScriptCard card,
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

    user.push_id(size_t(node));
    user.state->element_storage.push("Card");
    auto val = user.state->element_storage.value();

    if (val->userdata == nullptr)
    {
        val->userdata = ALLOCATOR_MALLOC.alloc(sizeof(ScriptCardData));
        *(ScriptCardData *)val->userdata = {};
    }

    auto data = (ScriptCardData *)val->userdata;

    if (data->dragging)
    {
        el.position = AutoLayoutPosition::absolute;
        el.offset = data->drag_offset;
        el.pop = true;
        val->pop = true;
    }

    user.begin_generic(el, background, border,
                       user.state->element_storage.id());

    if (user.actively_hovered() && user.state->input.k[INPUT_MB_LEFT].pressed &&
        node->type != ScriptNode::Type::event)
    {
        data->dragging = true;
        data->drag_peg = user.state->input.mouse_pos / user.state->dpi_scale -
                         val->border_box.pos / user.state->dpi_scale;

        dnd.dragging = data;
        dnd.node = node;
    }

    if ((user.hovered() ||
         (user.state->input.mouse_pos.x > val->border_box.pos.x &&
          user.state->input.mouse_pos.y > val->border_box.pos.y)) &&
        dnd.node != node)
    {
        dnd.insert_after = (ScriptNode *)node;
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

ScriptCardAction show_script_card(ScriptCardDragNDrop &dnd, ScriptNode *node,
                                  UiPass &user, bool shadow = false,
                                  bool highlight = false)
{
    ScriptCardAction action = {};

    if (shadow)
    {
        show_generic_script_card(dnd, node, user, {"...", 0xCCCCCC22}, [&] {});
        return action;
    }

    uint32_t colormask = highlight ? 0x33333333 : 0x0;

    switch (node->type)
    {
    case ScriptNode::Type::event:
        show_generic_script_card(
            dnd, node, user, {"On...", 0xCCCC0099 | colormask}, [&] {
                const char *name = "";
                switch (node->event)
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
        show_generic_script_card(
            dnd, node, user, {"Say...", 0x0000CC99 | colormask},
            [&] { input(user, "Message", node->say, 256); });
        break;
    case ScriptNode::Type::yesno:
        show_generic_script_card(
            dnd, node, user, {"Ask...", 0x0000CC99 | colormask}, [&] {
                input(user, "Message", node->yesno.question, 256);
                AutoLayoutElement el = {};
                el.layout.type = AutoLayout::row;

                user.begin_generic(el, {}, {});
                show_script_card_btn(
                    user, {"Yes", 0xCCCC0099 | colormask, true}, [&] {},
                    [&] {
                        action.parent = node;
                        action.yes = true;
                    });
                show_script_card_btn(
                    user, {"No", 0xCCCC0099 | colormask, true}, [&] {},
                    [&] {
                        action.parent = node;
                        action.yes = false;
                    });
                user.end_generic();
            });
        break;
    default:;
    }

    return action;
}

void ScriptEditor::show_palette(UiPass &user)
{
    static ScriptNode stub;
    AutoLayoutElement el = {};
    el.margin.t = -user.state->element_storage.value()->scroll.y;
    user.begin_generic(el, {}, {});
    show_generic_script_card(
        dnd, &stub, user, {"Card pallete...", 0x11111111}, [&] {
            label(user, "Select", {2, 2}, UiMakeBrush::make_solid(0xFFFFFFFF));
        });

    for (int i = 1; i < int(ScriptNode::Type::count_); i++)
    {
        user.push_id(size_t(palette.nodes[i]));
        show_script_card(dnd, palette.nodes[i], user);
        user.pop_id();
        if (dnd.node == palette.nodes[i])
        {
            palette.nodes[i] = script->nodes.alloc();
            *palette.nodes[i] = {};
            palette.nodes[i]->type = ScriptNode::Type(i);
        }
    }
    dnd.insert_after = nullptr;

    user.end_generic();
}

void ScriptEditor::show(UiPass &user)
{
    user.begin_generic({}, {}, {});
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

    ScriptCardAction action = {};

    ScriptNode *node = this->current;
    ScriptNode *prev = nullptr;

    bool found_place = false;
    while (node)
    {
        if (node != dnd.node)
        {
            user.push_id(size_t(node));
            if (action.parent)
            {
                show_script_card(dnd, node, user);
            }
            else
            {
                action = show_script_card(dnd, node, user);
            }
            user.pop_id();
        }

        if (node->next == dnd.node)
        {
            prev = node;
        }

        if (dnd.node != nullptr && dnd.insert_after_final == node)
        {
            found_place = true;
            user.push_id(size_t(dnd.node) + 1);
            ScriptCardDragNDrop mock;
            show_script_card(mock, dnd.node, user, false, true);
            user.pop_id();
        }

        node = node->next;
    }

    dnd.insert_after_final = dnd.insert_after;

    if (dnd.node != nullptr)
    {
        user.push_id(size_t(dnd.node));
        action = show_script_card(dnd, dnd.node, user, found_place);
        user.pop_id();
    }

    if (user.state->input.k[INPUT_MB_LEFT].released && found_place)
    {
        if (prev)
            prev->next = dnd.node->next;

        if (dnd.node->parent == nullptr)
        {
            dnd.node->parent = this->current;
        }

        // insert into the new place
        dnd.node->next = dnd.insert_after->next;
        dnd.insert_after->next = dnd.node;
    }
    if (user.state->input.k[INPUT_MB_LEFT].released)
    {
        if (!found_place && dnd.node)
        {
            if (prev && prev->next == dnd.node)
            {
                prev->next = dnd.node->next;
            }

            script->nodes.free(dnd.node);
        }
        dnd.dragging = nullptr;
        dnd.node = nullptr;
    }

    if (action.parent)
    {
        if (action.yes)
        {
            if (!action.parent->yesno.yes)
            {
                action.parent->yesno.yes =
                    script->append_node(ScriptNode::Type::event, action.parent);
                action.parent->yesno.yes->event = ScriptNode::EventType::yes;
            }
            current = action.parent->yesno.yes;
        }
        else
        {
            if (!action.parent->yesno.no)
            {
                action.parent->yesno.no =
                    script->append_node(ScriptNode::Type::event, action.parent);
                action.parent->yesno.no->event = ScriptNode::EventType::no;
            }
            current = action.parent->yesno.no;
        }
    }

    user.end_generic();
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

    for (int i = 1; i < int(ScriptNode::Type::count_); i++)
    {
        result.palette.nodes[i] = script->nodes.alloc();
        *result.palette.nodes[i] = {};
        result.palette.nodes[i]->type = ScriptNode::Type(i);
    }

    return result;
}
