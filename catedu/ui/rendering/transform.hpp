#pragma once
#include "catedu/core/math/math.hpp"
#include "core.hpp"
#include <vector>

struct UiTransform
{
    Rect base;
    Vector2 origin;
    Vector2 scale;
    Vector2 translate;
    float rotation;

    Matrix4 scalem_;
    Matrix4 rotationm_;
    Matrix4 translationm_;
    Matrix4 combinem_;

    void stack(UiTransform transform);
    Matrix4 world_matrix();
    Matrix4 calculate_matrix(Vector2 screen_size);
};

struct UiTransformer
{
    UiRenderingCore *core;
    std::vector<UiTransform> transforms;
    Vector2 screen_size;

    static UiTransformer init(UiRenderingCore *core, Vector2 screen_size);
    void push(UiTransform transform);
    void pop();

    Vector2 transform_point(Vector2 point);
};
