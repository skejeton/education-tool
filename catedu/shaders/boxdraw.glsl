////////////////////////////////////////////////////////////////////////////////
@vs boxdraw_vs
uniform boxdraw_vs_params {
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
////////////////////////////////////////////////////////////////////////////////
@fs boxdraw_fs
in vec4 color;
out vec4 frag_color;

void main() {
    frag_color = color;
}
@end
////////////////////////////////////////////////////////////////////////////////
@program boxdraw_prog boxdraw_vs boxdraw_fs
