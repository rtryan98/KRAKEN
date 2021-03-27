#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/Camera.h"
#include "Yggdrasil/core/graphics/Graphics.h"
#include "Yggdrasil/core/input/Input.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ygg::graphics
{
    void Camera::create()
    {
        this->cameraUniformBuffer = createBuffer(sizeof(this->matrices),
            memory::BUFFER_TYPE_UNIFORM, memory::BUFFER_USAGE_UPDATE_EVERY_FRAME);
        updateRotation();
    }

    void Camera::onUpdate()
    {
        uploadBufferData(this->cameraUniformBuffer, &this->matrices, sizeof(CameraData));
    }

    void Camera::destroy()
    {
        destroyBuffer(this->cameraUniformBuffer);
    }

    constexpr static glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

    void Camera::updateRotation()
    {
        glm::vec3 newFront
        {
            glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch)),
            glm::sin(glm::radians(this->pitch)),
            glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch))
        };
        this->front = glm::normalize(newFront);
        this->right = glm::normalize(glm::cross(this->front, WORLD_UP));
        this->up    = glm::normalize(glm::cross(this->right, this->front));
    }

    void Camera::updateView()
    {
        this->matrices.view = glm::lookAt(this->position, this->position + this->front, this->up);
    }

    void CameraController::create()
    {
        this->camera.create();
    }

    void CameraController::destroy()
    {
        this->camera.destroy();
    }

    void CameraController::onUpdate(float_t dt)
    {
        float_t dtSpeed{ this->speed * dt };
        if (input::isKeyPressed(YGGDRASIL_KEY_W) && input::isKeyPressed(YGGDRASIL_KEY_S))
        {} // skip
        else if (input::isKeyPressed(YGGDRASIL_KEY_W))
        {
            this->camera.position += camera.front * dtSpeed;
        }
        else if (input::isKeyPressed(YGGDRASIL_KEY_S))
        {
            this->camera.position -= camera.front * dtSpeed;
        }
        if (input::isKeyPressed(YGGDRASIL_KEY_A) && input::isKeyPressed(YGGDRASIL_KEY_D))
        {} // skip
        else if (input::isKeyPressed(YGGDRASIL_KEY_A))
        {
            this->camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * dtSpeed;
        }
        else if (input::isKeyPressed(YGGDRASIL_KEY_D))
        {
            this->camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * dtSpeed;
        }
        if (input::isKeyPressed(YGGDRASIL_KEY_LEFT_SHIFT) && input::isKeyPressed(YGGDRASIL_KEY_SPACE))
        {} // skip
        else if (input::isKeyPressed(YGGDRASIL_KEY_LEFT_SHIFT))
        {
            this->camera.position -= camera.up * dtSpeed;
        }
        else if (input::isKeyPressed(YGGDRASIL_KEY_SPACE))
        {
            this->camera.position += camera.up * dtSpeed;
        }

        if (input::isMouseButtonPressed(YGGDRASIL_MOUSE_BUTTON_1))
        {
            float_t xOffset
            {
                (static_cast<float_t>(input::getMousePosition().x) - static_cast<float_t>(this->lastX)) * this->sensitivity
            };
            float_t yOffset
            {
                (static_cast<float_t>(this->lastY) - static_cast<float_t>(input::getMousePosition().y)) * this->sensitivity
            };
            this->camera.yaw += xOffset;
            this->camera.pitch += yOffset;
            if (this->camera.pitch > 89.0f)
            {
                this->camera.pitch = 89.0f;
            }
            if (this->camera.pitch < -89.0f)
            {
                this->camera.pitch = -89.0f;
            }
            this->camera.updateRotation();
        }
        this->lastX = static_cast<float_t>(input::getMousePosition().x);
        this->lastY = static_cast<float_t>(input::getMousePosition().y);
        this->camera.updateView();
    }

    void CameraController::setPerspective(float_t fov)
    {
        this->camera.matrices.projection = glm::perspective
        (
            glm::radians(fov),
            static_cast<float_t>(globals::APPLICATION->getWindow()->getWidth()) / static_cast<float_t>(globals::APPLICATION->getWindow()->getHeight()),
            0.01f,
            1024.0f
        );
    }
}
