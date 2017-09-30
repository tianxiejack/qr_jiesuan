#version 330
in vec4 vVertex; 
in vec2 vTexCoords;
smooth out vec2 vVaryingTexCoords;
smooth out vec2 vVaryingPostion;

void main(void)
{
    gl_Position = vVertex;
    vVaryingPostion.xy = vVertex.xy;
    vVaryingTexCoords = vTexCoords;
}
