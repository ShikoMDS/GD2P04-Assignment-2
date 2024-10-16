#include "Scene2.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>

constexpr float PlantScaleFactor = 0.005f;
constexpr float SphereScaleFactor = 0.5f;

Scene2::Scene2(Camera& camera, LightManager& lightManager)
	: LightingShader("resources/shaders/VertexShader.vert", "resources/shaders/FragmentShader.frag"), GardenPlant(
		  "resources/models/AncientEmpire/SM_Env_Garden_Plants_01.obj",
		  "PolygonAncientWorlds_Texture_01_A.png"), Sphere("resources/models/Sphere/Sphere_HighPoly.obj", ""),
	  GCamera(camera),
	  GLightManager(lightManager),
	  material()
{
	std::cout << "Scene2 constructor called" << std::endl;
}

void Scene2::load() {
    std::cout << "Loading resources for Scene2..." << std::endl;

    // Initialize material
    material.Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    material.Specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.Shininess = 32.0f;

    std::cout << "Scene2 loaded successfully" << std::endl;
}

void Scene2::update(float deltaTime) {
    // Handle any updates per frame here (if needed)
}

void Scene2::render() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the lighting shader and set view/projection matrices
    LightingShader.use();
    LightingShader.setMat4("view", GCamera.getViewMatrix());
    LightingShader.setMat4("projection", GCamera.getProjectionMatrix(800, 600));
    LightingShader.setVec3("viewPos", GCamera.VPosition);

    // Set spotlight properties based on camera position and direction
    GLightManager.setSpotLightPosition(GCamera.VPosition);
    GLightManager.setSpotLightDirection(GCamera.VFront);

    // Set material properties for objects
    LightingShader.setMaterial(material);

    // Update lighting based on toggles (point lights, directional lights, etc.)
    GLightManager.updateLighting(LightingShader);

    // Render garden plants as ground
    LightingShader.setBool("useTexture", true);
    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    for (int X = -5; X <= 5; X++) {
        for (int Z = -5; Z <= 5; Z++) {
            ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(X, -1.0f, Z));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(PlantScaleFactor));
            LightingShader.setMat4("model", ModelMatrix);
            GardenPlant.draw(LightingShader);  // Draw plant model
        }
    }

    // Render point light spheres
    glm::vec3 SpherePositions[] = {
        glm::vec3(-2.0f, 0.5f, 0.0f),
        glm::vec3(2.0f, 0.5f, 0.0f)
    };

    LightingShader.setBool("useTexture", false);

    for (int I = 0; I < 2; I++) {
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, SpherePositions[I]);
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(SphereScaleFactor));
        LightingShader.setMat4("model", ModelMatrix);

        // Update sphere colors based on point light state
        glm::vec3 SphereColor = GLightManager.isPointLightsOn() ? GLightManager.getPointLight(I).Colour : glm::vec3(0.0f);
        LightingShader.setVec3("solidColor", SphereColor);

        Sphere.draw(LightingShader);  // Draw sphere (light source indicators)
    }
}

void Scene2::cleanup() {
    std::cout << "Cleaning up Scene2 resources..." << std::endl;

    // 1. Delete shader program to free GPU resources
    if (LightingShader.getId() != 0) {  // Check if shader is valid
        glDeleteProgram(LightingShader.getId());  // Assuming getID() returns shader program ID
    }

    // 2. Clean up models (delete VAOs, VBOs, etc.)
    GardenPlant.cleanup();  // Assuming Model has a cleanup method
    Sphere.cleanup();       // Cleanup resources for Sphere model

    // Add more cleanup if you have other OpenGL objects like textures or buffers
}
