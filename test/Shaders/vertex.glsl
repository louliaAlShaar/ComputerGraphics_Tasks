#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec3 aNormal; // <-- ÊãÊ ÇáÅÖÇÝÉ: ÇáäæÑãÇá

out vec2 TexCoord;
out vec4 VertexColor;
out vec3 FragPos;     // <-- ÊãÊ ÇáÅÖÇÝÉ: ãæÞÚ ÇáÝÑÇÌãäÊ
out vec3 Normal;      // <-- ÊãÊ ÇáÅÖÇÝÉ: ÇÊÌÇå ÇáäæÑãÇá

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    TexCoord = aTexCoord;
    VertexColor = aColor;

    FragPos = vec3(model * vec4(aPos, 1.0));  // <-- áÅÖÇÁÉ Phong
    Normal = mat3(transpose(inverse(model))) * aNormal; // <-- ÊÕÍíÍ ÇÊÌÇå ÇáäæÑãÇá
}
