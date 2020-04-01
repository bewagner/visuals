#version 150

#ifdef GL_ES
precision mediump float;
#endif

uniform float ciElapsedSeconds;
uniform vec2 uResolution;
uniform vec2 mousePosition;
uniform vec4 ciPosition;
in vec4 position;

out vec4 oColor;



#define PI 3.1415926535897932384626433832795

//this is a basic Pseudo Random Number Generator
float hash(in float n)
{
    return fract(sin(n)*43758.5453123);
}

void main(void)
{

    vec2 xy = (2.* gl_FragCoord.xy - uResolution.xy) / uResolution.y;
    //rotating light
    vec3 center = vec3(sin(ciElapsedSeconds), 1., cos(ciElapsedSeconds * .5));

    //temporary vector
    vec3 pp = vec3(0.);

    //maximum distance of the surface to the center (try a value of 0.1 for example)
    float length = 4.;

    //this is the number of cells
    const float count = 13.;
    for( float i = 0.; i < count; i+=1. )
    {
        //random cell: create a point around the center

        //gets a 'random' angle around the center
        float an = sin( ciElapsedSeconds * PI * .00001 ) - hash( i ) * PI * 2.;

        //gets a 'random' radius ( the 'spacing' between cells )
        float ra = sqrt( hash( an ) ) * .5;

        //creates a temporary 2d vector
        vec2 p = vec2( center.x + cos( an ) * ra, center.z + sin( an ) * ra );

        //finds the closest cell from the fragment's XY coords

        //compute the distance from this cell to the fragment's coordinates
        float di = distance( xy, p );

        //and check if this length is inferior to the minimum length
        length = min( length, di );

        //if this cell was the closest
        if( length == di )
        {
            //stores the XY values of the cell and compute a 'Z' according to them
            pp.xy = p;
            pp.z = i / count * xy.x * xy.y;
        }
    }

    //shimmy shake:
    //uses the temp vector's coordinates and uses the angle and the temp vector
    //to create light & shadow (quick & dirty )
    vec3 shade = vec3( 1. ) * ( 1. - max( 0.0, dot( pp, center ) ) );


    oColor=vec4( pp + shade, 1. );


}