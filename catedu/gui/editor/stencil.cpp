#include "stencil.hpp"

void StencilEdit::map(StencilEditMap f)
{
    switch (this->type)
    {
    case StencilType::Rectangle: {
        int x0 = this->start.x;
        int y0 = this->start.y;
        int x1 = this->end.x;
        int y1 = this->end.y;

        if (x0 > x1)
        {
            int tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        if (y0 > y1)
        {
            int tmp = y0;
            y0 = y1;
            y1 = tmp;
        }

        for (int x = x0; x <= x1; x++)
        {
            for (int y = y0; y <= y1; y++)
            {
                f(x, y, 0);
            }
        }
    }
    break;
    case StencilType::Freeform:
    case StencilType::Line: {
        int x0 = this->start.x;
        int y0 = this->start.y;
        int x1 = this->end.x;
        int y1 = this->end.y;

        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        float angle = atan2f(y1 - y0, x1 - x0);

        while (true)
        {
            f(x0, y0, angle);

            if (x0 == x1 && y0 == y1)
            {
                break;
            }
            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }
    break;
    case StencilType::LineRectangle: {
        int x0 = this->start.x;
        int y0 = this->start.y;
        int x1 = this->end.x;
        int y1 = this->end.y;

        if (x0 > x1)
        {
            int tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        if (y0 > y1)
        {
            int tmp = y0;
            y0 = y1;
            y1 = tmp;
        }

        for (int x = x0 + 1; x < x1; x++)
        {
            f(x, y0, 0);   // north
            f(x, y1, 180); // south
        }
        for (int y = y0; y <= y1; y++)
        {
            f(x0, y, 90);  // west
            f(x1, y, 270); // east
        }
    }
    break;
    case StencilType::Ellipse: {
        int x0 = this->start.x;
        int y0 = this->start.y;
        int x1 = this->end.x;
        int y1 = this->end.y;

        if (x0 > x1)
        {
            int tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        if (y0 > y1)
        {
            int tmp = y0;
            y0 = y1;
            y1 = tmp;
        }

        int a = (x1 - x0) / 2;
        int b = (y1 - y0) / 2;
        int cx = x0 + a;
        int cy = y0 + b;

        for (int y = -b; y <= b; y++)
        {
            for (int x = -a; x <= a; x++)
            {
                if ((x * x * b * b + y * y * a * a) <= (a * a * b * b))
                {
                    f(cx + x, cy + y, atan2f(y, x));
                }
            }
        }
    }
    }
}
