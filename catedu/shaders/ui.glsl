
////////////////////////////////////////////////////////////////////////////////
@vs ui_vs
uniform ui_vs_params {
    mat4 mvp;
    vec4 color_top;
    vec4 color_bottom;
};

in vec2 position;
in float gradient;
out vec4 color;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    color = mix(color_top, color_bottom, gradient);
}

@end
////////////////////////////////////////////////////////////////////////////////
@fs ui_fs
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end
////////////////////////////////////////////////////////////////////////////////
@program ui_prog ui_vs ui_fs
