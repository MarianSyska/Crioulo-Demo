#ifndef NODE_H
#define NODE_H

#include <glm/mat4x4.hpp>
#include <list>
#include <memory>
#include <map>
#include <string>

class Node 
{
    public:

        Node();
        Node(Node* node);

        void attachChild(Node* node);
        void detach();

        void rotate(glm::vec3 axis, float radians);
        
        void translate(glm::vec3 value);
        void scale(glm::vec3 value);
        glm::mat4 getTransform();
        glm::mat4 getGlobalTransform();
        void setTransform(const glm::mat4& transform);
        void setGlobalTransform(const glm::mat4& transform);

    private:
        Node* m_parent;
        std::map<std::string, Node*> m_children;
        std::string m_name;

        glm::mat4 m_localTransform;
        glm::mat4 m_globalTransform;

        void _adjustName(Node* node);
        void _propagateTransformChange();
};
#endif