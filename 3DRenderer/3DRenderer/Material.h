#pragma once

#include <string>
#include "linmath.h"
#include "Shader.h"
#include "Texture.h"

const int TEXTURE_ALBEDO = 1;
const int TEXTURE_METALIC = 2;
const int TEXTURE_SPECULAR = 3;
const int TEXTURE_ROUGHNESS = 4;
const int TEXTURE_NORMAL = 5;

class Material
{
public:
	Material();
	Material(Shader vs, Shader fs, Shader gs);
	Material(Shader vs, Shader fs, Shader gs, lm::vec4 albedo);
	Material(lm::vec4 albedo);
	~Material();

	void SetAlbedo(float r, float g, float b, float a);
	lm::vec4 GetAlbedo();
	void SetMetalic(float value);
	float GetMetalic();
	void SetRoughness(float value);
	float GetRoughness();
	void SetSpecular(float value);
	float GetSpecular();

	void SetName(std::string name);
	std::string GetName();

	void LoadTexture(const char* path, int slot = 0);
	void DeleteTexture(int slot);

	void Use();
	void Bind();

	void ClearLights();

	void SetCamPos(float x, float y, float z);
	void SetCamPos(lm::vec3 position);
	void SetView(lm::mat4 value);
	void SetProj(lm::mat4 value);
	unsigned int program;
	unsigned int sceneSlot;
	Texture* texture;
	Texture* specTex;
	Texture* metalTex;
	Texture* roughTex;
	Texture* normal;
private:
	void CompileShaders();
	Shader vs;
	Shader fs;
	Shader gs;
	lm::vec4 albedo = {1, 1, 1, 1};
	float metalic = 0.0;
	float roughness = 0.5;
	float specular = 0.5;

	int camPos;
	int view;
	int proj;
	int color;

	std::string name = "Material";
};