#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

class Model {
public:
	Model(std::vector<Mesh> meshes);
	Model(std::string path, bool load_immediately = true);
	void Draw(Shader shader);

private:
	std::vector<Mesh> _meshes;
	std::string _path;
	std::vector<Texture> _loaded_textures;

	void Load(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadTextures(aiMaterial* material, aiTextureType texture_type, std::string texture_type_name);
};