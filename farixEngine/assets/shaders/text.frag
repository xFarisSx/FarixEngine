
#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec4 textColor;

void main()
{
    FragColor = texture(textTexture, TexCoord);
}
