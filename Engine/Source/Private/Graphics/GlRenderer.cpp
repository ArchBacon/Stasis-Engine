#include "GlRenderer.hpp"

#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Blackbox.hpp"
#include "GlShader.hpp"
#include "Core/Engine.hpp"
#include "Core/Window.hpp"
#include "glm/gtx/quaternion.hpp"

namespace blackbox::graphics
{
    GlRenderer::GlRenderer()
    {
        shader = GlShader {"Shaders/basic.vert", "Shaders/basic.frag"};

        float vertices[]
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
        };

        unsigned indices[]
        {
            0, 1, 3,
            1, 2, 3,
        };

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        // Create, bind, and pass data to Vertex Buffer Object
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        // Set vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Set indices
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i ndices), indices, GL_STATIC_DRAW);

        // Set vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        glBindVertexArray(0);

        // Create textures
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);

        // Texture #1
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char* data = stbi_load("Content/ContainerWood.png", &width, &height, &channels, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        // Texture #1
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        data = stbi_load("Content/awesomeface.png", &width, &height, &channels, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        shader.Use();
        shader.SetInt("texture1", 0);
        shader.SetInt("texture2", 1);

        glEnable(GL_DEPTH_TEST);

        camera.location = {0, 0, 3};
    }

    GlRenderer::~GlRenderer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    };

    void GlRenderer::Render()
    {
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
        };

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // KEYBOARD CONTROLS
            const auto keystate = SDL_GetKeyboardState(nullptr);
            float x, y;
            const auto mousestate = SDL_GetMouseState(&x, &y);
            if (keystate[SDL_SCANCODE_W])
            {
                camera.location += camera.speed * ::Engine.DeltaTime() * camera.front;
            }
            if (keystate[SDL_SCANCODE_A])
            {
                camera.location -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * ::Engine.DeltaTime();
            }
            if (keystate[SDL_SCANCODE_S])
            {
                camera.location -= camera.speed * ::Engine.DeltaTime() * camera.front;
            }
            if (keystate[SDL_SCANCODE_D])
            {
                camera.location += glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * ::Engine.DeltaTime();
            }
            if (keystate[SDL_SCANCODE_Q])
            {
                camera.location -= camera.speed * ::Engine.DeltaTime() * camera.up;
            }
            if (keystate[SDL_SCANCODE_E])
            {
                camera.location += camera.speed * ::Engine.DeltaTime() * camera.up;
            }

            if (mousestate & SDL_BUTTON_RMASK)
            {
                SDL_HideCursor();
                SDL_SetWindowMouseGrab(::Engine.Window().window, true);
                SDL_WarpMouseInWindow(::Engine.Window().window, ::Engine.Window().Width<float>() / 2, ::Engine.Window().Height<float>() / 2);
                
                if (!camera.firstClick)
                {
                    const bool invertY = false;
                    float2 relativeMouseMovement {
                        (x - ::Engine.Window().Width<float>() / 2) / ::Engine.Window().Width<float>(),
                        (y - ::Engine.Window().Height<float>() / 2) / ::Engine.Window().Height<float>() * (float)(2 * invertY - 1)
                    };

                    const float mouseSensitivity = 10000.0f;
                    float2 mouseMovement = relativeMouseMovement * (mouseSensitivity * ::Engine.DeltaTime());

                    camera.rotation.yaw += mouseMovement.x;
                    camera.rotation.pitch += mouseMovement.y;
                    camera.rotation.pitch = glm::clamp(camera.rotation.pitch, -89.f, 89.f);
                    camera.front = glm::normalize(float3{
                        cos(glm::radians(camera.rotation.yaw)) * cos(glm::radians(camera.rotation.pitch)),
                        sin(glm::radians(camera.rotation.pitch)),
                        sin(glm::radians(camera.rotation.yaw)) * cos(glm::radians(camera.rotation.pitch))
                    });
                }

                camera.firstClick = false;
            }
            else
            {
                SDL_ShowCursor();
                SDL_SetWindowMouseGrab(::Engine.Window().window, false);
                camera.firstClick = true;
            }
        }

        glm::mat4 view = glm::lookAt(camera.location, camera.location + camera.front, camera.up);
        auto model = glm::rotate(glm::mat4(1.0f), ::Engine.Uptime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        auto projection = glm::perspective(glm::radians(camera.fov), ::Engine.Window().Width<float>() / ::Engine.Window().Height<float>(), 0.1f, 100.0f);

        shader.Use();
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void GlRenderer::SetRenderMode(const RenderMode mode) const
    {
        switch (mode)
        {
        case RenderMode::Default:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RenderMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        default:
            LogRenderer->Warn("Render mode `{}` not implemented.", static_cast<int>(mode));
        }
    }
}
