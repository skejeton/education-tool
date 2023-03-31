///
/// BRIEF: Collection of math types and functions
/// 

#ifndef H_MATH_CATEDU
#define H_MATH_CATEDU
#include <math.h>
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


inline Vector2 operator+(Vector2 a, Vector2 b) {
  return a = {a.x+b.x, a.y+b.y};
}


inline Vector2 operator+=(Vector2 &a, Vector2 b) {
  return a = {a.x+b.x, a.y+b.y};
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


inline Vector4 operator*(Vector4 a, Vector4 b) {
  return {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
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

#if 1
inline bool ray3_vs_box3_slow(Ray3 r, Box3 b, float max_distance) {
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

  if (tmax < 0)
  {
    return false;
  }

  if (tmin > tmax)
  {
    return false;
  }

  return true;
}
#else

inline bool ray3_vs_box3_slow(Ray3 ray, Box3 box, float max_distance) {
  Vector3 direction = vector3_normalize(ray.direction);
  size_t step_count = 100;
  float step = max_distance/step_count;

  for (int i = 0; i < step_count; i++) {
    if (vector3_vs_box3(ray.origin, box)) {
      return true;
    }
    ray.origin += direction * step;
  }

  return false;
}
#endif


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
    float f_range = far / (far - near);

    Matrix4 result = {};
    return {
      width, 0,      0,              0,
      0,     height, 0,              0,
      0,     0,      (near + far) / (near - far),         -1.0f,
      0,     0,     (2.0f * near * far) / (near - far ), 0
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