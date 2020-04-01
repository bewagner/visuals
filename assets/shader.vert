#version 150

// Cinder will automatically send the default matrices and attributes to our shader.
uniform mat4 ciModelViewProjection;
in vec4 ciPosition;



void main(void)
{
	// Transform the vertex from object space to '2D space'
	// and pass it to the rasterizer.
	gl_Position = ciModelViewProjection * ciPosition;
}