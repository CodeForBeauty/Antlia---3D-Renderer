#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	Material();
	Material(Shader* vs, Shader* fs);
	Material(Shader* vs, Shader* fs, float* albedo);
	Material(Shader* vs, Shader* fs, float* albedo, float metalic);
	Material(float* albedo);
	~Material();

	void SetAlbedo(float r, float g, float b, float a);
	float* GetAlbedo();
	void SetMetalic(float value);
	float GetMetalic();
	void SetRoughness(float value);
	float GetRoughness();
	void SetSpecular(float value);
	float GetSpecular();

	void LoadTexture(const char* path, int slot = 0);

	void SetCamPos(float x, float y, float z);
	void SetView(float* value);
	void SetProj(float* value);
	unsigned int program;
	unsigned int sceneSlot;
private:
	void CompileShaders();
	Shader* vs;
	Shader* fs;
	Texture* texture;
	float* albedo = new float[4] {1, 1, 1, 1};
	float metalic = 0.0;
	float roughness = 0.5;
	float specular = 0.5;

	int camPos;
	int view;
	int proj;
	int color;
};