#pragma once
#include "Shader.h"
#include "Camera.h"
#include "LightManager.h"
#include "Model.h"
#include "Skybox.h"
#include <memory>

// Enum to track the active scene
enum class SceneType { SCENE_1, SCENE_2, SCENE_3, SCENE_4 };

class Scene {
public:
    virtual ~Scene() = default;
    virtual void load() = 0;      // Load shaders, models, textures
    virtual void update(float deltaTime) = 0; // Update scene elements (input, animations, etc.)
    virtual void render() = 0;    // Render the scene
    virtual void cleanup() = 0;   // Clean up resources

    // Add the switchScene function
    static void switchScene(SceneType newScene, std::unique_ptr<Scene>& currentScene, SceneType& activeScene, Camera& camera, LightManager& lightManager);
};
