#include "Model.h"

Model::Model() {
}

Model::Model(std::vector<Mesh> meshes) {
	this->_meshes = meshes;
}

Model::Model(std::string path, bool load_immediately) {
	if (load_immediately) {
		Load(path);
	}
}

void Model::Draw(Shader shader) {
	for (auto mesh : _meshes) {
		mesh.Draw(shader);
	}
}

void Model::Load(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	_path = path.substr(0, path.find_last_of('/'));
	
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vertex.Position = position;

		glm::vec3 normal = glm::vec3(0.0f);
		if (mesh->mNormals != NULL) {
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
		}
		vertex.Normal = normal;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texture_coordinates;
			texture_coordinates.x = mesh->mTextureCoords[0][i].x;
			texture_coordinates.y = mesh->mTextureCoords[0][i].y;
			vertex.TextureCoordinates = texture_coordinates;
		}
		else {
			vertex.TextureCoordinates = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuse_maps = LoadTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		std::vector<Texture> specular_maps = LoadTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadTextures(aiMaterial* material, aiTextureType texture_type, std::string texture_type_name) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(texture_type); i++) {
		aiString str;
		material->GetTexture(texture_type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < _loaded_textures.size(); j++) {
			if (std::strcmp(_loaded_textures[j].Path.data(), str.C_Str()) == 0) {
				textures.push_back(_loaded_textures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.Id = Texture::Load(_path + "/" + std::string(str.C_Str()));
			texture.Type = texture_type_name;
			texture.Path = str.C_Str();
			textures.push_back(texture);
			_loaded_textures.push_back(texture);
		}
	}
	return textures;
}
