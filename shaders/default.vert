varying vec3 vertex;
void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vertex = (gl_ModelViewMatrix*gl_Vertex).xyz;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}