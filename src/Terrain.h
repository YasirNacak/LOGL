#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Model.h"
#include "Mesh.h"
#include "Vertex.h"

#include <stb_image/stb_image.h>

class Terrain {
public:
	Terrain(int size, std::string heightmap_path, std::string texturemap_path);
	Terrain(int size, std::string heightmap_path, std::string splatmap_path, std::string texture0_path, std::string texture1_path, std::string texture2_path);

	Model GetModel();

	int GetSize();

private:
	Model Generate(int size, std::string heightmap_path);
	
	float GetHeight(int x, int z, unsigned char* heightmap, int heightmap_size);
	glm::vec3 GetNormal(int x, int z, unsigned char* heightmap, int heightmap_size);

	bool _is_single_texture;
	Model _terrain_model;
	Texture _texture0;
	Texture _texture1;
	Texture _texture2;
	Texture _splatmap_texture;
	int _size;

	
};