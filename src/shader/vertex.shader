#version 330 core
in vec3 in_position;
in vec4 in_color;

out vec4 o_color;

uniform mat4x4 in_mvp;

void main () {
    gl_Position = in_mvp * vec4( in_position.xyz, 1.0 );
    o_color = in_color;
}