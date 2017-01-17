 #version 330 core
 in vec4 o_color;

 layout( location = 0 ) out vec4 o_fragColor;
 layout( std140 ) uniform ColorPrefix {
     float r, g;
 };
 layout( std140 ) uniform ColorSuffix {
     float b, a;
 };

 void main() {
     o_fragColor = vec4( r, g, b, a ) * vec4( o_color );
 }