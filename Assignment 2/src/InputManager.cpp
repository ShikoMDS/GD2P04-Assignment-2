#include "InputManager.h"
#include <iostream>

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

void InputManager::changeScene(int sceneNumber)
{
    std::cout << "Changing to scene " << sceneNumber << std::endl;
    // Implement scene unloading and loading logic here
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

    const float OffsetX = PosX - MLastX;
    const float OffsetY = MLastY - PosY;  // Reversed since y-coordinates go from bottom to top

    MLastX = PosX;
    MLastY = PosY;

    // Update the camera's yaw and pitch based on the mouse movement
    MCamera.processMouseMovement(OffsetX, OffsetY);
}

void InputManager::scrollCallback(GLFWwindow* Window, double OffsetX, const double OffsetY) const
{
    MCamera.processMouseScroll(static_cast<float>(OffsetY));
}
