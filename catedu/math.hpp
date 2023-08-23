///
/// BRIEF: Collection of math types and functions
///

#ifndef H_MATH_CATEDU
#define H_MATH_CATEDU
#include <cmath>
#define MATH_TAU 6.283185307179586
#define MATH_EPSILON 0.00001

struct Vector2 {
    union {
        struct {
            float x, y;
        };
        float values[2];
    };
};

inline Vector2 operator-(Vector2 a) {
    return {-a.x, -a.y};
}

inline Vector2 operator+(Vector2 a, Vector2 b) {
    return {a.x+b.x, a.y+b.y};
}

inline Vector2 operator+=(Vector2 &a, Vector2 b) {
    return a = {a.x+b.x, a.y+b.y};
}

inline Vector2 operator-(Vector2 a, Vector2 b) {
    return {a.x-b.x, a.y-b.y};
}

inline Vector2 operator-=(Vector2 &a, Vector2 b) {
    return a = {a.x-b.x, a.y-b.y};
}

inline Vector2 operator*(Vector2 a, Vector2 b) {
    return {a.x*b.x, a.y*b.y};
}

inline Vector2 operator*=(Vector2 &a, Vector2 b) {
    return a = {a.x*b.x, a.y*b.y};
}

inline Vector2 operator/(Vector2 a, Vector2 b) {
    return {a.x/b.x, a.y/b.y};
}

inline Vector2 operator/(Vector2 a, float b) {
    return {a.x/b, a.y/b};
}

inline Vector2 operator/=(Vector2 &a, Vector2 b) {
    return a = {a.x/b.x, a.y/b.y};
}

struct Vector3 {
    union {
        struct {
            float x, y, z;
        };
        float values[3];
    };
};


struct Vector4 {
    union {
        struct {
            float x, y, z, w;
        };
        float values[4];
    };
};


struct Ray3 {
    Vector3 origin;
    Vector3 direction;
};


struct Box3 {
    Vector3 min;
    Vector3 max;
};


struct Rect {
    Vector2 pos;
    Vector2 siz;
};

inline Vector3 operator+(Vector3 a, Vector3 b) {
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}


inline Vector3 operator-(Vector3 a, Vector3 b) {
    return {a.x-b.x, a.y-b.y, a.z-b.z};
}


inline Vector3 operator*(Vector3 a, Vector3 b) {
    return {a.x*b.x, a.y*b.y, a.z*b.z};
}


inline Vector3 operator/(Vector3 a, Vector3 b) {
    return {a.x/b.x, a.y/b.y, a.z/b.z};
}


inline Vector3 operator*(Vector3 a, float b) {
    return {a.x*b, a.y*b, a.z*b};
}


inline Vector3 operator/(Vector3 a, float b) {
    return {a.x/b, a.y/b, a.z/b};
}


inline Vector3 operator+=(Vector3 &a, Vector3 b) {
    return a = {a.x+b.x, a.y+b.y, a.z+b.z};
}


inline Vector3 operator-=(Vector3 &a, Vector3 b) {
    return a = {a.x-b.x, a.y-b.y, a.z-b.z};
}


inline Vector3 operator*=(Vector3 &a, Vector3 b) {
    return a = {a.x*b.x, a.y*b.y, a.z*b.z};
}


inline Vector3 operator/=(Vector3 &a, Vector3 b) {
    return a = {a.x/b.x, a.y/b.y, a.z/b.z};
}


inline Vector3 operator*=(Vector3 &a, float b) {
    return a = {a.x*b, a.y*b, a.z*b};
}


inline Vector3 operator/=(Vector3 &a, float b) {
    return a = {a.x/b, a.y/b, a.z/b};
}


inline float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


inline float vector3_length(Vector3 a) {
    return sqrt(vector3_dot(a, a));
}


inline Vector3 vector3_normalize(Vector3 a) {
    return a / vector3_length(a);
}


inline Vector3 vector3_cross(Vector3 a, Vector3 b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

inline float vector2_dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

inline float vector2_length(Vector2 a) {
    return sqrt(vector2_dot(a, a));
}

inline Vector2 vector2_normalize(Vector2 a) {
    return a / vector2_length(a);
}


inline Vector4 operator*(Vector4 a, Vector4 b) {
    return {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
}

inline Box3 box3_extrude_from_point_volume(Vector3 point, Vector3 extrude) {
    extrude /= 2;

    Vector3 min = point;
    min.x -= extrude.x;
    min.y -= extrude.y;
    min.z -= extrude.z;
    Vector3 max = point;
    max.x += extrude.x;
    max.y += extrude.y;
    max.z += extrude.z;

    return {min, max};
}


inline Box3 box3_extrude_from_point(Vector3 point, Vector3 extrude) {
    Vector3 min = point;
    min.x -= extrude.x;
    min.y -= extrude.y;
    min.z -= extrude.z;
    Vector3 max = point;
    max.x += extrude.x;
    max.y += extrude.y;
    max.z += extrude.z;

    return {min, max};
}


inline bool vector3_vs_box3(Vector3 vector, Box3 box) {
    bool x_plane = vector.x >= box.min.x && vector.x <= box.max.x;
    bool y_plane = vector.y >= box.min.y && vector.y <= box.max.y;
    bool z_plane = vector.z >= box.min.z && vector.z <= box.max.z;
    return x_plane && y_plane && z_plane;
}

inline Box3 box3_translate(Box3 box, Vector3 vector) {
    box.min.x += vector.x;
    box.min.y += vector.y;
    box.min.z += vector.z;

    box.max.x += vector.x;
    box.max.y += vector.y;
    box.max.z += vector.z;

    return box;
}

// NOTE: The "foot" of the player
inline Vector3 box3_base_midpoint(Box3 box) {
    Vector3 pos = {};
    pos.x = (box.max.x - box.min.x) / 2;
    pos.y = box.min.y;
    pos.z = (box.max.z - box.min.z) / 2;

    return pos;
}

inline Vector3 box3_center(Box3 box) {
    Vector3 pos = {};
    pos.x = (box.max.x - box.min.x) / 2;
    pos.y = (box.max.y - box.min.y) / 2;
    pos.z = (box.max.z - box.min.z) / 2;

    return pos;
}


// NOTE: The "eyes" of the player
inline Vector3 box3_eyes_midpoint(Box3 box) {
    Vector3 pos = {};
    pos.x = (box.max.x - box.min.x) / 2;
    pos.y = box.max.y;
    pos.z = (box.max.z - box.min.z) / 2;

    return pos;
}


inline bool rect_vs_vector2(Rect rect, Vector2 point) {
    return
            point.x >= rect.pos.x && point.y >= rect.pos.y &&
            point.x < (rect.pos.x+rect.siz.x) && point.y < (rect.pos.y+rect.siz.y);
}


inline bool rect_vs_rect(Rect r1, Rect r2) {
    return (r1.pos.x + r1.siz.x) > (r2.pos.x) && (r1.pos.y + r1.siz.y) > (r2.pos.y)
           && (r1.pos.x) < (r2.pos.x + r2.siz.x) && (r1.pos.y) < (r2.pos.y + r2.siz.y);
}


inline Rect rect_from_min_max(Vector2 min, Vector2 max) {
    return {min.x, min.y, max.x-min.x, max.y-min.y};
}


inline Vector2 rect_vs_rect_snap(Rect r, Rect along) {
    float x_left = r.pos.x + r.siz.x - along.pos.x;
    float y_up = r.pos.y + r.siz.y - along.pos.y;

    float x_right = along.pos.x + along.siz.x - r.pos.x;
    float y_down = along.pos.y + along.siz.y - r.pos.y;

    float min_x = 0, min_y = 0;

    if (fabs(x_left) < fabs(x_right)) {
        min_x = -x_left;
    } else {
        min_x = x_right;
    }

    if (fabs(y_up) < fabs(y_down)) {
        min_y = -y_up;
    } else {
        min_y = y_down;
    }

    if (fabs(min_x) < fabs(min_y)) {
        return {min_x, 0};
    }
    return {0, min_y};
}


inline bool ray3_vs_box3(Ray3 r, Box3 b, float max_distance, float *distance)
{
    r.direction = vector3_normalize(r.direction);
    Vector3 dirfrac;
    dirfrac.x = 1.0f / r.direction.x;
    dirfrac.y = 1.0f / r.direction.y;
    dirfrac.z = 1.0f / r.direction.z;
    float t1 = (b.min.x - r.origin.x)*dirfrac.x;
    float t2 = (b.max.x - r.origin.x)*dirfrac.x;
    float t3 = (b.min.y - r.origin.y)*dirfrac.y;
    float t4 = (b.max.y - r.origin.y)*dirfrac.y;
    float t5 = (b.min.z - r.origin.z)*dirfrac.z;
    float t6 = (b.max.z - r.origin.z)*dirfrac.z;

    float tmin = fmax(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
    float tmax = fmin(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

    if (tmax < 0) {
        *distance = tmax;
        return false;
    }

    if (tmin > tmax) {
        *distance = tmax;
        return false;
    }

    *distance = tmin;

    return true;
}

inline bool ray3_vs_horizontal_plane(Ray3 r, float y, float *t)
{
    if (r.origin.y < y) {
        if (r.direction.y > 0) {
            *t = (y - r.origin.y) / r.direction.y;
            return true;
        }
    } else if (r.origin.y > y) {
        if (r.direction.y < 0) {
            *t = (r.origin.y - y) / -r.direction.y;
            return true;
        }
    }

    return false;
}

inline Vector3 ray3_at(Ray3 r, float t)
{
    return r.origin + r.direction * t;
}

struct Matrix4 {
    float values[16];


    static Matrix4 identity() {
        return {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        };
    }


    static Matrix4 perspective(float fov, float aspect, float near, float far) {
        float height = 1 / tanf(fov / 2);
        float width = height / aspect;

        return {
                width, 0,      0,              0,
                0,     height, 0,              0,
                0,     0,      (near + far) / (near - far),         -1.0f,
                0,     0,     (2.0f * near * far) / (near - far ), 0
        };
    }

    static Matrix4 ortho(float left, float right, float bottom, float top, float near, float far) {
        return {
                2.0f / (right - left), 0,                     0,                    0,
                0,                     2.0f / (top - bottom), 0,                    0,
                0,                     0,                     2.0f / (near - far),  0,
                (left + right) / (left - right), (bottom + top) / (bottom - top), (near + far) / (near - far), 1
        };
    }

    static Matrix4 look_at(Vector3 center, Vector3 eye, Vector3 up) {
        Vector3 z = vector3_normalize(center - eye);
        Vector3 x = vector3_normalize(vector3_cross(z, up));
        Vector3 y = vector3_cross(x, z);

        float xdot = -vector3_dot(x, center);
        float ydot = -vector3_dot(y, center);
        float zdot = -vector3_dot(z, center);

        return {
                x.x,  y.x,  -z.x,  0,
                x.y,  y.y,  -z.y,  0,
                x.z,  y.z,  -z.z,  0,
                xdot, ydot, -zdot, 1
        };
    }


    static Matrix4 translate(Vector3 center) {
        return {
                1,        0,        0,        0,
                0,        1,        0,        0,
                0,        0,        1,        0,
                center.x, center.y, center.z, 1
        };
    }


    static Matrix4 scale(float factor) {
        return {
                factor,   0,        0,        0,
                0,        factor,   0,        0,
                0,        0,        factor,   0,
                0,        0,        0,        1
        };
    }


    static Matrix4 scale_v(Vector3 factor) {
        return {
                factor.x, 0,        0,        0,
                0,        factor.y, 0,        0,
                0,        0,        factor.z, 0,
                0,        0,        0,        1
        };
    }


    static Matrix4 rotate_x(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f,    c,    s, 0.0f,
                0.0f,   -s,    c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
        };
    }


    static Matrix4 rotate_y(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return {
                c, 0.0f,   -s, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                s, 0.0f,    c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }


    static Matrix4 rotate_z(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        return {
                c,    s, 0.0f, 0.0f,
                -s,    c, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }
};


inline Matrix4 operator*(Matrix4 a, Matrix4 b) {
    Matrix4 result = {};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.values[i*4+j] += a.values[k*4+j] * b.values[i*4+k];
            }
        }
    }

    return result;
}


inline Vector3 operator*(Matrix4 a, Vector3 b) {
    float data[4] = {b.x, b.y, b.z, 1.0};
    float ret[4] = {};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ret[i] += a.values[j*4+i] * data[j];
        }
    }

    return {ret[0], ret[1], ret[2]};
}

#endif // H_MATH_CATEDU
