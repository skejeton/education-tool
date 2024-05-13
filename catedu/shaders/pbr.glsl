////////////////////////////////////////////////////////////////////////////////
@vs pbr_vs
uniform pbr_vs_params {
    vec4 color_mul;
    mat4 model;
    mat4 viewproj;
    float lightness;
};

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 color;
out vec2 frag_uv;
out vec3 frag_normal;

void main() {
    gl_Position = viewproj * model * vec4(position, 1.0);

    vec3 light_dir = -normalize(vec3(0.4, -1.3, 1.0));
    vec4 light_color = vec4(0.7, 0.5, 1.0, 1.0);
    vec4 ambient_color = vec4(0.7, 0.7, 0.9, 1.0);
    vec3 transformed_normal = normalize((model * vec4(normal, 0.0)).xyz);
    float dp = dot(light_dir, transformed_normal);
    vec4 all = vec4(dp, dp, dp, 1) * light_color + ambient_color;

    color = color_mul*mix(all, vec4(1, 1, 1, 1), lightness);

    frag_uv = uv;
    frag_normal = normal;
}

@end
////////////////////////////////////////////////////////////////////////////////
@fs pbr_fs

uniform texture2D image;
uniform sampler image_sampler;
in vec4 color;
in vec2 frag_uv;
in vec3 frag_normal;
out vec4 frag_color;

void main() {
    vec4 sample_value = texture(sampler2D(image, image_sampler), frag_uv);
    frag_color = color * sample_value;
}
@end
////////////////////////////////////////////////////////////////////////////////
@program pbr_prog pbr_vs pbr_fs

