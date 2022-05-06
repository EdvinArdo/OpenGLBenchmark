attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
attribute vec3 aTangent;
attribute vec3 aBitangent;
attribute vec4 aBoneIDs;
attribute vec4 aWeights;

varying vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}