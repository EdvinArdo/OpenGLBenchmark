precision mediump float;
varying vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    gl_FragColor = texture2D(ourTexture, TexCoord);
}