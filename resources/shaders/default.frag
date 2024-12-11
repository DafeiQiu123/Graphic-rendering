#version 330 core
struct GlobalData{
    float ka;
    float kd;
    float ks;
    float kt;
};
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 worldPos;
in vec3 worldNormal;
in vec2 fragTexCoord;
in vec4 ShadowCoord;

uniform GlobalData globalData;
uniform Material material;
uniform int numLights;
uniform vec3 camPos;
uniform vec3 lightDirection[8];
uniform vec3 lightColor[8];
uniform vec3 lightFunction[8];
uniform vec3 lightPos[8];
uniform int lightType[8];
uniform float lightAngle[8];
uniform float lightPenumbra[8];


uniform float blend;
uniform bool isTexture;
uniform sampler2D textureSampler;
uniform bool shadow_on;
uniform sampler2D shadowMap;

out vec4 fragColor;

bool ShadowCalculation() {
    vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    return currentDepth - bias > closestDepth;
}

void main() {
    vec3 directionToCamera = normalize(camPos - worldPos);
    vec3 reWorldNormal = normalize(worldNormal);
    vec3 result = vec3(0.0f);
    vec3 ambientTerm = material.ambient * globalData.ka;
    result += ambientTerm;
    for (int i = 0; i < numLights; ++i) {
        vec3 actualLight = lightColor[i];
        vec3 directionToLight;
        float closeness, attenuation;
        // Directional Light
        if (lightType[i] == 0) {
            directionToLight = -lightDirection[i];
            attenuation = 1.0f;
        } else {
            directionToLight = normalize(lightPos[i] - worldPos);
            float dist = distance(worldPos, lightPos[i]);
            attenuation = min(1.0f, 1.0f / dot(lightFunction[i], vec3(1.0f, dist, dist * dist)));
            // Point Light
            if (lightType[i] == 1) {
            // Spot Light
            } else {
                float angleX = abs(acos(max(dot(-directionToLight, lightDirection[i]), 0.0f)));
                float inner = lightAngle[i] - lightPenumbra[i];
                float outer = lightAngle[i];
                if (angleX > outer) actualLight = vec3(0.0f);
                else if (angleX <= inner) actualLight = lightColor[i];
                else {
                    float factor = (angleX - inner) / (outer - inner);
                    float falloff = -2.0f * factor * factor * factor + 3.0f * factor * factor;
                    actualLight *= (1.0f - falloff);
                }
            }
        }

        float Lambert = max(dot(reWorldNormal, directionToLight), 0.0f);
        vec3 diffuseColor = material.diffuse * globalData.kd;
        if (isTexture){
            diffuseColor = blend * vec3(texture(textureSampler, fragTexCoord)) + (1 - blend) * diffuseColor;
        }
        vec3 diffuseTerm = attenuation * Lambert * diffuseColor * actualLight;

        vec3 reflectDir = reflect(-directionToLight, reWorldNormal);
        closeness = dot(reflectDir, directionToCamera);
        float powResult;
        if (closeness < 0.0f || (closeness == 0.0f && material.shininess <= 0.0f)) powResult = 0.0f;
        else powResult = pow(max(closeness, 0.0f), material.shininess);
        vec3 specularTerm = attenuation * powResult * material.specular * globalData.ks * actualLight;
        if (lightType[i] == 0 && shadow_on) {
            bool inShadow = ShadowCalculation();
            if (!inShadow) result += diffuseTerm + specularTerm;
        } else {
            result += diffuseTerm + specularTerm;
        }
    }
    fragColor = vec4(result, 1.0f);
}
