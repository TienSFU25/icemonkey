#version 330 core
in vec2 TexCoord;
in float ObjectId;
in vec3 IdentificationColor;

uniform sampler2D texture1;
uniform vec3 aColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(IdentificationColor, 1.0);
}
