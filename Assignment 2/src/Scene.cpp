#include "Scene.h"
#include "Scene1.h"
#include "Scene2.h"
#include <iostream>

void Scene::switchScene(SceneType newScene, std::unique_ptr<Scene>& currentScene, SceneType& activeScene, Camera& camera, LightManager& lightManager) {
    std::cout << "Switching to new scene..." << std::endl;

    if (currentScene == nullptr || activeScene != newScene) {
        if (currentScene) {
            std::cout << "Cleaning up current scene..." << std::endl;
            currentScene->cleanup();  // Clean up the previous scene
        }

        std::cout << "Attempting to create new scene..." << std::endl;

        switch (newScene) {
        case SceneType::SCENE_1:
            currentScene = std::make_unique<Scene1>(camera, lightManager);
            std::cout << "Scene1 created successfully" << std::endl;
            break;
        case SceneType::SCENE_2:
            currentScene = std::make_unique<Scene2>(camera, lightManager);
            std::cout << "Scene2 created successfully" << std::endl;
            break;
        default:
            std::cerr << "Invalid scene type" << std::endl;
            break;
        }

        if (currentScene) {
            currentScene->load();
            std::cout << "Scene loaded successfully" << std::endl;
        }
        else {
            std::cerr << "Failed to create the new scene." << std::endl;
        }

        activeScene = newScene;
    }
    else {
        std::cout << "Already in the active scene, no need to switch." << std::endl;
    }
}
