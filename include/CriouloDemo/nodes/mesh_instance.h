#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "nodes/node.h"
#include "core/renderer/mesh_instance.h"

class MeshInstance : public Node
{
    public:
        MeshInstance();
        MeshInstance(const MeshInstance& meshInstance);
        MeshInstance(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Model>& model);

        void MeshInstance::setShader(const shared_ptr<Shader>& shader);
        void MeshInstance::setModel(const shared_ptr<Model> &model);
        shared_ptr<Model> getModel();
        shared_ptr<Shader> getShader();

    private:

        Renderer::MeshInstance* m_meshInstance;
};
#endif