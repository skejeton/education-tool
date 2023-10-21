#include "scrolling.hpp"
#include <algorithm>

void
UiScroller::set_scroll_pixels(float pixels)
{
    this->scroll =
      std::max(pixels, std::max(content_height - content_window, 0.0f));
}

void
UiScroller::scroll_by_pixels(float pixels)
{
    set_scroll_pixels(this->scroll + pixels);
}
