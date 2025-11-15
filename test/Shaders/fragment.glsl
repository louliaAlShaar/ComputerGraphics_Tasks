#version 330 core
in vec2 TexCoord;
in vec4 VertexColor;
in vec3 FragPos;   // <-- ãä ÇáÝíÑÊßÓ
in vec3 Normal;    // <-- ãä ÇáÝíÑÊßÓ

out vec4 FragColor;

uniform bool useTexture;
uniform sampler2D tex;
uniform bool useVertexColor;
uniform vec4 objectColor;

// --------- ÇáÅÖÇÁÉ ÇáÌÏíÏÉ ---------
uniform vec3 lightPos;    // <-- ãæÞÚ ÇáÖæÁ
uniform vec3 lightColor;  // <-- áæä ÇáÖæÁ
uniform vec3 viewPos;     // <-- ãæÞÚ ÇáßÇãíÑÇ
uniform float ambientStrength; // <-- ÞæÉ ambient
uniform float specularStrength; // <-- ÞæÉ specular
uniform float shininess;        // <-- ÈÑíÞ ÇáÓÈíßæáÇÑ
// -----------------------------------

void main()
{
    vec4 baseColor = objectColor;


    if (useVertexColor)
        baseColor = VertexColor;

    if (useTexture)
        baseColor = texture(tex, TexCoord);


    // ----------------- ÇáÅÖÇÁÉ: Phong -----------------

    // Normalize normal
    vec3 norm = normalize(Normal);

    // Light direction
    vec3 lightDir = normalize(lightPos - FragPos);

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0) * baseColor;
}
