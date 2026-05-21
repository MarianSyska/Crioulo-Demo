#version 450 core

#include "pbr"

in vec2 vTexCoords;
in vec3 vWorldPos;
in vec3 vNormal;

out vec4 oFragColor;

layout (std140) uniform uPointLights {
    PointLight uLigths[4]; 
};

layout (std140) uniform uSceneData {
    uint uLightCount;
    vec3 uCameraPosition;
};

uniform sampler2D uDiffuseMap1;
uniform sampler2D uMetalnessMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;
uniform sampler2D uNormalMap;

void main()
{	
    SurfaceProperties properties;

    properties.albedo = pow(texture(uDiffuseMap1, vTexCoords).rgb, vec3(2.2));
    properties.metallic = texture(uMetalnessMap, vTexCoords).r;
    properties.roughness = texture(uRoughnessMap, vTexCoords).r;
    properties.ao = texture(uAOMap, vTexCoords).r;
    properties.normal = vNormal;
    properties.worldPosition = vWorldPos;

    oFragColor =  vec4(crioulo_pbr(properties, uLigths, uLightCount, uCameraPosition), 1.0);
    
    // HDR tonemapping
    // oFragColor = oFragColor / (oFragColor + vec3(1.0));
    // gamma correct
    oFragColor = vec4(pow(oFragColor.rgb, vec3(1.0 / 2.2)), oFragColor.a); 
}