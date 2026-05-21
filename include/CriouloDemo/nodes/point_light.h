#ifndef POTNT_LIGHT_H
#define POTNT_LIGHT_H

#include <glm/vec3.hpp>
#include "nodes/node.h"

class PointLight : public Node 
{
    public:
        float light_intensity;
        glm::vec3 color;
};
#endif