
////////////////////////////////////////////////////////////////////////////////
@vs ui_vs
uniform ui_vs_params {
    mat4 mvp;
    vec4 color_top;
    vec4 color_bottom;
};


in vec2 position;
in vec2 uv;
out vec4 color;
out vec2 frag_uv;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    color = mix(color_top, color_bottom, uv.y);
    frag_uv = uv;
}

@end
////////////////////////////////////////////////////////////////////////////////
@fs ui_fs

uniform texture2D image;
uniform sampler image_sampler;
in vec4 color;
in vec2 frag_uv;
out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(image, image_sampler), frag_uv) * color;
}
@end
////////////////////////////////////////////////////////////////////////////////
@program ui_prog ui_vs ui_fs
