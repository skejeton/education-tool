float deadband(float x, float g)
{
    g = min(g, 0.999);
    return (max(abs(x) - g, 0.0) * sign(x))/(1.0-g);
}

bool superellipse_intersect_point(vec4 q, vec2 point, vec4 gapsx, vec4 gapsy)
{
    float n;
    vec2 g;

    // top right, bottom right, bottom left, top left
    if (point.x >= 0.0 && point.y <= 0.0)
    {
        g = vec2(gapsx.y, gapsy.y);
        n = q.y;
    }
    else if (point.x >= 0.0 && point.y >= 0.0)
    {
        g = vec2(gapsx.w, gapsy.w);
        n = q.w;
    }
    else if (point.x <= 0.0 && point.y >= 0.0)
    {
        g = vec2(gapsx.z, gapsy.z);
        n = q.z;
    }
    else if (point.x <= 0.0 && point.y <= 0.0)
    {
        g = vec2(gapsx.x, gapsy.x);
        n = q.x;
    }

    point.x = deadband(point.x, g.x);
    point.y = deadband(point.y, g.y);

    float x = exp(n*log(abs(point.x)));
    float y = exp(n*log(abs(point.y)));
    return (x + y) <= 1;
}
