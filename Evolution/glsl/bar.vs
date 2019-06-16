layout (location = 0) in vec3 position;
layout (location = 1) in float ratio;
layout (location = 2) in vec2 shape;

out vec4 a;

uniform vec4 color;
uniform vec2 scl;
uniform vec2 offset;

void main() {
    a = color;
    const float _width = 0.125f;
    vec2 local_offset = vec2((ratio - 1.0f) * position.z, position.z * (1.0f + _width));
    gl_Position = vec4((position.xy + (shape * vec2(ratio, position.z * _width * 0.75f) + local_offset) + offset) * scl, 0.0f, 1.0f);
}
