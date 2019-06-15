layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 shape;

out vec4 a;

uniform vec2 scl;
uniform vec2 offset;

void main() {
    a = color;
    gl_Position = vec4((position.xy + position.z * shape + offset) * scl, 0.0f, 1.0f);
}
