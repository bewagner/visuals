#version 150

#ifdef GL_ES
precision mediump float;
#endif

uniform float ciElapsedSeconds;
uniform vec2 mousePosition;

out vec4 oColor;


#define PI 3.1415926535897932384626433832795

//this is a basic Pseudo Random Number Generator
float hash(in float n)
{
    return fract(sin(n)*43758.5453123);
}


void main(void)
{
    oColor = vec4(1.0);
    oColor.rg = mousePosition;
    oColor.b = sin((mousePosition.x + mousePosition.y)/ 2);
}