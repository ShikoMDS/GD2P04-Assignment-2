#include "Camera.h"
#include "LightManager.h"
#include "InputManager.h"
#include "Scene.h"
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <memory>


// Settings
constexpr unsigned int ScrWidth = 800;
constexpr unsigned int ScrHeight = 600;

// Timing
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

// Camera
Camera GCamera(glm::vec3(0.0f, 0.0f, 5.0f));

// Lighting
LightManager GLightManager;

// Input Manager
InputManager GInputManager(GCamera, GLightManager);

// Scene management variables
std::unique_ptr<Scene> currentScene;
SceneType activeScene = SceneType::SCENE_1;

void checkGlError(const std::string& Location)
{
    GLenum Err;
    while ((Err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error at " << Location << ": " << Err << '\n';
    }
}

int main() {
    // Initialize and configure GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << '\n';
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable MSAA

    // Create window and OpenGL context
    GLFWwindow* Window = glfwCreateWindow(ScrWidth, ScrHeight, "OpenGL Lighting Demo", nullptr, nullptr);
    if (!Window) {
        std::cerr << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    // Initialize raw mouse motion
    GInputManager.enableRawMouseMotion(Window);

    // Set input callbacks (ensure these don't interfere with scene creation)
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

    // Set GLEW to use experimental features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << '\n';
        return -1;
    }

    // Enable OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);

    // Initialize the first scene
    std::cout << "Initializing scene..." << std::endl;
    Scene::switchScene(SceneType::SCENE_1, currentScene, activeScene, GCamera, GLightManager);

    // Check if the scene loaded successfully
    if (!currentScene) {
        std::cerr << "Failed to load Scene 1" << std::endl;
        return -1;
    }

    // Main render loop
    while (!glfwWindowShouldClose(Window)) {
        float CurrentFrame = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        GInputManager.processInput(Window, DeltaTime);

        // Update and render the current scene
        currentScene->update(DeltaTime);
        currentScene->render();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // Cleanup
    if (currentScene) {
        currentScene->cleanup();
    }

    glfwTerminate();
    return 0;
}
