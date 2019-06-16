layout (location = 0) in vec4 position;
layout (location = 1) in vec2 sizes;
layout (location = 2) in vec2 shape;

out vec4 a;

uniform vec4 color;
uniform vec2 scl;
uniform vec2 offset;

void main() {
    a = color;
    vec2 shape1 = sizes.y * shape;
    shape1.y += sign(shape1.y) * sizes.x * 0.5f;
    vec2 shape2 = vec2(shape1.x * position.z - shape1.y * position.w, shape1.x * position.w + shape1.y * position.z);
    gl_Position = vec4((position.xy + shape2 + offset) * scl, 0.0f, 1.0f);
}
