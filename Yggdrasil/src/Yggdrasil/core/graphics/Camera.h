#pragma once
#include <glm/glm.hpp>

namespace yggdrasil::graphics
{
    namespace memory
    {
        class Buffer;
    }

    class Camera
    {
    public:
        void create();
        void onUpdate();
        void destroy();
        void updateRotation();
        void updateView();
    private:
        glm::vec3 position{};
        glm::vec3 front{ 0.0f, 0.0f, -1.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 right{};
        float_t yaw{};
        float_t pitch{};
        struct CameraData
        {
            glm::mat4 projection{ 1.0f };
            glm::mat4 view{ 1.0f };
        } matrices{};
        memory::Buffer* cameraUniformBuffer{ nullptr };
    private:
        friend class GraphicsEngine;
        friend class SceneRenderer;
        friend class CameraController;
    };

    class CameraController
    {
    public:
        virtual void create();
        virtual void onUpdate(float_t dt);
        virtual void destroy();
        virtual void setPerspective(float_t fov);
    private:
        Camera camera{};
        float_t speed{ 4.5f };
        float_t sensitivity{ 0.25f };
        double_t lastX{ 0.0 };
        double_t lastY{ 0.0 };
    private:
        friend class GraphicsEngine;
    };
}
