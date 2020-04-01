#version 150

#ifdef GL_ES
precision mediump float;
#endif

uniform float ciElapsedSeconds;
uniform vec2 uResolution;
uniform vec2 facePosition;
uniform vec4 ciPosition;
out vec4 oColor;




void main(void)
{
    vec2 face_position = vec2(1.-facePosition.x, 1.- facePosition.y);
    float distance_to_face = distance((gl_FragCoord.xy / uResolution), face_position);

    if (distance_to_face < 0.01){
        oColor = vec4(0.);
        oColor.a = 1.;
    } else {
        oColor = vec4(1.);
    }


}