#include "InputManager.h"
#include "Scene.h"
#include "Scene1.h"
#include "Scene2.h"  // Include your Scene2 here
#include <iostream>

extern std::unique_ptr<Scene> currentScene;
extern SceneType activeScene;
extern Camera GCamera;
extern LightManager GLightManager;

InputManager::InputManager(Camera& Camera, LightManager& LightManager)
    : MCamera(Camera), MLightManager(LightManager), MWireframe(false), MCursorVisible(false),
    MFirstMouse(true), MLastX(800 / 2.0f), MLastY(600 / 2.0f)
{
    MKeyState = {
        {GLFW_KEY_1, false},
        {GLFW_KEY_2, false},
        {GLFW_KEY_3, false},
        {GLFW_KEY_4, false},
        {GLFW_KEY_C, false},
        {GLFW_KEY_X, false}
    };
}

void InputManager::processInput(GLFWwindow* Window, const float DeltaTime)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(Window, true);

    // Handle scene changes
    if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
        changeScene(1);
    if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
        changeScene(2);
    if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
        changeScene(3);
    if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
        changeScene(4);

    // Handle wireframe toggle (X key)
    if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS && !MKeyState[GLFW_KEY_X])
    {
        MKeyState[GLFW_KEY_X] = true;
        toggleWireframeMode();
    }
    else if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        MKeyState[GLFW_KEY_X] = false;
    }

    // Handle cursor visibility toggle (C key)
    if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_PRESS && !MKeyState[GLFW_KEY_C])
    {
        MKeyState[GLFW_KEY_C] = true;
        toggleCursorVisibility(Window);
    }
    else if (glfwGetKey(Window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        MKeyState[GLFW_KEY_C] = false;
    }

    // Movement controls (W, A, S, D, Q, E)
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
        MCamera.processKeyboard(Forward, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
        MCamera.processKeyboard(Backward, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
        MCamera.processKeyboard(Left, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
        MCamera.processKeyboard(Right, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
        MCamera.processKeyboard(Up, DeltaTime);
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
        MCamera.processKeyboard(Down, DeltaTime);
}

void InputManager::toggleWireframeMode()
{
    MWireframe = !MWireframe;
    if (MWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void InputManager::toggleCursorVisibility(GLFWwindow* Window)
{
    MCursorVisible = !MCursorVisible;
    if (MCursorVisible)
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        MFirstMouse = true;
    }
    else
    {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void InputManager::changeScene(int sceneNumber) {
    std::cout << "Changing to scene " << sceneNumber << std::endl;

    SceneType newScene = static_cast<SceneType>(sceneNumber - 1);  // Assuming sceneNumber 1 corresponds to SCENE_1, 2 to SCENE_2, etc.

    if (currentScene == nullptr || activeScene != newScene) {
        if (currentScene) {
            std::cout << "Cleaning up current scene..." << std::endl;
            currentScene->cleanup();  // Clean up the previous scene
        }

        std::cout << "Attempting to create new scene..." << std::endl;

        switch (newScene) {
        case SceneType::SCENE_1:
            std::cout << "Switching to Scene 1..." << std::endl;
            currentScene = std::make_unique<Scene1>(GCamera, GLightManager);
            break;
        case SceneType::SCENE_2:
            std::cout << "Switching to Scene 2..." << std::endl;
            currentScene = std::make_unique<Scene2>(GCamera, GLightManager);  // Assuming Scene2 is implemented
            break;
        default:
            std::cerr << "Invalid scene number!" << std::endl;
            return;
        }

        if (currentScene) {
            currentScene->load();
            std::cout << "Scene loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to load the new scene." << std::endl;
        }

        activeScene = newScene;
    }
    else {
        std::cout << "Already in the active scene, no need to switch." << std::endl;
    }
}
void InputManager::frameBufferSizeCallback(GLFWwindow* Window, const int Width, const int Height)
{
    glViewport(0, 0, Width, Height);
}

void InputManager::mouseCallback(GLFWwindow* Window, const double PosX, const double PosY)
{
    // If the cursor is visible, do not process camera movement
    if (MCursorVisible) return;

    // If this is the first mouse movement (after cursor was disabled), reset the last position
    if (MFirstMouse)
    {
        MLastX = PosX;
        MLastY = PosY;
        MFirstMouse = false;
    }

    const double OffsetX = PosX - MLastX;
    const double OffsetY = MLastY - PosY;  // Reversed since y-coordinates go from bottom to top

    MLastX = PosX;
    MLastY = PosY;

    // Update the camera's yaw and pitch based on the mouse movement
    MCamera.processMouseMovement(static_cast<float>(OffsetX), static_cast<float>(OffsetY));
}

void InputManager::scrollCallback(GLFWwindow* Window, double OffsetX, const double OffsetY) const
{
    MCamera.processMouseScroll(static_cast<float>(OffsetY));
}

void InputManager::enableRawMouseMotion(GLFWwindow* window)
{
    if (glfwRawMouseMotionSupported())  // Check if raw mouse motion is supported
    {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);  // Enable raw mouse motion
        std::cout << "Raw mouse motion enabled." << std::endl;
    }
    else
    {
        std::cout << "Raw mouse motion not supported on this system." << std::endl;
    }
}