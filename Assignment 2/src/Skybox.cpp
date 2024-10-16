/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2024 Media Design School

File Name : Skybox.cpp
Description : Implementations for Skybox class
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Skybox.h"

#include "stb_image.h"

#include <iostream>

Skybox::Skybox(const std::vector<std::string>& Faces)
{
	MCubeMapTexture = loadCubeMap(Faces);
	setupSkybox();
}

void Skybox::draw(const Shader& Shader) const
{
	glBindVertexArray(MVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, MCubeMapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Skybox::render(const Shader& skyboxShader, const Camera& camera, int scrWidth, int scrHeight) const
{
	glDepthFunc(GL_LEQUAL); // Ensure skybox is drawn correctly
	skyboxShader.use();
	skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));  // Remove translation component of view matrix
	skyboxShader.setMat4("projection", camera.getProjectionMatrix(scrWidth, scrHeight));
	draw(skyboxShader); // Call the existing draw method
	glDepthFunc(GL_LESS); // Reset depth function to default
}

void Skybox::setupSkybox()
{
	constexpr float SkyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, &MVao);
	glGenBuffers(1, &MVbo);
	glBindVertexArray(MVao);
	glBindBuffer(GL_ARRAY_BUFFER, MVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glBindVertexArray(0);
}

unsigned int Skybox::loadCubeMap(const std::vector<std::string>& Faces)
{
	unsigned int TextureId;
	glGenTextures(1, &TextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);

	int Width, Height, NrComponents;
	for (unsigned int I = 0; I < Faces.size(); I++)
	{
		unsigned char* Data = stbi_load(Faces[I].c_str(), &Width, &Height, &NrComponents, 0);
		if (Data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + I,
			             0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data
			);
			stbi_image_free(Data);
		}
		else
		{
			std::cerr << "Cubemap texture failed to load at path: " << Faces[I] << '\n';
			stbi_image_free(Data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return TextureId;
}
