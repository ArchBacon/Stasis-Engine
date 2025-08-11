#version 330 core

out vec4 outColor;

in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D inTexture;

void main()
{
    outColor = texture(inTexture, texCoord);
}
