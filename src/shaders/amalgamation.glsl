@ctype mat4 Matrix4
@ctype vec4 Vector4

@vs vs
uniform samalg_vs_params {
  mat4 mvp;
  vec4 top_color;
  vec4 bottom_color;
};

in vec4 position;
in float isbottomvertex;

out vec4 color;

void main() {
  color = top_color;
  if (isbottomvertex > 0.5) {
    color = bottom_color;
  }

  gl_Position = mvp * position;
}
@end

@fs fs
in vec4 color;
out vec4 frag_color;

void main() {
  frag_color = color;
}
@end

@program samalg vs fs