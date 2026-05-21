#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "nodes/node.h"
#include "glm/vec3.hpp"
#include "node.h"

#define ERR_FAIL_NULL(ptr) if(ptr){ /*log*/ return; }

Node::Node()
{
    m_parent = nullptr;
    m_localTransform = glm::mat4(1);
    m_globalTransform = glm::mat4(1);
}

Node::Node(Node* parent)
{
    m_parent = parent;
    m_localTransform = glm::mat4(1);
    m_globalTransform = parent->getGlobalTransform();
}

void Node::attachChild(Node* node)
{
    ERR_FAIL_NULL(node)

    if(node->m_parent)
    {
        node->m_parent->m_children.erase(m_name);
    }

    node->m_parent = this;

    if(m_children.count(node->m_name))
    {
        _adjustName(node);
    }
    m_children.insert({node->m_name, node});
    node->m_globalTransform = node->m_localTransform * m_globalTransform;
    node->_propagateTransformChange();
}

void Node::detach()
{
    ERR_FAIL_NULL(m_parent);
    m_parent->m_children.erase(m_name);
    m_parent = nullptr;
    m_globalTransform = m_localTransform;
    _propagateTransformChange();
}

void Node::rotate(glm::vec3 axis, float radians)
{
    glm::rotate(m_localTransform, radians, axis);
    if (m_parent)
    {
        m_globalTransform = m_localTransform * m_parent->m_globalTransform;
    }
    _propagateTransformChange();
}

void Node::translate(glm::vec3 value)
{
    glm::translate(m_localTransform, value);
    if(m_parent){
        m_globalTransform = m_localTransform * m_parent->m_globalTransform;
    }
    _propagateTransformChange();
}

void Node::scale(glm::vec3 value)
{
    glm::scale(m_localTransform, value);
    if(m_parent){
        m_globalTransform = m_localTransform * m_parent->m_globalTransform;
    }
    _propagateTransformChange();
}

glm::mat4 Node::getTransform()
{
    return m_localTransform;
}

glm::mat4 Node::getGlobalTransform(){
    return m_globalTransform;
}

void Node::setTransform(const glm::mat4& transform)
{
    m_localTransform = transform;
    if(m_parent)
    {
        m_globalTransform = m_localTransform * m_paretn->m_globalTransform;
    }
    _propagateTransformChange();
    
}

void Node::setGlobalTransform(const glm::mat4 &transform)
{
    m_globalTransform = transform;
    if(m_parent)
    {
        m_localTransform = m_globalTransform / m_parent->m_globalTransform;
    }
    _propagateTransformChange();
}

void Node::_adjustName(Node *node)
{
    //implement
    /*std:string suffix = "(2)";
    if(node->m_name.size() >= 3 && std::equal(std::string("(2)").rbegin(), ending.rend(), value.rbegin()))
    {

    }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());*/
}

void Node::_propagateTransformChange()
{
    for (auto const& [key, val] : m_children)
    {
        val->m_globalTransform = val->m_localTransform * m_globalTransform
        val->_propagateTransformChange();
    }
}
