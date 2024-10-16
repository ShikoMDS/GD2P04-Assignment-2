#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"

class Scene2 : public Scene {
public:
    Scene2(Camera& camera, LightManager& lightManager);
    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    Shader LightingShader;
    Model GardenPlant;
    Model Sphere;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;
};
