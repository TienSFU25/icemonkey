#version 330 core
in vec2 TexCoord;
in float ObjectId;
in vec3 IdentificationColor;

uniform sampler2D texture1;
uniform vec3 aColor;

out vec4 FragColor;

void main()
{
    // 1: Circles
    // 2: Rink
    // 3: Slider
    // 4: SliderHandle
    
    //    FragColor = texture(texture1, TexCoord);
    if (ObjectId == 1.0) {
        FragColor = vec4(aColor, 1.0);
    } else if (ObjectId == 2.0) {
        FragColor = texture(texture1, TexCoord);
    } else if (ObjectId == 3.0) {
        FragColor = vec4(aColor, 1.0);
    } else if (ObjectId == 4.0) {
        FragColor = vec4(aColor, 1.0);
    }
    
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
//        FragColor = texture(texture1, TexCoord);
    }
}
