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
    vec2 distance_to_face = pow((gl_FragCoord.xy / uResolution) - face_position, vec2(2.));

    if (distance_to_face.x + distance_to_face.y < 0.01){
        oColor = vec4(0.);
        oColor.a = 1.;
    } else {
        oColor = vec4(1.);
    }


}