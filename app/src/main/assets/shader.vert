attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
attribute vec3 aTangent;
attribute vec3 aBitangent;
attribute vec4 aBoneIDs;
attribute vec4 aWeights;

varying vec2 TexCoord;
varying vec3 FragPos;
varying vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    FragPos = aPos;
    Normal = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}