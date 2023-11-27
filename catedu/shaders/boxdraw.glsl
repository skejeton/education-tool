
////////////////////////////////////////////////////////////////////////////////
@vs boxdraw_vs
uniform boxdraw_vs_params {
    mat4 mvp;
    vec4 color_top;
    vec4 color_bottom;
    vec4 color_mul;
    vec2 uv_min, uv_max;
    vec2 size;
    vec2 tile_count;
};


in vec3 position;
in vec3 normal;
in vec2 uv;
out vec4 color;
out vec2 frag_uv;
out vec2 frag_uv_min;
out vec2 frag_uv_max;
out vec2 frag_size;
out vec2 frag_tile_count;

void main() {
    gl_Position = mvp * vec4(position, 1.0);


    vec3 light_dir = -normalize(vec3(0.4, -1.3, 1.0));
    vec4 light_color = vec4(1.0, 1.0, 0.5, 1.0);
    vec4 ambient_color = vec4(0.1, 0.1, 0.5, 1.0);
    float dp = dot(light_dir, normal);
    vec4 all = vec4(dp, dp, dp, 1) * light_color + ambient_color;

    color = mix(color_top, color_bottom, -position.y+0.5)*all;

    frag_uv = uv;
    frag_uv_min = uv_min;
    frag_uv_max = uv_max;
    frag_size = size;
    frag_tile_count = max(tile_count, vec2(1.0, 1.0));
}

@end
////////////////////////////////////////////////////////////////////////////////
@fs boxdraw_fs

uniform texture2D image;
uniform sampler image_sampler;
in vec4 color;
in vec2 frag_uv;
out vec4 frag_color;
in vec2 frag_uv_min;
in vec2 frag_uv_max;
in vec2 frag_size;
in vec2 frag_tile_count;

void main() {
    vec2 uv = mod(frag_tile_count * frag_uv, vec2(1, 1)) * (frag_uv_max - frag_uv_min) + frag_uv_min;

    float offs_bound_x = 1.0 / frag_size.x;
    float offs_bound_y = 1.0 / frag_size.y;
    float min_bound_x = frag_uv_min.x + offs_bound_x;
    float max_bound_x = frag_uv_max.x - offs_bound_x;
    float min_bound_y = frag_uv_min.y + offs_bound_y;
    float max_bound_y = frag_uv_max.y - offs_bound_y;
    float dist = gl_FragCoord.z / gl_FragCoord.w;

    const float LOG2 = 1.442695;
    float fogFactor = pow(10, -0.03 *
                   0.03 *
                   dist *
                   dist *
                   LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    uv = clamp(uv, vec2(min_bound_x, min_bound_y), vec2(max_bound_x, max_bound_y));

    vec4 sample_value = texture(sampler2D(image, image_sampler), uv);
    frag_color = mix(vec4(0.7, 0.7, 0.5, 1.0), sample_value * color, fogFactor);
}
@end
////////////////////////////////////////////////////////////////////////////////
@program boxdraw_prog boxdraw_vs boxdraw_fs

