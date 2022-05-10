precision mediump float;
varying vec2 TexCoord;
varying vec3 Normal;
varying vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float shininess;

void main()
{
    vec4 diffuseTexture = texture2D(texture_diffuse1, TexCoord);
    vec4 specularTexture = texture2D(texture_specular1, TexCoord);

    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor * diffuseTexture.rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = min(max(dot(norm, lightDir), 0.0), 1.0);
    vec3 diffuse = diff * lightColor * diffuseTexture.rgb;

    // specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = min(pow(max(dot(viewDir, reflectDir), 0.0), shininess), 1.0);
    vec3 specular = specularStrength * spec * lightColor * specularTexture.rgb;

    gl_FragColor = vec4(diffuse + ambient + specular, 1.0);
}