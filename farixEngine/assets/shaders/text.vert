#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 uScreenSize;

void main()
{

    vec2 ndcXY = (aPos.xy / uScreenSize) * 2.0 - 1.0;
    ndcXY.y = -ndcXY.y; 
    gl_Position = vec4(ndcXY, 0.0, 1.0);

    TexCoord = aTex;
}
