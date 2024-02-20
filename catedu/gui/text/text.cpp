#include "text.hpp"
#include "catedu/ui/widgets.hpp"

struct VocabluaryEntry
{
    const char *word;
    const char *translation;
    const char *image_path;
    bool learned;
};

VocabluaryEntry vocabluary[] = {
    {"apple", "яблоко", "assets/examples/apple.png"},
    {"banana", "банан", "assets/examples/banana.png"},
    {"cherry", "вишня", "assets/examples/cherry.png"},
    {"grape", "виноград", "assets/examples/grape.png"}};

const int vocabluary_count = sizeof(vocabluary) / sizeof(vocabluary[0]);

void show_database(GuiText *gui, UiUser &user, const char *text)
{
    if (button(user, "Learn"))
    {
        gui->mode = LEARNING;
        gui->word = rand() % vocabluary_count;
        gui->revealed = false;
    }

    if (button(user, "Learn2"))
    {
        gui->mode = LEARNING2;
        gui->word = rand() % vocabluary_count;
        gui->revealed = false;
    }

    AutoLayoutElement wrapper = {};
    wrapper.layout.type = AutoLayout::Type::Row;

    user.begin_generic(wrapper, {}, {});

    for (int c = 0; c < 3; c++)
    {

        AutoLayoutElement wrapper = {};
        wrapper.layout.type = AutoLayout::Type::Column;
        wrapper.width.type = AutoLayoutDimension::Pixel;
        wrapper.width.value = 70;

        user.begin_generic(wrapper, {}, {});

        for (int i = 0; i < vocabluary_count; i++)
        {
            VocabluaryEntry &entry = vocabluary[i];

            switch (c)
            {
            case 0:
                label(user, entry.word);
                break;
            case 1:
                label(user, entry.translation);
                break;
            case 2:
                if (entry.learned)
                {
                    label(
                        user, "y", {1, 1},
                        UiMakeBrush::make_gradient({0, 1, 0, 1}, {0, 1, 1, 1}));
                }
                else
                {
                    label(user, " ");
                }
                break;
            }
        }

        user.end_generic();
    }

    user.end_generic();
}
void show_learning(GuiText *gui, UiUser &user, const char *text)
{
    VocabluaryEntry &entry = vocabluary[gui->word];

    img(user, entry.image_path);
    label(user, entry.word);

    if (gui->revealed)
    {
        label(user, entry.translation);

        if (button(user, "Yes"))
        {
            gui->word = rand() % vocabluary_count;
            gui->revealed = false;
            vocabluary[gui->word].learned = true;
        }

        if (button(user, "No"))
        {
            gui->word = rand() % vocabluary_count;
            gui->revealed = false;
        }
    }
    else
    {
        gui->revealed = button(user, "Reveal");
    }

    if (button(user, "Back"))
    {
        gui->mode = DATABASE;
    }
}

void show_learning2(GuiText *gui, UiUser &user, const char *text)
{
    VocabluaryEntry &entry = vocabluary[gui->word];

    img(user, entry.image_path);
    label(user, entry.translation);

    if (gui->revealed)
    {
        label(user, entry.word);

        if (strcmp(gui->guess, entry.word) == 0)
        {
            vocabluary[gui->word].learned = true;
            label(user, "Correct", {1, 1},
                  UiMakeBrush::make_gradient({0, 1, 0, 1}, {0, 1, 1, 1}));
        }
        else
        {
            label(user, "Wrong", {1, 1},
                  UiMakeBrush::make_gradient({1, 0, 0, 1}, {1, 1, 0, 1}));
        }

        if (button(user, "Next"))
        {
            memset(gui->guess, 0, sizeof(gui->guess));
            gui->word = rand() % vocabluary_count;
            gui->revealed = false;
        }
    }
    else
    {
        input(user, "guess", gui->guess, sizeof(gui->guess));
        gui->revealed = button(user, "Check");
    }

    if (button(user, "Back"))
    {
        gui->mode = DATABASE;
    }
}

void GuiText::show(UiUser &user, const char *text)
{
    WindowInfo info = {};
    info.title = "Learning tool";
    info.rect = {20, 20, 200, 250};

    begin_show_window(user, info);
    switch (this->mode)
    {
    case DATABASE:
        show_database(this, user, text);
        break;
    case LEARNING:
        show_learning(this, user, text);
        break;
    case LEARNING2:
        show_learning2(this, user, text);
        break;
    }
    end_show_window(user);
}
