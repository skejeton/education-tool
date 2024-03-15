#include "transform.hpp"

UiTransform default_transform()
{
    UiTransform result = {};
    result.scale = {1.0f, 1.0f};
    result.scalem_ = Matrix4::identity();
    result.rotationm_ = Matrix4::identity();
    result.translationm_ = Matrix4::identity();
    result.combinem_ = Matrix4::identity();
    return result;
}

static Matrix4 pivoted_at(Matrix4 mat, Matrix4 parent, Vector2 pivot)
{
    Matrix4 result = Matrix4::identity();

    Vector3 p = parent * Vector3{pivot.x, pivot.y, 0.0f};

    result *= Matrix4::translate(p);
    result *= mat;
    result *= Matrix4::translate(p * -1);

    return result;
}

static Matrix4 pivoted_at_relative(Matrix4 mat, Matrix4 parent,
                                   UiTransform &transform)
{
    Vector2 pivot = {transform.base.pos.x + transform.translate.x +
                         transform.base.siz.x * transform.origin.x,
                     -(transform.base.pos.y + transform.translate.y +
                       transform.base.siz.y * transform.origin.y)};

    return pivoted_at(mat, parent, pivot);
}

Matrix4 UiTransform::calculate_matrix(Vector2 screen_size)
{
    Matrix4 matrix = Matrix4::identity();
    // And then convert it into screen space coordinates.
    matrix *= Matrix4::translate({-1.0f, 1.0f});
    matrix *=
        Matrix4::scale_v({2.0f / screen_size.x, 2.0f / screen_size.y, 1.0f});

    // Then we apply the transforms.
    matrix *= this->combinem_;

    // First we scale the rect to the size of the texture so we can make use
    // of absolute pivots.
    matrix *= Matrix4::scale_v({this->base.siz.x, this->base.siz.y, 1.0f});
    matrix *= Matrix4::translate({0.5, -0.5f});
    return matrix;
}

void UiTransform::stack(UiTransform previous)
{
    Matrix4 parent = previous.combinem_;

    this->scalem_ = pivoted_at_relative(
        Matrix4::scale_v({this->scale.x, this->scale.y, 1.0f}), parent, *this);
    this->rotationm_ = pivoted_at_relative(
        Matrix4::rotate_z(-this->rotation * (MATH_TAU / 360)), parent, *this);
    this->translationm_ =
        Matrix4::translate({this->base.pos.x + this->translate.x,
                            -this->base.pos.y - this->translate.y, 0.0f});

    this->scalem_ *= previous.scalem_;
    this->rotationm_ *= previous.rotationm_;
    this->translationm_ *= previous.translationm_;

    Matrix4 combine =
        parent * this->scalem_ * this->rotationm_ * this->translationm_;

    this->combinem_ = combine;
}

UiTransformer UiTransformer::init(UiRenderingCore *core, Vector2 screen_size)
{
    UiTransformer result = {};
    result.core = core;
    result.screen_size = screen_size;
    return result;
}

void UiTransformer::push(UiTransform transform)
{
    UiTransform previous_transform = this->transforms.size() > 0
                                         ? this->transforms.back()
                                         : default_transform();
    transform.stack(previous_transform);

    this->core->matrix = transform.calculate_matrix(this->screen_size);
    this->core->rect = transform.base;

    this->transforms.push_back(transform);
}

void UiTransformer::pop()
{
    assert(this->transforms.size() > 0);
    this->transforms.pop_back();

    UiTransform *transform =
        this->transforms.size() > 0 ? &this->transforms.back() : NULL;
    this->core->matrix = transform
                             ? transform->calculate_matrix(this->screen_size)
                             : Matrix4::identity();
    this->core->rect = transform ? transform->base : Rect{0, 0, 0, 0};
}

Vector2 UiTransformer::transform_point(Vector2 point)
{
    UiTransform transform = this->transforms.size() > 0
                                ? this->transforms.back()
                                : default_transform();
    Matrix4 inv = Matrix4::identity();
    Matrix4 combined = transform.scalem_;
    combined *= transform.rotationm_;
    combined *= Matrix4::translate(
        {transform.translate.x, -transform.translate.y, 0.0f});

    combined.inverse(&inv);

    Vector3 result = inv * Vector3{point.x, -point.y, 0.0f};
    return {result.x, -result.y};
}
