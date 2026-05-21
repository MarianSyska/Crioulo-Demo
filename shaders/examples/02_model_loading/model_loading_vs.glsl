#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoords;

layout (location = 0) uniform mat4 uModel;

layout (std140) uniform uTransformMatrices {
    mat4 uView;
    mat4 uProjection;
};

void main()
{
    vWorldPos = (uModel * vec4(aPos, 1.0)).xyz;
    vNormal = transpose(inverse(mat3(uModel))) * aNormal;
    vTexCoords = aTexCoords;    
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}