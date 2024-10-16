/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : main.cpp
Description : Entry point for OpenGL Framebuffers and Shaders for Assignment 2
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Shader.h"
#include "Camera.h"
#include "LightManager.h"
#include "Model.h"
#include "Skybox.h"
#include "InputManager.h"

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

// Settings
constexpr unsigned int ScrWidth = 800;
constexpr unsigned int ScrHeight = 600;

// Timing
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

// Scale factors for models
constexpr float ModelScaleFactor = 0.01f;
constexpr float PlantScaleFactor = 0.005f;
constexpr float SphereScaleFactor = 0.5f;

// Camera
Camera GCamera(glm::vec3(0.0f, 0.0f, 5.0f));

// Lighting
LightManager GLightManager;

// Input Manager
InputManager GInputManager(GCamera, GLightManager);

void checkGlError(const std::string& Location)
{
	GLenum Err;
	while ((Err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error at " << Location << ": " << Err << '\n';
	}
}

int main()
{
    // Initialize and configure GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << '\n';
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable MSAA

    // Create window and OpenGL context
    GLFWwindow* Window = glfwCreateWindow(ScrWidth, ScrHeight, "OpenGL Lighting Demo", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    // Enable raw mouse motion
    GInputManager.enableRawMouseMotion(Window);

    // Set input callbacks
    glfwSetFramebufferSizeCallback(Window, [](GLFWwindow* window, int width, int height) {
        GInputManager.frameBufferSizeCallback(window, width, height);
        });
    glfwSetCursorPosCallback(Window, [](GLFWwindow* window, double xpos, double ypos) {
        GInputManager.mouseCallback(window, xpos, ypos);
        });
    glfwSetScrollCallback(Window, [](GLFWwindow* window, double xoffset, double yoffset) {
        GInputManager.scrollCallback(window, xoffset, yoffset);
        });

    // Capture the mouse
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << '\n';
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);

    // Initialize shaders
    Shader LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag");
    Shader SkyboxShader("resources/shaders/SkyboxVertexShader.vert", "resources/shaders/SkyboxFragmentShader.frag");

    // Load skybox and models
    Skybox LSkybox;
    Model GardenPlant("resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj", "PolygonAncientWorlds_Texture_01_A.png");
    Model Tree("resources/models/AncientEmpire/SM_Env_Tree_Palm_01.obj", "PolygonAncientWorlds_Texture_01_A.png");
    Model Statue("resources/models/AncientEmpire/SM_Prop_Statue_01.obj", "PolygonAncientWorlds_Texture_01_A.png");
    Model Sphere("resources/models/Sphere/Sphere_HighPoly.obj", "");

    // Define material properties
    Material material;
    material.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;

    // Initialize lighting
    GLightManager.initialize();

    std::cout << "Starting rendering loop..." << '\n';

    // Rendering loop
    while (!glfwWindowShouldClose(Window))
    {
        // Per-frame time logic
        float CurrentFrame = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        // Input
        GInputManager.processInput(Window, DeltaTime);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set shader and pass uniforms
        LightingShader.use();
        LightingShader.setMat4("view", GCamera.getViewMatrix());
        LightingShader.setMat4("projection", GCamera.getProjectionMatrix(ScrWidth, ScrHeight));
        LightingShader.setVec3("viewPos", GCamera.VPosition);

        // Update spotlight properties based on camera position and direction
        GLightManager.setSpotLightPosition(GCamera.VPosition);
        GLightManager.setSpotLightDirection(GCamera.VFront);

        // Set material properties
        LightingShader.setMaterial(material);

        // Update lighting based on toggles
        GLightManager.updateLighting(LightingShader);

        // Render garden plants as ground
        LightingShader.setBool("useTexture", true);
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        for (int X = -5; X <= 5; X++)
        {
            for (int Z = -5; Z <= 5; Z++)
            {
                ModelMatrix = glm::mat4(1.0f);
                ModelMatrix = glm::translate(ModelMatrix, glm::vec3(X, -1.0f, Z));
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(PlantScaleFactor));
                LightingShader.setMat4("model", ModelMatrix);
                GardenPlant.draw(LightingShader);
            }
        }

        // Render trees
        glm::vec3 TreePositions[] = {
            {-5.0f, -1.0f, -5.0f}, {5.0f, -1.0f, -5.0f},
            {-5.0f, -1.0f, 5.0f}, {5.0f, -1.0f, 5.0f}
        };

        for (glm::vec3 Pos : TreePositions)
        {
            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, Pos);
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(ModelScaleFactor));
            LightingShader.setMat4("model", ModelMatrix);
            Tree.draw(LightingShader);
        }

        // Render statue
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(ModelScaleFactor));
        LightingShader.setMat4("model", ModelMatrix);
        Statue.draw(LightingShader);

        // Render point light spheres
        glm::vec3 SpherePositions[] = {
            glm::vec3(-2.0f, 0.5f, 0.0f),
            glm::vec3(2.0f, 0.5f, 0.0f)
        };

        LightingShader.setBool("useTexture", false);

        for (int I = 0; I < 2; I++)
        {
            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, SpherePositions[I]);
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(SphereScaleFactor));
            LightingShader.setMat4("model", ModelMatrix);

            // Update sphere colors based on point light state
            glm::vec3 SphereColor = GLightManager.isPointLightsOn() ? GLightManager.getPointLight(I).Colour : glm::vec3(0.0f);
            LightingShader.setVec3("solidColor", SphereColor);

            Sphere.draw(LightingShader);
        }

        // Render skybox
        LSkybox.render(SkyboxShader, GCamera, ScrWidth, ScrHeight);

        // Swap buffers and poll IO events
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // Clean up
    glfwTerminate();
    return 0;
}
