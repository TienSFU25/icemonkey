#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in float ObjectId;

uniform sampler2D texture1;
uniform vec3 aColor;

void main()
{
    // 1: Circles
    // 2: Rink
    // 3: Slider
    
    //    FragColor = texture(texture1, TexCoord);
    if (ObjectId == 1.0) {
        FragColor = vec4(aColor, 1.0);
    } else if (ObjectId == 2.0) {
        FragColor = texture(texture1, TexCoord);
    } else if (ObjectId == 3.0) {
        FragColor = vec4(aColor, 1.0);
    } else {
        //        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        FragColor = texture(texture1, TexCoord);
    }
}
