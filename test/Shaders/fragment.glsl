#version 330 core
in vec2 TexCoord;
in vec4 VertexColor;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

// ---------------- Material & Textures ----------------
uniform bool useTexture;
uniform sampler2D tex;
uniform bool useVertexColor;
uniform vec4 objectColor;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;
uniform float specularStrength;
uniform float ambientStrength;

// ---------------- Viewer ----------------
uniform vec3 viewPos;

// ---------------- Structs ----------------
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

// ---------------- Function Prototypes ----------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// ---------------- Main ----------------
void main()
{
    vec4 baseColor = objectColor;
    if(useVertexColor)
        baseColor = VertexColor;
    if(useTexture)
        baseColor = texture(tex, TexCoord);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // Directional Light
    result += CalcDirLight(dirLight, norm, viewDir);

    // Point Lights
    for(int i=0; i<NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // Spotlight / Flashlight
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0) * baseColor;
}

// ---------------- Lighting Functions ----------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoord));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoord));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoord));

    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
