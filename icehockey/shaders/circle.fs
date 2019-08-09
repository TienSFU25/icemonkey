#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 aColor;

void main()
{
    if (TexCoord.x < 0.5f) {
        FragColor = texture(texture1, vec2(TexCoord.x * 2, TexCoord.y));
    } else {
        FragColor = vec4(aColor, 1.0);
    }
//    FragColor = texture(texture1, TexCoord);
}
