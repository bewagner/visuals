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


#define PI 3.1415926535897932384626433832795

//this is a basic Pseudo Random Number Generator
float hash(in float n)
{
    return fract(sin(n)*43758.5453123);
}

void main(void)
{
    //    vec2 face_position = vec2(1.-facePosition.x, 1.- facePosition.y);
    //    vec2 normalized_fragment_coordinate= gl_FragCoord.xy / uResolution;
    //    float global_distance_to_nearest_eye = 1000;
    //    for (int i = 0; i< MAX_NUMBER_OF_EYE_PAIRS; ++i) {
    //        PairOfEyes eye_pair = uEyePairs[i];
    //        if (eye_pair.left_x < 0 && eye_pair.left_y < 0 && eye_pair.right_x < 0 && eye_pair.right_y < 0){
    //            break;
    //        }
    //
    //        float current_distance_to_left_eye = distance(normalized_fragment_coordinate, vec2(eye_pair.left_x, eye_pair.left_y));
    //        float current_distance_to_right_eye = distance(normalized_fragment_coordinate, vec2(eye_pair.right_x, eye_pair.right_y));
    //        if (current_distance_to_left_eye < global_distance_to_nearest_eye){
    //            global_distance_to_nearest_eye = current_distance_to_left_eye;
    //        }
    //        if (current_distance_to_right_eye < global_distance_to_nearest_eye){
    //            global_distance_to_nearest_eye = current_distance_to_right_eye;
    //        }
    //    }
    //
    //
    //    if (global_distance_to_nearest_eye < 0.01){
    //        oColor = vec4(0.);
    //        oColor.a = 1.;
    //    } else {
    //        oColor = vec4(1.);
    //    }


    //"squarified" coordinates
    //    vec2 xy = (2.* gl_FragCoord.xy - uResolution.xy) / uResolution.y;
    vec2 xy =  gl_FragCoord.xy /  uResolution;

    //rotating light
    PairOfEyes first_face = uEyePairs[0];
    vec3 center;
    if (first_face.left_x<0 && first_face.left_y <0) {
        center = vec3(sin(ciElapsedSeconds), 1., cos(ciElapsedSeconds * .5));
    } else {
        center = vec3(first_face.left_x, 1., first_face.left_y);
    }

    //temporary vector
    vec3 pp = vec3(0.);

    //maximum distance of the surface to the center (try a value of 0.1 for example)
    float length = 4.;

    //this is the number of cells
    const float count = 100.;

    for (float i = 0.; i < count; i+=1.)
    {
        //random cell: create a point around the center

        //gets a 'random' angle around the center
        float an = sin(ciElapsedSeconds * PI * .00001) - hash(i) * PI * 2.;

        //gets a 'random' radius ( the 'spacing' between cells )
        float ra = sqrt(hash(an)) * .5;

        //creates a temporary 2d vector
        vec2 p = vec2(center.x + cos(an) * ra, center.z + sin(an) * ra);

        //finds the closest cell from the fragment's XY coords

        //compute the distance from this cell to the fragment's coordinates
        float di = distance(xy, p);

        //and check if this length is inferior to the minimum length
        length = min(length, di);

        //if this cell was the closest
        if (length == di)
        {
            //stores the XY values of the cell and compute a 'Z' according to them
            pp.xy = p;
            pp.z = i / count * xy.x * xy.y;
        }
    }

    //shimmy shake:
    //uses the temp vector's coordinates and uses the angle and the temp vector
    //to create light & shadow (quick & dirty )
    vec3 shade = vec3(1.) * (1. - max(0.0, dot(pp, center)));

    //final color
    oColor = vec4(pp + shade, 1.);


}