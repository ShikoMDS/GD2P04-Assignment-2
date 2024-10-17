#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Camera.h"
#include "LightManager.h"
#include "Terrain.h"  // Include the Terrain header
#include <iostream>

class Scene1 : public Scene {
public:
    Scene1(Camera& camera, LightManager& lightManager);

    void load() override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

private:
    Shader LightingShader;
    Shader SkyboxShader;
    Shader TerrainShader;
    Model GardenPlant, Tree, Statue, Sphere;
    Skybox LSkybox;
    Camera& GCamera;
    LightManager& GLightManager;
    Material material;

    // Add terrain instance
    Terrain terrain;
};
