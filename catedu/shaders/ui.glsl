////////////////////////////////////////////////////////////////////////////////
@vs ui_vs
uniform ui_vs_params {
    mat4 mvp;
    vec4 color_top;
    vec4 color_bottom;
    vec4 color_mul;
    vec2 uv_min, uv_max;
};


in vec2 position;
in vec2 uv;
out vec4 color;
out vec2 frag_uv;
out vec2 frag_uv_min;
out vec2 frag_uv_max;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    color = mix(color_top, color_bottom, uv.y);
    frag_uv = uv;
    frag_uv_min = uv_min;
    frag_uv_max = uv_max;
}

@end
////////////////////////////////////////////////////////////////////////////////
@fs ui_fs

uniform texture2D image;
uniform sampler image_sampler;
in vec4 color;
in vec2 frag_uv;
out vec4 frag_color;
in vec2 frag_uv_min;
in vec2 frag_uv_max;

void main() {
    vec2 uv = frag_uv * (frag_uv_max - frag_uv_min) + frag_uv_min;

    vec4 sample_value = texture(sampler2D(image, image_sampler), uv);
    frag_color = sample_value * color;
}
@end
////////////////////////////////////////////////////////////////////////////////
@program ui_prog ui_vs ui_fs
