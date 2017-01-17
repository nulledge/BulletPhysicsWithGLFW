#version 330 core
in vec3 in_position;
in vec4 in_color;

out vec4 o_color;

const mat4x4 scale = mat4x4(
    1/20.0, 0.0, 0.0, 0.0,
    0.0, 1/20.0, 0.0, 0.0,
    0.0, 0.0, 1/20.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

uniform mat4x4 in_rotate;

const mat4x4 transpos = mat4x4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

void main () {
    gl_Position = transpos * in_rotate * scale * vec4( in_position.xyz, 1.0 );
    o_color = in_color;
}