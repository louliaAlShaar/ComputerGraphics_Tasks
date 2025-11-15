#version 330 core
in vec2 TexCoord;
in vec4 VertexColor;

out vec4 FragColor;

uniform bool useTexture;
uniform sampler2D tex;
uniform bool useVertexColor;
uniform vec4 flatColor;

void main()
{
    vec4 finalColor = flatColor;

    if (useVertexColor)
        finalColor = VertexColor;

    if (useTexture)
        finalColor = texture(tex, TexCoord);

    FragColor = finalColor;
}
