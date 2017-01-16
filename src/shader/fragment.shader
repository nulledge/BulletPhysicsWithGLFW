 #version 330 core
 in vec4 o_color;

 layout( location = 0 ) out vec4 o_fragColor;

 void main() {
     o_fragColor = vec4( o_color );
 }