#ifndef CRIOULO_DEMO_UTIL_H
#define CRIOULO_DEMO_UTIL_H

#include <string>
#include <fstream>
#include <sstream>
#include <stb_image/stb_image.h>

struct TextureData
{
    int height;
    int width;
    int nrComponents;
    unsigned char* data;
};

inline std::string loadFile(const char* filename)
{
    std::string content;
    std::ifstream file;
    std::stringstream strStream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    file.open(filename);
    strStream << file.rdbuf();
    file.close();

    content = strStream.str();

    return content;
}

inline TextureData loadTextureFromFile(const std::string& filename)
{
    //stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        std::cout << "Failure Reason: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }

    return { height, width, nrComponents, data };
}

#endif
