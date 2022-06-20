#include "PPpch.h"
#include "UniformBufferLibrary.h"
namespace Puppet {

    void UniformBufferLibrary::Add(const std::string& name, const Ref<UniformBuffer>& uniformBuffer)
    {
        PP_CORE_ASSERT(!Exists(name), "uniformBuffer already exists!");
        m_UniformBuffers[name] = uniformBuffer;
    }
    Ref<UniformBuffer> UniformBufferLibrary::Get(const std::string& name)
    {
        PP_CORE_ASSERT(Exists(name), "uniformBuffer not found!");
        return m_UniformBuffers[name];
    }
    UniformBufferLibrary::UniformBufferLibrary()
    {
        Ref<UniformBuffer> CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
        Add("CameraUniform", CameraUniformBuffer);

        Ref<UniformBuffer> LightMatricesUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4x4) * 16, 1);
        Add("LightMatricesUniform", LightMatricesUniformBuffer);
    }
    bool UniformBufferLibrary::Exists(const std::string& name) const
    {
        return m_UniformBuffers.find(name) != m_UniformBuffers.end();
    }
}
