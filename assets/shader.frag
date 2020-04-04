#version 150

#ifdef GL_ES
precision mediump float;
#endif


#define MAX_NUMBER_OF_EYE_PAIRS 10

struct PairOfEyes {
    float left_x;
    float left_y;
    float right_x;
    float right_y;
};

layout (std140) uniform eye_pairs {
    PairOfEyes uEyePairs[MAX_NUMBER_OF_EYE_PAIRS];
};



uniform float ciElapsedSeconds;
uniform vec2 uResolution;
uniform vec2 facePosition;
uniform vec4 ciPosition;
out vec4 oColor;




void main(void)
{
    vec2 face_position = vec2(1.-facePosition.x, 1.- facePosition.y);
    vec2 normalized_fragment_coordinate= gl_FragCoord.xy / uResolution;
    float global_distance_to_nearest_eye = 1000;
    for (int i = 0; i< MAX_NUMBER_OF_EYE_PAIRS; ++i) {
        PairOfEyes eye_pair = uEyePairs[i];
        if (eye_pair.left_x < 0 && eye_pair.left_y < 0 && eye_pair.right_x < 0 && eye_pair.right_y < 0){
            break;
        }

        float current_distance_to_left_eye = distance(normalized_fragment_coordinate, vec2(eye_pair.left_x, eye_pair.left_y));
        float current_distance_to_right_eye = distance(normalized_fragment_coordinate, vec2(eye_pair.right_x, eye_pair.right_y));
        if (current_distance_to_left_eye < global_distance_to_nearest_eye){
            global_distance_to_nearest_eye = current_distance_to_left_eye;
        }
        if (current_distance_to_right_eye < global_distance_to_nearest_eye){
            global_distance_to_nearest_eye = current_distance_to_right_eye;
        }
    }


    if (global_distance_to_nearest_eye < 0.01){
        oColor = vec4(0.);
        oColor.a = 1.;
    } else {
        oColor = vec4(1.);
    }


}