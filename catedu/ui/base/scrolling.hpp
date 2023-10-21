#ifndef HPP_CATEDU_UI_BASE_SCROLLING
#define HPP_CATEDU_UI_BASE_SCROLLING

struct UiScroller
{
    float content_window;
    float content_height;
    float scroll;

    UiScroller(float content_window, float content_height, float scroll);

    void set_scroll_pixels(float pixels);
    void scroll_by_pixels(float pixels);
};

#endif
