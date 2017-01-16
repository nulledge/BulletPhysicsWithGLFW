#version 330 core
in vec3 in_position;
in vec4 in_color;

out vec4 o_color;

void main () {
    gl_Position = vec4( in_position.xy, 0.0, 1.0 );
    o_color = in_color;
}