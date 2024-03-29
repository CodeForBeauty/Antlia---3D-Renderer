#include <array>
#include <iostream>
#include <string>

#include "Scene.h"
//#include "functions.h"


Scene::Scene()
{
	AddMaterial(new Material());
	// Creating vertex array, vertex buffer, index buffer objects.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 100000, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 400000, nullptr, GL_DYNAMIC_DRAW);


	glActiveTexture(GL_TEXTURE0);
	glGenFramebuffers(1, &directShadowFBO);
	glGenTextures(1, &directShadowMap);
	glBindTexture(GL_TEXTURE_2D, directShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	glBindFramebuffer(GL_FRAMEBUFFER, directShadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	directShadowProgram = glCreateProgram();
	glAttachShader(directShadowProgram, DirectShadowVert.id);
	glAttachShader(directShadowProgram, DirectShadowFrag.id);
	glLinkProgram(directShadowProgram);
	glValidateProgram(directShadowProgram);


	glGenFramebuffers(1, &cubeShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cubeShadowFBO);
	glGenTextures(1, &cubeShadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeShadowMap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	cubeShadowProgram = glCreateProgram();
	glAttachShader(cubeShadowProgram, CubeShadowVert.id);
	glAttachShader(cubeShadowProgram, CubeShadowGeom.id);
	glAttachShader(cubeShadowProgram, CubeShadowFrag.id);
	glLinkProgram(cubeShadowProgram);
	glValidateProgram(cubeShadowProgram);
	glUseProgram(cubeShadowProgram);
	
	lm::mat4 perspective = lm::perspective(lm::radians(90), 0.1f, 10.0f);
	lm::mat4 metricies[6] =
	{
		lm::lookAt({0, 0, 0}, { 1,  0,  0}, {0, -1,  0}) * perspective,
		lm::lookAt({0, 0, 0}, {-1,  0,  0}, {0, -1,  0}) * perspective,
		lm::lookAt({0, 0, 0}, { 0, -1,  0}, {0,  0, 1}) * perspective,
		lm::lookAt({0, 0, 0}, { 0,  1,  0}, {0,  0,  -1}) * perspective,
		lm::lookAt({0, 0, 0}, { 0,  0,  1}, {0, -1,  0}) * perspective,
		lm::lookAt({0, 0, 0}, { 0,  0, -1}, {0, -1,  0}) * perspective,
	};


	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[0]"), 1, GL_TRUE, metricies[0]);
	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[1]"), 1, GL_TRUE, metricies[1]);
	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[2]"), 1, GL_TRUE, metricies[2]);
	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[3]"), 1, GL_TRUE, metricies[3]);
	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[4]"), 1, GL_TRUE, metricies[4]);
	glUniformMatrix4fv(glGetUniformLocation(cubeShadowProgram, "matrix[5]"), 1, GL_TRUE, metricies[5]);
	
	
	glUniform1f(glGetUniformLocation(cubeShadowProgram, "u_FarPlane"), 10);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
	for (auto i = objects.begin(); i != objects.end(); i++)
		delete *i;
	for (auto i = entities.begin(); i != entities.end(); i++)
		delete *i;
	for (auto i = lights.begin(); i != lights.end(); i++)
		delete *i;
	for (int i = 0; i < materials.size(); i++)
		delete materials[i];
}

void Scene::AddObject(Mesh* object)
{
	int offset = 0;
	int mat = 0;
	if (object->material)
		mat = object->material->sceneSlot;
	else
		object->material = materials[0];

	for (Mesh* i : materialGroup[mat])
		offset += i->geometry->verticiesCount;
	
	for (int i = 0; i < object->geometry->indeciesCount; i++)
		object->geometry->transformedIndecies[i] = object->geometry->indecies[i] + offset;
	
	objects.insert(object);
	materialGroup[mat].push_back(object);
}

void Scene::AddEntity(Entity* entity)
{
	entities.insert(entity);
}

void Scene::AddLight(Light* light)
{
	lights.insert(light);
	updateLight = true;
}

void Scene::AddMaterial(Material* material)
{
	materials.push_back(material);
	materialGroup.push_back({});
	material->sceneSlot = materialGroup.size()-1;
}

void Scene::SetObjectMaterial(Mesh* object, Material* material)
{
	bool start = false;
	for (Mesh* i : materialGroup[object->material->sceneSlot])
	{
		if (start)
			for (int j = 0; j < i->geometry->indeciesCount; j++)
				i->geometry->transformedIndecies[j] -= object->geometry->verticiesCount;
		if (i == object) start = true;
	}
	
	materialGroup[object->material->sceneSlot].erase(std::remove_if(materialGroup[object->material->sceneSlot].begin(),
		materialGroup[object->material->sceneSlot].end(),
		[&](Mesh* i) { return i == object; }), materialGroup[object->material->sceneSlot].end());
	
	materialGroup[material->sceneSlot].push_back(object);
	object->material = material;

	int offset = 0;
	for (Mesh* i : materialGroup[material->sceneSlot])
		offset += i->geometry->verticiesCount;
	
	for (int i = 0; i < object->geometry->indeciesCount; i++)
		object->geometry->transformedIndecies[i] = object->geometry->indecies[i] + offset;
}

void Scene::DeleteMaterial(Material* material)
{
	int offset = 0;
	for (Mesh* i : materialGroup[0])
		offset += i->geometry->verticiesCount;

	for (Mesh* i : materialGroup[material->sceneSlot])
	{
		materialGroup[0].push_back(i);
		for (int j = 0; j < i->geometry->indeciesCount; j++)
			i->geometry->transformedIndecies[j] = i->geometry->indecies[j] + offset;
		offset += i->geometry->indeciesCount;
	}
	
	materials.erase(materials.begin() + material->sceneSlot);
	materialGroup.erase(materialGroup.begin() + material->sceneSlot);
}

void Scene::DeleteLight(Light* light)
{
	lights.erase(light);
}

void Scene::DeleteObject(Mesh* object)
{
	bool start = false;
	for (Mesh* i : materialGroup[object->material->sceneSlot])
	{
		if (i == object) start = true;
		if (start)
			for (int j = 0; j < i->geometry->indeciesCount; j++)
				i->geometry->transformedIndecies[j] = i->geometry->transformedIndecies[j] - object->geometry->verticiesCount;
	}

	materialGroup[object->material->sceneSlot].erase(std::remove_if(materialGroup[object->material->sceneSlot].begin(),
		materialGroup[object->material->sceneSlot].end(),
		[&](Mesh* i) { return i == object; }), materialGroup[object->material->sceneSlot].end());
}


void Scene::Render(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, directShadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, cubeShadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	preview.ClearBuffers();

	for (int i = 0; i < materialGroup.size(); i++)
	{
		glEnable(GL_DEPTH_TEST);
		preview.UpdateProjection(width, height);
		materials[i]->Use();
		materials[i]->SetProj(preview.camProjection);
		materials[i]->ClearLights();

		if (preview.update)
		{
			materials[i]->SetCamPos(preview.GetPosition());
			materials[i]->SetView(preview.camMetricies);
		}

		std::vector <Vertex> batchVerticies;
		std::vector <unsigned int> batchIndecies;

		int c = 0;

		for (Mesh* j : materialGroup[i])
		{
			batchVerticies.insert(batchVerticies.end(), j->geometry->transformedVerticies,
				j->geometry->transformedVerticies + j->geometry->verticiesCount);

			batchIndecies.insert(batchIndecies.end(), j->geometry->transformedIndecies,
				j->geometry->transformedIndecies + j->geometry->indeciesCount);
			c++;
		}

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batchVerticies.size() * sizeof(Vertex), batchVerticies.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batchIndecies.size() * sizeof(unsigned int), batchIndecies.data());
		

		bool rendered = false;
		bool renderedSpot = false;
		bool renderedPoint = false;
		bool second = false;

		int count = 0;

		int countDirect = 0;
		int countPoint = 0;
		int countSpot = 0;


		for (auto j = lights.begin(); j != lights.end(); j++)
		{
			int type = (*j)->GetType();
			if (type == 1)
			{
				DirectLight* light = static_cast<DirectLight*>(*j);
				if (updateLight)
				{
					materials[i]->Use();
					std::string buff = "u_DirectLightColor[" + std::to_string(countDirect) + "]";
					lm::vec4 color = light->GetColor();
					glUniform4f(glGetUniformLocation(materials[i]->program, buff.c_str()), color.x, color.y, color.z, light->intensity);
					buff = "u_DirectLightDir[" + std::to_string(countDirect) + "]";
					lm::vec3 dir = light->GetForward();
					glUniform3f(glGetUniformLocation(materials[i]->program, buff.c_str()), dir.x, dir.y, dir.z);
				}
				lm::vec3 camPos = preview.GetPosition();
				glBindFramebuffer(GL_FRAMEBUFFER, directShadowFBO);
				glUseProgram(directShadowProgram);
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, shadowWidth, shadowHeight);
				glUniformMatrix4fv(glGetUniformLocation(directShadowProgram, "lightProj"), 1, GL_TRUE, light->projection);
				glUniform3f(glGetUniformLocation(directShadowProgram, "u_Pos"), -camPos.x, -camPos.y, -camPos.z);
				glDrawElements(GL_TRIANGLES, batchIndecies.size(), GL_UNSIGNED_INT, nullptr);
				
				preview.RenderDirectShadow(light->projection, directShadowMap, rendered, batchIndecies.size());
				rendered = true;
				countDirect++;
			}
			if (type == 2)
			{
				PointLight* light = static_cast<PointLight*>(*j);
				lm::vec3 position = light->GetPosition();
				if (updateLight)
				{
					materials[i]->Use();
					std::string buff = "u_PointLightColor[" + std::to_string(countPoint) + "]";
					lm::vec4 color = light->GetColor();
					glUniform4f(glGetUniformLocation(materials[i]->program, buff.c_str()), color.x, color.y, color.z, light->intensity);
					buff = "u_PointLightPos[" + std::to_string(countPoint) + "]";
					glUniform4f(glGetUniformLocation(materials[i]->program, buff.c_str()), -position.x, -position.y, -position.z, light->GetDistance());
				}

				glBindFramebuffer(GL_FRAMEBUFFER, cubeShadowFBO);
				glUseProgram(cubeShadowProgram);
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, shadowWidth, shadowHeight);

				glUniform3f(glGetUniformLocation(cubeShadowProgram, "u_LightPos"), -position.x, -position.y, -position.z);
				glUniform3f(glGetUniformLocation(cubeShadowProgram, "u_Pos"), position.x, position.y, position.z);
				glDrawElements(GL_TRIANGLES, batchIndecies.size(), GL_UNSIGNED_INT, nullptr);


				preview.RenderPointShadow(light->projection, position, cubeShadowMap, renderedPoint, batchIndecies.size());
				renderedPoint = true;
				countPoint++;
			}
			if (type == 3)
			{
				SpotLight* light = static_cast<SpotLight*>(*j);
				lm::vec3 position = light->GetPosition();
				if (updateLight)
				{
					materials[i]->Use();
					std::string buff = "u_SpotLightColor[" + std::to_string(countSpot) + "]";
					lm::vec4 color = light->GetColor();
					glUniform4f(glGetUniformLocation(materials[i]->program, buff.c_str()), color.x, color.y, color.z, light->intensity);

					buff = "u_SpotLightPos[" + std::to_string(countSpot) + "]";
					glUniform4f(glGetUniformLocation(materials[i]->program, buff.c_str()), position.x, position.y, position.z, light->GetDistance());

					buff = "u_SpotLightDir[" + std::to_string(countSpot) + "]";
					lm::vec3 dir = light->GetForward();
					glUniform3f(glGetUniformLocation(materials[i]->program, buff.c_str()), dir.x, dir.y, dir.z);

					buff = "u_SpotLightAngle[" + std::to_string(countSpot) + "]";
					float angle = light->GetAngle();
					glUniform2f(glGetUniformLocation(materials[i]->program, buff.c_str()), std::cos(lm::radians(angle-5)),
						std::cos(lm::radians(angle)));
				}
				glBindFramebuffer(GL_FRAMEBUFFER, directShadowFBO);
				glUseProgram(directShadowProgram);
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, shadowWidth, shadowHeight);
				glUniformMatrix4fv(glGetUniformLocation(directShadowProgram, "lightProj"), 1, GL_TRUE, light->projection);
				glUniform3f(glGetUniformLocation(directShadowProgram, "u_Pos"), -position.x, -position.y, -position.z);
				glDrawElements(GL_TRIANGLES, batchIndecies.size(), GL_UNSIGNED_INT, nullptr);

				preview.RenderSpotShadow(light->projection, position, directShadowMap, renderedSpot, batchIndecies.size());
				renderedSpot = true;
				countSpot++;
			}
			count++;
		}

		preview.Render(materials[i], batchIndecies.size(), width, height);
	}
	preview.update = false;
	updateLight = true;
}

